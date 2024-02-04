#include "lcd.h"

#include "xboxInternals.h"
#include "xenium.h"
#include "stringUtility.h"

#define SPI_DELAY 10

void lcd::hideDisplay()
{
	xenium::sendCharacter(2);
	Sleep(SPI_DELAY);
}
	
void lcd::showDisplay()
{
	xenium::sendCharacter(3); 
	Sleep(SPI_DELAY);
}

void lcd::hideCursor()
{
	xenium::sendCharacter(4);
	Sleep(SPI_DELAY);
}
	
void lcd::showUnderlineCursor()
{
	xenium::sendCharacter(5);
	Sleep(SPI_DELAY);
}

void lcd::showBlockCursor()
{
	xenium::sendCharacter(6); 
	Sleep(SPI_DELAY);
}

void lcd::showInvertedCursor()
{
	xenium::sendCharacter(7); 
	Sleep(SPI_DELAY);
}

void lcd::backspace()
{
	xenium::sendCharacter(8); 
	Sleep(SPI_DELAY);
}

void lcd::lineFeed()
{
	xenium::sendCharacter(10);
	Sleep(SPI_DELAY);
}
	
void lcd::deleteInPlace()
{
	xenium::sendCharacter(11); 
	Sleep(SPI_DELAY);
}
	
void lcd::formFeed()
{
	xenium::sendCharacter(12); 
	Sleep(SPI_DELAY);
}
	
void lcd::carriageReturn()
{
	xenium::sendCharacter(13);
	Sleep(SPI_DELAY);
}

void lcd::setBacklight(unsigned int level)
{
	if (level > 25) level = 25;
	xenium::sendCharacter(14);
	xenium::sendCharacter(level*4);
	Sleep(SPI_DELAY);
}
	
void lcd::setContrast(unsigned int level)
{
	if (level > 25) level = 25;
	xenium::sendCharacter(15);
	xenium::sendCharacter(level*4);; 
	Sleep(SPI_DELAY);
}

void lcd::setCursorPosition(unsigned int col, unsigned int row )
{
	if (row > 3) row = 3;
	if (col > 19) col = 19; 
	xenium::sendCharacter(17);
	xenium::sendCharacter(col);
	xenium::sendCharacter(row);
	Sleep(SPI_DELAY);
}

void lcd::horizontalBarGraph(unsigned char graphIndex, unsigned char style, unsigned char startCol, unsigned char endCol, unsigned char length, unsigned char row)
{
	xenium::sendCharacter(18);
	xenium::sendCharacter(graphIndex);
	xenium::sendCharacter(style);
	xenium::sendCharacter(startCol);
	xenium::sendCharacter(endCol);
	xenium::sendCharacter(length);
	xenium::sendCharacter(row);
	Sleep(SPI_DELAY);
}

void lcd::scrollOn()
{
	xenium::sendCharacter(19);
	Sleep(SPI_DELAY);
}

void lcd::scrollOff()
{
	xenium::sendCharacter(20); 
	Sleep(SPI_DELAY);
}

void lcd::wrapOn()
{
	xenium::sendCharacter(23);
	Sleep(SPI_DELAY);
}

void lcd::wrapOff()
{
	xenium::sendCharacter(24);
	Sleep(SPI_DELAY);
}

void lcd::reboot()
{
	for (int i=0;i<9;i++)
	{
		xenium::sendCharacter(' ');
	}
	xenium::sendCharacter(26);
	xenium::sendCharacter(26); 
	Sleep(SPI_DELAY);
}

void lcd::cursorUp()
{
	xenium::sendCharacter(27);
	xenium::sendCharacter(91);
	xenium::sendCharacter(65);
	Sleep(SPI_DELAY);
}

void lcd::cursorDown()
{
	xenium::sendCharacter(27);
	xenium::sendCharacter(91);
	xenium::sendCharacter(66);
	Sleep(SPI_DELAY);
}

void lcd::cursorRight()
{
	xenium::sendCharacter(27);
	xenium::sendCharacter(91);
	xenium::sendCharacter(67);
	Sleep(SPI_DELAY);
}

void lcd::cursorLeft()
{
	xenium::sendCharacter(27);
	xenium::sendCharacter(91);
	xenium::sendCharacter(68);
	Sleep(SPI_DELAY);
}

void lcd::largeBlockNumber( unsigned char style, unsigned char column, unsigned char number)
{
	if (style > 1) style = 1;
	if (!style && column > 17) column = 17;
	if (style && column > 16) column = 16;
	if (number < '0') number = '0';
	if (number > '9') number = '9';
	xenium::sendCharacter(28);
	xenium::sendCharacter(style);
	xenium::sendCharacter(column);
	xenium::sendCharacter(number);
	Sleep(SPI_DELAY);
}

void lcd::init()
{
	showDisplay();
	hideCursor();
	scrollOff();
	wrapOff();
	setContrast(60/4);
	setBacklight(100/4);
}

void lcd::printMessage(const char* message)
{
  //HD44780

  uint8_t LCD[256] = 
                    { //HD44780 charset ROM code A00
                        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
                        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
                        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0xa4, 0x5d, 0x5e, 0x5f,
                        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
                        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0xb0, 0x20,
                        0x20, 0x20, 0x2c, 0x20, 0x22, 0x20, 0x20, 0x20, 0x5e, 0x20, 0x53, 0x3c, 0x20, 0x20, 0x5a, 0x20,
                        0x20, 0x27, 0x27, 0x22, 0x22, 0xa5, 0xb0, 0xb0, 0xb0, 0x20, 0x73, 0x3e, 0x20, 0x20, 0x7a, 0x59,
                        0xff, 0x21, 0x20, 0x20, 0x20, 0x5c, 0x7c, 0x20, 0x22, 0x20, 0x20, 0xff, 0x0E, 0x0A, 0x09, 0x08, // Custom characters
                        0xdf, 0x20, 0x20, 0x20, 0x27, 0xe4, 0x20, 0xa5, 0x20, 0x20, 0xdf, 0x3e, 0x20, 0x20, 0x20, 0x3f,
                        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x20, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
                        0x44, 0x4e, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x78, 0x30, 0x55, 0x55, 0x55, 0x55, 0x59, 0x20, 0xe2,
                        0x61, 0x61, 0x61, 0x61, 0xe1, 0x61, 0x20, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,
                        0x6f, 0x6e, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0xfd, 0x6f, 0x75, 0x75, 0xfb, 0xf5, 0x79, 0x20, 0x79
                    };
           
  for (int i = 0; i < (int)strlen(message); ++i)
  {
    uint8_t cLCD = message[i];
    cLCD = LCD[cLCD];
	xenium::sendCharacter(cLCD);
  }
  Sleep(SPI_DELAY);
}
