#include "generalOptionsScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
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
		sceneManager::popScene();
		return;
	}

	int32_t itemCount = 7;
	if (context::getModchip()->supportsLed() == false)
	{
		itemCount = itemCount - 1;
	}
	if (context::getModchip()->supportsLcd() == false)
	{
		itemCount = itemCount - 2;
	}
	if (context::getModchip()->supportsLcdContrast() == false)
	{
		itemCount = itemCount - 1;
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < itemCount ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : itemCount; 
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft) || inputManager::buttonPressed(ButtonX))
	{
		int index = 0;

		if (mSelectedControl == index)
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
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t musicVolume = settingsManager::getMusicVolume();
			musicVolume = (musicVolume >= 5) ? (musicVolume - 5) : 100;
			settingsManager::setMusicVolume(musicVolume);
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t soundVolume = settingsManager::getSoundVolume();
			soundVolume = (soundVolume >= 5) ? (soundVolume - 5) : 100;
			settingsManager::setSoundVolume(soundVolume);
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t minFanSpeed = settingsManager::getMinFanSpeed();
			minFanSpeed = (minFanSpeed >= 20) ? (minFanSpeed - 10) : 100;
			settingsManager::setMinFanSpeed(minFanSpeed);
			return;
		}
		
		if (context::getModchip()->supportsLed() == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t ledColor = settingsManager::getLedColor();
				ledColor = (ledColor >= 1) ? (ledColor - 1) : 7;
				settingsManager::setLedColor(ledColor);
				context::getModchip()->setLedColor(ledColor);
				return;
			}
		}

		if (context::getModchip()->supportsLcd() == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdEnabled = settingsManager::getLcdEnabled();
				settingsManager::setLcdEnabled(!lcdEnabled);
				return;
			}

			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdBacklight = settingsManager::getLcdBacklight();
				lcdBacklight = (lcdBacklight >= 4) ? (lcdBacklight - 4) : 100;
				settingsManager::setLcdBacklight(lcdBacklight);
				return;
			}

			if (context::getModchip()->supportsLcdContrast() == true)
			{
				index++;

				if (mSelectedControl == index)
				{
					uint8_t lcdContrast = settingsManager::getLcdContrast();
					lcdContrast = (lcdContrast >= 4) ? (lcdContrast - 4) : 100;
					settingsManager::setLcdContrast(lcdContrast);
					return;
				}
			}
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		int index = 0;

		if (mSelectedControl == index)
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
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t musicVolume = settingsManager::getMusicVolume();
			musicVolume = (musicVolume >= 100) ? 0 : (musicVolume + 5);
			settingsManager::setMusicVolume(musicVolume);
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t soundVolume = settingsManager::getSoundVolume();
			soundVolume = (soundVolume >= 100) ? 0 : (soundVolume + 5);
			settingsManager::setSoundVolume(soundVolume);
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t minFanSpeed = settingsManager::getMinFanSpeed();
			minFanSpeed = (minFanSpeed >= 100) ? 10 : (minFanSpeed + 10);
			settingsManager::setMinFanSpeed(minFanSpeed);
			return;
		}

		if (context::getModchip()->supportsLed() == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t ledColor = settingsManager::getLedColor();
				ledColor = (ledColor >= 7) ? 0 : (ledColor + 1);
				settingsManager::setLedColor(ledColor);
				context::getModchip()->setLedColor(ledColor);
				return;
			}
		}
		
		if (context::getModchip()->supportsLcd() == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdEnabled = settingsManager::getLcdEnabled();
				settingsManager::setLcdEnabled(!lcdEnabled);
				return;
			}

			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdBacklight = settingsManager::getLcdBacklight();
				lcdBacklight = (lcdBacklight >= 100) ? 0 : (lcdBacklight + 4);
				settingsManager::setLcdBacklight(lcdBacklight);
				return;
			}

			if (context::getModchip()->supportsLcdContrast() == true)
			{
				index++;

				if (mSelectedControl == index)
				{
					uint8_t lcdContrast = settingsManager::getLcdContrast();
					lcdContrast = (lcdContrast >= 100) ? 0 : (lcdContrast + 4);
					settingsManager::setLcdContrast(lcdContrast);
					return;
				}
			}
		}
	}
}

void generalOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "General Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	const char* ledColors[] = { "Off", "Red", "Green", "Yellow", "Blue", "Purple", "Turquoise", "White" };

	char* autoBootDelay = stringUtility::formatString("Auto Boot Delay: %i Seconds", settingsManager::getAutoBootDelay());
	char* musicVolume = stringUtility::formatString("Music Volume: %i%%", settingsManager::getMusicVolume());
	char* soundVolume = stringUtility::formatString("Sound Volume: %i%%", settingsManager::getSoundVolume());
	char* minFanSpeed = stringUtility::formatString("Min Fan Speed: %i%%", settingsManager::getMinFanSpeed());
	char* ledColor = stringUtility::formatString("LED Color: %s", ledColors[settingsManager::getLedColor()]);
	char* lcdEnabled = stringUtility::formatString("LCD Enabled: %s", settingsManager::getLcdEnabled() ? "Yes" : "No");
	char* lcdBacklight = stringUtility::formatString("LCD Backlight: %i%%", settingsManager::getLcdBacklight());
	char* lcdContrast = stringUtility::formatString("LCD Contrast: %i%%", settingsManager::getLcdContrast());

	pointerVector* menuItems = new pointerVector(false);
	menuItems->add(strdup(autoBootDelay));
	menuItems->add(strdup(musicVolume));
	menuItems->add(strdup(soundVolume));
	menuItems->add(strdup(minFanSpeed));

	if (context::getModchip()->supportsLed() == true)
	{
		menuItems->add(strdup(ledColor));
	}

	if (context::getModchip()->supportsLcd() == true)
	{
		menuItems->add(strdup(lcdEnabled));
		menuItems->add(strdup(lcdBacklight));
		if (context::getModchip()->supportsLcdContrast() == true)
		{
			menuItems->add(strdup(lcdContrast));
		}
	}

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)menuItems->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)menuItems->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)menuItems->count()) - start; 
	if (itemCount > 0)
	{
		uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		for (int32_t i = 0; i < itemCount; i++)
		{
			uint32_t index = start + i;
			char* menuText = (char*)menuItems->get(index);
			component::button(mSelectedControl == index, false, menuText, 193, yPos, 322, 30);
			yPos += 40;
		}
	}

	free(lcdContrast);
	free(lcdBacklight);
	free(lcdEnabled);
	free(ledColor);
	free(minFanSpeed);
	free(soundVolume);
	free(musicVolume);
	free(autoBootDelay);

	delete(menuItems);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}