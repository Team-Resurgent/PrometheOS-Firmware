#include "temperatureManager.h"
#include "utils.h"
#include "xboxInternals.h"
#include "settingsManager.h"
#include "xboxConfig.h"

namespace
{
	bool mInitialized = false;
	int32_t mCounter = 0;
	int32_t mTargetTemp = 55;
	int32_t mLastTemp = 0;
	double mCpuFreq = 733;
	int32_t mMinFanSpeed = 10;
	int32_t mSystemFanSpeed = 0;
	int32_t mCurrentFanSpeed = 0;
	int32_t mTooHotCount = 0;
	int32_t mTooColdCount = 0;
	xboxConfig::xboxVersionEnum mXboxVer = xboxConfig::xboxVersionUnknown;
}

void temperatureManager::init()
{
	if (mInitialized == true)
	{
		return;
	}

	mXboxVer = xboxConfig::getXboxVersion();
	mSystemFanSpeed = getFanSpeed();
	mCurrentFanSpeed = mSystemFanSpeed;
	mLastTemp = max(getCpuTemp(), getMbTemp());
	mCpuFreq = xboxConfig::getCPUFreq();
	mMinFanSpeed = max(mCpuFreq > 760 ? 30 : 10, (settingsManager::getMinFanSpeed() / 2));
}

int32_t temperatureManager::getCpuTemp()
{
	if(mXboxVer != xboxConfig::xboxVersion16 && mXboxVer != xboxConfig::xboxVersion16b)
	{
		DWORD cpu = 0;
		HalReadSMBusByte(0x4C << 1, 0x01, &cpu);
		DWORD cpudec = 0;
		HalReadSMBusByte(0x4C << 1, 0x10, &cpudec);
		return (uint8_t)(cpu + cpudec / 256);
	}

	uint32_t cpuTemp = 0;
	HalReadSMBusByte(PIC_ADDRESS, CPU_TEMP, &cpuTemp);
	return cpuTemp;
}

int32_t temperatureManager::getMbTemp()
{
	uint32_t tempMb = 0;
	HalReadSMBusByte(PIC_ADDRESS, MB_TEMP, &tempMb);
	return mXboxVer == xboxConfig::xboxVersion16 || mXboxVer == xboxConfig::xboxVersion16b 
		? (int32_t)(tempMb * 0.8f) 
		: (int32_t)tempMb;
}

int32_t temperatureManager::getFanSpeed()
{
	uint32_t fanSpeed = 0;
	HalReadSMBusByte(PIC_ADDRESS, FAN_READBACK, &fanSpeed);
	return (int32_t)fanSpeed;
}

void temperatureManager::setFanSpeed(uint32_t fanspeed)
{
  if (fanspeed > 50) 
  {
	  return;
  }
  if (mCurrentFanSpeed == fanspeed)
  {
	  return;
  }
  mCurrentFanSpeed = fanspeed;
  for (uint32_t i = 0; i < 2; i++)
  {
	HalWriteSMBusByte(PIC_ADDRESS, FAN_MODE, 1);
	Sleep(10);
	HalWriteSMBusByte(PIC_ADDRESS, FAN_REGISTER, fanspeed);
	Sleep(10);
  }
}

void temperatureManager::refresh()
{
	// We dont want to refresh every frame as this will lock up the smbus
	if (mCounter != 50)
	{
		mCounter++;
		return;
	}
	mCounter = 0;

	mMinFanSpeed = max(mCpuFreq > 760 ? 30 : 10, (settingsManager::getMinFanSpeed() / 2));

	int32_t temp = max(getCpuTemp(), getMbTemp());
	float targetTempFloor = mTargetTemp - 0.75f;
    float targetTempCeiling = mTargetTemp + 0.75f;

	int calculatedFanSpeed = mCurrentFanSpeed;

	if ((temp >= targetTempFloor) && (temp <= targetTempCeiling))
	{
		mTooHotCount = 0;
		mTooColdCount = 0;
		if (temp > mLastTemp)
		{
			calculatedFanSpeed++;
		}
		else if (temp < mLastTemp)
		{
			calculatedFanSpeed--;
		}
	}
	else if (temp < targetTempFloor)
	{
		if (temp == mLastTemp)
		{
			mTooColdCount++;
		}
		else if (temp > mLastTemp)
		{
			mTooColdCount--;
		}
		if ((temp < mLastTemp) || (mTooColdCount == 12))
		{
			calculatedFanSpeed--;
			mTooColdCount = 0;
		}
	}
	else if (temp > targetTempCeiling)
	{
		if (temp == mLastTemp)
		{
			mTooHotCount++;
		}
		else if (temp < mLastTemp)
		{
			mTooHotCount--;
		}
		if ((temp > mLastTemp) || (mTooHotCount == 12))
		{
			calculatedFanSpeed++;
			mTooHotCount = 0;
		}
	}

	calculatedFanSpeed = min(max(calculatedFanSpeed, mMinFanSpeed), 50);
	setFanSpeed(calculatedFanSpeed);
	mLastTemp = temp;
}