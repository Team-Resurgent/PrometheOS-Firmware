#pragma once

#include "xboxInternals.h"

class lcd
{
public:
	static void hideDisplay();
	static void showDisplay();
	static void hideCursor();
	static void showUnderlineCursor();
	static void showBlockCursor();
	static void showInvertedCursor();
	static void backspace();
	static void lineFeed();
	static void deleteInPlace();
	static void formFeed();
	static void carriageReturn();
	static void setBacklight(unsigned int level);
	static void setContrast(unsigned int level);
	static void setCursorPosition(  unsigned int col, unsigned int row);
	static void horizontalBarGraph( unsigned char graphIndex, unsigned char style, unsigned char startCol, unsigned char endCol, unsigned char length, unsigned char row);
	static void scrollOn();
	static void scrollOff();
	static void wrapOn();
	static void wrapOff();
	static void reboot();
	static void cursorUp();
	static void cursorDown();
	static void cursorRight();
	static void cursorLeft();
	static void largeBlockNumber( unsigned char style, unsigned char column, unsigned char number);

	static void init(uint8_t backLight, uint8_t contrast);
	static void printMessage(const char* message);
};
