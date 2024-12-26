#include "miscellaneousOptionsScene.h"
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

miscellaneousOptionsScene::miscellaneousOptionsScene()
{
	mSelectedControl = 0;
}

void miscellaneousOptionsScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	int32_t itemCount = 4;
	if (context::getModchip()->getSupportInfo(true).supportsLed == false)
	{
		itemCount = itemCount - 1;
	}

	if (xboxConfig::getHasRtcExpansion() == false)
	{
		itemCount = itemCount - 1;
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < (itemCount - 1) ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (itemCount - 1); 
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft) || inputManager::buttonPressed(ButtonX))
	{
		int index = 0;

		if (mSelectedControl == index)
		{
			uint8_t minFanSpeed = settingsManager::getMinFanSpeed();
			minFanSpeed = (minFanSpeed >= 20) ? (minFanSpeed - 10) : 100;
			settingsManager::setMinFanSpeed(minFanSpeed);
			return;
		}
		
		index++;

		if (mSelectedControl == index)
		{
			uint8_t driveSetup = settingsManager::getDriveSetup();
			driveSetup = (driveSetup > 0) ? (driveSetup - 1) : 3;
			settingsManager::setDriveSetup(driveSetup);
			return;
		}
		
		if (context::getModchip()->getSupportInfo(true).supportsLed == true)
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

		if (xboxConfig::getHasRtcExpansion() == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				bool rtcEnable = settingsManager::getRtcEnable();
				settingsManager::setRtcEnable(!rtcEnable);
				return;
			}
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		int index = 0;

		if (mSelectedControl == index)
		{
			uint8_t minFanSpeed = settingsManager::getMinFanSpeed();
			minFanSpeed = (minFanSpeed >= 100) ? 10 : (minFanSpeed + 10);
			settingsManager::setMinFanSpeed(minFanSpeed);
			return;
		}

		index++;

		if (mSelectedControl == index)
		{
			uint8_t driveSetup = settingsManager::getDriveSetup();
			driveSetup = (driveSetup >= 3) ? 0 : (driveSetup + 1);
			settingsManager::setDriveSetup(driveSetup);
			return;
		}

		if (context::getModchip()->getSupportInfo(true).supportsLed == true)
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

		if (xboxConfig::getHasRtcExpansion() == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				bool rtcEnable = settingsManager::getRtcEnable();
				settingsManager::setRtcEnable(!rtcEnable);
				return;
			}
		}
	}
}

void miscellaneousOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Miscellaneous Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	const char* ledColors[] = { "Off", "Red", "Green", "Yellow", "Blue", "Purple", "Turquoise", "White" };
	const char* driveModes[] = { "HDD & DVD", "HDD & No DVD (Legacy)", "HDD & No DVD (Modern)", "Dual HDD" };

	char* minFanSpeed = stringUtility::formatString("Min Fan Speed: %i%%", settingsManager::getMinFanSpeed());
	char* driveSetup = stringUtility::formatString("Drive Setup: %s", driveModes[settingsManager::getDriveSetup()]);
	char* ledColor = stringUtility::formatString("LED Color: %s", ledColors[settingsManager::getLedColor()]);
	char* rtcEnable = stringUtility::formatString("RTC Enabled: %s", settingsManager::getRtcEnable() == true ? "Yes" : "No");

	pointerVector<char*>* menuItems = new pointerVector<char*>(false);
	menuItems->add(strdup(minFanSpeed));
	menuItems->add(strdup(driveSetup));

	if (context::getModchip()->getSupportInfo(true).supportsLed == true)
	{
		menuItems->add(strdup(ledColor));
	}

	if (xboxConfig::getHasRtcExpansion() == true)
	{
		menuItems->add(strdup(rtcEnable));
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
			char* menuText = menuItems->get(index);
			component::button(mSelectedControl == index, false, menuText, 193, yPos, 322, 30);
			yPos += 40;
		}
	}

	free(rtcEnable);
	free(ledColor);
	free(driveSetup);
	free(minFanSpeed);

	delete(menuItems);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}