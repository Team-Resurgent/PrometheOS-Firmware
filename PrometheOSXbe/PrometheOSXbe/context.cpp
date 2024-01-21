#include "context.h"
#include "inputManager.h"
#include "pointerMap.h"

namespace 
{
	bool mNetworkInitialized;
	LPDIRECT3DDEVICE8 mD3dDevice;
	int mBufferWidth;
	int mBufferHeight;
	int mBufferPitch;
	int* mBuffer;
	pointerMap* mImageMap;
	bitmapFont* mBitmapFontSmall;
	bitmapFont* mBitmapFontMedium;
	bitmapFont* mBitmapFontLarge;
}

void context::setNetworkInitialized(bool value)
{
	mNetworkInitialized = value;
}

bool context::getNetworkInitialized()
{
	return mNetworkInitialized;
}

void context::setD3dDevice(LPDIRECT3DDEVICE8 d3dDevice)
{
	mD3dDevice = d3dDevice;
}

LPDIRECT3DDEVICE8 context::getD3dDevice()
{
	return mD3dDevice;
}

void context::setBufferWidth(int bufferWidth)
{
	mBufferWidth = bufferWidth;
}

int context::getBufferWidth()
{
	return mBufferWidth;
}

void context::setBufferHeight(int bufferHeight)
{
	mBufferHeight = bufferHeight;
}

int context::getBufferHeight()
{
	return mBufferHeight;
}

void context::setBufferPitch(int bufferPitch)
{
	mBufferPitch = bufferPitch;
}

int context::getBufferPitch()
{
	return mBufferPitch;
}

void context::setBuffer(int* buffer)
{
	mBuffer = buffer;
}

int* context::getBuffer()
{
	return mBuffer;
}

void context::setImageMap(pointerMap* imageMap)
{
	mImageMap = imageMap;
}

pointerMap* context::getImageMap()
{
	return mImageMap;
}

void context::setBitmapFontSmall(bitmapFont* font)
{
	mBitmapFontSmall = font;
}

bitmapFont* context::getBitmapFontSmall()
{
	return mBitmapFontSmall;
}

void context::setBitmapFontMedium(bitmapFont* font)
{
	mBitmapFontMedium = font;
}

bitmapFont* context::getBitmapFontMedium()
{
	return mBitmapFontMedium;
}

void context::setBitmapFontLarge(bitmapFont* font)
{
	mBitmapFontLarge = font;
}

bitmapFont* context::getBitmapFontLarge()
{
	return mBitmapFontLarge;
}