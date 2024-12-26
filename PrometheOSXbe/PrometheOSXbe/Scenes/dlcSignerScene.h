#pragma once

#include "scene.h"

#include "..\pointerVector.h"
#include "..\xboxInternals.h"

struct SearchResult {
	DWORD TitleId;
	DWORD FileCount;
	pointerVector<PCHAR>* Files;
};

class dlcSignerScene : public scene
{
public:
	dlcSignerScene();
	~dlcSignerScene();
	void update();
	void render();
private:
	uint32_t mCurrentSceneId;
	pointerVector<SearchResult*>* mSearchResults;
	DWORD mFilesFound;
	BOOL mOnlySignIfInvalid;
	DWORD mWarningSimpleContentFiles;
	DWORD mNumFilesSigned;
	DWORD mNumFilesFailed;
	DWORD mNumFilesSkipped;
	
	void signContent(pointerVector<SearchResult*>* SearchResults);
	BOOL searchContentMetaFiles(DWORD &TotalFiles, pointerVector<SearchResult*>* Results);
	VOID freeSearchResults(pointerVector<SearchResult*>* Results);
	BOOL signContentMeta(PCHAR File, DWORD TitleId, BOOL OnlySignIfInvalid, BOOL &FailedToAccessFile, BOOL &BadFileSize, BOOL &BadHeaderSize, BOOL &WasInvalidMagic, BOOL &WasSkippedBecauseValid, BOOL &WasSimpleContentMeta); 
	BOOL calculateContentHeaderSignature(DWORD TitleId, PBYTE buffer, DWORD length, PBYTE result); 
	DWORD getAlternateTitleId();
	VOID setAlternateTitleId(DWORD AlternateTitleId);
	BOOL checkXBEHeader();
	BOOL memoryPageWritable(LPVOID Address);
	BOOL ValidHexString(CHAR* string, DWORD length);
	VOID CopyHexToBytes(CHAR* string, DWORD length, BYTE* buffer, BOOL reverse);
};
