#pragma once

#include "utils.h"
#include "pointerVector.h"

class webApi {
  public:
    static char* urlDecode(const char* query);
    static char* jsonEscape(const char* value);
    static char* getStatusJson();
    static char* getDateTimeJson();
    static bool setDateTime(int year, int month, int day, int hour, int minute, int second);
    static char* getSystemInfoJson();
    static char* getFilesJson(const char* path);
    static void requestDriveMount();
    static bool isDriveMountReady();
    static bool isAllowedPath(const char* path);
    static bool isDriveRootPath(const char* path);
    static utils::dataContainer* readFileForDownload(const char* path);
    static bool restoreEepromFromData(const char* data, uint32_t size);
    static bool parseLaunchBankQuery(const char* query, int& bankId, bool& safeMode);
    static bool parseSafeModeFlag(const char* query);
};
