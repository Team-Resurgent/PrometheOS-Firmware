#pragma once

#include "inputManager.h"
#include "math.h"
#include "pointerMap.h"
#include "modchip.h"
#include "xboxInternals.h"

#include <string>

typedef struct image
{
	int width;
	int height;
	D3DTexture *texture;
	math::rectF uvRect;

	~image()
	{
		texture->Release();
	}

} image;

typedef struct bitmapFont
{
	pointerMap<math::rectI*>* charMap;
	image* image;
	int lineHeight;
	int spacing;

	~bitmapFont()
	{
		delete(charMap);
		delete(image);
	}

} bitmapFont;

class context
{
public:
	static modchip* getModchip();
	static void setModchipType(modchipType modchipType);
	static modchipType getModchipType();
	static void setCurrentIp(const char* ip);
	static char* getCurrentIp();
	static void setCurrentFreeMem(uint32_t freeMem);
	static uint32_t getCurrentFreeMem();
	static void setCurrentTitle(const char* title);
	static char* getCurrentTitle();
	static void setCurrentFanSpeed(int32_t fanSpeed);
	static int32_t getCurrentFanSpeed();
	static void setCurrentCpuTemp(int32_t cpuTemp);
	static int32_t getCurrentCpuTemp();
	static void setCurrentMbTemp(int32_t mbTemp);
	static int32_t getCurrentMbTemp();
	static void setNetworkInitialized(bool value);
	static bool getNetworkInitialized();
	static void setD3dDevice(LPDIRECT3DDEVICE8 d3dDevice);
	static LPDIRECT3DDEVICE8 getD3dDevice();
	static void setBufferWidth(int32_t bufferWidth);
	static int32_t getBufferWidth();
	static void setBufferHeight(int32_t bufferHeight);
	static int32_t getBufferHeight();
	static void setBufferPitch(int32_t bufferPitch);
	static int32_t getBufferPitch();
	static void setBuffer(int32_t* buffer);
	static int32_t* getBuffer();
	static void setImageMap(pointerMap<image*>* imageMap);
	static pointerMap<image*>* getImageMap();
	static inputManager* getInputManager();
	static void setBitmapFontSmall(bitmapFont* font);
	static bitmapFont* getBitmapFontSmall();
	static void setBitmapFontMedium(bitmapFont* font);
	static bitmapFont* getBitmapFontMedium();
	static void setBitmapFontLarge(bitmapFont* font);
	static bitmapFont* getBitmapFontLarge();
	static void setSoundPackPath(const char* soundPackPath);
	static char* getSoundPackPath();
	static void setMusicId(uint32_t musicId);
	static uint32_t getMusicId();
	static void setMasterPassword(utils::dataContainer* masterPassword);
	static utils::dataContainer* getMasterPassword();
	static void setUserPassword(utils::dataContainer* userPassword);
	static utils::dataContainer* getUserPassword();
	static void setDriveModel(const char* model);
	static char* getDriveModel();
	static void setDriveSerial(const char* serial);
	static char* getDriveSerial();
	static void setTakeScreenshot(bool takeScreenshot);
	static bool getTakeScreenshot();
	static void setScreenshot(utils::dataContainer* screenshot);
	static utils::dataContainer* getScreenshot();
};
