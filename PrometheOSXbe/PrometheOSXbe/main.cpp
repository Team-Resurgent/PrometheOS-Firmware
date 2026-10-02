#include "stdafx.h"
#include <xtl.h>
#include <string>
#include <conio.h>
#include "xboxinternals.h"

#include "expansion.h"
#include "xmvPlayer.h"
#include "crc32.h"
#include "harddrive.h"
#include "rtcManager.h"
#include "temperatureManager.h"
#include "context.h"
#include "drawing.h"
#include "sdcard.h"
#include "Scenes\scene.h"
#include "Scenes\mainScene.h"
#include "Scenes\autoBootScene.h"
#include "Scenes\flashFlowScene.h"
#include "Scenes\sceneManager.h"
#include "inputManager.h"
#include "settingsManager.h"
#include "xboxConfig.h"
#include "xboxinternals.h"
#include "meshUtility.h"
#include "utils.h"
#include "resources.h"
#include "stringUtility.h"
#include "settingsManager.h"
#include "json.h"
#include "theme.h"
#include "ssfn.h"
#include "ftpServer.h"
#include "httpServer.h"
#include "driveManager.h"
#include "XKUtils\XKEEPROM.h"
#include "timeUtility.h"
#include "network.h"
#include "audioPlayer.h"
#include "modchipXenium.h"
#include "modchipXecuter.h"
#include "modchipXchanger.h"
#include "modchipSmartxx.h"
#include "modchipModxo.h"
#include "modchipAladdin1mb.h"
#include "modchipAladdin2mb.h"
#include "modchipDummy.h"
#include "Threads\lcdRender.h"
#include "Threads\flashBackup.h"
#include "cerbiosIniHelper.h"
#include "webApi.h"
#include "xboxinternals.h"

#include <xgraphics.h>

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE)

enum apiActionEnum {
    apiActionNone = 0,
    apiActionLaunchBank = 1,
    apiActionLaunchTsop = 2,
    apiActionLaunchRecovery = 3,
    apiActionLaunchSdCard = 4,
    apiActionReboot = 5,
    apiActionShutdown = 6
};

namespace {
apiActionEnum mApiAction = apiActionNone;
int mBankId = 0;
bool mSafeMode = false;
int mCounter = 0;
} // namespace

utils::dataContainer* onGetCallback(const char* path, const char* query) {
    utils::dataContainer* body = NULL;

#ifdef TOOLS
    bool isTools = true;
#else
    bool isTools = false;
#endif

    if (stringUtility::equals(path, "\\", true)) {
        return httpServer::redirect();
    } else if (stringUtility::equals(path, "\\pico.css", true)) {
        body = new utils::dataContainer((char*)&pico_css, sizeof(pico_css), sizeof(pico_css));
    } else if (stringUtility::equals(path, "\\favicon.ico", true)) {
        body = new utils::dataContainer((char*)&favicon_ico, sizeof(favicon_ico), sizeof(favicon_ico));
    } else if (isTools == false && stringUtility::equals(path, "\\flash.html", true)) {
        body = new utils::dataContainer((char*)&flash_html, sizeof(flash_html), sizeof(flash_html));
    } else if (isTools == false && stringUtility::equals(path, "\\flash.js", true)) {
        body = new utils::dataContainer((char*)&flash_js, sizeof(flash_js), sizeof(flash_js));
    } else if (isTools == false && stringUtility::equals(path, "\\remove.html", true)) {
        body = new utils::dataContainer((char*)&remove_html, sizeof(remove_html), sizeof(remove_html));
    } else if (isTools == false && stringUtility::equals(path, "\\remove.js", true)) {
        body = new utils::dataContainer((char*)&remove_js, sizeof(remove_js), sizeof(remove_js));
    } else if (stringUtility::equals(path, "\\remoteview.html", true)) {
        body = new utils::dataContainer((char*)&remoteview_html, sizeof(remoteview_html), sizeof(remoteview_html));
    } else if (stringUtility::equals(path, "\\remoteview.js", true)) {
        body = new utils::dataContainer((char*)&remoteview_js, sizeof(remoteview_js), sizeof(remoteview_js));
    } else if (isTools == false && stringUtility::equals(path, "\\launch.html", true)) {
        body = new utils::dataContainer((char*)&launch_html, sizeof(launch_html), sizeof(launch_html));
    } else if (isTools == false && stringUtility::equals(path, "\\launch.js", true)) {
        body = new utils::dataContainer((char*)&launch_js, sizeof(launch_js), sizeof(launch_js));
    } else if (stringUtility::equals(path, "\\cerbiosini.html", true)) {
        body = new utils::dataContainer((char*)&cerbiosini_html, sizeof(cerbiosini_html), sizeof(cerbiosini_html));
    } else if (stringUtility::equals(path, "\\cerbiosini.js", true)) {
        body = new utils::dataContainer((char*)&cerbiosini_js, sizeof(cerbiosini_js), sizeof(cerbiosini_js));
    } else if (stringUtility::equals(path, "\\datetime.html", true)) {
        body = new utils::dataContainer((char*)&datetime_html, sizeof(datetime_html), sizeof(datetime_html));
    } else if (stringUtility::equals(path, "\\datetime.js", true)) {
        body = new utils::dataContainer((char*)&datetime_js, sizeof(datetime_js), sizeof(datetime_js));
    } else if (isTools == false && stringUtility::equals(path, "\\download.html", true)) {
        body = new utils::dataContainer((char*)&download_html, sizeof(download_html), sizeof(download_html));
    } else if (isTools == false && stringUtility::equals(path, "\\download.js", true)) {
        body = new utils::dataContainer((char*)&download_js, sizeof(download_js), sizeof(download_js));
    } else if (stringUtility::equals(path, "\\downloads.html", true)) {
#ifdef TOOLS
        body = new utils::dataContainer(
            (char*)&downloads_tools_html, sizeof(downloads_tools_html), sizeof(downloads_tools_html));
#else
        body = new utils::dataContainer((char*)&downloads_html, sizeof(downloads_html), sizeof(downloads_html));
#endif
    } else if (stringUtility::equals(path, "\\downloads.js", true)) {
#ifdef TOOLS
        body = new utils::dataContainer(
            (char*)&downloads_tools_js, sizeof(downloads_tools_js), sizeof(downloads_tools_js));
#else
        body = new utils::dataContainer((char*)&downloads_js, sizeof(downloads_js), sizeof(downloads_js));
#endif
    } else if (stringUtility::equals(path, "\\index.html", true)) {
#ifdef TOOLS
        body = new utils::dataContainer((char*)&index_tools_html, sizeof(index_tools_html), sizeof(index_tools_html));
#else
        body = new utils::dataContainer((char*)&index_html, sizeof(index_html), sizeof(index_html));
#endif
    } else if (stringUtility::equals(path, "\\index.js", true)) {
#ifdef TOOLS
        body = new utils::dataContainer((char*)&index_tools_js, sizeof(index_tools_js), sizeof(index_tools_js));
#else
        body = new utils::dataContainer((char*)&index_js, sizeof(index_js), sizeof(index_js));
#endif
    } else if (stringUtility::equals(path, "\\main.css", true)) {
        body = new utils::dataContainer((char*)&main_css, sizeof(main_css), sizeof(main_css));
    } else if (stringUtility::equals(path, "\\prom.js", true)) {
        body = new utils::dataContainer((char*)&prom_js, sizeof(prom_js), sizeof(prom_js));
    } else if (isTools == false && stringUtility::equals(path, "\\systeminfo.html", true)) {
        body = new utils::dataContainer((char*)&systeminfo_html, sizeof(systeminfo_html), sizeof(systeminfo_html));
    } else if (isTools == false && stringUtility::equals(path, "\\systeminfo.js", true)) {
        body = new utils::dataContainer((char*)&systeminfo_js, sizeof(systeminfo_js), sizeof(systeminfo_js));
    } else if (isTools == false && stringUtility::equals(path, "\\files.html", true)) {
        body = new utils::dataContainer((char*)&files_html, sizeof(files_html), sizeof(files_html));
    } else if (isTools == false && stringUtility::equals(path, "\\files.js", true)) {
        body = new utils::dataContainer((char*)&files_js, sizeof(files_js), sizeof(files_js));
    } else if (isTools == false && stringUtility::equals(path, "\\eeprom.html", true)) {
        body = new utils::dataContainer((char*)&eeprom_html, sizeof(eeprom_html), sizeof(eeprom_html));
    } else if (isTools == false && stringUtility::equals(path, "\\eeprom.js", true)) {
        body = new utils::dataContainer((char*)&eeprom_js, sizeof(eeprom_js), sizeof(eeprom_js));
    } else if (stringUtility::equals(path, "\\api\\status.json", true)) {
        char* statusJson = webApi::getStatusJson();
        body = new utils::dataContainer(statusJson, strlen(statusJson), strlen(statusJson));
        free(statusJson);
    } else if (stringUtility::equals(path, "\\api\\datetime.json", true)) {
        char* json = webApi::getDateTimeJson();
        body = new utils::dataContainer(json, strlen(json), strlen(json));
        free(json);
    } else if (isTools == false && stringUtility::equals(path, "\\api\\systeminfo.json", true)) {
        char* systemInfoJson = webApi::getSystemInfoJson();
        body = new utils::dataContainer(systemInfoJson, strlen(systemInfoJson), strlen(systemInfoJson));
        free(systemInfoJson);
    } else if (isTools == false && stringUtility::equals(path, "\\api\\mountdrives", true)) {
        webApi::requestDriveMount();
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\mountready.json", true)) {
        char* json = stringUtility::formatString("{\"ready\":%s}", webApi::isDriveMountReady() ? "true" : "false");
        body = new utils::dataContainer(json, strlen(json), strlen(json));
        free(json);
    } else if (isTools == false && stringUtility::equals(path, "\\api\\files.json", true)) {
        if (webApi::isDriveMountReady() == false) {
            return httpServer::generateResponse(503, "Drives not mounted.");
        }
        char* decodedPath = webApi::urlDecode(query);
        char* filesJson = webApi::getFilesJson(decodedPath);
        body = new utils::dataContainer(filesJson, strlen(filesJson), strlen(filesJson));
        free(filesJson);
        free(decodedPath);
    } else if (isTools == false && stringUtility::equals(path, "\\api\\downloadfile", true)) {
        char* decodedPath = webApi::urlDecode(query);
        body = webApi::readFileForDownload(decodedPath);
        free(decodedPath);
        if (body == NULL) {
            return httpServer::generateResponse(404, "File not found.");
        }
    } else if (isTools == false && stringUtility::equals(path, "\\api\\deletefile", true)) {
        char* decodedPath = webApi::urlDecode(query);
        if (webApi::isAllowedPath(decodedPath) == false) {
            free(decodedPath);
            return httpServer::generateResponse(403, "Path not allowed.");
        }

        if (webApi::isDriveRootPath(decodedPath) == true) {
            free(decodedPath);
            return httpServer::generateResponse(403, "Cannot delete drive root.");
        }

        bool exists = false;
        bool isDirectory = fileSystem::directoryExists(decodedPath, exists) == true && exists == true;
        if (isDirectory == false) {
            fileSystem::fileExists(decodedPath, exists);
            if (exists == false) {
                free(decodedPath);
                return httpServer::generateResponse(404, "File not found.");
            }
        }

        bool deleted = isDirectory ? fileSystem::directoryDelete(decodedPath, false)
                                   : fileSystem::fileDelete(decodedPath);
        free(decodedPath);
        if (deleted == false) {
            return httpServer::generateResponse(500, "Delete failed.");
        }
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\createdir", true)) {
        char* decodedPath = webApi::urlDecode(query);
        if (webApi::isAllowedPath(decodedPath) == false) {
            free(decodedPath);
            return httpServer::generateResponse(403, "Path not allowed.");
        }
        bool created = fileSystem::directoryCreate(decodedPath);
        free(decodedPath);
        if (created == false) {
            return httpServer::generateResponse(500, "Create failed.");
        }
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\freeslots.json", true)) {
        char* freeSlotsJson = settingsManager::getFreeSlotsJson();
        body = new utils::dataContainer(freeSlotsJson, strlen(freeSlotsJson), strlen(freeSlotsJson));
        free(freeSlotsJson);
    } else if (isTools == false && stringUtility::equals(path, "\\api\\bankinfo.json", true)) {
        char* bankInfosJson = settingsManager::getBankInfosJson();
        body = new utils::dataContainer(bankInfosJson, strlen(bankInfosJson), strlen(bankInfosJson));
        free(bankInfosJson);
    } else if (stringUtility::equals(path, "\\api\\shutdown", true)) {
        mApiAction = apiActionShutdown;
        return httpServer::generateResponse(200, "OK");
    } else if (stringUtility::equals(path, "\\api\\reboot", true)) {
        mApiAction = apiActionReboot;
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\launchbank", true)) {
        int id = 0;
        bool safeMode = false;
        if (webApi::parseLaunchBankQuery(query, id, safeMode) == false) {
            return httpServer::generateResponse(400, "Expected bank id.");
        }
        utils::debugPrint("Launching bank %i safeMode=%i", id, safeMode);
        mApiAction = apiActionLaunchBank;
        mBankId = id;
        mSafeMode = safeMode;
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\launchtsop", true)) {
        mSafeMode = webApi::parseSafeModeFlag(query);
        utils::debugPrint("Launching TSOP safeMode=%i", mSafeMode);
        mApiAction = apiActionLaunchTsop;
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\launchrecovery", true)) {
        mSafeMode = webApi::parseSafeModeFlag(query);
        utils::debugPrint("Launching Recovery safeMode=%i", mSafeMode);
        mApiAction = apiActionLaunchRecovery;
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\toggleautoboot", true)) {
        pointerVector<char*>* queryParts = stringUtility::split(query, ",", false);
        if (queryParts->count() != 2) {
            delete (queryParts);
            return httpServer::generateResponse(400, "Expected id,category query.");
        }

        const int id = stringUtility::toInt(queryParts->get(0));
        const int category = stringUtility::toInt(queryParts->get(1));
        delete (queryParts);

        if (category < bankCategoryFlash || category > bankCategoryRecovery) {
            return httpServer::generateResponse(400, "Invalid category.");
        }

        utils::debugPrint("Toggling auto boot for id %i category %i", id, category);
        settingsManager::toggleAutoBootBank((uint8_t)id, (bankCategory)category != bankCategoryFlash);
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\removebank", true)) {
        int id = stringUtility::toInt(query);
        utils::debugPrint("Removing bank %i", id);
        settingsManager::deleteBank(id);
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\downloadbank", true)) {
        int id = stringUtility::toInt(query);
        utils::debugPrint("Downloading bank %i", id);
        body = settingsManager::readBank(id);
    } else if (stringUtility::equals(path, "\\api\\downloadeeprom", true)) {
        XKEEPROM* eeprom = new XKEEPROM();
        eeprom->ReadFromXBOX();
        body = new utils::dataContainer(EEPROM_SIZE);
        eeprom->GetEEPROMData((LPEEPROMDATA)body->data);
        delete (eeprom);
    } else if (isTools == false && stringUtility::equals(path, "\\api\\downloadprom", true)) {
        body = context::getModchip()->readFlash(false);
    } else if (stringUtility::equals(path, "\\api\\cerbiosini", true)) {
        char* temp = (char*)malloc(65536);
        cerbiosConfig config = cerbiosIniHelper::loadConfig();
        cerbiosIniHelper::buildConfig(&config, temp);
        body = new utils::dataContainer(temp, strlen(temp), strlen(temp));
        free(temp);
    } else if (stringUtility::equals(path, "\\api\\takescreenshot", true)) {
        context::requestScreenshot();
        return httpServer::generateResponse(200, "OK");
    } else if (stringUtility::equals(path, "\\api\\screenshotready.json", true)) {
        char* json = stringUtility::formatString("{\"ready\":%s}", context::isScreenshotReady() ? "true" : "false");
        body = new utils::dataContainer(json, strlen(json), strlen(json));
        free(json);
    } else if (stringUtility::equals(path, "\\api\\downloadscreenshot", true)) {
        if (context::isScreenshotReady() == false) {
            return httpServer::generateResponse(404, "Screenshot not ready.");
        }
        body = context::getScreenshot();
    } else if (stringUtility::equals(path, "\\api\\takeremoteview", true)) {
        context::requestRemoteView();
        return httpServer::generateResponse(200, "OK");
    } else if (stringUtility::equals(path, "\\api\\remoteviewready.json", true)) {
        char* json = stringUtility::formatString("{\"ready\":%s}", context::isRemoteViewReady() ? "true" : "false");
        body = new utils::dataContainer(json, strlen(json), strlen(json));
        free(json);
    } else if (stringUtility::equals(path, "\\api\\downloadremoteview.jpg", true)) {
        if (context::isRemoteViewReady() == false) {
            return httpServer::generateResponse(404, "Remote view not ready.");
        }
        body = context::getRemoteView();
    } else if (stringUtility::equals(path, "\\api\\controller", true)) {
        int keyId = stringUtility::toInt(query);
        if (keyId >= JoystickButtonA && keyId <= JoystickButtonRightTrigger) {
            utils::debugPrint("Controller input %i", keyId);
            inputManager::injectButton((JoystickButton)keyId);
            return httpServer::generateResponse(200, "OK");
        }
        return httpServer::generateResponse(400, "Invalid controller key.");
    }

    if (body == NULL) {
        return httpServer::generateResponse(404, "Not Found");
    }

    char* extension = fileSystem::getExtension(path);
    char* mimeType = httpServer::getMimeType(extension);
    char* contentLength = stringUtility::formatString("%lu", body->size);
    char* header = stringUtility::formatString(
        "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nCache-Control: no-cache\r\nContent-Type: "
        "%s\r\nX-Content-Type-Options: nosniff\r\nContent-Length: %s\r\n\r\n",
        mimeType, contentLength);

    utils::debugPrint("Header len = %i\n", strlen(header));
    utils::debugPrint("Response = %s\n", header);

    utils::dataContainer* result = new utils::dataContainer(strlen(header) + body->size);
    memcpy(result->data, header, strlen(header));
    memcpy(result->data + strlen(header), body->data, body->size);

    delete (body);
    free(extension);
    free(mimeType);
    free(contentLength);
    free(header);

    return result;
}

utils::dataContainer* onPostCallback(const char* path, const char* query, pointerVector<FormPart*>* formParts) {
    utils::debugPrint("Post reciecved\n");

#ifdef TOOLS
    bool isTools = true;
#else
    bool isTools = false;
#endif

    if (stringUtility::equals(path, "\\api\\upload", true)) {
        if (formParts->count() != 2) {
            return httpServer::generateResponse(400, "Unexpected form parts.");
        }

        FormPart* formPart = formParts->get(0);

        if (context::getModchip()->isValidBankSize(formPart->body->size) == false) {
            return httpServer::generateResponse(406, "Invalid size detected.");
        }

        FormPart* jsonPart = formParts->get(1);

        char* bankName = NULL;
        uint8_t ledColor = 0;

        char* json = (char*)jsonPart->body->data;
        struct json_value_s* root = json_parse(json, strlen(json));
        struct json_object_s* object = (struct json_object_s*)root->payload;

        struct json_object_element_s* element = object->start;
        while (element != NULL) {
            if (stringUtility::equals(element->name->string, "ledColor", true)) {
                struct json_number_s* numberValue = json_value_as_number(element->value);
                ledColor = (uint8_t)stringUtility::toInt(numberValue->number);
            } else if (stringUtility::equals(element->name->string, "bankName", true)) {
                struct json_string_s* stringValue = json_value_as_string(element->value);
                bankName = strdup(stringValue->string);
            }
            element = element->next;
        }

        uint8_t slotsNeeded = (uint8_t)(formPart->body->size >> 18);
        settingsManager::optimizeBanks(slotsNeeded);

        bool flashSuccess = false;

        uint8_t bankId = 0;
        if (settingsManager::tryGetFreeBank(slotsNeeded, bankId)) {
            settingsManager::eraseBank(bankId, formPart->body->size);
            settingsManager::writeBank(bankId, formPart->body, bankName == NULL ? "Unknown" : bankName, ledColor);
            flashSuccess = settingsManager::verifyBank(bankId, formPart->body);
        }

        free(bankName);
        free(root);

        if (flashSuccess == true) {
            return httpServer::generateResponse(200, "OK");
        }

        return httpServer::generateResponse(406, "No enough free slots.");
    } else if (stringUtility::equals(path, "\\api\\datetime", true)) {
        if (formParts->count() < 1) {
            return httpServer::generateResponse(400, "Unexpected form parts.");
        }

        FormPart* jsonPart = formParts->get(0);
        char* json = (char*)jsonPart->body->data;
        struct json_value_s* root = json_parse(json, strlen(json));
        if (root == NULL) {
            return httpServer::generateResponse(400, "Invalid JSON.");
        }
        struct json_object_s* object = (struct json_object_s*)root->payload;

        int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        struct json_object_element_s* element = object->start;
        while (element != NULL) {
            struct json_number_s* numberValue = json_value_as_number(element->value);
            int value = numberValue != NULL ? (int)stringUtility::toInt(numberValue->number) : 0;
            if (stringUtility::equals(element->name->string, "year", true)) {
                year = value;
            } else if (stringUtility::equals(element->name->string, "month", true)) {
                month = value;
            } else if (stringUtility::equals(element->name->string, "day", true)) {
                day = value;
            } else if (stringUtility::equals(element->name->string, "hour", true)) {
                hour = value;
            } else if (stringUtility::equals(element->name->string, "min", true)) {
                minute = value;
            } else if (stringUtility::equals(element->name->string, "sec", true)) {
                second = value;
            }
            element = element->next;
        }
        free(root);

        if (webApi::setDateTime(year, month, day, hour, minute, second) == true) {
            return httpServer::generateResponse(200, "OK");
        }
        return httpServer::generateResponse(400, "Invalid date/time values.");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\rename", true)) {
        if (formParts->count() < 1) {
            return httpServer::generateResponse(400, "Unexpected form parts.");
        }

        FormPart* jsonPart = formParts->get(0);
        char* json = (char*)jsonPart->body->data;
        struct json_value_s* root = json_parse(json, strlen(json));
        if (root == NULL) {
            return httpServer::generateResponse(400, "Invalid JSON.");
        }
        struct json_object_s* object = (struct json_object_s*)root->payload;

        char* source = NULL;
        char* dest = NULL;
        struct json_object_element_s* element = object->start;
        while (element != NULL) {
            struct json_string_s* stringValue = json_value_as_string(element->value);
            const char* value = stringValue != NULL ? stringValue->string : "";
            if (stringUtility::equals(element->name->string, "source", true)) {
                source = strdup(value);
            } else if (stringUtility::equals(element->name->string, "dest", true)) {
                dest = strdup(value);
            }
            element = element->next;
        }

        bool ok = false;
        if (source != NULL && dest != NULL && webApi::isAllowedPath(source) == true &&
            webApi::isAllowedPath(dest) == true && webApi::isDriveRootPath(source) == false) {
            ok = fileSystem::fileMove(source, dest);
        }
        free(source);
        free(dest);
        free(root);

        if (ok == true) {
            return httpServer::generateResponse(200, "OK");
        }
        return httpServer::generateResponse(500, "Rename failed (target may already exist).");
    } else if (stringUtility::equals(path, "\\api\\cerbiosini", true)) {
        if (formParts->count() != 1) {
            return httpServer::generateResponse(400, "Unexpected form parts.");
        }

        FormPart* formPart = formParts->get(0);

        if (formPart->body->size > 65535) {
            return httpServer::generateResponse(406, "Invalid size detected.");
        }

        FormPart* bodyPart = formParts->get(0);
        char* body = (char*)bodyPart->body->data;
        cerbiosIniHelper::saveConfig(body);

        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\restoreeeprom", true)) {
        if (formParts->count() != 1) {
            return httpServer::generateResponse(400, "Expected EEPROM upload.");
        }

        FormPart* formPart = formParts->get(0);
        if (webApi::restoreEepromFromData(formPart->body->data, formPart->body->size) == false) {
            return httpServer::generateResponse(406, "Invalid EEPROM image.");
        }

        mApiAction = apiActionReboot;
        return httpServer::generateResponse(200, "OK");
    } else if (isTools == false && stringUtility::equals(path, "\\api\\uploadfile", true)) {
        if (formParts->count() != 1) {
            return httpServer::generateResponse(400, "Expected file upload.");
        }

        char* decodedPath = webApi::urlDecode(query);
        if (webApi::isAllowedPath(decodedPath) == false) {
            free(decodedPath);
            return httpServer::generateResponse(403, "Path not allowed.");
        }

        FormPart* formPart = formParts->get(0);
        uint32_t bytesWritten = 0;
        bool ok = fileSystem::fileWrite(decodedPath, formPart->body->data, formPart->body->size, bytesWritten)
            && bytesWritten == formPart->body->size;
        free(decodedPath);
        if (ok == false) {
            return httpServer::generateResponse(500, "Upload failed.");
        }
        return httpServer::generateResponse(200, "OK");
    }

    return httpServer::generateResponse(404, "Not Found");
}

void onResponseSentCallback() {
    if (mApiAction == apiActionShutdown) {
        utils::shutdown();
        Sleep(5000);
    } else if (mApiAction == apiActionReboot) {
        utils::reboot();
        Sleep(5000);
    } else if (mApiAction == apiActionLaunchBank) {
        settingsManager::launchBank(mBankId, mSafeMode);
        Sleep(5000);
    } else if (mApiAction == apiActionLaunchTsop) {
        settingsManager::launchTsop(mSafeMode);
        Sleep(5000);
    } else if (mApiAction == apiActionLaunchRecovery) {
        settingsManager::launchRecovery(mSafeMode);
        Sleep(5000);
    } else if (mApiAction == apiActionLaunchSdCard) {
        settingsManager::launchSdCard(mSafeMode);
        Sleep(5000);
    }

    mSafeMode = false;
}

typedef struct {
    DWORD dwWidth;
    DWORD dwHeight;
    bool  fProgressive;
    bool  fWideScreen;
    DWORD dwFreq;
    AvRes avRes;
} DISPLAY_MODE;

DISPLAY_MODE displayModes[] = {
    // HDTV Interlaced Modes
    //{  1920,   1080,    false,  true,  60, AVRES_1080I_60HZ },         // 1920x1080 interlaced 16x9

    // HDTV Progressive Modes
    {  1280,    720,    true,   true,  60, AVRES_720P_60HZ },         // 1280x720 progressive 16x9

    // EDTV Progressive Modes
    {   720,    480,    true,   true,  60, AVRES_480P_60HZ },         // 720x480 progressive 16x9
    {   640,    480,    true,   true,  60, AVRES_480P_60HZ },         // 640x480 progressive 16x9
    {   720,    480,    true,   false, 60, AVRES_480P_60HZ },         // 720x480 progressive 4x3
    {   640,    480,    true,   false, 60, AVRES_480P_60HZ },         // 640x480 progressive 4x3

    // SDTV PAL-50 Interlaced Modes
    {   720,    480,    false,  true,  50, AVRES_480I_50HZ },         // 720x480 interlaced 16x9 50Hz
    {   640,    480,    false,  true,  50, AVRES_480I_50HZ },         // 640x480 interlaced 16x9 50Hz
    {   720,    480,    false,  false, 50, AVRES_480I_50HZ },         // 720x480 interlaced 4x3  50Hz
    {   640,    480,    false,  false, 50, AVRES_480I_50HZ },         // 640x480 interlaced 4x3  50Hz

    // SDTV NTSC / PAL-60 Interlaced Modes
    {   720,    480,    false,  true,  60, AVRES_480I_60HZ },         // 720x480 interlaced 16x9
    {   640,    480,    false,  true,  60, AVRES_480I_60HZ },         // 640x480 interlaced 16x9
    {   720,    480,    false,  false, 60, AVRES_480I_60HZ },         // 720x480 interlaced 4x3
    {   640,    480,    false,  false, 60, AVRES_480I_60HZ },         // 640x480 interlaced 4x3
};

#define NUM_MODES (sizeof(displayModes) / sizeof(displayModes[0]))


bool SupportsMode( DISPLAY_MODE mode, DWORD dwVideoStandard, DWORD dwVideoFlags )
{
    if( mode.dwFreq == 60 && !(dwVideoFlags & XC_VIDEO_FLAGS_PAL_60Hz) && (dwVideoStandard == XC_VIDEO_STANDARD_PAL_I))
        return false;
    
    if( mode.dwFreq == 50 && (dwVideoStandard != XC_VIDEO_STANDARD_PAL_I))
        return false;

    // Need to check for widescreen on 480 modes only - 
    // 720p and 1080i are by definition widescreen.
    if( mode.dwHeight == 480 && mode.fWideScreen && !(dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN ) )
        return false;

    // Explicit check for 480p
    if( mode.dwHeight == 480 && mode.fProgressive && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_480p) )
        return false;

    // Explicit check for 720p (only 720 mode)
    if( mode.dwHeight == 720 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_720p) )
        return false;

    // Explicit check for 1080i (only 1080 mode)
    if( mode.dwHeight == 1080 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_1080i) )
        return false;

    return true;
}

bool createDevice() {
    uint32_t videoFlags = XGetVideoFlags();
    uint32_t videoStandard = XGetVideoStandard();

    uint32_t currentMode;
    for(currentMode = 0; currentMode < NUM_MODES - 1; currentMode++)
    {
        if (SupportsMode( displayModes[ currentMode ], videoStandard, videoFlags )) {
            break;
        }
    }

    context::setAvRes(displayModes[ currentMode ].avRes);

    LPDIRECT3D8 d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (d3d == NULL) {
        utils::debugPrint("Failed to create d3d\n");
        return false;
    }

    context::setBufferWidth(720);
    context::setBufferHeight(480);
    // context::setBufferWidth(displayModes[currentMode].dwWidth);
    // context::setBufferHeight(displayModes[currentMode].dwHeight);

    D3DPRESENT_PARAMETERS params;
    ZeroMemory(&params, sizeof(params));
    params.BackBufferWidth = displayModes[currentMode].dwWidth;
    params.BackBufferHeight = displayModes[currentMode].dwHeight;
    params.Flags = displayModes[currentMode].fProgressive ? D3DPRESENTFLAG_PROGRESSIVE : D3DPRESENTFLAG_INTERLACED;
    params.Flags |= displayModes[currentMode].fWideScreen ? D3DPRESENTFLAG_WIDESCREEN : 0;
    params.FullScreen_RefreshRateInHz = displayModes[currentMode].dwFreq;
    params.BackBufferFormat = D3DFMT_X8R8G8B8;
    params.BackBufferCount = 1;
    params.EnableAutoDepthStencil = FALSE;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    LPDIRECT3DDEVICE8 d3dDevice;
    if (FAILED(d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &d3dDevice))) {
        utils::debugPrint("Failed to create device\n");
        return false;
    }
    context::setD3dDevice(d3dDevice);

    D3DXMATRIX matProjection;
    D3DXMatrixOrthoOffCenterLH(
        &matProjection, 0, (float)context::getBufferWidth(), 0, (float)context::getBufferHeight(), 1.0f, 100.0f);
    context::getD3dDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

    D3DXMATRIX matView;
    D3DXMatrixIdentity(&matView);
    context::getD3dDevice()->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    context::getD3dDevice()->SetTransform(D3DTS_WORLD, &matWorld);

    context::getD3dDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
    context::getD3dDevice()->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    context::getD3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    context::getD3dDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    context::getD3dDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

    context::getD3dDevice()->BeginScene();
    context::getD3dDevice()->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);
    context::getD3dDevice()->EndScene();
    //context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);

    return true;
}

void refreshInfo() {
    if (network::isReady() == true) {
        XNADDR addr;
        memset(&addr, 0, sizeof(addr));
        DWORD dwState = XNetGetTitleXnAddr(&addr);
        if (dwState != XNET_GET_XNADDR_PENDING) {
            char* ipAddress = (XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE)
                ? stringUtility::formatString("%i.%i.%i.%i", addr.ina.S_un.S_un_b.s_b1, addr.ina.S_un.S_un_b.s_b2,
                    addr.ina.S_un.S_un_b.s_b3, addr.ina.S_un.S_un_b.s_b4)
                : strdup("0.0.0.0");
            char* currentIp = context::getCurrentIp();
            if (stringUtility::equals(ipAddress, currentIp, false) == false) {
                context::setCurrentIp(ipAddress);
                mCounter = 0;
            }
            free(ipAddress);
            free(currentIp);
        }
    }

    if (mCounter == 0) {
        context::setCurrentFreeMem(utils::getFreePhysicalMemory());
        context::setCurrentFanSpeed(temperatureManager::getFanSpeed() * 2);
        context::setCurrentCpuTemp(temperatureManager::getCpuTemp());
        context::setCurrentMbTemp(temperatureManager::getMbTemp());
        mCounter = 60;
    } else {
        mCounter--;
    }
}

void checkForSpecialButtons() {
    // Toggle PrometheOS VGA setting on/off
    if (inputManager::buttonDown(JoystickButtonLeftTrigger) && inputManager::buttonDown(JoystickButtonRightTrigger) &&
        inputManager::buttonDown(JoystickButtonWhite)) {
        settingsManager::setVgaEnable(!settingsManager::getVgaEnable());
        Sleep(200);
        HalReturnToFirmware(1); // BIOS reboot
    }

    // Toggle between NTSC-M and PAL-50
    if (inputManager::buttonDown(JoystickButtonLeftTrigger) && inputManager::buttonDown(JoystickButtonRightTrigger) &&
        inputManager::buttonDown(JoystickButtonBlack)) {
        if (xboxConfig::getVideoStandardNTSCM()) {
            xboxConfig::setVideoStandardPALI50();
        } else {
            xboxConfig::setVideoStandardNTSCM();
        }
        Sleep(200);
        HalReturnToFirmware(1); // BIOS reboot
    }

    // Disable all HDTV AV modes (Force480i)
    if (inputManager::buttonDown(JoystickButtonLeftTrigger) && inputManager::buttonDown(JoystickButtonRightTrigger) &&
        inputManager::buttonDown(JoystickButtonY)) {
        DWORD flags = xboxConfig::getVideoFlags();
        if((flags & EEPROM_VIDEO_HD_FLAGS_MASK) == 0) {
            xboxConfig::setVideo480p(true);
        } else {
            xboxConfig::setVideoFlags(flags & ~EEPROM_VIDEO_HD_FLAGS_MASK);
        }
        Sleep(200);
        HalReturnToFirmware(1); // BIOS reboot
    }
}

uint16_t crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

void __cdecl main() {
#ifndef TOOLS
    utils::debugPrint("Welcome to PrometheOS\n");
#else
    utils::debugPrint("Welcome to PrometheOS Tools\n");
#endif

//    #define XECUTER_LCD_BRIGHTNESS 0
//#define XECUTER_LCD_CONTRAST 1
//#define XECUTER_LCD_DAT 2  
//#define XECUTER_LCD_CMD 3 
//
//    while (true)
//    {
//        for (int i = 0; i < 255; i++)
//        {
//            uint8_t payload[2];
//            payload[0] = XECUTER_LCD_BRIGHTNESS;
//            payload[1] = 255;//(uint8_t)(backlight * 1.27f);
//            expansion::send_data(0x30, payload, 2);
//            Sleep(1);
//        }
//    }

    //uint8_t device_id = expansion::get_device_id(0x30);
    //char* device_name = expansion::get_device_name(0x30);
    //free(device_name);

//while (true)
//{
//    uint8_t payload[255];
//
//    memcpy(payload, "Hello   ", 8); 
//    expansion::send_data(0x30, payload, 8);
//    Sleep(1000);
//
//    memcpy(payload, "Harcroft", 8); 
//    expansion::send_data(0x30, payload, 8);
//    Sleep(1000);
//}

//for (int i = 0; i < 0x7f; i++)
//{
//    if (expansion::i2c_device_exists(i))
//    {
//        //expansion::send_data(i, "HI");
//    }
//}

//int qq =1;
//    //uint8_t ready = utils::ioInputByte(EXPANSION_COMMAND_GET_RESPONSE_READY);
////EXPANSION_COMMAND_GET_RESPONSE_I2C_ADDRESS_EXISTS
//
//    uint8_t test = 0;
    //uint8_t leds = 0;
    //while (true)
    //{
    //    utils::ioOutputByte(0xDEA2, leds);
    //    leds += 1;
    //    leds &= 3;
    //    Sleep(1000);
    //}

    //while (true)
    //{
    //    uint8_t bank_switch = utils::ioInputByte(0xDEA3);
    //    //uint8_t bank_switch = utils::ioInputByte(0xF501);
    //    utils::debugPrint("bank switch state = %s", (bank_switch & 128) == 128 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 64) == 64 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 32) == 32 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 16) == 16 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 8) == 8 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 4) == 4 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 2) == 2 ? "1 " : "0 ");
    //    utils::debugPrint("%s", (bank_switch & 1) == 1 ? "1 \n" : "0 \n");
    //    Sleep(3000);
    //}

#ifdef XENIUM
    context::setModchipType(modchipTypeXenium);
#elif XECUTER
    context::setModchipType(modchipTypeXecuter);
#elif XCHANGER
    context::setModchipType(modchipTypeXchanger);
#elif SMARTXXOPX
    context::setModchipType(modchipTypeSmartxxOpx);
#elif SMARTXX
    context::setModchipType(modchipTypeSmartxx);
#elif MODXO
    context::setModchipType(modchipTypeModxo);
#elif ALADDIN1MB
    context::setModchipType(modchipTypeAladdin1mb);
#elif ALADDIN2MB
    context::setModchipType(modchipTypeAladdin2mb);
#elif DUMMY
    context::setModchipType(modchipTypeDummy);
#elif TOOLS
    context::setModchipType(modchip::detectModchip());
#endif

    // XboxPartitionTable PartTbl;
    // XboxPartitionTable* KernelPartTblAdd;
    // DWORD g_dwUserSectors;
    // DWORD lba48PatchVersion;

    // if (harddrive::readLbaInfo(&PartTbl, &KernelPartTblAdd, &g_dwUserSectors, &lba48PatchVersion))
    //{
    // }

    // format example
    // status = XapiFormatFATVolumeEx(&partition_str[i], ClusterSize << 10);

    utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 |
        SMC_LED_STATES_GREEN_STATE3);

    bool deviceCreated = createDevice();

    temperatureManager::init();

    context::setNetworkInitialized(false);

    driveManager::mountDrive("HDD0-C");
    driveManager::mountDrive("HDD0-E");

    xboxConfig::init();
    xboxConfig::autoFix();
    settingsManager::loadSettings();

    if (settingsManager::getRtcEnable() == true && xboxConfig::getHasXRtc() == true) {
        rtcDateTime dateTime = rtcManager::getDateTime();
        rtcManager::setXboxDateTime(dateTime);
    }

    httpServer::registerOnGetCallback(onGetCallback);
    httpServer::registerOnPostCallback(onPostCallback);
    httpServer::registerOnResponseSentCallback(onResponseSentCallback);

    if (deviceCreated == false) {
        network::init();
        temperatureManager::refresh(true);
        while (true) {
            //temperatureManager::refresh();
            Sleep(100);
        }
    }

    xmvPlayer::init();

    context::setImageMap(new pointerMap<image*>(true));

    drawing::loadFont(&font_sfn[0]);

    //\xC2\xA1 = A
    //\xC2\xA2 = B
    //\xC2\xA3 = X
    //\xC2\xA4 = Y
    //\xC2\xA5 = Folder
    //\xC2\xA6 = File
    //\xC2\xA7 = P
    //\xC2\xA8 = R
    //\xC2\xA9 = O
    //\xC2\xAA = M
    //\xC2\xAB = E
    //\xC2\xAC = T
    //\xC2\xAD = H
    //\xC2\xAE = S
    //\xC2\xAF = Cursor On
    //\xC2\xB0 = Cursor Off
    //\xC2\xB1 = Deg
    //\xC2\xB2 = LT
    //\xC2\xB3 = RT
    //\xC2\xB4 = L
    //\xC2\xB5 = White
    //\xC2\xB6 = Black

    bitmapFont* fontExtraSmall = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 16, 16, 0, 256);
    context::setBitmapFontExtraSmall(fontExtraSmall);
    bitmapFont* fontSmall = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 18, 18, 0, 256);
    context::setBitmapFontSmall(fontSmall);
    bitmapFont* fontMedium = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 25, 25, 0, 256);
    context::setBitmapFontMedium(fontMedium);
    bitmapFont* fontLarge = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 32, 32, 0, 512);
    context::setBitmapFontLarge(fontLarge);

    drawing::renderRoundedRect("panel-fill", 24, 24, 6, 0xffffffff, 0x00000000, 0);
    drawing::renderRoundedRect("panel-stroke", 24, 24, 6, 0x01010100, 0xffffffff, 2);

    lcdRender::startThread();
    sdcard::init();

    sceneManager::pushScene(sceneItemMainScene);
    if (settingsManager::hasAutoBootBank() == true) {
        sceneManager::pushScene(sceneItemAutoBootScene);
    }

    char* skinName = settingsManager::getSkinName();
    theme::loadSkin(skinName);
    free(skinName);

    audioPlayer::init();
    char* soundPackName = settingsManager::getSoundPackName();
    theme::loadSoundPack(soundPackName);
    free(soundPackName);

    // PEParam_t* params;
    // params = (PEParam_t*)malloc(sizeof(PEParam_t));
    // memset(params, 0, sizeof(PEParam_t));

    // char* name = PEProcess::getPluginName("E:\\Root\\plugin.nxe");
    // uint32_t result = PEProcess::PE_Run("E:\\Root\\plugin.nxe", params);
    // free(name);

    temperatureManager::refresh(true);

    while (TRUE) {
        context::getD3dDevice()->BeginScene();

        //temperatureManager::refresh();
        inputManager::processController();
        checkForSpecialButtons();
        drawing::clearBackground();

        refreshInfo();

        sceneManager::getScene()->update();
        sceneManager::getScene()->render();

        context::processPendingScreenshot();
        context::processPendingRemoteView();

        context::getD3dDevice()->EndScene();
        context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);
    }
}
