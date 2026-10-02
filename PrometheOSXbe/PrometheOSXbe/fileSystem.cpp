#include "fileSystem.h"
#include "xboxInternals.h"
#include "stringUtility.h"
#include "driveManager.h"
#include "pointerMap.h"
#include "sdcard.h"

#include <cstring>
#include <ctime>
#include <io.h>

namespace {
        
    typedef struct SdCardFileState {
        char* path;
        fileSystem::FileMode fileMode;
        uint32_t position;
        uint32_t size;
    } SdCardFileState;

    typedef struct FileContainer {
        void* file;
        fileSystem::FileContainerType type;
    } FileContainer;

    uint32_t mMaxFileContainerID = 0;

    pointerMap<FileContainer*>* mFileContainerMap = new pointerMap<FileContainer*>(true);

    uint32_t addFileContainer(FileContainer* fileContainer) {
        uint32_t result = mMaxFileContainerID;
        mFileContainerMap->add(result, fileContainer);
        mMaxFileContainerID++;
        return result;
    }

    FileContainer* getFileContainer(uint32_t fileHandle) {
        FileContainer* fileContainer = mFileContainerMap->get(fileHandle);
        return fileContainer;
    }

    void deleteFileContainer(uint32_t fileHandle) {
        mFileContainerMap->removeKey(fileHandle);
    }

    static uint32_t minUint32(uint32_t a, uint32_t b) {
        return a < b ? a : b;
    }

    static const char* SDCARD_PATH_PREFIX = "SDCARD:";
    static const size_t SDCARD_PATH_PREFIX_LENGTH = 7;

    static bool isSdCardPrefixedPath(const char* path) {
        if (path == NULL) {
            return false;
        }
        return _strnicmp(path, SDCARD_PATH_PREFIX, SDCARD_PATH_PREFIX_LENGTH) == 0;
    }

    static char* sdCardEnsureLeadingBackslash(char* path) {
        if (path == NULL) {
            return NULL;
        }
        if (path[0] == '\\') {
            return path;
        }
        size_t length = strlen(path);
        char* result = (char*)malloc(length + 2);
        if (result == NULL) {
            free(path);
            return NULL;
        }
        result[0] = '\\';
        memcpy(result + 1, path, length + 1);
        free(path);
        return result;
    }

    static char* resolveSdCardPath(const char* path) {
        if (isSdCardPrefixedPath(path) == false) {
            return NULL;
        }
        const char* remainder = path + SDCARD_PATH_PREFIX_LENGTH;
        while (*remainder == '\\') {
            remainder++;
        }
        char* result = strdup(remainder);
        if (result == NULL) {
            return NULL;
        }
        return sdCardEnsureLeadingBackslash(result);
    }

    static char* buildSdCardPrefixedPath(const char* sdPath) {
        size_t length = SDCARD_PATH_PREFIX_LENGTH + strlen(sdPath);
        char* result = (char*)malloc(length + 1);
        if (result == NULL) {
            return NULL;
        }
        strcpy(result, SDCARD_PATH_PREFIX);
        strcat(result, sdPath);
        return result;
    }

    static char* combineSdCardPrefixedPath(const char* prefixedPath, const char* name) {
        char* sdPath = resolveSdCardPath(prefixedPath);
        if (sdPath == NULL) {
            return NULL;
        }
        size_t sdLength = strlen(sdPath);
        size_t nameLength = strlen(name);
        bool needsSeparator = sdLength > 0 && sdPath[sdLength - 1] != '\\' && nameLength > 0;
        size_t combinedLength = sdLength + (needsSeparator ? 1 : 0) + nameLength;
        char* combinedSdPath = (char*)malloc(combinedLength + 1);
        if (combinedSdPath == NULL) {
            free(sdPath);
            return NULL;
        }
        strcpy(combinedSdPath, sdPath);
        if (needsSeparator) {
            strcat(combinedSdPath, "\\");
        }
        strcat(combinedSdPath, name);
        free(sdPath);
        char* result = buildSdCardPrefixedPath(combinedSdPath);
        free(combinedSdPath);
        return result;
    }

    static void sdCardSetDefaultFileTime(fileSystem::FileTime& fileTime) {
        fileTime.year = 2000;
        fileTime.month = 1;
        fileTime.day = 1;
        fileTime.hour = 0;
        fileTime.minute = 0;
        fileTime.second = 0;
    }

    static void sdCardFatFsDateTimeToFileTime(uint32_t packedDateTime, fileSystem::FileTime& fileTime) {
        if (packedDateTime == 0) {
            sdCardSetDefaultFileTime(fileTime);
            return;
        }

        uint16_t fdate = (uint16_t)(packedDateTime >> 16);
        uint16_t ftime = (uint16_t)(packedDateTime & 0xffff);

        fileTime.year = (uint16_t)(((fdate >> 9) & 0x7f) + 1980);
        fileTime.month = (uint16_t)((fdate >> 5) & 0x0f);
        fileTime.day = (uint16_t)(fdate & 0x1f);
        fileTime.hour = (uint16_t)((ftime >> 11) & 0x1f);
        fileTime.minute = (uint16_t)((ftime >> 5) & 0x3f);
        fileTime.second = (uint16_t)((ftime & 0x1f) * 2);

        if (fileTime.month == 0 || fileTime.month > 12 || fileTime.day == 0 || fileTime.day > 31) {
            sdCardSetDefaultFileTime(fileTime);
        }
    }

    static void sdCardPopulateFileTimes(fileSystem::FileInfoDetail* fileInfoDetail) {
        sdCardFatFsDateTimeToFileTime(sdcard::get_file_created(), fileInfoDetail->accessTime);
        sdCardFatFsDateTimeToFileTime(sdcard::get_file_modified(), fileInfoDetail->writeTime);
    }

    static bool sdCardFetchFileInfo(const char* sdPath, fileSystem::FileInfoDetail*& fileInfoDetail) {
        sdcard::open();
        sdcard::set_path(sdPath);
        if (sdcard::get_file_info_from_path() != false) {
            sdcard::close();
            return false;
        }

        uint8_t fileFlags = sdcard::get_file_flags();
        fileInfoDetail = new fileSystem::FileInfoDetail();
        fileInfoDetail->path = buildSdCardPrefixedPath(sdPath);
        fileInfoDetail->isDirectory = sdcard::is_file_directory(fileFlags);
        fileInfoDetail->isFile = !fileInfoDetail->isDirectory;
        fileInfoDetail->size = sdcard::get_file_size();
        sdCardPopulateFileTimes(fileInfoDetail);

        sdcard::close();
        return fileInfoDetail->path != NULL;
    }

    static void sdCardReleaseState(SdCardFileState* state) {
        if (state == NULL) {
            return;
        }
        free(state->path);
        delete state;
    }

    static bool sdCardDeletePath(const char* sdPath) {
        sdcard::open();
        bool result = sdcard::delete_path(sdPath);
        sdcard::close();
        return result;
    }

    static bool sdCardRenamePath(const char* sourceSdPath, const char* destSdPath) {
        sdcard::open();
        bool result = sdcard::rename_path(sourceSdPath, destSdPath);
        sdcard::close();
        return result;
    }

    static bool openHostFile(const char* path, fileSystem::FileMode const fileMode, fileSystem::FileContainerType const containerType, uint32_t& fileHandle) {
        char* access = "";
        if (fileMode == fileSystem::FileModeRead) {
            access = "rb";
        } else if (fileMode == fileSystem::FileModeWrite) {
            access = "wb";
        } else if (fileMode == fileSystem::FileModeAppend) {
            access = "ab";
        } else if (fileMode == fileSystem::FileModeReadUpdate) {
            access = "r+b";
        } else if (fileMode == fileSystem::FileModeWriteUpdate) {
            access = "w+b";
        } else if (fileMode == fileSystem::FileModeAppendUpdate) {
            access = "a+b";
        }
        FileContainer* fileContainer = new FileContainer();
        fileContainer->type = containerType;
        fileContainer->file = fopen(path, access);
        if (fileContainer->file == NULL) {
            delete (fileContainer);
            return false;
        }
        fileHandle = addFileContainer(fileContainer);
        return true;
    }

    static bool sdCardOpenFile(const char* sdPath, fileSystem::FileMode const fileMode, uint32_t& fileHandle) {
        SdCardFileState* state = new SdCardFileState();
        state->path = strdup(sdPath);
        if (state->path == NULL) {
            delete state;
            return false;
        }
        state->fileMode = fileMode;
        state->position = 0;
        state->size = 0;

        sdcard::open();
        bool opened = false;

        if (fileMode == fileSystem::FileModeRead || fileMode == fileSystem::FileModeReadUpdate) {
            opened = sdcard::open_file_from_path(sdPath);
            if (opened) {
                state->size = sdcard::get_file_size();
                sdcard::close_file();
            }
        } else if (fileMode == fileSystem::FileModeWrite || fileMode == fileSystem::FileModeWriteUpdate) {
            opened = sdcard::create_file(sdPath, 0);
            state->size = 0;
        } else if (fileMode == fileSystem::FileModeAppend || fileMode == fileSystem::FileModeAppendUpdate) {
            opened = sdcard::open_file_from_path(sdPath);
            if (opened) {
                state->size = sdcard::get_file_size();
                state->position = state->size;
                sdcard::close_file();
            } else {
                opened = sdcard::create_file(sdPath, 0);
                state->size = 0;
                state->position = 0;
            }
        }

        sdcard::close();

        if (opened == false) {
            sdCardReleaseState(state);
            return false;
        }

        FileContainer* fileContainer = new FileContainer();
        fileContainer->type = fileSystem::SdCard;
        fileContainer->file = state;
        fileHandle = addFileContainer(fileContainer);
        return true;
    }

    static bool sdCardReadBytes(SdCardFileState* state, char* readBuffer, uint32_t bytesToRead, uint32_t& bytesRead) {
        sdcard::open();
        if (sdcard::open_file_from_path(state->path) == false) {
            sdcard::close();
            return false;
        }

        bytesRead = 0;
        while (bytesRead < bytesToRead && state->position < state->size) {
            uint32_t chunkIndex = state->position >> SDCARD_FILE_CHUNK_SIZE_SHIFT;
            uint32_t chunkOffset = state->position & (SDCARD_FILE_CHUNK_SIZE - 1);
            uint32_t chunkLength = 0;
            uint8_t* chunkData = sdcard::read_chunk(chunkIndex, &chunkLength);
            if (chunkData == NULL || chunkLength == 0) {
                sdcard::close_file();
                sdcard::close();
                return false;
            }

            uint32_t availableInChunk = chunkLength > chunkOffset ? chunkLength - chunkOffset : 0;
            uint32_t remainingInFile = state->size - state->position;
            uint32_t toCopy = minUint32(bytesToRead - bytesRead, minUint32(availableInChunk, remainingInFile));
            if (toCopy > 0) {
                memcpy(readBuffer + bytesRead, chunkData + chunkOffset, toCopy);
                bytesRead += toCopy;
                state->position += toCopy;
            }
            free(chunkData);

            if (toCopy == 0) {
                break;
            }
        }

        sdcard::close_file();
        sdcard::close();
        return true;
    }

    static bool sdCardWriteBytes(SdCardFileState* state, char* writeBuffer, uint32_t bytesToWrite, uint32_t& bytesWritten) {
        sdcard::open();
        if (sdcard::open_file_from_path(state->path) == false) {
            sdcard::close();
            bytesWritten = 0;
            return false;
        }

        bytesWritten = 0;
        while (bytesWritten < bytesToWrite) {
            uint32_t chunkIndex = state->position >> SDCARD_FILE_CHUNK_SIZE_SHIFT;
            uint32_t toWrite = minUint32(bytesToWrite - bytesWritten, SDCARD_FILE_CHUNK_SIZE);

            if (sdcard::write_chunk(chunkIndex, (uint8_t*)(writeBuffer + bytesWritten), (uint16_t)toWrite) == false) {
                sdcard::close_file();
                sdcard::close();
                return false;
            }

            bytesWritten += toWrite;
            state->position += toWrite;
            if (state->position > state->size) {
                state->size = state->position;
            }
        }

        sdcard::close_file();
        sdcard::close();
        return bytesWritten == bytesToWrite;
    }

}

fileSystem::FileInfoDetail* fileSystem::fileGetFileInfoDetail(const char* path) {
    char* sdPath = resolveSdCardPath(path);
    if (sdPath != NULL) {
        FileInfoDetail* fileInfoDetail = NULL;
        if (sdCardFetchFileInfo(sdPath, fileInfoDetail) == false) {
            free(sdPath);
            return NULL;
        }
        free(sdPath);
        return fileInfoDetail;
    }

    const DWORD invalidFileAttributes = (DWORD)0xFFFFFFFF;

    DWORD attributes = GetFileAttributesA(path);
    if (attributes == invalidFileAttributes) {
        return NULL;
    }

    FileInfoDetail* fileInfoDetail = new FileInfoDetail();
    fileInfoDetail->path = strdup(path);
    fileInfoDetail->isDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    fileInfoDetail->isFile = (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    fileInfoDetail->accessTime.month = 1;
    fileInfoDetail->accessTime.day = 1;
    fileInfoDetail->accessTime.year = 2000;
    fileInfoDetail->accessTime.hour = 0;
    fileInfoDetail->accessTime.minute = 0;
    fileInfoDetail->accessTime.second = 0;
    fileInfoDetail->writeTime.month = 1;
    fileInfoDetail->writeTime.day = 1;
    fileInfoDetail->writeTime.year = 2000;
    fileInfoDetail->writeTime.hour = 0;
    fileInfoDetail->writeTime.minute = 0;
    fileInfoDetail->writeTime.second = 0;

    HANDLE fileHandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        fileInfoDetail->isDirectory ? FILE_FLAG_BACKUP_SEMANTICS : 0, NULL);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        FILETIME fileTimeAccess;
        FILETIME fileTimeWrite;
        if (GetFileTime(fileHandle, NULL, &fileTimeAccess, &fileTimeWrite) == FALSE) {
            CloseHandle(fileHandle);
            delete (fileInfoDetail);
            return NULL;
        }

        SYSTEMTIME systemTimeAccessLocal;
        if (FileTimeToSystemTime(&fileTimeAccess, &systemTimeAccessLocal) == FALSE) {
            CloseHandle(fileHandle);
            delete (fileInfoDetail);
            return NULL;
        }

        fileInfoDetail->accessTime.month = systemTimeAccessLocal.wMonth;
        fileInfoDetail->accessTime.day = systemTimeAccessLocal.wDay;
        fileInfoDetail->accessTime.year = systemTimeAccessLocal.wYear;
        fileInfoDetail->accessTime.hour = systemTimeAccessLocal.wHour;
        fileInfoDetail->accessTime.minute = systemTimeAccessLocal.wMinute;
        fileInfoDetail->accessTime.second = systemTimeAccessLocal.wSecond;

        if (fileTimeWrite.dwHighDateTime != 0 || fileTimeWrite.dwLowDateTime) {
            FILETIME fileTimeWriteLocal;
            if (FileTimeToLocalFileTime(&fileTimeWrite, &fileTimeWriteLocal) == FALSE) {
                CloseHandle(fileHandle);
                delete (fileInfoDetail);
                return NULL;
            }

            SYSTEMTIME systemTimeWriteLocal;
            if (FileTimeToSystemTime(&fileTimeWriteLocal, &systemTimeWriteLocal) == FALSE) {
                CloseHandle(fileHandle);
                delete (fileInfoDetail);
                return NULL;
            }

            fileInfoDetail->writeTime.month = systemTimeWriteLocal.wMonth;
            fileInfoDetail->writeTime.day = systemTimeWriteLocal.wDay;
            fileInfoDetail->writeTime.year = systemTimeWriteLocal.wYear;
            fileInfoDetail->writeTime.hour = systemTimeWriteLocal.wHour;
            fileInfoDetail->writeTime.minute = systemTimeWriteLocal.wMinute;
            fileInfoDetail->writeTime.second = systemTimeAccessLocal.wSecond;
        }

        DWORD fileSize = GetFileSize(fileHandle, NULL);
        fileInfoDetail->size = fileSize;

        CloseHandle(fileHandle);
    } else {
        fileInfoDetail->size = 0;
    }
    return fileInfoDetail;
}

pointerVector<fileSystem::FileInfoDetail*>* fileSystem::fileGetFileInfoDetails(const char* path) {
    char* sdPath = resolveSdCardPath(path);
    if (sdPath != NULL) {
        pointerVector<FileInfoDetail*>* fileInfoDetails = new pointerVector<FileInfoDetail*>(true);

        sdcard::open();
        sdcard::set_cwd(sdPath);

        uint32_t fileIndex = 0;
        while (true) {
            uint8_t fileCount = sdcard::get_file_list(fileIndex);
            if (fileCount == 0) {
                break;
            }
            for (uint8_t i = 0; i < fileCount; i++) {
                if (sdcard::get_file_info_from_index(i) != false) {
                    continue;
                }
                uint8_t fileFlags = sdcard::get_file_flags();
                if (sdcard::is_file_visible(fileFlags) == false) {
                    free(sdcard::get_file_name());
                    continue;
                }
                uint32_t fileSize = sdcard::get_file_size();
                char* fileName = sdcard::get_file_name();
                if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
                    free(fileName);
                    continue;
                }
                char* entryPath = combineSdCardPrefixedPath(path, fileName);
                free(fileName);
                if (entryPath == NULL) {
                    sdcard::close();
                    free(sdPath);
                    delete (fileInfoDetails);
                    return NULL;
                }
                FileInfoDetail* fileInfoDetail = new FileInfoDetail();
                fileInfoDetail->path = entryPath;
                fileInfoDetail->isDirectory = sdcard::is_file_directory(fileFlags);
                fileInfoDetail->isFile = !fileInfoDetail->isDirectory;
                fileInfoDetail->size = fileSize;
                sdCardPopulateFileTimes(fileInfoDetail);
                fileInfoDetails->add(fileInfoDetail);
            }
            fileIndex += SDCARD_MAX_ENTRIES;
        }

        sdcard::close();
        free(sdPath);

		if (fileInfoDetails->count() > 1) {
			for (uint32_t i = 0; i < fileInfoDetails->count() - 1; i++) {
				for (uint32_t j = i + 1; j < fileInfoDetails->count(); j++) {
					FileInfoDetail* fileInfoDetail1 = fileInfoDetails->get(i);
					FileInfoDetail* fileInfoDetail2 = fileInfoDetails->get(j);
					if (stricmp(fileInfoDetail1->path, fileInfoDetail2->path) > 0) {
						fileInfoDetails->swap(i, j);
					}
				}
			}
		}

        return fileInfoDetails;
    }

    pointerVector<FileInfoDetail*>* fileInfoDetails = new pointerVector<FileInfoDetail*>(true);

    WIN32_FIND_DATAA findData;

    char* searchPath = combinePath(path, "*");
    HANDLE findHandle = FindFirstFileA(searchPath, &findData);
    free(searchPath);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return fileInfoDetails;
    }

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }
        char* currentPath = combinePath(path, findData.cFileName);

        FileInfoDetail* fileInfoDetail = fileGetFileInfoDetail(currentPath);
        free(currentPath);

        if (fileInfoDetail == NULL) {
            FindClose(findHandle);
            delete (fileInfoDetails);
            return NULL;
        }
        if (fileInfoDetail->isFile) {
            fileInfoDetail->size = findData.nFileSizeLow;
        }
        fileInfoDetails->add(fileInfoDetail);
    } while (FindNextFile(findHandle, &findData));
    FindClose(findHandle);

	if (fileInfoDetails->count() > 1) {
		for (uint32_t i = 0; i < fileInfoDetails->count() - 1; i++) {
			for (uint32_t j = i + 1; j < fileInfoDetails->count(); j++) {
				FileInfoDetail* fileInfoDetail1 = fileInfoDetails->get(i);
				FileInfoDetail* fileInfoDetail2 = fileInfoDetails->get(j);
				if (stricmp(fileInfoDetail1->path, fileInfoDetail2->path) > 0) {
					fileInfoDetails->swap(i, j);
				}
			}
		}
	}

    return fileInfoDetails;
}

bool fileSystem::fileOpen(const char* path, FileMode const fileMode, FileContainerType const containerType, uint32_t& fileHandle) {
    char* sdPath = resolveSdCardPath(path);
    if (sdPath != NULL) {
        bool result = sdCardOpenFile(sdPath, fileMode, fileHandle);
        free(sdPath);
        return result;
    }

    return openHostFile(path, fileMode, containerType, fileHandle);
}

bool fileSystem::fileRead(uint32_t fileHandle, char* readBuffer, uint32_t const bytesToRead, uint32_t& bytesRead) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        return false;
    }
    if (fileContainer->type == SdCard) {
        return sdCardReadBytes((SdCardFileState*)fileContainer->file, readBuffer, bytesToRead, bytesRead);
    }
    bytesRead = (uint32_t)fread(readBuffer, 1, bytesToRead, (FILE*)fileContainer->file);
    return true;
}

bool fileSystem::fileWrite(uint32_t fileHandle, char* writeBuffer, uint32_t bytesToWrite, uint32_t& bytesWritten) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        bytesWritten = 0;
        return false;
    }
    if (fileContainer->type == SdCard) {
        return sdCardWriteBytes((SdCardFileState*)fileContainer->file, writeBuffer, bytesToWrite, bytesWritten);
    }
    bytesWritten = (uint32_t)fwrite(writeBuffer, 1, bytesToWrite, (FILE*)fileContainer->file);
    return bytesWritten == bytesToWrite;
}

bool fileSystem::fileWrite(const char* path, char* writeBuffer, uint32_t bytesToWrite, uint32_t& bytesWritten) {
    uint32_t fileHandle;
    if (fileOpen(path, FileModeWrite, FileSystem, fileHandle) == false) {
        return false;
    }
    bool result = true;
    if (fileWrite(fileHandle, writeBuffer, bytesToWrite, bytesWritten) == false) {
        result = false;
    }
    return fileClose(fileHandle) && result;
}

bool fileSystem::fileClose(uint32_t fileHandle) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        return false;
    }
    bool result = true;
    if (fileContainer->type == SdCard) {
        sdCardReleaseState((SdCardFileState*)fileContainer->file);
        fileContainer->file = NULL;
    } else {
        result = fclose((FILE*)fileContainer->file) == 0;
    }
    deleteFileContainer(fileHandle);
    return result;
}

bool fileSystem::fileSeek(uint32_t fileHandle, FileSeekMode const fileSeekMode, uint32_t const offset) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        return false;
    }
    if (fileContainer->type == SdCard) {
        SdCardFileState* state = (SdCardFileState*)fileContainer->file;
        uint32_t newPosition = state->position;
        if (fileSeekMode == FileSeekModeStart) {
            newPosition = offset;
        } else if (fileSeekMode == FileSeekModeEnd) {
            newPosition = state->size + offset;
        } else {
            newPosition = state->position + offset;
        }
        if (newPosition > state->size) {
            if (state->fileMode == FileModeRead) {
                return false;
            }
        }
        state->position = newPosition;
        return true;
    }

    int seek = SEEK_CUR;
    if (fileSeekMode == FileSeekModeStart) {
        seek = SEEK_SET;
    } else if (fileSeekMode == FileSeekModeEnd) {
        seek = SEEK_END;
    }
    FILE* file = (FILE*)fileContainer->file;
    fseek(file, offset, seek);
    return true;
}

bool fileSystem::filePosition(uint32_t fileHandle, uint32_t& position) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        return false;
    }
    if (fileContainer->type == SdCard) {
        position = ((SdCardFileState*)fileContainer->file)->position;
        return true;
    }
    FILE* file = (FILE*)fileContainer->file;
    position = ftell(file);
    return true;
}

bool fileSystem::fileTruncate(uint32_t fileHandle, uint32_t& position) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        return false;
    }
    if (fileContainer->type == SdCard) {
        return false;
    }
    FILE* file = (FILE*)fileContainer->file;
    return _chsize(fileno(file), position) == 0;
}

bool fileSystem::setFileTime(const char* path, const FileTime fileTime) {
    if (isSdCardPrefixedPath(path)) {
        return false;
    }

    SYSTEMTIME systemTime;
    systemTime.wYear = fileTime.year;
    systemTime.wMonth = fileTime.month;
    systemTime.wDay = fileTime.day;
    systemTime.wDayOfWeek = 0;
    systemTime.wHour = fileTime.hour;
    systemTime.wMinute = fileTime.minute;
    systemTime.wSecond = fileTime.second;
    systemTime.wMilliseconds = 0;

    FILETIME tempFileTime;
    SystemTimeToFileTime(&systemTime, &tempFileTime);

    HANDLE fileHandle = CreateFile(path, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    ::SetFileTime(fileHandle, NULL, NULL, &tempFileTime);
    CloseHandle(fileHandle);
    return true;
}

bool fileSystem::directoryCreate(const char* path) {
    char* sdPath = resolveSdCardPath(path);
    if (sdPath != NULL) {
        sdcard::open();
        bool result = sdcard::create_directory(sdPath);
        sdcard::close();
        free(sdPath);
        return result;
    }

    bool exists;
    if (directoryExists(path, exists) == false) {
        return false;
    }
    if (exists) {
        return true;
    }
    return CreateDirectoryA(path, NULL) == TRUE;
}

bool fileSystem::directoryDelete(const char* path, bool const recursive) {
    char* sdPath = resolveSdCardPath(path);
    if (sdPath != NULL) {
        if (recursive) {
            pointerVector<FileInfoDetail*>* fileInfoDetails = fileGetFileInfoDetails(path);
            if (fileInfoDetails == NULL) {
                free(sdPath);
                return false;
            }

            for (uint32_t i = 0; i < fileInfoDetails->count(); i++) {
                FileInfoDetail* fileInfoDetail = fileInfoDetails->get(i);
                if (fileInfoDetail->isDirectory) {
                    if (directoryDelete(fileInfoDetail->path, true) == false) {
                        delete (fileInfoDetails);
                        free(sdPath);
                        return false;
                    }
                } else {
                    if (fileDelete(fileInfoDetail->path) == false) {
                        delete (fileInfoDetails);
                        free(sdPath);
                        return false;
                    }
                }
            }

            delete (fileInfoDetails);
        }

        bool result = sdCardDeletePath(sdPath);
        free(sdPath);
        return result;
    }

    pointerVector<FileInfoDetail*>* fileInfoDetails = fileGetFileInfoDetails(path);
    if (fileInfoDetails == NULL) {
        return false;
    }

    for (uint32_t i = 0; i < fileInfoDetails->count(); i++) {
        FileInfoDetail* fileInfoDetail = fileInfoDetails->get(i);
        if (fileInfoDetail->isDirectory) {
            const char* directoryToDelete = fileInfoDetail->path;
            if (recursive && directoryDelete(directoryToDelete, true) == false) {
                delete (fileInfoDetails);
                return false;
            }
        } else {
            const char* fileToDelete = fileInfoDetail->path;
            if (fileDelete(fileToDelete) == false) {
                delete (fileInfoDetails);
                return false;
            }
        }
    }

    delete (fileInfoDetails);
    return RemoveDirectoryA(path) == TRUE;
}

bool fileSystem::fileDelete(const char* path) {
    char* sdPath = resolveSdCardPath(path);
    if (sdPath != NULL) {
        bool result = sdCardDeletePath(sdPath);
        free(sdPath);
        return result;
    }
    return remove(path) == 0;
}

bool fileSystem::fileCopy(const char* sourcePath, const char* destPath) {
    uint32_t sourceFileHandle;
    if (fileOpen(sourcePath, FileModeRead, FileSystem, sourceFileHandle) == false) {
        return false;
    }

    uint32_t destFileHandle;
    if (fileOpen(destPath, FileModeWrite, FileSystem, destFileHandle) == false) {
        fileClose(sourceFileHandle);
        return false;
    }

    char* buffer = (char*)malloc(32768);

    uint32_t bytesRead;
    if (fileRead(sourceFileHandle, buffer, 32768, bytesRead) == false) {
        free(buffer);
        fileClose(sourceFileHandle);
        fileClose(destFileHandle);
        return false;
    }

    while (bytesRead > 0) {
        uint32_t bytesWritten;
        if (fileWrite(destFileHandle, buffer, bytesRead, bytesWritten) == false || bytesWritten != bytesRead) {
            free(buffer);
            fileClose(sourceFileHandle);
            fileClose(destFileHandle);
            return false;
        }
        if (fileRead(sourceFileHandle, buffer, 32768, bytesRead) == false) {
            free(buffer);
            fileClose(sourceFileHandle);
            fileClose(destFileHandle);
            return false;
        }
    }

    free(buffer);

    fileClose(sourceFileHandle);
    fileClose(destFileHandle);
    return true;
}

bool fileSystem::fileMove(const char* sourcePath, const char* destPath) {
    char* sourceSdPath = resolveSdCardPath(sourcePath);
    char* destSdPath = resolveSdCardPath(destPath);
    if (sourceSdPath != NULL || destSdPath != NULL) {
        if (sourceSdPath == NULL || destSdPath == NULL) {
            free(sourceSdPath);
            free(destSdPath);
            return false;
        }
        bool result = sdCardRenamePath(sourceSdPath, destSdPath);
        free(sourceSdPath);
        free(destSdPath);
        return result;
    }
    BOOL result = MoveFileA(sourcePath, destPath);
    return result == TRUE;
}

bool fileSystem::fileSize(uint32_t fileHandle, uint32_t& size) {
    FileContainer* fileContainer = getFileContainer(fileHandle);
    if (fileContainer == NULL) {
        return false;
    }
    if (fileContainer->type == SdCard) {
        size = ((SdCardFileState*)fileContainer->file)->size;
        return true;
    }
    FILE* file = (FILE*)fileContainer->file;
    uint32_t filePosition = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, filePosition, SEEK_SET);
    return true;
}

bool fileSystem::fileExists(const char* path, bool& exists) {
    FileInfoDetail* fileInfoDetail = fileGetFileInfoDetail(path);
    if (fileInfoDetail == NULL) {
        exists = false;
        return true;
    }
    exists = fileInfoDetail->isFile;
    delete (fileInfoDetail);
    return true;
}

bool fileSystem::directoryExists(const char* path, bool& exists) {
    FileInfoDetail* fileInfoDetail = fileGetFileInfoDetail(path);
    if (fileInfoDetail == NULL) {
        exists = false;
        return true;
    }
    exists = fileInfoDetail->isDirectory;
    delete (fileInfoDetail);
    return true;
}

char* fileSystem::getFileName(const char* path) {
    uint32_t length = 0;
    for (int i = (uint32_t)strlen(path); i > 0; i--) {
        if (path[i - 1] == '\\') {
            break;
        }
        length++;
    }

    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, path + strlen(path) - length, length);
    result[length] = 0;
    return result;
}

char* fileSystem::getRootPath(const char* path) {
    if (!path) {
        return strdup("");
    }

    if (path[0] == '\\') {
        return strdup(path);
    }

    uint32_t length = 0;
    size_t pathLen = strlen(path);

    for (int i = 0; i < (int)pathLen; i++) {
        if (path[i] == ':') {
            break;
        }
        length++;
    }

    char* result = (char*)malloc(pathLen + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, path + length + 1, pathLen - length - 1);
    result[pathLen - length - 1] = 0;
    return result;
}

char* fileSystem::getDriveLetter(const char* path) {
    uint32_t length = 0;

    if (!path || path[0] == '\\') {
        return strdup("");
    }

    for (int i = 0; i < (int)strlen(path); i++) {
        if (path[i] == ':') {
            break;
        }
        length++;
    }

    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, path, length);
    result[length] = 0;
    return result;
}

char* fileSystem::getFileNameWithoutExtension(const char* path) {
    char* fileName = getFileName(path);
    char* extension = getExtension(fileName);
    char* result = stringUtility::substr(fileName, 0, strlen(fileName) - strlen(extension));
    free(extension);
    free(fileName);
    return result;
}

char* fileSystem::getExtension(const char* path) {
    uint32_t length = 0;
    for (int i = (uint32_t)strlen(path); i > 0; i--) {
        if (path[i - 1] == '.') {
            length++;
            break;
        }
        if (path[i - 1] == '\\' || path[i - 1] == '.') {
            break;
        }
        length++;
    }

    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, path + strlen(path) - length, length);
    result[length] = 0;
    return result;
}

char* fileSystem::getDirectory(const char* path) {
    int32_t length = 0;
    for (int i = (uint32_t)strlen(path); i > 0; i--) {
        if (path[i - 1] == '\\') {
            break;
        }
        length++;
    }

    length = length == 0 ? 0 : strlen(path) - length - 1;
    if (length < 0) {
        return strdup("");
    }

    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, path, length);
    result[length] = 0;
    return result;
}

char* fileSystem::combinePath(const char* first, const char* second) {
    if (strlen(first) == 0) {
        return strdup(second);
    }

    if (strlen(second) == 0) {
        return strdup(first);
    }

    char* trimmedFirst = stringUtility::rightTrim(first, '\\');
    char* trimmedSecond = stringUtility::leftTrim(second, '\\');

    uint32_t length = strlen(trimmedFirst) + 1 + strlen(trimmedSecond) + 1;
    char* result = (char*)malloc(length);
    if (result == NULL) {
        free(trimmedFirst);
        free(trimmedSecond);
        return NULL;
    }

    strcpy(result, trimmedFirst);
    strcpy(result + strlen(trimmedFirst), "\\");
    strcpy(result + strlen(trimmedFirst) + 1, trimmedSecond);
    result[length - 1] = 0;
    
    free(trimmedFirst);
    free(trimmedSecond);
    return result;
}