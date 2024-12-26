#include "context.h"
#include "inputManager.h"
#include "pointerMap.h"
#include "modchip.h"
#include "globalTypes.h"
#include "modchipDummy.h"
#include "modchipXenium.h"
#include "modchipXecuter.h"
#include "modchipSmartxx.h"
#include "modchipModxo.h"
#include "modchipAladdin1mb.h"
#include "modchipAladdin2mb.h"
#include "modchipXchanger.h"

namespace 
{
	modchip* mModchip;
	modchipType mModchipType;
	char* mCurrentIp = NULL;
	uint32_t mCurrentFreeMem = NULL;
	char* mCurrentTitle = NULL;
	uint32_t mCurrentFanSpeed = 0;
	uint32_t mCurrentCpuTemp = 0;
	uint32_t mCurrentMbTemp = 0;
	bool mNetworkInitialized;
	LPDIRECT3DDEVICE8 mD3dDevice;
	int32_t mBufferWidth;
	int32_t mBufferHeight;
	int32_t mBufferPitch;
	int32_t* mBuffer;
	pointerMap<image*>* mImageMap = NULL;
	bitmapFont* mBitmapFontSmall = NULL;
	bitmapFont* mBitmapFontMedium = NULL;
	bitmapFont* mBitmapFontLarge = NULL;
	char* mSoundPackPath = NULL;
	uint32_t mMusicId;
	utils::dataContainer* mMasterPassword = NULL;
	utils::dataContainer* mUserPassword = NULL;
	char* mDriveModel = NULL;
	char* mDriveSerial = NULL;
	bool mTakeScreenshot = false;
	utils::dataContainer* mScreenshot = NULL;
	CRITICAL_SECTION mMutex;
}

modchip* context::getModchip()
{
	return mModchip;
}


void context::setModchipType(modchipType modchipType)
{
	if (mModchip != NULL)
	{
		delete(mModchip);
	}
	if (modchipType == modchipTypeXenium)
	{
		mModchip = new modchipXenium();
	}
	else if (modchipType == modchipTypeXecuter)
	{
		mModchip = new modchipXecuter();
	}
	else if (modchipType == modchipTypeSmartxx)
	{
		mModchip = new modchipSmartxx();
	}
	else if (modchipType == modchipTypeModxo)
	{
		mModchip = new modchipModxo();
	}
	else if (modchipType == modchipTypeAladdin1mb)
	{
		mModchip = new modchipAladdin1mb();
	}
	else if (modchipType == modchipTypeAladdin2mb)
	{
		mModchip = new modchipAladdin2mb();
	}
	else if (modchipType == modchipTypeXchanger)
	{
		mModchip = new modchipXchanger();
	}
	else if (modchipType == modchipTypeDummy)
	{
		mModchip = new modchipDummy();
	}
	mModchipType = modchipType;
}

modchipType context::getModchipType()
{
	return mModchipType;
}

void context::setCurrentIp(const char* ip)
{
	free(mCurrentIp);
	mCurrentIp = strdup(ip);
}

char* context::getCurrentIp()
{
	return mCurrentIp == NULL ? strdup("0.0.0.0") : strdup(mCurrentIp);
}

void context::setCurrentFreeMem(uint32_t freeMem)
{
	mCurrentFreeMem = freeMem;
}

uint32_t context::getCurrentFreeMem()
{
	return mCurrentFreeMem;
}

void context::setCurrentTitle(const char* title)
{
	free(mCurrentTitle);
	mCurrentTitle = strdup(title);
}

char* context::getCurrentTitle()
{
	return mCurrentTitle == NULL ? strdup("") : strdup(mCurrentTitle);
}

void context::setCurrentFanSpeed(int32_t fanSpeed)
{
	mCurrentFanSpeed = fanSpeed;
}

int32_t context::getCurrentFanSpeed()
{
	return mCurrentFanSpeed;
}

void context::setCurrentCpuTemp(int32_t cpuTemp)
{
	mCurrentCpuTemp = cpuTemp;
}

int32_t context::getCurrentCpuTemp()
{
	return mCurrentCpuTemp;
}

void context::setCurrentMbTemp(int32_t mbTemp)
{
	mCurrentMbTemp = mbTemp;
}

int32_t context::getCurrentMbTemp()
{
	return mCurrentMbTemp;
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

void context::setImageMap(pointerMap<image*>* imageMap)
{
	mImageMap = imageMap;
}

pointerMap<image*>* context::getImageMap()
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

void context::setSoundPackPath(const char* soundPackPath)
{
	free(mSoundPackPath);
	mSoundPackPath = strdup(soundPackPath);
}

char* context::getSoundPackPath()
{
	return strdup(mSoundPackPath);
}

void context::setMusicId(uint32_t musicId)
{
	mMusicId = musicId;
}

uint32_t context::getMusicId()
{
	return mMusicId;
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

void context::setDriveSerial(const char* serial)
{
	free(mDriveSerial);
	mDriveSerial = strdup(serial);
}

char* context::getDriveSerial()
{
	return mDriveSerial == NULL ? strdup("") : strdup(mDriveSerial);
}

void context::setTakeScreenshot(bool takeScreenshot)
{
	mTakeScreenshot = takeScreenshot;
}

bool context::getTakeScreenshot()
{
	return mTakeScreenshot;
}

void context::setScreenshot(utils::dataContainer* screenshot)
{
	if (mScreenshot != NULL)
	{
		delete(mScreenshot);
	}
	mScreenshot = screenshot;
}

utils::dataContainer* context::getScreenshot()
{
	if (mScreenshot == NULL)
	{
		return NULL;
	}
	utils::dataContainer* screenshotCopy = new utils::dataContainer(mScreenshot->data, mScreenshot->size, mScreenshot->size);
	return screenshotCopy;
}