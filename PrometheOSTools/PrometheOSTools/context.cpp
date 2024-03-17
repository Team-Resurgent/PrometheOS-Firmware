#include "context.h"
#include "inputManager.h"
#include "pointerMap.h"
#include "modchip.h"

namespace 
{
	modchip* mModchip;
	modchipType mModchipType;
	bool mNetworkInitialized;
	LPDIRECT3DDEVICE8 mD3dDevice;
	int32_t mBufferWidth;
	int32_t mBufferHeight;
	int32_t mBufferPitch;
	int32_t* mBuffer;
	pointerMap* mImageMap = NULL;
	bitmapFont* mBitmapFontSmall = NULL;
	bitmapFont* mBitmapFontMedium = NULL;
	bitmapFont* mBitmapFontLarge = NULL;
	char* mSoundPackPath = NULL;
	uint32_t mMusicId;
	utils::dataContainer* mMasterPassword = NULL;
	utils::dataContainer* mUserPassword = NULL;
	char* mDriveModel = NULL;
	char* mDriveSerial = NULL;
}

void context::setModchip(modchip* modchip, modchipType modchipType)
{
	if (mModchip != NULL)
	{
		delete(mModchip);
	}
	mModchip = modchip;
	mModchipType = modchipType;
}

modchip* context::getModchip()
{
	return mModchip;
}

modchipType context::getModchipType()
{
	return mModchipType;
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

void context::setBufferWidth(int32_t bufferWidth)
{
	mBufferWidth = bufferWidth;
}

int32_t context::getBufferWidth()
{
	return mBufferWidth;
}

void context::setBufferHeight(int32_t bufferHeight)
{
	mBufferHeight = bufferHeight;
}

int32_t context::getBufferHeight()
{
	return mBufferHeight;
}

void context::setBufferPitch(int32_t bufferPitch)
{
	mBufferPitch = bufferPitch;
}

int32_t context::getBufferPitch()
{
	return mBufferPitch;
}

void context::setBuffer(int32_t* buffer)
{
	mBuffer = buffer;
}

int32_t* context::getBuffer()
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

void context::setMasterPassword(utils::dataContainer* masterPassword)
{
	delete(mMasterPassword);
	mMasterPassword = masterPassword;
}

utils::dataContainer* context::getMasterPassword()
{  
	return mMasterPassword;
}

void context::setUserPassword(utils::dataContainer* userPassword)
{
	delete(mUserPassword);
	mUserPassword = userPassword;
}

utils::dataContainer* context::getUserPassword()
{
	return mUserPassword;
}

void context::setDriveModel(const char* model)
{
	free(mDriveModel);
	mDriveModel = strdup(model);
}

char* context::getDriveModel()
{
	return mDriveModel == NULL ? strdup("") : strdup(mDriveModel);
}

void context::setDriveSeriall(const char* serial)
{
	free(mDriveSerial);
	mDriveSerial = strdup(serial);
}

char* context::getDriveSerial()
{
	return mDriveSerial == NULL ? strdup("") : strdup(mDriveSerial);
}