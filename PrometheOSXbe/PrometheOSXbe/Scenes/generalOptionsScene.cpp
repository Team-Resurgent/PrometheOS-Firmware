#include "generalOptionsScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\fileSystem.h"

generalOptionsScene::generalOptionsScene()
{
	mSelectedControl = 0;
}

void generalOptionsScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::openScene(sceneItemPrometheOsSettingsScene);
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < 3 ? mSelectedControl + 1 : 4;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 4; 
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft) || inputManager::buttonPressed(ButtonX))
	{
		if (mSelectedControl == 0)
		{
			uint8_t autoBootDelay = settingsManager::getAutoBootDelay();
			if (autoBootDelay > 5)
			{
				autoBootDelay -= 5;
			}
			else if (autoBootDelay > 0)
			{
				autoBootDelay--;
			}
			else
			{
				autoBootDelay = 30;
			}
			settingsManager::setAutoBootDelay(autoBootDelay);
		}
		else if (mSelectedControl == 1)
		{
			uint8_t ledColor = settingsManager::getLedColor();
			ledColor = (ledColor >= 1) ? (ledColor - 1) : 7;
			settingsManager::setLedColor(ledColor);
			xenium::setLedColor((xenium::ledColorEnum)ledColor);
		}
		else if (mSelectedControl == 2)
		{
			uint8_t lcdEnabled = settingsManager::getLcdEnabled();
			settingsManager::setLcdEnabled(!lcdEnabled);
		}
		else if (mSelectedControl == 3)
		{
			uint8_t lcdBacklight = settingsManager::getLcdBacklight();
			lcdBacklight = (lcdBacklight >= 4) ? (lcdBacklight - 4) : 100;
			settingsManager::setLcdBacklight(lcdBacklight);
		}
		else if (mSelectedControl == 4)
		{
			uint8_t lcdContrast = settingsManager::getLcdContrast();
			lcdContrast = (lcdContrast >= 4) ? (lcdContrast - 4) : 100;
			settingsManager::setLcdContrast(lcdContrast);
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		if (mSelectedControl == 0)
		{
			uint8_t autoBootDelay = settingsManager::getAutoBootDelay();
			if (autoBootDelay >= 30)
			{
				autoBootDelay = 0;
			}
			else if (autoBootDelay >= 5)
			{
				autoBootDelay += 5;
			}
			else
			{
				autoBootDelay++;
			}
			settingsManager::setAutoBootDelay(autoBootDelay);
		}
		else if (mSelectedControl == 1)
		{
			uint8_t ledColor = settingsManager::getLedColor();
			ledColor = (ledColor >= 7) ? 0 : (ledColor + 1);
			settingsManager::setLedColor(ledColor);
			xenium::setLedColor((xenium::ledColorEnum)ledColor);
		}
		else if (mSelectedControl == 2)
		{
			uint8_t lcdEnabled = settingsManager::getLcdEnabled();
			settingsManager::setLcdEnabled(!lcdEnabled);
		}
		else if (mSelectedControl == 3)
		{
			uint8_t lcdBacklight = settingsManager::getLcdBacklight();
			lcdBacklight = (lcdBacklight >= 100) ? 0 : (lcdBacklight + 4);
			settingsManager::setLcdBacklight(lcdBacklight);
		}
		else if (mSelectedControl == 4)
		{
			uint8_t lcdContrast = settingsManager::getLcdContrast();
			lcdContrast = (lcdContrast >= 100) ? 0 : (lcdContrast + 4);
			settingsManager::setLcdContrast(lcdContrast);
		}
	}
}

void generalOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "General Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t itemCount = 4; 
	int32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	const char* ledColors[] = { "Off", "Red", "Green", "Yellow", "Blue", "Purple", "Turquoise", "White" };

	char* autoBootDelay = stringUtility::formatString("Auto Boot Delay: %i Seconds", settingsManager::getAutoBootDelay());
	char* ledColor = stringUtility::formatString("LED Color: %s", ledColors[settingsManager::getLedColor()]);
	char* lcdEnabled = stringUtility::formatString("LCD Enabled: %s", settingsManager::getLcdEnabled() ? "Yes" : "No");
	char* lcdBacklight = stringUtility::formatString("LCD Backlight: %i%%", settingsManager::getLcdBacklight());
	char* lcdContrast = stringUtility::formatString("LCD Contrast: %i%%", settingsManager::getLcdContrast());

	component::button(mSelectedControl == 0, false, autoBootDelay, 193, yPos, 322, 30);
	yPos += 40;
	component::button(mSelectedControl == 1, false, ledColor, 193, yPos, 322, 30);
	yPos += 40;
	component::button(mSelectedControl == 2, false, lcdEnabled, 193, yPos, 322, 30);
	yPos += 40;
	component::button(mSelectedControl == 3, false, lcdBacklight, 193, yPos, 322, 30);
	yPos += 40;
	component::button(mSelectedControl == 4, false, lcdContrast, 193, yPos, 322, 30);

	free(lcdContrast);
	free(lcdBacklight);
	free(lcdEnabled);
	free(ledColor);
	free(autoBootDelay);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}