#pragma once

#include "inputManager.h"
#include "math.h"
#include "pointerMap.h"
#include "modchip.h"
#include "xboxInternals.h"

#include <string>

enum modchipType
{
	modchipTypeXenium,
	modchipTypeXecuter,
	modchipTypeAladdin1mb,
	modchipTypeAladdin2mb,
	modchipTypeXchanger,
};

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
	pointerMap* charMap;
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
	static void setModchip(modchip* modchip, modchipType modchipType);
	static modchip* getModchip();
	static modchipType getModchipType();
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
	static void setImageMap(pointerMap* imageMap);
	static pointerMap* getImageMap();
	static inputManager* getInputManager();
	static void setBitmapFontSmall(bitmapFont* font);
	static bitmapFont* getBitmapFontSmall();
	static void setBitmapFontMedium(bitmapFont* font);
	static bitmapFont* getBitmapFontMedium();
	static void setBitmapFontLarge(bitmapFont* font);
	static bitmapFont* getBitmapFontLarge();
	static void setMasterPassword(utils::dataContainer* masterPassword);
	static utils::dataContainer* getMasterPassword();
	static void setUserPassword(utils::dataContainer* userPassword);
	static utils::dataContainer* getUserPassword();
	static void setDriveModel(const char* model);
	static char* getDriveModel();
	static void setDriveSeriall(const char* serial);
	static char* getDriveSerial();
};
