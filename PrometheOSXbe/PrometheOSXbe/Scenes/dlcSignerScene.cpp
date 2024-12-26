#include "dlcSignerScene.h"
#include "sceneManager.h"
#include "filePickerScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\driveManager.h"
#include "..\fileSystem.h"
#include "..\theme.h"
#include "..\Threads\driveMounter.h"

#define XBEBaseAddress	0x00010000
#define XBEMagic		0x58424548
#define XCSFMagic		0x46534358
#define SHA_CONTEXT_SIZE 116
#define SHA_DIGEST_SIZE 20

dlcSignerScene::dlcSignerScene()
{
	driveMounter::startThread(false);

	mCurrentSceneId = 0;
	mSearchResults = new pointerVector<SearchResult*>(true);
	mFilesFound = 0;
}

dlcSignerScene::~dlcSignerScene()
{
	freeSearchResults(mSearchResults);
	delete(mSearchResults);
}

void dlcSignerScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (driveMounter::completed() == true)
		{
			driveMounter::closeThread();
			mCurrentSceneId = 1;
			return;
		}
		return;
	}
	
	if (mCurrentSceneId == 1)
	{
		BOOL searchSucceeded = searchContentMetaFiles(mFilesFound, mSearchResults);
		mCurrentSceneId = searchSucceeded ? 2 : 5;
		return;
	}

	if (mCurrentSceneId == 2)
	{
		if (inputManager::buttonPressed(ButtonA))
		{
			mOnlySignIfInvalid = FALSE;
			mCurrentSceneId = 3;
			signContent(mSearchResults);
			return;
		}

		if (inputManager::buttonPressed(ButtonY))
		{
			mOnlySignIfInvalid = TRUE;
			mCurrentSceneId = 3;
			signContent(mSearchResults);
			return;
		}
	}

	if (mCurrentSceneId == 3)
	{
		mCurrentSceneId = 4;
	}

	// Back Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}
}

void dlcSignerScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "DLC / Update Signer...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);


	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 40, yPos);

	yPos += 40;
	
	bool showContinue = false;
	if (mCurrentSceneId == 0)
	{
		component::textBox("Mounting Drives...", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}
	else if (mCurrentSceneId == 1)
	{
		component::textBox("Scanning...", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}
	else if (mCurrentSceneId == 2)
	{
		if (mSearchResults->count() > 0) 
		{
			char* progress = stringUtility::formatString("Found %d folders containing a total of %d content packages.", mSearchResults->count(), mFilesFound);
			component::textBox(progress, false, false, horizAlignmentCenter, 40, yPos, 640, 44);
			free(progress);
		}
		else
		{
			component::textBox("Search completed. No content packages found", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
		}
	}
	else if (mCurrentSceneId == 3)
	{
		component::textBox("Signing...", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}
	else if (mCurrentSceneId == 4)
	{
		char* progress = stringUtility::formatString("Signing complete. %d files signed, %d failed, %d skipped", mNumFilesSigned, mNumFilesFailed, mNumFilesSkipped);
		component::textBox(progress, false, false, horizAlignmentCenter, 40, yPos, 640, 44);
		free(progress);
	}
	else if (mCurrentSceneId == 5)
	{
		component::textBox("Failed", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}
	
	if (mCurrentSceneId == 2 && mSearchResults->count() > 0)
	{
		drawing::drawBitmapString(context::getBitmapFontSmall(),  "\xC2\xA1 Sign All, \xC2\xA4 Sign Only Invalid", theme::getFooterTextColor(), 40, theme::getFooterY());
	}
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);

}

void dlcSignerScene::signContent(pointerVector<SearchResult*>* SearchResults)
{
	BOOL Error_FileAccess = FALSE;
	BOOL Error_BadFileSize = FALSE;	
	BOOL Error_BadHeaderSize = FALSE;
	BOOL Error_BadMagic = FALSE;

	BOOL Warn_SkippedBecauseValid = FALSE;
	BOOL Warn_IsSimpleContent = FALSE;

	mNumFilesSigned = 0;
	mNumFilesFailed = 0;
	mNumFilesSkipped = 0;
	mWarningSimpleContentFiles = 0;

	const PCHAR TitleDataRegion = "T:\\"; //symlink of TitleId specific TDATA folder
	const PCHAR UserDataRegion =  "U:\\"; //symlink of TitleId specific UDATA folder

	for(DWORD SearchResultIndex = 0; SearchResultIndex < SearchResults->count(); SearchResultIndex++) {

		SearchResult* CurrentSearchResult = SearchResults->get(SearchResultIndex);
		if (CurrentSearchResult->FileCount == 0)
			continue;

		setAlternateTitleId(CurrentSearchResult->TitleId);

		CHAR AlternateTitleDriveLetter = { 0 };
		XMountAlternateTitle(TitleDataRegion, CurrentSearchResult->TitleId, &AlternateTitleDriveLetter);

		for(DWORD FileIndex = 0; FileIndex < CurrentSearchResult->Files->count(); FileIndex++) {

			PCHAR CurrentFile = CurrentSearchResult->Files->get(FileIndex);

			if (!signContentMeta(CurrentFile, CurrentSearchResult->TitleId, mOnlySignIfInvalid,
				Error_FileAccess, Error_BadFileSize, Error_BadHeaderSize, Error_BadMagic, Warn_SkippedBecauseValid, Warn_IsSimpleContent))
			{
				if (Error_FileAccess || Error_BadFileSize || Error_BadHeaderSize || Error_BadMagic)
					mNumFilesFailed++;

				if (Warn_SkippedBecauseValid)
					mNumFilesSkipped++;

			}
			else 
			{

				mNumFilesSigned++;

				if (Warn_IsSimpleContent && (strlen(CurrentFile) > 0)) 
				{
					mWarningSimpleContentFiles++;
				}

			}

		}

		XUnmountAlternateTitle(AlternateTitleDriveLetter);
	}
}

BOOL dlcSignerScene::searchContentMetaFiles(DWORD &TotalFiles, pointerVector<SearchResult*>* Results) 
{
	/*
	1. Mount E:\ partition
	2. Search E:\TDATA\ for valid TitleId folders and build list of TitleId folders
	3. Loop all TitleId's, overwrite AlternateTitleId in XBE header memory with current TitleId from loop, and mount AlternateTitle drive W:\
	4. Search W:\ for $C and $U folders
	5. If W:\$C exists, search W:\$C for valid OfferId folders and build list of OfferId folders
	6. Grab ContentMeta.xbx from all OfferId folders in W:\$C if exists
	7. Grab ContentMeta.xbx from W:\$U if exists
	*/

	const PCHAR TitleDataRegion = "T:\\"; //symlink of TitleId specific TDATA folder
	const PCHAR UserDataRegion =  "U:\\"; //symlink of TitleId specific UDATA folder

	WIN32_FIND_DATA SearchContext;
	HANDLE SearchHandle;

	PCHAR SearchCriteria = (PCHAR)malloc(MAX_PATH);
	ZeroMemory(SearchCriteria, MAX_PATH);

	sprintf(SearchCriteria, "HDD0-E:\\TDATA\\*");

	if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) == INVALID_HANDLE_VALUE) {
		/*if (ConsoleIsInitialized())
		{		
			ConsoleWriteLine(L"     ERROR: Failed to access E:\\ or E:\\TDATA is empty");
		}*/
		free(SearchCriteria);
		return FALSE;
	}

	BYTE* TitleIds = (BYTE*)malloc((2048 * 4)); // there's ~1000 games for the Xbox (+ breathing room + homebrew titles)
	ZeroMemory(TitleIds, (2048 * 4));
	DWORD CurrentTitleId = 0;
	DWORD TitleIdCount = 0;

	if ((strlen(SearchContext.cFileName) == 8) && 
		ValidHexString(SearchContext.cFileName, 8) && 
		((SearchContext.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)) 
	{

		CopyHexToBytes(SearchContext.cFileName, 8, (BYTE*)&CurrentTitleId, TRUE); // convert hex to uint32, reverse to little endian

		if (CurrentTitleId != 0 && CurrentTitleId != 0xFFFFFFFF) {
			*(DWORD*)(TitleIds + (TitleIdCount * 4)) = CurrentTitleId; //store title id
			TitleIdCount++;
		}
	}

	while(FindNextFile(SearchHandle, &SearchContext)) {

		if ((strlen(SearchContext.cFileName) == 8) && 
			ValidHexString(SearchContext.cFileName, 8) &&
			((SearchContext.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
		{
			CopyHexToBytes(SearchContext.cFileName, 8, (BYTE*)&CurrentTitleId, TRUE); // convert hex to uint32, reverse to little endian

			if (CurrentTitleId != 0 && CurrentTitleId != 0xFFFFFFFF) {
				*(DWORD*)(TitleIds + (TitleIdCount * 4)) = CurrentTitleId; //store title id
				TitleIdCount++;
			}
		}
	}

	FindClose(SearchHandle);

	if (TitleIdCount == 0) {

		free(SearchCriteria);
		free(TitleIds);

		//if (ConsoleIsInitialized())
		//	ConsoleWriteLine(L"      ERROR: No valid content folders found");

		return FALSE;
	}

	//for some unicode formatting
	PWCHAR PrintBuffer = (PWCHAR)malloc((125 * sizeof(WCHAR)));

	CHAR AlternateTitleDriveLetter = { 0 };

	//Results = new pointerVector<SearchResult*>;

	for(DWORD i = 0; i < TitleIdCount; i++) {

		if (getAlternateTitleId() != *(DWORD*)(TitleIds + (i * 4))) {

			if (AlternateTitleDriveLetter != '\0') {
				XUnmountAlternateTitle(AlternateTitleDriveLetter);
				AlternateTitleDriveLetter = '\0';
			}

			if (*(DWORD*)(TitleIds + (i * 4)) == 0)
				continue;

			setAlternateTitleId(*(DWORD*)(TitleIds + (i * 4))); //write title id into XBE header to trick XAPI into giving access when asking to mount
			XMountAlternateTitle(TitleDataRegion, *(DWORD*)(TitleIds + (i * 4)), &AlternateTitleDriveLetter); //mount alternate title id, pass T:\\ to force mounting to W:\\ drive
		}

		BOOL ContentFolderExists = FALSE;
		BOOL UpdateFolderExists = FALSE;

		ZeroMemory(SearchCriteria, MAX_PATH);
		sprintf(SearchCriteria, "%c:\\$c", AlternateTitleDriveLetter);

		if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) != INVALID_HANDLE_VALUE) {

			ContentFolderExists = TRUE;
			FindClose(SearchHandle);
		}

		ZeroMemory(SearchCriteria, MAX_PATH);
		sprintf(SearchCriteria, "%c:\\$u", AlternateTitleDriveLetter);

		if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) != INVALID_HANDLE_VALUE) {

			UpdateFolderExists = TRUE;
			FindClose(SearchHandle);
		}

		if (!ContentFolderExists && !UpdateFolderExists)
			continue;


		//begin file search

		SearchResult* Result = new SearchResult;
		Result->TitleId = *(DWORD*)(TitleIds + (i * 4));
		Result->FileCount = 0;
		Result->Files = new pointerVector<PCHAR>(false);

		if (ContentFolderExists) {

			ZeroMemory(SearchCriteria, MAX_PATH);
			sprintf(SearchCriteria, "%c:\\$c\\ContentMeta.xbx", AlternateTitleDriveLetter);

			//check if a ContentMeta file exists in the root of $C (not usually)
			if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) != INVALID_HANDLE_VALUE) {

				PCHAR File = (PCHAR)malloc(MAX_PATH);
				ZeroMemory(File, MAX_PATH);

				sprintf(File, "%c:\\$c\\ContentMeta.xbx", AlternateTitleDriveLetter);

				Result->Files->add(File);
				Result->FileCount++;

				FindClose(SearchHandle);
			}

			//check for offerid folders

			PCHAR OfferIds = (PCHAR)malloc(16 * 100); //allocate for 100 offerid folders (overkill)
			DWORD OfferIdsCount = 0;

			ZeroMemory(SearchCriteria, MAX_PATH);
			sprintf(SearchCriteria, "%c:\\$c\\*", AlternateTitleDriveLetter);

			if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) != INVALID_HANDLE_VALUE) {

				if ((strlen(SearchContext.cFileName) == 16) && 
					ValidHexString(SearchContext.cFileName, 16) &&
					((SearchContext.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)) 
				{

					sprintf((OfferIds + OfferIdsCount), SearchContext.cFileName);
					OfferIdsCount++;			
				}

				while(FindNextFile(SearchHandle, &SearchContext)) {

					if ((strlen(SearchContext.cFileName) == 16) && 
						ValidHexString(SearchContext.cFileName, 16) &&
						((SearchContext.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)) 

					{
						sprintf((OfferIds + (OfferIdsCount * 16)), SearchContext.cFileName);
						OfferIdsCount++;
					}

				}

				FindClose(SearchHandle);

			}

			//search for ContentMeta.xbx files within offerid folders

			if (OfferIdsCount > 0) {

				for(DWORD i = 0; i < OfferIdsCount; i++) {

					ZeroMemory(SearchCriteria, MAX_PATH);
					sprintf(SearchCriteria, "%c:\\$c\\%.16s\\ContentMeta.xbx", AlternateTitleDriveLetter, (OfferIds + (i * 16)));

					if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) != INVALID_HANDLE_VALUE) {

						PCHAR File = (PCHAR)malloc(MAX_PATH);
						ZeroMemory(File, MAX_PATH);

						sprintf(File, "%c:\\$c\\%.16s\\ContentMeta.xbx", AlternateTitleDriveLetter, (OfferIds + (i * 16)));

						Result->Files->add(File);
						Result->FileCount++;

						FindClose(SearchHandle);

					}

				}

			}

			free(OfferIds);

		}

		if (UpdateFolderExists) {

			ZeroMemory(SearchCriteria, MAX_PATH);
			sprintf(SearchCriteria, "%c:\\$u\\ContentMeta.xbx", AlternateTitleDriveLetter);

			if ((SearchHandle = FindFirstFile(SearchCriteria, &SearchContext)) != INVALID_HANDLE_VALUE) {

				PCHAR File = (PCHAR)malloc(MAX_PATH);
				ZeroMemory(File, MAX_PATH);

				sprintf(File, "%c:\\$u\\ContentMeta.xbx", AlternateTitleDriveLetter);

				Result->Files->add(File);
				Result->FileCount++;

				FindClose(SearchHandle);
			}

		}

		Results->add(Result);
		TotalFiles += Result->FileCount;
	}

	if (AlternateTitleDriveLetter != '\0')
	{
		XUnmountAlternateTitle(AlternateTitleDriveLetter);
	}

	free(SearchCriteria);
	free(TitleIds);
	free(PrintBuffer);

	return TRUE;
}

VOID dlcSignerScene::freeSearchResults(pointerVector<SearchResult*>* Results) {

	for(DWORD result_index = 0; result_index < Results->count(); result_index++) 
	{
		SearchResult* result = Results->get(result_index);
		if (result) 
		{
			if (result->Files) 
			{
				delete result->Files;
			}
		}		
	}
}

BOOL dlcSignerScene::signContentMeta(PCHAR File, DWORD TitleId, BOOL OnlySignIfInvalid, BOOL &FailedToAccessFile, BOOL &BadFileSize, BOOL &BadHeaderSize, BOOL &WasInvalidMagic, BOOL &WasSkippedBecauseValid, BOOL &WasSimpleContentMeta) 
{
	FailedToAccessFile = FALSE;
	BadFileSize = FALSE;
	WasInvalidMagic = FALSE;
	WasSkippedBecauseValid = FALSE;
	WasSimpleContentMeta = FALSE;

	if (!File)
	{
		return FALSE;
	}

	const DWORD MinimumHeaderSize = 0x6C;
	BOOL UseTitleIdFromMetadata = (TitleId == 0 || TitleId == 0xFFFFFFFF);

	HANDLE FileHandle = NULL;
	if ((FileHandle = CreateFile(File, GENERIC_ALL, GENERIC_ALL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE) {
		FailedToAccessFile = TRUE;
		return FALSE;
	}

	DWORD FileSize = GetFileSize(FileHandle, NULL);
	if (FileSize < MinimumHeaderSize)
	{
		BadFileSize = TRUE;
		CloseHandle(FileHandle);
		return FALSE;
	}

	PBYTE FileBuffer = (PBYTE)malloc(FileSize);

	DWORD BytesRead = 0;
	if (!ReadFile(FileHandle, FileBuffer, FileSize, &BytesRead, NULL) || BytesRead != FileSize) {

		FailedToAccessFile = TRUE;
		CloseHandle(FileHandle);
		free(FileBuffer);
		return FALSE;
	}

	if (*(DWORD*)(FileBuffer + 0x14) != XCSFMagic) {

		WasInvalidMagic = TRUE;
		CloseHandle(FileHandle);
		free(FileBuffer);
		return FALSE;
	}

	DWORD HeaderSize = *(DWORD*)(FileBuffer + 0x18);
	if (HeaderSize < MinimumHeaderSize) 
	{
		BadHeaderSize = TRUE;
		CloseHandle(FileHandle);
		free(FileBuffer);
		return FALSE;
	}

	DWORD InternalTitleId = 0;
	if (UseTitleIdFromMetadata)
	{
		InternalTitleId = *(DWORD*)(FileBuffer + 0x24);
	}

	PBYTE Signature = (PBYTE)malloc(SHA_DIGEST_SIZE);
	calculateContentHeaderSignature((UseTitleIdFromMetadata ? InternalTitleId : TitleId), (FileBuffer + 0x14), (HeaderSize - 0x14), Signature);

	BOOL SignaturesMatch = (memcmp(FileBuffer, Signature, SHA_DIGEST_SIZE) == 0);

	if (OnlySignIfInvalid && SignaturesMatch) 
	{
		WasSkippedBecauseValid = TRUE;
		CloseHandle(FileHandle);
		free(FileBuffer);
		free(Signature);
		return FALSE;
	}

	SetFilePointer(FileHandle, 0, NULL, FILE_BEGIN);

	DWORD BytesWritten = 0;
	if (!WriteFile(FileHandle, Signature, SHA_DIGEST_SIZE, &BytesWritten, NULL) || BytesWritten != SHA_DIGEST_SIZE) {

		FailedToAccessFile = TRUE;
		CloseHandle(FileHandle);
		free(FileBuffer);
		free(Signature);
		return FALSE;
	}

	FlushFileBuffers(FileHandle);
	CloseHandle(FileHandle);

	free(FileBuffer);
	free(Signature);

	if ((strstr(File, "$c") != NULL) || (strstr(File, "$C") != NULL)) 
	{
		if (HeaderSize == MinimumHeaderSize)
		{
			WasSimpleContentMeta = TRUE;
		}
	}

	return TRUE;
}

BOOL dlcSignerScene::calculateContentHeaderSignature(DWORD TitleId, PBYTE buffer, DWORD length, PBYTE result) 
{
	if (buffer && length > 0 && result) 
	{
		PBYTE ContentKey = (PBYTE)malloc(SHA_DIGEST_SIZE);
		//calc content key (HMAC SHA1 hash of TitleId bytes using XboxHDKey as the key)
		XcHMAC(&XboxHDKey[0], 0x10, (PBYTE)&TitleId, 4, NULL, NULL, ContentKey);
		//calc signature (HMAC SHA1 hash of supplied data using "content key" as the key)
		XcHMAC(ContentKey, SHA_DIGEST_SIZE, buffer, length, NULL, NULL, result);
		free(ContentKey);
		return TRUE;
	}
	return FALSE;
}

DWORD dlcSignerScene::getAlternateTitleId() 
{
	//NOTE: only using the first alternate title id, it's possible to use up to 16 total
	//NOTE(feudalnate): might be a good idea to cache the alternate title id's address instead of fetching every time

	DWORD CertificateAddress = *(DWORD*)(XBEBaseAddress + 0x118); //get certificate address from XBE header
	DWORD AlternateTitleIdsAddress = (CertificateAddress + 0x5C); //alternate titleids are located 0x5C from beginning of certificate (in all cert. revisions)

	return *(DWORD*)AlternateTitleIdsAddress;
}

VOID dlcSignerScene::setAlternateTitleId(DWORD AlternateTitleId) 
{
	DWORD CertificateAddress = *(DWORD*)(XBEBaseAddress + 0x118);
	DWORD AlternateTitleIdsAddress = (CertificateAddress + 0x5C);

	*(DWORD*)AlternateTitleIdsAddress = AlternateTitleId;
}

BOOL dlcSignerScene::checkXBEHeader() 
{
	if (memoryPageWritable((LPVOID)XBEBaseAddress))
	{
		if (*(DWORD*)XBEBaseAddress == XBEMagic)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL dlcSignerScene::memoryPageWritable(LPVOID Address) 
{

	/*
	WinNT memory page protection flags (one or more will _always_ be set)

	PAGE_NOACCESS			0x01
	PAGE_READONLY			0x02
	PAGE_READWRITE			0x04
	PAGE_WRITECOPY			0x08
	PAGE_EXECUTE			0x10
	PAGE_EXECUTE_READ		0x20
	PAGE_EXECUTE_READWRITE	0x40
	PAGE_EXECUTE_WRITECOPY	0x80
	PAGE_GUARD				0x100
	PAGE_NOCACHE			0x200
	PAGE_WRITECOMBINE		0x400
	*/

	DWORD Flags = XQueryMemoryProtect(Address);

	if ( (Flags == 0) || ((Flags & PAGE_GUARD) == PAGE_GUARD) || ((Flags & PAGE_NOACCESS) == PAGE_NOACCESS) )
	{
		return FALSE;
	}
	if ( ((Flags & PAGE_READWRITE) == PAGE_READWRITE) || ((Flags & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE) )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL dlcSignerScene::ValidHexString(CHAR* string, DWORD length) {

	//this could be done better/faster

	if (!string || length == 0 || (length % 2) != 0)
		return FALSE;

	const CHAR* valid_chars = "0123456789abcdefABCDEF";
	const DWORD valid_chars_len = 22;

	BOOL valid;
	CHAR c, v;
	for(DWORD i = 0; i < length; i++) {

		valid = FALSE;
		c = *(CHAR*)(string + i);

		for(DWORD x = 0; x < valid_chars_len; x++) {

			v = *(CHAR*)(valid_chars + x);

			if (c == v) {
				valid = TRUE;
				break;
			}

		}

		if (!valid)
			return FALSE;

	}

	return TRUE;
}

VOID dlcSignerScene::CopyHexToBytes(CHAR* string, DWORD length, BYTE* buffer, BOOL reverse) 
{

	if (string && length > 0 && (length % 2) == 0 && buffer) {

		CHAR c;
		BYTE b;

		DWORD buffer_index = 0;

		for(DWORD i = 0; i < length; i += 2) {

			//first nibble
			c = *(CHAR*)(string + i);
			
			if (c >= '0' && c <= '9')
				b = (BYTE)((c - '0') << 4);
			else if (c >= 'A' && c <= 'F')
				b = (BYTE)((c - 'A' + 10) << 4);
			else
				b = (BYTE)((c - 'a' + 10) << 4);

			//second nibble
			c = *(CHAR*)(string + (i + 1));
			
			if (c >= '0' && c <= '9')
				b = (BYTE)(b | (c - '0'));
			else if (c >= 'A' && c <= 'F')
				b = (BYTE)(b | (c - 'A' + 10));
			else
				b = (BYTE)(b | (c - 'a' + 10));

			*(BYTE*)(buffer + buffer_index) = b;
			buffer_index++;
		}

		if (reverse) {

			DWORD x = 0;
			DWORD y = (length / 2);

			while (x < y) {
				b = *(BYTE*)(buffer + x);
				*(BYTE*)(buffer + x) = *(BYTE*)(buffer + (y - 1));
				*(BYTE*)(buffer + (y - 1)) = b;
				x++;
				y--;
			}

		}

	}
}

