#include "component.h"
#include "drawing.h"
#include "ssfn.h"
#include "theme.h"

#include <string>

void component::panel(uint32_t fill, uint32_t stroke, int x, int y, int width, int height)
{
	drawing::drawNinePatch("panel-fill", fill, x, y, width, height);
	drawing::drawNinePatch("panel-stroke", stroke, x, y, width, height);
}

void component::button(bool selected, bool active, const char* label, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	if (active)
	{
		if (selected) 
		{
			panel(theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveHoverTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (selected) 
		{
			panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveHoverTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveTextColor(), x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
		}
	}
}

void component::splitButton(bool selected, bool active, const char* label1, int label1Width, const char* label2, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label2, &textWidth, &textHeight);

	if (active)
	{
		if (selected) 
		{
			panel(theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveHoverTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (selected) 
		{
			panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveHoverTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveTextColor(), x + 10 + label1Width, y + ((height - textHeight) / 2) - 3);
		}
	}
}


void component::fileButton(bool selected, bool active, bool isFile, const char* label, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	if (active)
	{
		if (selected) 
		{
			panel(theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveHoverTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonActiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonActiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (selected) 
		{
			panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveHoverTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonInactiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else
		{
			panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveTextColor(), x + 30, y + ((height - textHeight) / 2) - 3);
			drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5", theme::getButtonInactiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
	}
}

void component::ledButton(bool selected, uint8_t ledColor, const char* label, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	if (selected) 
	{
		if (ledColor == 0)
		{
			panel(theme::getButtonLedOffHoverFillColor(), theme::getButtonLedOffHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedOffHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 1)
		{
			panel(theme::getButtonLedRedHoverFillColor(), theme::getButtonLedRedHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedRedHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 2)
		{
			panel(theme::getButtonLedGreenHoverFillColor(), theme::getButtonLedGreenHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedGreenHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 3)
		{
			panel(theme::getButtonLedYellowHoverFillColor(), theme::getButtonLedYellowHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedYellowHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 4)
		{
			panel(theme::getButtonLedBlueHoverFillColor(),theme::getButtonLedBlueHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedBlueHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 5)
		{
			panel(theme::getButtonLedPurpleHoverFillColor(),theme::getButtonLedPurpleHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedPurpleHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 6)
		{
			panel(theme::getButtonLedTurquoiseHoverFillColor(), theme::getButtonLedTurquoiseHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedTurquoiseHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 7)
		{
			panel(theme::getButtonLedWhiteHoverFillColor(), theme::getButtonLedWhiteHoverStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedWhiteHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
	}
	else
	{
		if (ledColor == 0)
		{
			panel(theme::getButtonLedOffFillColor(), theme::getButtonLedOffStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedOffTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 1)
		{
			panel(theme::getButtonLedRedFillColor(), theme::getButtonLedRedStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedRedTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 2)
		{
			panel(theme::getButtonLedGreenFillColor(), theme::getButtonLedGreenStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedGreenTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 3)
		{
			panel(theme::getButtonLedYellowFillColor(), theme::getButtonLedYellowStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedYellowTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 4)
		{
			panel(theme::getButtonLedBlueFillColor(), theme::getButtonLedBlueStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedBlueTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 5)
		{
			panel(theme::getButtonLedPurpleFillColor(), theme::getButtonLedPurpleStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedPurpleTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 6)
		{
			panel(theme::getButtonLedTurquoiseFillColor(), theme::getButtonLedTurquoiseStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedTurquoiseTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
		else if (ledColor == 7)
		{
			panel(theme::getButtonLedWhiteFillColor(), theme::getButtonLedWhiteStrokeColor(), x, y, width, height);
			drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedWhiteTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
		}
	}
}

void component::text(const char* label, bool disabled, horizAlignment hAlign, int x, int y, int width, int height)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	int xPos = x;
	if (hAlign == horizAlignmentCenter)
	{
		xPos = x + ((width - textWidth) / 2);
	}
	else if (hAlign == horizAlignmentRight)
	{
		xPos = x + (width - textWidth);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), label, disabled ? theme::getTextDisabledColor() : theme::getTextColor(), xPos, y + ((height - textHeight) / 2) - 3);
}

void component::textBox(const char* label, bool selected, bool disabled, horizAlignment hAlign, int x, int y, int width, int height, bool vAlignTop, bool solidBg)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

	int xPos = x + 16;
	if (hAlign == horizAlignmentCenter)
	{
		xPos = x + ((width - textWidth) / 2);
	}
	else if (hAlign == horizAlignmentRight)
	{
		xPos = (x + 16) + ((width - 32) - textWidth);
	}

	int yPos = vAlignTop ? (y + 16 - 3) : (y + ((height - textHeight) / 2) - 3);
	if (selected == true)
	{
		uint32_t fillColor = theme::getTextPanelHoverFillColor();
		if(solidBg) fillColor |= 0xff000000;
		panel(fillColor, theme::getTextPanelHoverStrokeColor(), x, y, width, height);
		drawing::drawBitmapString(context::getBitmapFontSmall(), label, disabled ? theme::getTextDisabledColor() : theme::getTextPanelHoverTextColor(), xPos, yPos);
	}
	else
	{
		uint32_t fillColor = theme::getTextPanelHoverFillColor();
		if(solidBg) fillColor |= 0xff000000;
		panel(fillColor, theme::getTextPanelStrokeColor(), x, y, width, height);
		drawing::drawBitmapString(context::getBitmapFontSmall(), label, disabled ? theme::getTextDisabledColor() : theme::getTextPanelTextColor(), xPos, yPos);
	}
	
}