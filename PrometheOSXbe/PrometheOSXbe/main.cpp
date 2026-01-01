#include "stdafx.h"
#include <xtl.h>
#include <string>

#include "harddrive.h"
#include "rtcManager.h"
#include "temperatureManager.h"
#include "context.h"
#include "drawing.h"
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
#include "hdmiSettingsManager.h"
#include "hdmiDevice.h"
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
#include "modchipXtremium.h"
#include "modchipXecuter.h"
#include "modchipXchanger.h"
#include "modchipSmartxx.h"
#include "modchipModxo.h"
#include "modchipAladdin1mb.h"
#include "modchipAladdin2mb.h"
#include "modchipDummy.h"
#include "Threads\lcdRender.h"
#include "Threads\flashBackup.h"
#include "Threads\hddFormat.h"
#include "Threads\hddInfo.h"
#include "Threads\hddLockUnlock.h"
#include "Plugins\PEProcess.h"
#include "cerbiosIniHelper.h"

#include "stb_image_write.h"

#include <xgraphics.h>

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

// Shouldn't be 0, something random maybe?
static int ExpectedNonce = 0;

enum apiActionEnum
{ 
	apiActionNone = 0,
	apiActionLaunchBank = 1,
	apiActionLaunchTsop = 2,
	apiActionReboot = 3,
	apiActionShutdown = 41
}; 

namespace
{
	apiActionEnum mApiAction = apiActionNone;
	int mBankId = 0;
	int mCounter = 0;
}

utils::dataContainer* onGetCallback(const char* path, const char* query)
{
	utils::dataContainer* body = NULL;

	if (stringUtility::equals(path, "\\", true))
	{
		return httpServer::redirect();
	}
	else if (stringUtility::equals(path, "\\pico.css", true))
	{
		body = new utils::dataContainer((char*)&pico_css, sizeof(pico_css), sizeof(pico_css));
	}
	else if (stringUtility::equals(path, "\\favicon.ico", true))
	{
		body = new utils::dataContainer((char*)&favicon_ico, sizeof(favicon_ico), sizeof(favicon_ico));
	}
	else if (stringUtility::equals(path, "\\flash.html", true))
	{
		body = new utils::dataContainer((char*)&flash_html, sizeof(flash_html), sizeof(flash_html));
	}
	else if (stringUtility::equals(path, "\\flash.js", true))
	{
		body = new utils::dataContainer((char*)&flash_js, sizeof(flash_js), sizeof(flash_js));
	}
	else if (stringUtility::equals(path, "\\remove.html", true))
	{
		body = new utils::dataContainer((char*)&remove_html, sizeof(remove_html), sizeof(remove_html));
	}
	else if (stringUtility::equals(path, "\\remove.js", true))
	{
		body = new utils::dataContainer((char*)&remove_js, sizeof(remove_js), sizeof(remove_js));
	}
	else if (stringUtility::equals(path, "\\remoteview.html", true))
	{
		body = new utils::dataContainer((char*)&remoteview_html, sizeof(remoteview_html), sizeof(remoteview_html));
	}
	else if (stringUtility::equals(path, "\\remoteview.js", true))
	{
		body = new utils::dataContainer((char*)&remoteview_js, sizeof(remoteview_js), sizeof(remoteview_js));
	}
	else if (stringUtility::equals(path, "\\launch.html", true))
	{
		body = new utils::dataContainer((char*)&launch_html, sizeof(launch_html), sizeof(launch_html));
	}
	else if (stringUtility::equals(path, "\\launch.js", true))
	{
		body = new utils::dataContainer((char*)&launch_js, sizeof(launch_js), sizeof(launch_js));
	}
	else if (stringUtility::equals(path, "\\cerbiosini.html", true))
	{
		body = new utils::dataContainer((char*)&cerbiosini_html, sizeof(cerbiosini_html), sizeof(cerbiosini_html));
	}
	else if (stringUtility::equals(path, "\\cerbiosini.js", true))
	{
		body = new utils::dataContainer((char*)&cerbiosini_js, sizeof(cerbiosini_js), sizeof(cerbiosini_js));
	}
	else if (stringUtility::equals(path, "\\download.html", true))
	{
		body = new utils::dataContainer((char*)&download_html, sizeof(download_html), sizeof(download_html));
	}
	else if (stringUtility::equals(path, "\\download.js", true))
	{
		body = new utils::dataContainer((char*)&download_js, sizeof(download_js), sizeof(download_js));
	}
	else if (stringUtility::equals(path, "\\downloads.html", true))
	{
		body = new utils::dataContainer((char*)&downloads_html, sizeof(downloads_html), sizeof(downloads_html));
	}
	else if (stringUtility::equals(path, "\\downloads.js", true))
	{
		body = new utils::dataContainer((char*)&downloads_js, sizeof(downloads_js), sizeof(downloads_js));
	}
	else if (stringUtility::equals(path, "\\index.html", true))
	{
		body = new utils::dataContainer((char*)&index_html, sizeof(index_html), sizeof(index_html));
	}
	else if (stringUtility::equals(path, "\\index.js", true))
	{
		body = new utils::dataContainer((char*)&index_js, sizeof(index_js), sizeof(index_js));
	}
	else if (stringUtility::equals(path, "\\main.css", true))
	{
		body = new utils::dataContainer((char*)&main_css, sizeof(main_css), sizeof(main_css));
	}
	else if (stringUtility::equals(path, "\\hddformat_select.html", true))
	{
		body = new utils::dataContainer((char*)&hddformat_select_html, sizeof(hddformat_select_html), sizeof(hddformat_select_html));
	}
	else if (stringUtility::equals(path, "\\hddformat_select.js", true))
	{
		body = new utils::dataContainer((char*)&hddformat_select_js, sizeof(hddformat_select_js), sizeof(hddformat_select_js));
	}
	else if (stringUtility::equals(path, "\\hddformat.html", true))
	{
		body = new utils::dataContainer((char*)&hddformat_html, sizeof(hddformat_html), sizeof(hddformat_html));
	}
	else if (stringUtility::equals(path, "\\hddformat.js", true))
	{
		body = new utils::dataContainer((char*)&hddformat_js, sizeof(hddformat_js), sizeof(hddformat_js));
	}
	else if (stringUtility::equals(path, "\\hddlock.html", true))
	{
		body = new utils::dataContainer((char*)&hddlock_html, sizeof(hddlock_html), sizeof(hddlock_html));
	}
	else if (stringUtility::equals(path, "\\hddlock.js", true))
	{
		body = new utils::dataContainer((char*)&hddlock_js, sizeof(hddlock_js), sizeof(hddlock_js));
	}
	else if (stringUtility::equals(path, "\\utilities.html", true))
	{
		body = new utils::dataContainer((char*)&utilities_html, sizeof(utilities_html), sizeof(utilities_html));
	}
	else if (stringUtility::equals(path, "\\api\\freeslots.json", true))
	{
		char* freeSlotsJson = settingsManager::getFreeSlotsJson();
		body = new utils::dataContainer(freeSlotsJson, strlen(freeSlotsJson), strlen(freeSlotsJson));
		free(freeSlotsJson);
	}
	else if (stringUtility::equals(path, "\\api\\bankinfo.json", true))
	{
		char* bankInfosJson = settingsManager::getBankInfosJson();
		body = new utils::dataContainer(bankInfosJson, strlen(bankInfosJson), strlen(bankInfosJson));
		free(bankInfosJson);
	}
	else if (stringUtility::equals(path, "\\api\\shutdown", true))
	{
		mApiAction = apiActionShutdown;
		return httpServer::generateResponse(200, "OK");
	}
	else if (stringUtility::equals(path, "\\api\\reboot", true))
	{
		mApiAction = apiActionReboot;
		return httpServer::generateResponse(200, "OK");
	}
	else if (stringUtility::equals(path, "\\api\\launchbank", true))
	{
		int id = stringUtility::toInt(query);
		utils::debugPrint("Launching bank %i", id);
		if (id >= 0 && id <= 3)
		{
			mApiAction = apiActionLaunchBank;
			mBankId = id;
		}
		return httpServer::generateResponse(200, "OK");
	}
	else if (stringUtility::equals(path, "\\api\\launchtsop", true))
	{
		utils::debugPrint("Launching TSOP");
		mApiAction = apiActionLaunchTsop;
		return httpServer::generateResponse(200, "OK");
	}
	else if (stringUtility::equals(path, "\\api\\removebank", true))
	{
		int id = stringUtility::toInt(query);
		utils::debugPrint("Removing bank %i", id);
		if (id >= 0 && id <= 3)
		{
			settingsManager::deleteBank(id);
		}
		return httpServer::generateResponse(200, "OK");
	}
	else if (stringUtility::equals(path, "\\api\\downloadbank", true))
	{
		int id = stringUtility::toInt(query);
		utils::debugPrint("Downloading bank %i", id);
		if (id >= 0 && id <= 3)
		{
			body = settingsManager::readBank(id);
		}
	}
	else if (stringUtility::equals(path, "\\api\\downloadeeprom", true))
	{
		XKEEPROM* eeprom = new XKEEPROM();
		eeprom->ReadFromXBOX();
		body = new utils::dataContainer(EEPROM_SIZE);
		eeprom->GetEEPROMData((LPEEPROMDATA)body->data);
		delete(eeprom);
	}
	else if (stringUtility::equals(path, "\\api\\downloadprom", true))
	{
		body = context::getModchip()->readFlash(false);
	}
	else if (stringUtility::equals(path, "\\api\\cerbiosini", true))
	{
		char* temp = (char*)malloc(65536);
		cerbiosConfig config = cerbiosIniHelper::loadConfig();
		cerbiosIniHelper::buildConfig(&config, temp);
		body = new utils::dataContainer(temp, strlen(temp), strlen(temp));
	}
	else if (stringUtility::equals(path, "\\api\\screenshot", true))
	{
		context::setScreenshot(NULL);
		context::setTakeScreenshot(true);
		while (context::getScreenshot() == NULL)
		{
			Sleep(100);
		}
		body = context::getScreenshot();
	}
	else if (stringUtility::equals(path, "\\api\\drives.json", true))
	{
		bool isDualHDDSetup = settingsManager::getDriveSetup() == 3;
		char* usableDrivesJson = stringUtility::formatString("{\"primaryDrive\":1,\"secondaryDrive\":%d}", isDualHDDSetup);
		body = new utils::dataContainer(usableDrivesJson, strlen(usableDrivesJson), strlen(usableDrivesJson));
		free(usableDrivesJson);
	}
	else if (stringUtility::equals(path, "\\api\\drive.json", true))
	{
		pointerVector<char*>* getParams = stringUtility::split(query, "&", true);
		if (getParams->count() < 2) {
			return httpServer::generateResponse(400, "Query must contain the drive idx and a random nonce");
		}
		bool driveIdx = stringUtility::toInt(getParams->get(0)) == 1;
		int clientNonce = stringUtility::toInt(getParams->get(1));
		delete(getParams);
		// Store the random nonce the client includes,
		// only the last one who called this endpoint 
		// can call the "protected" endpoints (lock, format)
		// Doesn't help much, but still prevents some direct api calls
		ExpectedNonce = clientNonce;
		hddInfo::startThread(driveIdx);
		while (!hddInfo::completed() == true) {
			Sleep(10);	
		}
		hddInfo::hddInfoResponse response = hddInfo::getResponse();
		if (response != hddInfo::hddInfoResponseFailureEeprom && response != hddInfo::hddInfoResponseTimeout) {
			bool isLocked = response == hddInfo::hddInfoResponseLocked;
			char* driveInfoJson = stringUtility::formatString("{\"locked\":%d,\"model\":\"%s\",\"serial\":\"%s\"}", isLocked, hddInfo::getModel(), hddInfo::getSerial());
			body = new utils::dataContainer(driveInfoJson, strlen(driveInfoJson), strlen(driveInfoJson));
			free(driveInfoJson);
		}
		hddInfo::closeThread();
	}
	else if (stringUtility::equals(path, "\\api\\hddlockunlock", true))
	{
		pointerVector<char*>* getParams = stringUtility::split(query, "&", true);
		if (getParams->count() < 2) {
			return httpServer::generateResponse(400, "Query must contain if the drive should be locked or unlocked, and the same nonce sent with the last drive info request");
		}
		bool isLockReq = stringUtility::toInt(getParams->get(0)) == 1;
		int clientNonce = stringUtility::toInt(getParams->get(1));
		delete(getParams);
		if (clientNonce != ExpectedNonce) {
			return httpServer::generateResponse(403, "Invalid nonce.");
		}
		if (isLockReq) {
			hddLockUnlock::startThread(hddLockUnlock::hddLockUnlockActionLock);
		} else {
			hddLockUnlock::startThread(hddLockUnlock::hddLockUnlockActionUnlock);
		}
		while (!hddLockUnlock::completed() == true) {
			Sleep(10);
		}
		hddLockUnlock::hddLockUnlockResponse response = hddLockUnlock::getResponse();
		hddLockUnlock::closeThread();
		char* lockResultJson = stringUtility::formatString("{\"status\":%d}", response);
		body = new utils::dataContainer(lockResultJson, strlen(lockResultJson), strlen(lockResultJson));
		free(lockResultJson);
	}

	if (body == NULL)
	{
		return httpServer::generateResponse(404, "Mot found");
	}

	char* extension = fileSystem::getExtension(path);
	char* mimeType = httpServer::getMimeType(extension);
	char* contentLength = stringUtility::formatString("%lu", body->size);
	char* header = stringUtility::formatString("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nCache-Control: no-cache\r\nContent-Type: %s\r\nX-Content-Type-Options: nosniff\r\nContent-Length: %s\r\n\r\n", mimeType, contentLength);

	utils::debugPrint("Header len = %i\n", strlen(header));
	utils::debugPrint("Response = %s\n", header);

	utils::dataContainer* result = new utils::dataContainer(strlen(header) + body->size);
	memcpy(result->data, header, strlen(header));
	memcpy(result->data + strlen(header), body->data, body->size);

	delete(body);
	free(extension);
	free(mimeType);
	free(contentLength);
	free(header);

	return result;
}

utils::dataContainer* onPostCallback(const char* path, const char* query, pointerVector<FormPart*>* formParts)
{
	utils::debugPrint("Post reciecved\n");

	if (stringUtility::equals(path, "\\api\\upload", true))
	{
		if (formParts->count() != 2) 
		{
			return httpServer::generateResponse(400, "Unexpected form parts.");
		}

		FormPart* formPart = formParts->get(0);

		if (context::getModchip()->isValidBankSize(formPart->body->size) == false)
		{
			return httpServer::generateResponse(406, "Invalid size detected.");
		}

		FormPart* jsonPart = formParts->get(1);

		char* bankName = NULL;
		uint8_t ledColor = 0;

		char *json = (char*)jsonPart->body->data;
		struct json_value_s* root = json_parse(json, strlen(json));
		struct json_object_s* object = (struct json_object_s*)root->payload;

		struct json_object_element_s* element = object->start;
		while (element != NULL)
		{
			if (stringUtility::equals(element->name->string, "ledColor", true))
			{
				struct json_number_s* numberValue = json_value_as_number(element->value);
				ledColor = (uint8_t)stringUtility::toInt(numberValue->number);
			}
			else if (stringUtility::equals(element->name->string, "bankName", true))
			{
				struct json_string_s* stringValue = json_value_as_string(element->value);
				bankName = strdup(stringValue->string);
			}
			element = element->next;
		}

		uint8_t slotsNeeded = (uint8_t)(formPart->body->size >> 18);
		settingsManager::optimizeBanks(slotsNeeded);
		
		bool flashSuccess = false;

		uint8_t bankId = 0;
		if (settingsManager::tryGetFreeBank(slotsNeeded, bankId))
		{
			settingsManager::eraseBank(bankId, formPart->body->size);
			settingsManager::writeBank(bankId, formPart->body, bankName == NULL ? "Unknown" : bankName, ledColor);
			flashSuccess = settingsManager::verifyBank(bankId, formPart->body);
		}

		free(bankName);
		free(root);

		if (flashSuccess == true) 
		{
			return httpServer::generateResponse(200, "OK");
		}

		return httpServer::generateResponse(406, "Not enough free slots.");
	}
	else if (stringUtility::equals(path, "\\api\\cerbiosini", true))
	{
		if (formParts->count() != 1) 
		{
			return httpServer::generateResponse(400, "Unexpected form parts.");
		}

		FormPart* formPart = formParts->get(0);

		if (formPart->body->size > 65535)
		{
			return httpServer::generateResponse(406, "Invalid size detected.");
		}

		FormPart* bodyPart = formParts->get(0);
		char* body = (char*)bodyPart->body->data;
		cerbiosIniHelper::saveConfig(body);
	}
	else if (stringUtility::equals(path, "\\api\\formatdrive", true))
	{
		if (formParts->count() != 2) {
			return httpServer::generateResponse(400, "Unexpected form parts.");
		}
		FormPart* formPartDriveIdx = formParts->get(0);
		char* sDriveIdx = (char*)formPartDriveIdx->body->data;
		int driveIdx = stringUtility::toInt(sDriveIdx);
		FormPart* formPartClientNonce = formParts->get(1);
		char* sClientNonce = (char*)formPartClientNonce->body->data;
		int clientNonce = stringUtility::toInt(sClientNonce);
		if (clientNonce != ExpectedNonce) {
			return httpServer::generateResponse(403, "Invalid nonce.");
		}
		if (driveIdx < 0 || driveIdx > 1) {
			return httpServer::generateResponse(400, "Invalid drive index");
		}
		// What if someone tries to call this twice?
		hddFormat::startThread(driveIdx);
		while (!hddFormat::completed() == true) {
			Sleep(100);
		}
		hddFormat::hddFormatResponse response = hddFormat::getResponse();
		hddFormat::closeThread();
		char* formatResultJson = stringUtility::formatString("{\"result\":%d}", response);
		utils::dataContainer* body = new utils::dataContainer(formatResultJson, strlen(formatResultJson), strlen(formatResultJson));
		free(formatResultJson);
		char* contentLength = stringUtility::formatString("%lu", body->size);
		char* header = stringUtility::formatString("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nCache-Control: no-cache\r\nContent-Type: application/json\r\nX-Content-Type-Options: nosniff\r\nContent-Length: %s\r\n\r\n", contentLength);
		utils::debugPrint("Header len = %i\n", strlen(header));
		utils::debugPrint("Response = %s\n", header);

		utils::dataContainer* result = new utils::dataContainer(strlen(header) + body->size);
		memcpy(result->data, header, strlen(header));
		memcpy(result->data + strlen(header), body->data, body->size);

		delete(body);
		free(contentLength);
		free(header);

		return result;
	}

	return httpServer::generateResponse(404, "Not found");
}

void onResponseSentCallback()
{
	if (mApiAction == apiActionShutdown)
	{
		utils::shutdown();
		Sleep(5000);
	}
	else if (mApiAction == apiActionReboot)
	{
		utils::reboot();
		Sleep(5000);
	}
	else if (mApiAction == apiActionLaunchBank)
	{
		settingsManager::launchBank(mBankId);
		Sleep(5000);
	}
	else if (mApiAction == apiActionLaunchTsop)
	{
		settingsManager::launchTsop();
		Sleep(5000);
	}
}

typedef struct {
    DWORD dwWidth;
    DWORD dwHeight;
    BOOL  fProgressive;
    BOOL  fWideScreen;
	DWORD dwFreq;
} DISPLAY_MODE;

DISPLAY_MODE displayModes[] =
{
    //{   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    //{   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    //{   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    //{   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    //{   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    //{   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3


	// Width  Height Progressive Widescreen

	// HDTV Progressive Modes
    {  1280,    720,    TRUE,   TRUE,  60 },         // 1280x720 progressive 16x9

	// EDTV Progressive Modes
    {   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    {   640,    480,    TRUE,   TRUE,  60 },         // 640x480 progressive 16x9
    {   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    {   640,    480,    TRUE,   FALSE, 60 },         // 640x480 progressive 4x3

	// HDTV Interlaced Modes
	//    {  1920,   1080,    FALSE,  TRUE,  60 },         // 1920x1080 interlaced 16x9

	// SDTV PAL-50 Interlaced Modes
    {   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    {   640,    480,    FALSE,  TRUE,  50 },         // 640x480 interlaced 16x9 50Hz
    {   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    {   640,    480,    FALSE,  FALSE, 50 },         // 640x480 interlaced 4x3  50Hz

	// SDTV NTSC / PAL-60 Interlaced Modes
    {   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    {   640,    480,    FALSE,  TRUE,  60 },         // 640x480 interlaced 16x9
    {   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3
    {   640,    480,    FALSE,  FALSE, 60 },         // 640x480 interlaced 4x3
};

#define NUM_MODES (sizeof(displayModes) / sizeof(displayModes[0]))

bool supportsMode(DISPLAY_MODE mode, DWORD dwVideoStandard, DWORD dwVideoFlags)
{
    if (mode.dwFreq == 60 && !(dwVideoFlags & XC_VIDEO_FLAGS_PAL_60Hz) && (dwVideoStandard == XC_VIDEO_STANDARD_PAL_I))
	{
		return false;
	}    
    if (mode.dwFreq == 50 && (dwVideoStandard != XC_VIDEO_STANDARD_PAL_I))
	{
		return false;
	}
    if (mode.dwHeight == 480 && mode.fWideScreen && !(dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN ))
	{
		return false;
	}
    if (mode.dwHeight == 480 && mode.fProgressive && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_480p))
	{
		return false;
	}
    if (mode.dwHeight == 720 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_720p))
	{
		return false;
	}
    if (mode.dwHeight == 1080 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_1080i))
	{
		return false;
	}
    return true;
}

bool createDevice()
{
	uint32_t videoFlags = XGetVideoFlags();
	uint32_t videoStandard = XGetVideoStandard();
	uint32_t currentMode;
    for (currentMode = 0; currentMode < NUM_MODES-1; currentMode++)
    {
		if (supportsMode(displayModes[currentMode], videoStandard, videoFlags)) 
		{
			break;
		}
    } 

	LPDIRECT3D8 d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if(d3d == NULL)
	{
		utils::debugPrint("Failed to create d3d\n");
        return false;
	}

	context::setBufferWidth(720);
	context::setBufferHeight(480);
	//context::setBufferWidth(displayModes[currentMode].dwWidth);
	//context::setBufferHeight(displayModes[currentMode].dwHeight);

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
	params.SwapEffect = D3DSWAPEFFECT_COPY;
    params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	LPDIRECT3DDEVICE8 d3dDevice;
    if (FAILED(d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &d3dDevice)))
	{
		utils::debugPrint("Failed to create device\n");
        return false;
	}
	context::setD3dDevice(d3dDevice);

	D3DXMATRIX matProjection;
	D3DXMatrixOrthoOffCenterLH(&matProjection, 0, (float)context::getBufferWidth(), 0, (float)context::getBufferHeight(), 1.0f, 100.0f);
	context::getD3dDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX  matView;
    D3DXMatrixIdentity(&matView);
    context::getD3dDevice()->SetTransform( D3DTS_VIEW, &matView);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	context::getD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld);

	context::getD3dDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	context::getD3dDevice()->SetVertexShader(D3DFVF_CUSTOMVERTEX);
	context::getD3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	context::getD3dDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	context::getD3dDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    context::getD3dDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	context::getD3dDevice()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	context::getD3dDevice()->BeginScene();
	context::getD3dDevice()->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);
	context::getD3dDevice()->EndScene();
	context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);

	return true;
}

void refreshInfo()
{
	if (network::isReady() == true)
	{
		XNADDR addr;
		memset(&addr, 0, sizeof(addr));
		DWORD dwState = XNetGetTitleXnAddr(&addr);
		if (dwState != XNET_GET_XNADDR_PENDING)
		{
			char* ipAddress = (XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE) ? stringUtility::formatString("%i.%i.%i.%i", 
				addr.ina.S_un.S_un_b.s_b1,
				addr.ina.S_un.S_un_b.s_b2,
				addr.ina.S_un.S_un_b.s_b3,
				addr.ina.S_un.S_un_b.s_b4) : strdup("0.0.0.0");
			char* currentIp = context::getCurrentIp();
			if (stringUtility::equals(ipAddress, currentIp, false) == false)
			{
				context::setCurrentIp(ipAddress);
				mCounter = 0;
			}
			free(ipAddress);
			free(currentIp);
		}
	}

	if (mCounter == 0)
	{
		context::setCurrentFreeMem(utils::getFreePhysicalMemory());
		context::setCurrentFanSpeed(temperatureManager::getFanSpeed() * 2);
		context::setCurrentCpuTemp(temperatureManager::getCpuTemp());
		context::setCurrentMbTemp(temperatureManager::getMbTemp());
		mCounter = 60;
	}
	else
	{
		mCounter--;
	}
}
#define WIFI_SSID_LENGTH 32 
#define WIFI_PASSWORD_LENGTH 63 

#define I2C_SLAVE_ADDRESS 0x60
#define I2C_DATA_LENGTH 256

#define I2C_READ_FLAG 0x00
#define I2C_WRITE_FLAG 0x80

#define I2C_COMMAND_EFFECT 0x00
#define I2C_COMMAND_EFFECT_PARAM (I2C_COMMAND_EFFECT + 1)
#define I2C_COMMAND_EFFECT_COLOR1_R (I2C_COMMAND_EFFECT_PARAM + 1)
#define I2C_COMMAND_EFFECT_COLOR1_G (I2C_COMMAND_EFFECT_COLOR1_R + 1)
#define I2C_COMMAND_EFFECT_COLOR1_B (I2C_COMMAND_EFFECT_COLOR1_G + 1)
#define I2C_COMMAND_EFFECT_COLOR2_R (I2C_COMMAND_EFFECT_COLOR1_B + 1)
#define I2C_COMMAND_EFFECT_COLOR2_G (I2C_COMMAND_EFFECT_COLOR2_R + 1)
#define I2C_COMMAND_EFFECT_COLOR2_B (I2C_COMMAND_EFFECT_COLOR2_G + 1)
#define I2C_COMMAND_COLOR1_R (I2C_COMMAND_EFFECT_COLOR2_B + 1)
#define I2C_COMMAND_COLOR1_G (I2C_COMMAND_COLOR1_R + 1)
#define I2C_COMMAND_COLOR1_B (I2C_COMMAND_COLOR1_G + 1)
#define I2C_COMMAND_COLOR2_R (I2C_COMMAND_COLOR1_B + 1)
#define I2C_COMMAND_COLOR2_G (I2C_COMMAND_COLOR2_R + 1)
#define I2C_COMMAND_COLOR2_B (I2C_COMMAND_COLOR2_G + 1)
#define I2C_COMMAND_COLOR3_R (I2C_COMMAND_COLOR2_B + 1)
#define I2C_COMMAND_COLOR3_G (I2C_COMMAND_COLOR3_R + 1)
#define I2C_COMMAND_COLOR3_B (I2C_COMMAND_COLOR3_G + 1)
#define I2C_COMMAND_COLOR4_R (I2C_COMMAND_COLOR3_B + 1)
#define I2C_COMMAND_COLOR4_G (I2C_COMMAND_COLOR4_R + 1)
#define I2C_COMMAND_COLOR4_B (I2C_COMMAND_COLOR4_G + 1)
#define I2C_COMMAND_WIFI_SSID_START (I2C_COMMAND_COLOR4_B + 1)
#define I2C_COMMAND_WIFI_SSID_END (I2C_COMMAND_WIFI_SSID_START + WIFI_SSID_LENGTH - 1)
#define I2C_COMMAND_WIFI_PASSWORD_START (I2C_COMMAND_WIFI_SSID_END + 1)
#define I2C_COMMAND_WIFI_PASSWORD_END (I2C_COMMAND_WIFI_PASSWORD_START + WIFI_PASSWORD_LENGTH - 1)

#define I2C_COMMAND_APPLY 0x7E
#define I2C_COMMAND_SET_COMMAND 0x7F

#define WIFI_SSID "EqUiNoX"
#define WIFI_PASSWORD "R1chm0nd"

void writeScratchSetting(uint8_t command, uint8_t value)
{
	HalWriteSMBusByte(I2C_SLAVE_ADDRESS << 1, command | I2C_WRITE_FLAG, value);
	Sleep(1);
}

void applySettings()
{
	HalWriteSMBusByte(I2C_SLAVE_ADDRESS << 1, I2C_COMMAND_APPLY | I2C_WRITE_FLAG, 0x00);
	Sleep(1);
}

void writeColor(uint8_t command, uint8_t r, uint8_t g, uint8_t b)
{
	writeScratchSetting(command, r);
	writeScratchSetting(command + 1, g);
	writeScratchSetting(command + 2, b);

	applySettings();
	Sleep(100);
}

void writeWifiDetails()
{
	uint8_t ssid_len = strlen(WIFI_SSID);
	for (uint8_t i = 0; i < ssid_len; i++)
	{
		writeScratchSetting(I2C_COMMAND_WIFI_SSID_START + i, WIFI_SSID[i]);
	}

	uint8_t password_len = strlen(WIFI_PASSWORD);
	for (uint8_t i = 0; i < password_len; i++)
	{
		writeScratchSetting(I2C_COMMAND_WIFI_PASSWORD_START + i, WIFI_PASSWORD[i]);
	}

	applySettings();
}

void readSetting(uint8_t command, uint8_t* value)
{
	HalWriteSMBusByte(I2C_SLAVE_ADDRESS << 1, I2C_COMMAND_SET_COMMAND | I2C_WRITE_FLAG, command);
	Sleep(1);

	uint32_t temp = 0;
	HalReadSMBusByte(I2C_SLAVE_ADDRESS << 1, I2C_READ_FLAG, &temp);
	Sleep(1);

	*value = (uint8_t)temp;
}

void checkForSpecialButtons() {
	// Toggle PrometheOS VGA setting on/off
	if(
		inputManager::buttonDown(ButtonTriggerLeft) &&
		inputManager::buttonDown(ButtonTriggerRight) &&
		inputManager::buttonDown(ButtonWhite)
	) {
		settingsManager::setVgaEnable(!settingsManager::getVgaEnable());
		Sleep(200);
		HalReturnToFirmware(1); // BIOS reboot
	}

	// Toggle between NTSC-M and PAL-50
	if(
		inputManager::buttonDown(ButtonTriggerLeft) &&
		inputManager::buttonDown(ButtonTriggerRight) &&
		inputManager::buttonDown(ButtonBlack)
	) {
		if(xboxConfig::getVideoStandardNTSCM()) {
			xboxConfig::setVideoStandardPALI50();
		} else {
			xboxConfig::setVideoStandardNTSCM();
		}
		Sleep(200);
		HalReturnToFirmware(1); // BIOS reboot
	}
}

void __cdecl main()
{
#ifndef TOOLS
	utils::debugPrint("Welcome to PrometheOS\n");
#else
	utils::debugPrint("Welcome to PrometheOS Tools\n");
#endif

	/*writeWifiDetails();

	while (true)
	{
		writeColor(I2C_COMMAND_COLOR2_R, 0xff, 0x00, 0x00);
		writeColor(I2C_COMMAND_COLOR2_R, 0xff, 0xff, 0x00);
		writeColor(I2C_COMMAND_COLOR2_R, 0x00, 0xff, 0x00);
		writeColor(I2C_COMMAND_COLOR2_R, 0x00, 0xff, 0xff);
		writeColor(I2C_COMMAND_COLOR2_R, 0x00, 0x00, 0xff);
		writeColor(I2C_COMMAND_COLOR2_R, 0xff, 0x00, 0xff);
	}*/

#ifdef XENIUM
	context::setModchipType(modchipTypeXenium);
#elif XTREMIUM
	context::setModchipType(modchipTypeXtremium);
#elif XECUTER
	context::setModchipType(modchipTypeXecuter);
#elif XCHANGER
	context::setModchipType(modchipTypeXchanger);
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

	//XboxPartitionTable PartTbl;
	//XboxPartitionTable* KernelPartTblAdd;
	//DWORD g_dwUserSectors;
	//DWORD lba48PatchVersion;

	//if (harddrive::readLbaInfo(&PartTbl, &KernelPartTblAdd, &g_dwUserSectors, &lba48PatchVersion))
	//{
	//}

	//format example
	//status = XapiFormatFATVolumeEx(&partition_str[i], ClusterSize << 10);

	utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);

	bool deviceCreated = createDevice();

	temperatureManager::init();

	context::setNetworkInitialized(false);

	driveManager::mountDrive("HDD0-C");
	driveManager::mountDrive("HDD0-E");
	
	xboxConfig::init();
	xboxConfig::autoFix();
	settingsManager::loadSettings();

	if (hdmiDevice::detectDevice() == true)
	{
		hdmiSettingsManager::loadSettings();
	}

	if (settingsManager::getRtcEnable() == true && xboxConfig::getHasRtcExpansion() == true)
	{
		rtcDateTime dateTime = rtcManager::getDateTime();
		rtcManager::setXboxDateTime(dateTime);
	}

	httpServer::registerOnGetCallback(onGetCallback);
	httpServer::registerOnPostCallback(onPostCallback);
	httpServer::registerOnResponseSentCallback(onResponseSentCallback);

	if (deviceCreated == false)
	{
		network::init();
		while (true)
		{
			temperatureManager::refresh();
			Sleep(100);
		}
	}

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

	bitmapFont* fontSmall = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 18, 18, 0, 256);
	context::setBitmapFontSmall(fontSmall);
	bitmapFont* fontMedium = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 25, 25, 0, 256);
	context::setBitmapFontMedium(fontMedium);
	bitmapFont* fontLarge = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 32, 32, 0, 512);
	context::setBitmapFontLarge(fontLarge);

	drawing::renderRoundedRect("panel-fill", 24, 24, 6, 0xffffffff, 0x00000000, 0);
	drawing::renderRoundedRect("panel-stroke", 24, 24, 6, 0x01010100, 0xffffffff, 2);

	lcdRender::startThread();

	sceneManager::pushScene(sceneItemMainScene);
	if (settingsManager::hasAutoBootBank() == true)
	{
		sceneManager::pushScene(sceneItemAutoBootScene);
	}

	char* skinName = settingsManager::getSkinName();
	theme::loadSkin(skinName);
	free(skinName);

	audioPlayer::init();
	char* soundPackName = settingsManager::getSoundPackName();
	theme::loadSoundPack(soundPackName);
	free(soundPackName);

	uint64_t start = timeUtility::getMillisecondsNow();
	
	uint32_t frameIndex = 0;
	int32_t frameDirection = -1;

	//PEParam_t* params;
	//params = (PEParam_t*)malloc(sizeof(PEParam_t));
	//memset(params, 0, sizeof(PEParam_t));

	//char* name = PEProcess::getPluginName("E:\\Root\\plugin.nxe");
	//uint32_t result = PEProcess::PE_Run("E:\\Root\\plugin.nxe", params);
	//free(name);


    while (TRUE)
    {
		context::getD3dDevice()->BeginScene();

		temperatureManager::refresh();
		inputManager::processController();
		checkForSpecialButtons();
		drawing::clearBackground((uint32_t)frameIndex);

		if (context::getTakeScreenshot() == true)
		{
			context::setScreenshot(drawing::takeScreenshot());
			context::setTakeScreenshot(false);
		}

		refreshInfo();

		sceneManager::getScene()->update();
		sceneManager::getScene()->render();

		context::getD3dDevice()->EndScene();
		context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);

		uint32_t backgroundCount = theme::getBackgroundFrameCount();
		if (backgroundCount > 0)
		{
			uint64_t end = timeUtility::getMillisecondsNow();
			if (end - start > theme::getBackgroundFrameDelay())
			{
				uint32_t backgroundCount = theme::getBackgroundFrameCount();
				if ( theme::getBackgroundFramePingPong() == true)
				{
					frameIndex = min(frameIndex, backgroundCount - 1);
					if (frameIndex == 0 || frameIndex == (backgroundCount - 1))
					{
						frameDirection = -1 * frameDirection;
					}
					frameIndex = frameIndex + frameDirection;
				}
				else
				{
					frameIndex = (frameIndex + 1) % backgroundCount;
				}

				start = end;
			}
		}
		else
		{
			frameIndex = 0;
		}
    }
}