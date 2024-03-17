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
	uint32_t mCpuFreq = 733;
	int32_t mMinFanSpeed = 10;
	int32_t mSystemFanSpeed = 0;
	int32_t mCurrentFanSpeed = 0;
	int32_t mTooHotCount = 0;
	int32_t mTooColdCount = 0;
}

void temperatureManager::init()
{
	if (mInitialized == true)
	{
		return;
	}

	mSystemFanSpeed = getFanSpeed();
	mCurrentFanSpeed = mSystemFanSpeed;
	mLastTemp = max(getCpuTemp(), getMbTemp());
	mCpuFreq = xboxConfig::getCPUFreq();
	mMinFanSpeed = max(mCpuFreq > 733 ? 30 : 10, (settingsManager::getMinFanSpeed() / 2));
}

//https://github.com/rkalz/xbmc4xbox/blob/50f38fb2fce74f2d7679ffc14f3d5002b9c65b6a/xbmc/utils/FanController.cpp#L261

int32_t temperatureManager::getCpuTemp()
{
	if(xboxConfig::getXboxVersion() != xboxConfig::xboxVersion16or16b)
	{
		utils::ioOutputByte(I2C_IO_BASE + 4, (0x4C << 1) | 1);
		utils::ioOutputByte(I2C_IO_BASE + 8, 0x01);
		utils::ioOutputShort(I2C_IO_BASE + 0, utils::ioInputShort(I2C_IO_BASE + 0)); 
		utils::ioOutputByte(I2C_IO_BASE + 2, 0x0a);
		while (utils::ioInputByte(I2C_IO_BASE + 0) & 8);
		uint16_t cpu = utils::ioInputShort(I2C_IO_BASE + 6);

		utils::ioOutputByte(I2C_IO_BASE + 4, (0x4C << 1) | 1);
		utils::ioOutputByte(I2C_IO_BASE + 8, 0x10);
		utils::ioOutputShort(I2C_IO_BASE + 0, utils::ioInputShort(I2C_IO_BASE + 0)); 
		utils::ioOutputByte(I2C_IO_BASE + 2, 0x0A);
		while (utils::ioInputByte(I2C_IO_BASE + 0) & 8);
		uint16_t cpudec = utils::ioInputShort(I2C_IO_BASE + 6);

		return (uint8_t)(cpu + cpudec / 256);
    }

	uint8_t cpuCount = 0;
	float cpuTotal = 0;
	while (cpuCount < 10)
	{
		utils::ioOutputByte(I2C_IO_BASE + 4, (0x70 << 1) | 1);
		utils::ioOutputByte(I2C_IO_BASE + 8, 0xC1);
		utils::ioOutputShort(I2C_IO_BASE + 0, utils::ioInputShort(I2C_IO_BASE + 0)); 
		utils::ioOutputByte(I2C_IO_BASE + 2, 0x0D);
		while (utils::ioInputByte(I2C_IO_BASE + 0) & 8);
		if (!(utils::ioInputByte(I2C_IO_BASE + 0) & 0x23))
		{
			utils::ioInputByte(I2C_IO_BASE + 4);
			uint16_t cpudec = utils::ioInputByte(I2C_IO_BASE + 9); 
			uint16_t cpu = utils::ioInputByte(I2C_IO_BASE + 9); 
			utils::ioInputByte(I2C_IO_BASE + 9);
			utils::ioInputByte(I2C_IO_BASE + 9);  
			cpuTotal += (float)cpu + (float)cpudec / 256;
		}
		cpuCount++;
	}
	return (int32_t)(cpuTotal / 10);
}

int32_t temperatureManager::getMbTemp()
{
	uint32_t tempMb = 0;
	HalReadSMBusByte(PIC_ADDRESS, MB_TEMP, &tempMb);
	return xboxConfig::getXboxVersion() == xboxConfig::xboxVersion16or16b ? (int32_t)(tempMb * 0.8f) : (int32_t)tempMb;
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

	mMinFanSpeed = max(mCpuFreq > 733 ? 30 : 10, (settingsManager::getMinFanSpeed() / 2));

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