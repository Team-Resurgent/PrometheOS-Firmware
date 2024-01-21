#include "stdafx.h"
#include <xtl.h>
#include <string>

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
#include "xenium.h"
#include "settingsManager.h"
#include "json.h"
#include "theme.h"
#include "ssfn.h"
#include "ftpServer.h"
#include "httpServer.h"
#include "driveManager.h"
#include "XKUtils\XKEEPROM.h"

#include <xmv.h>

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

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
	else if (stringUtility::equals(path, "\\launch.html", true))
	{
		body = new utils::dataContainer((char*)&launch_html, sizeof(launch_html), sizeof(launch_html));
	}
	else if (stringUtility::equals(path, "\\launch.js", true))
	{
		body = new utils::dataContainer((char*)&launch_js, sizeof(launch_js), sizeof(launch_js));
	}
	else if (stringUtility::equals(path, "\\download.html", true))
	{
		body = new utils::dataContainer((char*)&download_html, sizeof(download_html), sizeof(download_html));
	}
	else if (stringUtility::equals(path, "\\download.js", true))
	{
		body = new utils::dataContainer((char*)&download_js, sizeof(download_js), sizeof(download_js));
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

utils::dataContainer* onPostCallback(const char* path, const char* query, pointerVector* formParts)
{
	utils::debugPrint("Post reciecved\n");

	if (stringUtility::equals(path, "\\api\\upload", true))
	{
		if (formParts->count() != 2) 
		{
			return httpServer::generateResponse(400, "Unexpected form parts.");
		}

		FormPart* formPart = (FormPart*)formParts->get(0);

		if ((formPart->body->size > (1024 * 1024)) || (formPart->body->size % (256 * 1024)) != 0)
		{
			return httpServer::generateResponse(406, "Invalid size detected.");
		}

		FormPart* jsonPart = (FormPart*)formParts->get(1);

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

		return httpServer::generateResponse(406, "No wnough free slots.");
	}

	return httpServer::generateResponse(404, "Mot found");
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
    {   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    {   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    {   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    {   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    {   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    {   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3
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

void __cdecl main()
{
	utils::debugPrint("Welcome to PrometheOS\n");

	driveManager::mountDrive("C");

	utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);

	xboxConfig::init();
	xboxConfig::autoFix();
	settingsManager::loadSettings();

	httpServer::registerOnGetCallback(onGetCallback);
	httpServer::registerOnPostCallback(onPostCallback);
	httpServer::registerOnResponseSentCallback(onResponseSentCallback);

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
        return;
	}
	
	context::setImageMap(new pointerMap(false));
	context::setBufferWidth(displayModes[currentMode].dwWidth);
	context::setBufferHeight(displayModes[currentMode].dwHeight);

	D3DPRESENT_PARAMETERS params; 
    ZeroMemory(&params, sizeof(params));
	params.BackBufferWidth = context::getBufferWidth();
    params.BackBufferHeight = context::getBufferHeight();
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
        return;
	}
	context::setD3dDevice(d3dDevice);

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

	bitmapFont* fontSmall = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 18, 18, 0, 256);
	context::setBitmapFontSmall(fontSmall);
	bitmapFont* fontMedium = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 25, 25, 0, 256);
	context::setBitmapFontMedium(fontMedium);
	bitmapFont* fontLarge = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 32, 32, 0, 512);
	context::setBitmapFontLarge(fontLarge);

	drawing::renderRoundedRect("panel-fill", 24, 24, 6, 0xffffffff, 0x00000000, 0);
	drawing::renderRoundedRect("panel-stroke", 24, 24, 6, 0x01010100, 0xffffffff, 2);

	char* skinName = settingsManager::getSkinName();
	theme::loadSkin(skinName);
	free(skinName);

	context::setNetworkInitialized(false);

	sceneManager::setScene(settingsManager::hasAutoBootBank() ? (scene*)new autoBootScene() : (scene*)new mainScene());

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

	drawing::clearBackground();

	temperatureManager::init();

    while (TRUE)
    {
		context::getD3dDevice()->BeginScene();

		temperatureManager::refresh();
		inputManager::processController();
		sceneManager::getScene()->update();
		sceneManager::getScene()->render();

		context::getD3dDevice()->EndScene();
		context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);
    }
}
