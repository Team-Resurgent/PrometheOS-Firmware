#include "displayHD44780Smartxx.h"
#include "..\context.h"

static displayDriver* mInstance = NULL;
static uint8_t mSysConVer = 0;
static bool mGotSysConVer = false;

static uint8_t getSysConVer()
{
    if (mGotSysConVer) {
        return mSysConVer;
    }
    mGotSysConVer = true;
    mSysConVer = utils::ioInputByte(0xf701) & 0x0f;
    return mSysConVer;
}

// The backlight register is 6-bit on the classic SmartXX and 7-bit on the OPX,
// so scale the 0-100 setting to the register's range for the fitted chip.
static uint8_t scaleBacklight(uint8_t backlight) {
    float scale = ((context::getModchipType() == modchipTypeSmartxxOpx) || (getSysConVer() == 0x08))
                      ? 1.27f
                      : 0.63f;
    return (uint8_t)(backlight * scale);
}

displayDriver* displayHD44780Smartxx::getInstance() {
    if (mInstance == NULL) {
        mInstance = new displayHD44780Smartxx();
    }
    return mInstance;
}

bool displayHD44780Smartxx::isSpi2Par() {
    return false;
}

char* displayHD44780Smartxx::getModel() {
    return strdup("HD44780");
}

void displayHD44780Smartxx::init() {
    utils::ioOutputByte(SMARTXX_REGISTER_IO, SMARTXX_DISP_EN5V);
    writeValue(0x33, 0);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(0x32, 0);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_FUNCTION_SET | SMARTXX_DISP_N_FLAG | SMARTXX_DISP_RE_FLAG, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_SEGRAM_SET, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_EXT_CONTROL | SMARTXX_DISP_NW_FLAG, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_FUNCTION_SET | SMARTXX_DISP_N_FLAG, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_CONTROL | SMARTXX_DISP_D_FLAG, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_CLEAR, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_ENTRY_MODE_SET | SMARTXX_DISP_ID_FLAG, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    writeValue(SMARTXX_DISP_HOME, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
    utils::ioOutputByte(SMARTXX_REGISTER_LCD_BL, scaleBacklight(settingsManager::getLcdBacklight(true)));
    Sleep(SMARTXX_LCD_DELAY);
    if (getSysConVer() == 0x08) {
        uint8_t contrast = (uint8_t)(settingsManager::getLcdContrast(true) * 1.27f);
        utils::ioOutputByte(SMARTXX_REGISTER_LCD_BL, contrast | 0x80);
    }
    Sleep(SMARTXX_LCD_DELAY);
}

void displayHD44780Smartxx::printMessage(const char* message) {
    for (int i = 0; i < (int)strlen(message); ++i) {
        writeValue(message[i], SMARTXX_DISP_DAT);
    }
}

void displayHD44780Smartxx::setCursorPosition(uint8_t row, uint8_t col) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    uint8_t cursor = SMARTXX_DISP_DDRAM_SET | (col + row_offsets[row]);
    writeValue(cursor, SMARTXX_DISP_CMD);
    Sleep(SMARTXX_LCD_DELAY);
}

void displayHD44780Smartxx::getSupport(bool& lcdInfo, bool& backlight, bool& contrast) {
    lcdInfo = false;
    backlight = true;
    contrast = getSysConVer() == 0x08;
}

void displayHD44780Smartxx::setBacklight(uint8_t backlight) {
    utils::ioOutputByte(SMARTXX_REGISTER_LCD_BL, scaleBacklight(backlight));
    Sleep(SMARTXX_LCD_DELAY);
}

void displayHD44780Smartxx::setContrast(uint8_t contrast) {
    uint8_t value = (uint8_t)(contrast * 1.27f);
    utils::ioOutputByte(SMARTXX_REGISTER_LCD_BL, value | 0x80);
    Sleep(SMARTXX_LCD_DELAY);
}

uint8_t displayHD44780Smartxx::getI2cAddressCount() {
    return 0;
}

uint8_t displayHD44780Smartxx::getI2cAddress(uint8_t index) {
    return 0;
}

// Private

void displayHD44780Smartxx::writeValue(uint8_t value, uint8_t command) {
    uint8_t oDataHigh = (value >> 2) & 0x28;
    oDataHigh |= (value >> 0) & 0x50;
    utils::ioOutputByte(SMARTXX_REGISTER_DISP_O, command | oDataHigh);
    utils::ioOutputByte(SMARTXX_REGISTER_DISP_O, command | SMARTXX_DISP_CON_E | oDataHigh);
    utils::ioOutputByte(SMARTXX_REGISTER_DISP_O, command | oDataHigh);
    Sleep(SMARTXX_LCD_DELAY);

    uint8_t oDataLow = (value << 2) & 0x28;
    oDataLow |= (value << 4) & 0x50;
    utils::ioOutputByte(SMARTXX_REGISTER_DISP_O, command | oDataLow);
    utils::ioOutputByte(SMARTXX_REGISTER_DISP_O, command | SMARTXX_DISP_CON_E | oDataLow);
    utils::ioOutputByte(SMARTXX_REGISTER_DISP_O, command | oDataLow);
    Sleep(SMARTXX_LCD_DELAY);
}
