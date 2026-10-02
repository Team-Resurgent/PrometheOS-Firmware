#include "temperatureManager.h"
#include "utils.h"
#include "xboxInternals.h"
#include "settingsManager.h"
#include "xboxConfig.h"
#include "cpu_gpu.h"

namespace {
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
} // namespace

int32_t temperatureManager::calcFanSpeed() {
    uint8_t speed = settingsManager::getMinFanSpeed();

    if(mCpuFreq > 734 && speed == 0) {
        speed = cpugpu::cpuRatio() != 55 ? 60 : 40;
    }

    return min(max(speed / 2, 0), 50);
}

void temperatureManager::init() {
    if (mInitialized == true) {
        return;
    }

    cpugpu::poll();

    mXboxVer = xboxConfig::getXboxVersion();
    mSystemFanSpeed = getFanSpeed();
    mCurrentFanSpeed = mSystemFanSpeed;
    mLastTemp = max(getCpuTemp(), getMbTemp());
    mCpuFreq = cpugpu::getCpuMHz();
    mMinFanSpeed = calcFanSpeed();
}

int32_t temperatureManager::getCpuTemp() {
    if (mXboxVer != xboxConfig::xboxVersion16 && mXboxVer != xboxConfig::xboxVersion16b) {
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

int32_t temperatureManager::getMbTemp() {
    uint32_t tempMb = 0;
    HalReadSMBusByte(PIC_ADDRESS, MB_TEMP, &tempMb);
    return mXboxVer == xboxConfig::xboxVersion16 || mXboxVer == xboxConfig::xboxVersion16b ? (int32_t)(tempMb * 0.8f)
                                                                                            : (int32_t)tempMb;
}

int32_t temperatureManager::getFanSpeed() {
    uint32_t fanSpeed = 0;
    HalReadSMBusByte(PIC_ADDRESS, FAN_READBACK, &fanSpeed);
    return (int32_t)fanSpeed;
}

void temperatureManager::setFanSpeed(uint32_t fanspeed) {
    if (fanspeed > 50) {
        return;
    }
    if (mCurrentFanSpeed == fanspeed) {
        return;
    }

    UCHAR override = 1;
    if (fanspeed <= 0) override = 0;

    mCurrentFanSpeed = fanspeed;
    for (uint32_t i = 0; i < 2; i++) {
        HalWriteSMBusByte(PIC_ADDRESS, FAN_MODE, override);
        Sleep(1);
        HalWriteSMBusByte(PIC_ADDRESS, FAN_REGISTER, fanspeed);
        Sleep(1);
    }
}

void temperatureManager::refresh(bool instant) {
    // We dont want to refresh every frame as this will lock up the smbus
    if (!instant && mCounter != 50) {
        mCounter++;
        return;
    }
    mCounter = 0;

    mMinFanSpeed = calcFanSpeed();
    setFanSpeed(mMinFanSpeed);
}
