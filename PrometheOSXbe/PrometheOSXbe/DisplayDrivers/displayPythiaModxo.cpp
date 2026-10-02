#include "displayPythiaModxo.h"
#include "..\pythia.h"

static displayDriver* mInstance = NULL;


displayDriver* displayPythiaModxo::getInstance() {
    if (mInstance == NULL) {
        mInstance = new displayPythiaModxo();
    }
    return mInstance;
}

bool displayPythiaModxo::isSpi2Par() {
    return false;
}

char* displayPythiaModxo::getModel() {
    return strdup("Pythia");
}

void displayPythiaModxo::init() {
	pythia::clear_display(0x8);
    setBacklight(settingsManager::getLcdBacklight(true));
	setBacklight(settingsManager::getLcdContrast(true));
}

void displayPythiaModxo::printMessage(const char* message) {
	pythia::write_string(0x8, message);
}

void displayPythiaModxo::setCursorPosition(uint8_t row, uint8_t col) {
	pythia::set_cursor(0x8, row, col);
}

void displayPythiaModxo::getSupport(bool& lcdInfo, bool& backlight, bool& contrast) {
    lcdInfo = false;
    backlight = true;
    contrast = true;
}

void displayPythiaModxo::setBacklight(uint8_t backlight) {
	pythia::set_brightness(0x8, (uint8_t)(backlight * 2.55f));
}

void displayPythiaModxo::setContrast(uint8_t contrast) {
    pythia::set_contrast(0x8, (uint8_t)(contrast * 2.55f));
}

uint8_t displayPythiaModxo::getI2cAddressCount() {
    return 0;
}

uint8_t displayPythiaModxo::getI2cAddress(uint8_t index) {
    return 0;
}
