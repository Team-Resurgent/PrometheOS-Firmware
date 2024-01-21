#pragma once

#include "inputManager.h"
#include "math.h"
#include "pointerMap.h"

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
	static void setNetworkInitialized(bool value);
	static bool getNetworkInitialized();
	static void setD3dDevice(LPDIRECT3DDEVICE8 d3dDevice);
	static LPDIRECT3DDEVICE8 getD3dDevice();
	static void setBufferWidth(int bufferWidth);
	static int getBufferWidth();
	static void setBufferHeight(int bufferHeight);
	static int getBufferHeight();
	static void setBufferPitch(int bufferPitch);
	static int getBufferPitch();
	static void setBuffer(int* buffer);
	static int* getBuffer();
	static void setImageMap(pointerMap* imageMap);
	static pointerMap* getImageMap();
	static inputManager* getInputManager();
	static void setBitmapFontSmall(bitmapFont* font);
	static bitmapFont* getBitmapFontSmall();
	static void setBitmapFontMedium(bitmapFont* font);
	static bitmapFont* getBitmapFontMedium();
	static void setBitmapFontLarge(bitmapFont* font);
	static bitmapFont* getBitmapFontLarge();
};
