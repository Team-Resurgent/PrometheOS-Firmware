#include "stdafx.h"
#include <xtl.h>
#include <string>

#include "temperatureManager.h"
#include "context.h"
#include "drawing.h"
#include "Scenes\scene.h"
#include "Scenes\mainScene.h"
#include "Scenes\sceneManager.h"
#include "inputManager.h"
#include "xboxConfig.h"
#include "xboxinternals.h"
#include "meshUtility.h"
#include "utils.h"
#include "resources.h"
#include "stringUtility.h"
#include "hdmiSettingsManager.h"
#include "hdmiDevice.h"
#include "theme.h"
#include "ssfn.h"
#include "ftpServer.h"
#include "driveManager.h"
#include "XKUtils\XKEEPROM.h"
#include "timeUtility.h"
#include "modchipXenium.h"
#include "modchipXecuter.h"
#include "modchipAladdin1mb.h"
#include "modchipDummy.h"
#include "Threads\flashBackup.h"
#include "Plugins\PEProcess.h"

#include <xgraphics.h>

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


	// Width  Height Progressive Widescreen

	// HDTV Progressive Modes
 //   {  1280,    720,    TRUE,   TRUE,  60 },         // 1280x720 progressive 16x9

	//// EDTV Progressive Modes
 //   {   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
 //   {   640,    480,    TRUE,   TRUE,  60 },         // 640x480 progressive 16x9
 //   {   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
 //   {   640,    480,    TRUE,   FALSE, 60 },         // 640x480 progressive 4x3

	//// HDTV Interlaced Modes
	////    {  1920,   1080,    FALSE,  TRUE,  60 },         // 1920x1080 interlaced 16x9

	//// SDTV PAL-50 Interlaced Modes
 //   {   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
 //   {   640,    480,    FALSE,  TRUE,  50 },         // 640x480 interlaced 16x9 50Hz
 //   {   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
 //   {   640,    480,    FALSE,  FALSE, 50 },         // 640x480 interlaced 4x3  50Hz

	//// SDTV NTSC / PAL-60 Interlaced Modes
 //   {   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
 //   {   640,    480,    FALSE,  TRUE,  60 },         // 640x480 interlaced 16x9
 //   {   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3
 //   {   640,    480,    FALSE,  FALSE, 60 },         // 640x480 interlaced 4x3
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

void __cdecl main()
{
	utils::debugPrint("Welcome to PrometheOS\n");

	context::setModchip((modchip*)new modchipXenium(), modchipTypeXenium);

	utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);

	bool deviceCreated = createDevice();

	temperatureManager::init();

	context::setNetworkInitialized(false);

	driveManager::mountDrive("C");
	driveManager::mountDrive("E");
	
	xboxConfig::init();
	xboxConfig::autoFix();

	if (hdmiDevice::detectDevice() == true)
	{
		hdmiSettingsManager::loadSettings();
	}

	context::setImageMap(new pointerMap(false));

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

	bitmapFont* fontSmall = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 18, 18, 0, 256);
	context::setBitmapFontSmall(fontSmall);
	bitmapFont* fontMedium = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 25, 25, 0, 256);
	context::setBitmapFontMedium(fontMedium);
	bitmapFont* fontLarge = drawing::generateBitmapFont("FreeSans", SSFN_STYLE_REGULAR, 32, 32, 0, 512);
	context::setBitmapFontLarge(fontLarge);

	drawing::renderRoundedRect("panel-fill", 24, 24, 6, 0xffffffff, 0x00000000, 0);
	drawing::renderRoundedRect("panel-stroke", 24, 24, 6, 0x01010100, 0xffffffff, 2);

	sceneManager::pushScene(sceneItemMainScene);

	theme::loadSkin();

	uint64_t start = timeUtility::getMillisecondsNow();



    while (TRUE)
    {
		context::getD3dDevice()->BeginScene();

		temperatureManager::refresh();
		inputManager::processController();
		drawing::clearBackground(0);

		if (inputManager::buttonPressed(ButtonWhite))
		{
			if (fileSystem::directoryCreate("E:\\PrometheOS") == true && fileSystem::directoryCreate("E:\\PrometheOS\\Screenshots") == true)
			{
				char* time = timeUtility::getTimeString();
				char* filePath = stringUtility::formatString("E:\\PrometheOS\\Screenshots\\Screenshot-%s.bmp", time);
				IDirect3DSurface8* surface;
				context::getD3dDevice()->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &surface);
				XGWriteSurfaceToFile(surface, filePath);
				surface->Release();
				free(filePath);
				free(time);
			}
		}

		sceneManager::getScene()->update();
		sceneManager::getScene()->render();

		context::getD3dDevice()->EndScene();
		context::getD3dDevice()->Present(NULL, NULL, NULL, NULL);
    }
}