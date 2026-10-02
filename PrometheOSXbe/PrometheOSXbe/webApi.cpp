#include "webApi.h"

#include "Threads\driveMounter.h"
#include "context.h"
#include "cpu_gpu.h"
#include "driveManager.h"
#include "fileSystem.h"
#include "network.h"
#include "rtcManager.h"
#include "settingsManager.h"
#include "stringUtility.h"
#include "temperatureManager.h"
#include "theme.h"
#include "utils.h"
#include "xboxConfig.h"
#include "xboxinternals.h"
#include "XKUtils\XKEEPROM.h"

namespace {

bool gDriveMountThreadActive = false;

void finishDriveMountThreadIfNeeded() {
    if (gDriveMountThreadActive && driveMounter::completed()) {
        driveMounter::closeThread();
        gDriveMountThreadActive = false;
    }
}

char* getPrimaryDriveModel() {
    const char* model = HalDiskModelNumber->Buffer ? HalDiskModelNumber->Buffer : "";
    char* diskModelNumberString = stringUtility::formatString("%s", model);
    char* diskModelNumber = stringUtility::trim(diskModelNumberString, ' ');
    free(diskModelNumberString);
    return diskModelNumber;
}

char* getPrimaryDriveSerial() {
    const char* serial = HalDiskSerialNumber->Buffer ? HalDiskSerialNumber->Buffer : "";
    char* diskSerialNumberString = stringUtility::formatString("%s", serial);
    char* diskSerialNumber = stringUtility::trim(diskSerialNumberString, ' ');
    free(diskSerialNumberString);
    return diskSerialNumber;
}

bool jsonNeedsComma(const char* json) {
    if (json == NULL || json[0] == '\0') {
        return false;
    }

    size_t length = strlen(json);
    return length > 1 && json[length - 1] != '{' && json[length - 1] != '[';
}

char* appendJsonString(char* json, const char* fragment) {
    char* result = stringUtility::formatString("%s%s", json, fragment);
    free(json);
    return result;
}

char* appendJsonEscapedValue(char* json, const char* key, const char* value) {
    char* escaped = webApi::jsonEscape(value == NULL ? "" : value);
    char* fragment =
        stringUtility::formatString("%s\"%s\":\"%s\"", jsonNeedsComma(json) ? "," : "", key, escaped);
    free(escaped);
    return appendJsonString(json, fragment);
}

// The Xbox _vsnprintf mishandles the "ll" length modifier (%lld/%llu): it consumes only 32 bits of
// the value and misaligns the following varargs, so 64-bit numbers -- e.g. partition sizes over 4GB --
// came out corrupted (the low 32 bits as the value, and the high 32 bits leaking into the next field).
// Format 64-bit integers by hand and splice them in with %s instead.
char* formatUInt64String(uint64_t value) {
    char buffer[21]; // up to 20 digits for a 64-bit value, plus NUL
    int index = (int)sizeof(buffer) - 1;
    buffer[index] = '\0';
    do {
        buffer[--index] = (char)('0' + (int)(value % 10));
        value /= 10;
    } while (value != 0 && index > 0);
    return strdup(&buffer[index]);
}

char* appendJsonNumber(char* json, const char* key, int64_t value) {
    // Avoid %lld (see formatUInt64String); build the digits by hand. Negate via unsigned to stay
    // safe at INT64_MIN.
    uint64_t magnitude = value < 0 ? (uint64_t)(-(value + 1)) + 1 : (uint64_t)value;
    char* digits = formatUInt64String(magnitude);
    char* fragment = stringUtility::formatString(
        "%s\"%s\":%s%s", jsonNeedsComma(json) ? "," : "", key, value < 0 ? "-" : "", digits);
    free(digits);
    return appendJsonString(json, fragment);
}

char* appendJsonBool(char* json, const char* key, bool value) {
    char* fragment = stringUtility::formatString(
        "%s\"%s\":%s", jsonNeedsComma(json) ? "," : "", key, value ? "true" : "false");
    return appendJsonString(json, fragment);
}

char* getAutoBootLabel() {
    pointerVector<bankDetails*>* banks = settingsManager::getBankInfos(true);
    char* label = NULL;
    for (uint32_t i = 0; i < banks->count(); i++) {
        bankDetails* bank = banks->get(i);
        if (bank->autoBoot == true) {
            label = strdup(bank->name);
            break;
        }
    }
    delete (banks);
    return label;
}

} // namespace

char* webApi::urlDecode(const char* query) {
    if (query == NULL || query[0] == '\0') {
        return strdup("");
    }

    size_t length = strlen(query);
    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    size_t writeIndex = 0;
    for (size_t i = 0; i < length; i++) {
        if (query[i] == '%' && i + 2 < length) {
            char hex[3] = { query[i + 1], query[i + 2], '\0' };
            result[writeIndex++] = (char)strtol(hex, NULL, 16);
            i += 2;
        } else if (query[i] == '+') {
            result[writeIndex++] = ' ';
        } else {
            result[writeIndex++] = query[i];
        }
    }
    result[writeIndex] = '\0';
    return result;
}

char* webApi::jsonEscape(const char* value) {
    if (value == NULL) {
        return strdup("");
    }

    size_t extra = 0;
    for (const char* cursor = value; *cursor != '\0'; cursor++) {
        if (*cursor == '"' || *cursor == '\\' || *cursor == '\n' || *cursor == '\r' || *cursor == '\t') {
            extra++;
        }
    }

    char* result = (char*)malloc(strlen(value) + extra + 1);
    if (result == NULL) {
        return strdup("");
    }

    size_t writeIndex = 0;
    for (const char* cursor = value; *cursor != '\0'; cursor++) {
        switch (*cursor) {
            case '"':
                result[writeIndex++] = '\\';
                result[writeIndex++] = '"';
                break;
            case '\\':
                result[writeIndex++] = '\\';
                result[writeIndex++] = '\\';
                break;
            case '\n':
                result[writeIndex++] = '\\';
                result[writeIndex++] = 'n';
                break;
            case '\r':
                result[writeIndex++] = '\\';
                result[writeIndex++] = 'r';
                break;
            case '\t':
                result[writeIndex++] = '\\';
                result[writeIndex++] = 't';
                break;
            default:
                result[writeIndex++] = *cursor;
                break;
        }
    }
    result[writeIndex] = '\0';
    return result;
}

bool webApi::isDriveRootPath(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return true;
    }

    if (_strnicmp(path, "SDCARD:", 7) == 0) {
        for (size_t i = 7; path[i] != '\0'; i++) {
            if (path[i] != '\\') {
                return false;
            }
        }
        return true;
    }

    size_t length = strlen(path);
    if (length > 0 && path[length - 1] == ':') {
        return true;
    }

    return false;
}

bool webApi::isAllowedPath(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return true;
    }

    if (strstr(path, "..") != NULL) {
        return false;
    }

    if (_strnicmp(path, "SDCARD:", 7) == 0) {
        return true;
    }

    if (driveManager::isAllMounted() == false) {
        return false;
    }

    pointerVector<char*>* drives = driveManager::listMountedDriveLetters();
    bool allowed = false;
    for (uint32_t i = 0; i < drives->count(); i++) {
        char* mountPoint = drives->get(i);
        if (stringUtility::startsWith(path, mountPoint, true)) {
            allowed = true;
        }
    }
    delete (drives);
    return allowed;
}

char* webApi::getStatusJson() {
    temperatureManager::refresh();
    cpugpu::poll();

    semver version = settingsManager::getVersion();
    char* versionString = settingsManager::getVersionSting(version);
    char* modchipName = context::getModchip()->getName();
    char* currentIp = context::getCurrentIp();
    char* skinName = settingsManager::getSkinName();
    char* autoBootLabel = getAutoBootLabel();
    char* avResString = xboxConfig::getAvResString();

    char* json = strdup("{");
    json = appendJsonEscapedValue(json, "ip", currentIp);
    json = appendJsonEscapedValue(json, "version", versionString);
    json = appendJsonEscapedValue(json, "modchip", modchipName);
    json = appendJsonEscapedValue(json, "skin", skinName);
    json = appendJsonEscapedValue(json, "avRes", avResString);
    json = appendJsonNumber(json, "cpuTemp", context::getCurrentCpuTemp());
    json = appendJsonNumber(json, "mbTemp", context::getCurrentMbTemp());
    json = appendJsonNumber(json, "fanSpeed", context::getCurrentFanSpeed());
    json = appendJsonNumber(json, "freeMemKb", context::getCurrentFreeMem() >> 10);
    json = appendJsonNumber(json, "cpuMhz", (int64_t)cpugpu::getCpuMHz());
    json = appendJsonNumber(json, "gpuMhz", (int64_t)cpugpu::getGpuMHz());
    json = appendJsonBool(json, "online", network::isReady() == true);

    if (autoBootLabel != NULL) {
        json = appendJsonEscapedValue(json, "autoBoot", autoBootLabel);
    } else {
        json = appendJsonString(json, ",\"autoBoot\":null");
    }

    json = appendJsonString(json, "}");
    free(versionString);
    free(modchipName);
    free(currentIp);
    free(skinName);
    free(autoBootLabel);
    free(avResString);
    return json;
}

char* webApi::getDateTimeJson() {
    bool hasXRtc = xboxConfig::getHasXRtc();
    rtcDateTime dt = hasXRtc ? rtcManager::getDateTime() : rtcManager::getXboxDateTime();

    char* json = strdup("{");
    // "target" is what a save will write, so the page can state it. X-RTC when the chip is present
    // (it's the authoritative clock the console syncs from at boot), otherwise the system clock.
    json = appendJsonEscapedValue(json, "target", hasXRtc ? "xrtc" : "system");
    json = appendJsonNumber(json, "year", 2000 + dt.year);
    json = appendJsonNumber(json, "month", dt.month);
    json = appendJsonNumber(json, "day", dt.day);
    json = appendJsonNumber(json, "hour", dt.hour);
    json = appendJsonNumber(json, "min", dt.min);
    json = appendJsonNumber(json, "sec", dt.sec);
    json = appendJsonString(json, "}");
    return json;
}

bool webApi::setDateTime(int year, int month, int day, int hour, int minute, int second) {
    if (year < 2000 || year > 2099 || month < 1 || month > 12 || day < 1 || day > 31 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
        return false;
    }

    rtcDateTime dt;
    dt.year = (uint8_t)(year - 2000);
    dt.month = (uint8_t)month;
    dt.day = (uint8_t)day;
    dt.hour = (uint8_t)hour;
    dt.min = (uint8_t)minute;
    dt.sec = (uint8_t)second;
    dt.dotw = 0; // unused: the RTC / system-time conversions recompute the day-of-week

    if (xboxConfig::getHasXRtc()) {
        rtcManager::setDateTime(dt);     // write the external RTC (persists; the console syncs from it)
        rtcManager::setXboxDateTime(dt); // also update the running system clock so it takes effect now
    } else {
        rtcManager::setXboxDateTime(dt); // no X-RTC -- just set the system clock
    }
    return true;
}

char* webApi::getSystemInfoJson() {
    temperatureManager::refresh();
    cpugpu::poll();
    xboxConfig::init();

    char* json = strdup("{");

    char* jsonConsole = strdup("{");
    jsonConsole = appendJsonNumber(jsonConsole, "cpuMhz", (int64_t)cpugpu::getCpuMHz());
    jsonConsole = appendJsonNumber(jsonConsole, "gpuMhz", (int64_t)cpugpu::getGpuMHz());
    jsonConsole = appendJsonNumber(jsonConsole, "fsbMhz", (int64_t)cpugpu::getFsbMHz());
    jsonConsole = appendJsonNumber(jsonConsole, "memMhz", (int64_t)cpugpu::getMemMHz());
    jsonConsole = appendJsonNumber(jsonConsole, "ramMb", utils::getTotalPhysicalMemory() >> 20);
    char* xboxVersionString = xboxConfig::getXboxVersionString();
    jsonConsole = appendJsonEscapedValue(jsonConsole, "xboxRev", xboxVersionString);
    free(xboxVersionString);
    char* serialString = xboxConfig::getSerialString();
    jsonConsole = appendJsonEscapedValue(jsonConsole, "serial", serialString);
    free(serialString);
    char* macString = xboxConfig::getMacString();
    jsonConsole = appendJsonEscapedValue(jsonConsole, "mac", macString);
    free(macString);
    jsonConsole = appendJsonString(jsonConsole, "}");
    char* wrappedConsole = stringUtility::formatString("\"console\":%s", jsonConsole);
    json = appendJsonString(json, wrappedConsole);
    free(jsonConsole);
    free(wrappedConsole);

    char* jsonStorage = strdup("{");
    char* primaryModel = getPrimaryDriveModel();
    char* primarySerial = getPrimaryDriveSerial();
    char* secondaryModel = context::getSecondaryModel();
    char* secondarySerial = context::getSecondarySerial();
    jsonStorage = appendJsonEscapedValue(jsonStorage, "primaryModel", primaryModel);
    jsonStorage = appendJsonEscapedValue(jsonStorage, "primarySerial", primarySerial);
    jsonStorage = appendJsonEscapedValue(jsonStorage, "secondaryModel", secondaryModel);
    jsonStorage = appendJsonEscapedValue(jsonStorage, "secondarySerial", secondarySerial);
    free(primaryModel);
    free(primarySerial);
    free(secondaryModel);
    free(secondarySerial);

    jsonStorage = appendJsonString(jsonStorage, ",\"partitions\":[");
    static const char* mountPoints[] = {
        "HDD0-C", "HDD0-E", "HDD0-F", "HDD0-G", "HDD0-H", "HDD0-I",
        "HDD0-J", "HDD0-K", "HDD0-L", "HDD0-M", "HDD0-N",
        "HDD0-X", "HDD0-Y", "HDD0-Z",
        "HDD1-C", "HDD1-E", "HDD1-F", "HDD1-G", "HDD1-H", "HDD1-I",
        "HDD1-J", "HDD1-K", "HDD1-L", "HDD1-M", "HDD1-N",
        "HDD1-X", "HDD1-Y", "HDD1-Z",
    };
    static const char* partitionLabels[] = {
        "Drive0 C", "Drive0 E", "Drive0 F", "Drive0 G", "Drive0 H", "Drive0 I",
        "Drive0 J", "Drive0 K", "Drive0 L", "Drive0 M", "Drive0 N",
        "Drive0 X", "Drive0 Y", "Drive0 Z",
        "Drive1 C", "Drive1 E", "Drive1 F", "Drive1 G", "Drive1 H", "Drive1 I",
        "Drive1 J", "Drive1 K", "Drive1 L", "Drive1 M", "Drive1 N",
        "Drive1 X", "Drive1 Y", "Drive1 Z",
    };

    bool firstPartition = true;
    for (int pi = 0; pi < 28; pi++) {
        uint64_t total = 0;
        if (driveManager::getTotalNumberOfBytes(mountPoints[pi], total) && total > 0) {
            uint64_t freeBytes = 0;
            driveManager::getTotalFreeNumberOfBytes(mountPoints[pi], freeBytes);
            uint64_t used = freeBytes <= total ? total - freeBytes : 0;
            // %s with hand-formatted 64-bit values -- %llu is broken on the Xbox (see formatUInt64String).
            char* totalStr = formatUInt64String(total);
            char* usedStr = formatUInt64String(used);
            char* part = stringUtility::formatString(
                "%s{\"label\":\"%s\",\"mount\":\"%s\",\"total\":%s,\"used\":%s}",
                firstPartition ? "" : ",", partitionLabels[pi], mountPoints[pi], totalStr, usedStr);
            free(totalStr);
            free(usedStr);
            jsonStorage = appendJsonString(jsonStorage, part);
            free(part);
            firstPartition = false;
        }
    }
    jsonStorage = appendJsonString(jsonStorage, "]}");
    char* wrappedStorage = stringUtility::formatString(",\"storage\":%s", jsonStorage);
    json = appendJsonString(json, wrappedStorage);
    free(jsonStorage);
    free(wrappedStorage);

    char* jsonAudio = strdup("{");
    char* audioModeString = xboxConfig::getAudioModeString();
    jsonAudio = appendJsonEscapedValue(jsonAudio, "mode", audioModeString);
    free(audioModeString);
    jsonAudio = appendJsonBool(jsonAudio, "ac3", xboxConfig::getAudioAC3() == true);
    jsonAudio = appendJsonBool(jsonAudio, "dts", xboxConfig::getAudioDTS() == true);
    jsonAudio = appendJsonString(jsonAudio, "}");
    char* wrappedAudio = stringUtility::formatString(",\"audio\":%s", jsonAudio);
    json = appendJsonString(json, wrappedAudio);
    free(jsonAudio);
    free(wrappedAudio);

    char* jsonVideo = strdup("{");
    char* videoStandardString = xboxConfig::getVideoStandardString();
    jsonVideo = appendJsonEscapedValue(jsonVideo, "standard", videoStandardString);
    free(videoStandardString);
    char* gameRegionString = xboxConfig::getGameRegionString();
    jsonVideo = appendJsonEscapedValue(jsonVideo, "gameRegion", gameRegionString);
    free(gameRegionString);
    char* dvdRegionString = xboxConfig::getDvdRegionString();
    jsonVideo = appendJsonEscapedValue(jsonVideo, "dvdRegion", dvdRegionString);
    free(dvdRegionString);
    char* avPackString = xboxConfig::getAvPackString();
    jsonVideo = appendJsonEscapedValue(jsonVideo, "avPack", avPackString);
    free(avPackString);
    char* avResString = xboxConfig::getAvResString();
    jsonVideo = appendJsonEscapedValue(jsonVideo, "avRes", avResString);
    free(avResString);
    char* encoderString = xboxConfig::getEncoderString();
    jsonVideo = appendJsonEscapedValue(jsonVideo, "encoder", encoderString);
    free(encoderString);
    jsonVideo = appendJsonString(jsonVideo, "}");
    char* wrappedVideo = stringUtility::formatString(",\"video\":%s", jsonVideo);
    json = appendJsonString(json, wrappedVideo);
    free(jsonVideo);
    free(wrappedVideo);

    char* jsonAbout = strdup("{");
    semver version = settingsManager::getVersion();
    char* versionString = settingsManager::getVersionSting(version);
    jsonAbout = appendJsonEscapedValue(jsonAbout, "version", versionString);
    free(versionString);
    char* modchipName = context::getModchip()->getName();
    jsonAbout = appendJsonEscapedValue(jsonAbout, "build", modchipName);
    free(modchipName);
    char* skinAuthor = theme::getSkinAuthor();
    jsonAbout = appendJsonEscapedValue(jsonAbout, "skinAuthor", skinAuthor);
    free(skinAuthor);
    char* currentIp = context::getCurrentIp();
    jsonAbout = appendJsonEscapedValue(jsonAbout, "ip", currentIp);
    free(currentIp);
    jsonAbout = appendJsonString(jsonAbout, "}");
    char* wrappedAbout = stringUtility::formatString(",\"about\":%s", jsonAbout);
    json = appendJsonString(json, wrappedAbout);
    free(jsonAbout);
    free(wrappedAbout);

    json = appendJsonString(json, "}");
    return json;
}

void webApi::requestDriveMount() {
    if (driveManager::isAllMounted()) {
        finishDriveMountThreadIfNeeded();
        return;
    }

    finishDriveMountThreadIfNeeded();

    if (gDriveMountThreadActive) {
        return;
    }

    if (driveMounter::startThread(false)) {
        gDriveMountThreadActive = true;
    }
}

bool webApi::isDriveMountReady() {
    if (driveManager::isAllMounted()) {
        finishDriveMountThreadIfNeeded();
        return true;
    }

    if (gDriveMountThreadActive && driveMounter::completed()) {
        finishDriveMountThreadIfNeeded();
        return driveManager::isAllMounted();
    }

    return false;
}

char* webApi::getFilesJson(const char* path) {
    if (driveManager::isAllMounted() == false) {
        return strdup("{\"path\":\"\",\"entries\":[]}");
    }

    char* json = strdup("{\"path\":\"");
    char* escapedPath = jsonEscape(path == NULL || path[0] == '\0' ? "" : path);
    json = appendJsonString(json, escapedPath);
    free(escapedPath);
    json = appendJsonString(json, "\",\"entries\":[");

    if (path == NULL || path[0] == '\0') {
        pointerVector<char*>* drives = driveManager::listMountedDriveLetters();
        for (uint32_t i = 0; i < drives->count(); i++) {
            char* mountPoint = drives->get(i);
            char* openPath = stringUtility::formatString("%s:", mountPoint);
            char* alias = driveManager::convertMountPointToMountPointAlias(openPath);
            char* displayName = alias != NULL ? alias : mountPoint;
            char* entryPath = openPath;
            if (_strnicmp(mountPoint, "SDCARD", 6) == 0) {
                free(openPath);
                entryPath = strdup("SDCARD:\\");
            }
            char* escapedName = jsonEscape(displayName);
            char* escapedOpenPath = jsonEscape(entryPath);
            char* entry = stringUtility::formatString(
                "%s{\"name\":\"%s\",\"path\":\"%s\",\"dir\":true,\"size\":0}",
                i == 0 ? "" : ",", escapedName, escapedOpenPath);
            json = appendJsonString(json, entry);
            free(entry);
            free(escapedName);
            free(escapedOpenPath);
            if (entryPath != openPath) {
                free(entryPath);
            } else {
                free(openPath);
            }
            free(alias);
        }
        delete (drives);
    } else if (isAllowedPath(path) == true) {
        char* normalizedPath = strdup(path);
        size_t normalizedLength = strlen(normalizedPath);
        if (normalizedLength > 0 && normalizedPath[normalizedLength - 1] != '\\') {
            char* temp = stringUtility::formatString("%s\\", normalizedPath);
            free(normalizedPath);
            normalizedPath = temp;
        }

        pointerVector<fileSystem::FileInfoDetail*>* entries = fileSystem::fileGetFileInfoDetails(normalizedPath);
        if (entries != NULL) {
            for (uint32_t i = 0; i < entries->count(); i++) {
                fileSystem::FileInfoDetail* entry = entries->get(i);
                if (entry->path == NULL || stringUtility::equals(entry->path, ".", true)
                    || stringUtility::equals(entry->path, "..", true)) {
                    continue;
                }

                char* childPath = strdup(entry->path);
                char* displayName = fileSystem::getFileName(entry->path);
                char* escapedName = jsonEscape(displayName == NULL ? "" : displayName);
                char* escapedChildPath = jsonEscape(childPath == NULL ? "" : childPath);
                char* item = stringUtility::formatString(
                    "%s{\"name\":\"%s\",\"path\":\"%s\",\"dir\":%s,\"size\":%lu}",
                    json[strlen(json) - 1] == '[' ? "" : ",", escapedName, escapedChildPath,
                    entry->isDirectory ? "true" : "false", entry->size);
                json = appendJsonString(json, item);
                free(item);
                free(displayName);
                free(escapedName);
                free(escapedChildPath);
                free(childPath);
            }
            delete (entries);
        }
        free(normalizedPath);
    }

    json = appendJsonString(json, "]}");
    return json;
}

utils::dataContainer* webApi::readFileForDownload(const char* path) {
    if (isAllowedPath(path) == false) {
        return NULL;
    }

    uint32_t fileHandle = 0;
    if (fileSystem::fileOpen(path, fileSystem::FileModeRead, fileSystem::FileSystem, fileHandle) == false) {
        return NULL;
    }

    uint32_t fileSize = 0;
    if (fileSystem::fileSize(fileHandle, fileSize) == false || fileSize == 0) {
        fileSystem::fileClose(fileHandle);
        return NULL;
    }

    utils::dataContainer* body = new utils::dataContainer(fileSize);
    uint32_t bytesRead = 0;
    bool ok = fileSystem::fileRead(fileHandle, body->data, fileSize, bytesRead);
    fileSystem::fileClose(fileHandle);
    if (ok == false || bytesRead != fileSize) {
        delete (body);
        return NULL;
    }

    return body;
}

bool webApi::restoreEepromFromData(const char* data, uint32_t size) {
    if (data == NULL || size < EEPROM_SIZE) {
        return false;
    }

    const char* tempPath = "HDD0-E:\\PrometheOS\\eeprom_upload.bin";
    fileSystem::directoryCreate("HDD0-E:\\PrometheOS");

    uint32_t bytesWritten = 0;
    if (fileSystem::fileWrite(tempPath, (char*)data, EEPROM_SIZE, bytesWritten) == false
        || bytesWritten != EEPROM_SIZE) {
        return false;
    }

    XKEEPROM* eeprom = new XKEEPROM();
    bool ok = eeprom->ReadFromBINFile(tempPath, true);
    if (ok == true) {
        eeprom->WriteToXBOX();
    }
    delete (eeprom);
    fileSystem::fileDelete(tempPath);
    return ok;
}

bool webApi::parseLaunchBankQuery(const char* query, int& bankId, bool& safeMode) {
    bankId = 0;
    safeMode = false;
    if (query == NULL || query[0] == '\0') {
        return false;
    }

    pointerVector<char*>* parts = stringUtility::split(query, ",", false);
    if (parts->count() == 0) {
        delete (parts);
        return false;
    }

    bankId = stringUtility::toInt(parts->get(0));
    if (parts->count() > 1) {
        safeMode = stringUtility::toInt(parts->get(1)) != 0;
    }
    delete (parts);
    return true;
}

bool webApi::parseSafeModeFlag(const char* query) {
    return query != NULL && stringUtility::toInt(query) != 0;
}
