#include "lcdOptionsScene.h"
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

lcdOptionsScene::lcdOptionsScene()
{
	mSelectedControl = 0;
}

void lcdOptionsScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		settingsManager::loadSettings();
		sceneManager::popScene();
		return;
	}

	if (settingsManager::shouldSave() && inputManager::buttonPressed(ButtonA))
	{
		settingsManager::saveSettings();
	}

	int32_t itemCount = 5;
	if (context::getModchip()->getLcdModelCount(false) == 0)
	{
		itemCount = itemCount - 1;
	}
	if (context::getModchip()->getLcdAddressCount(false) == 0)
	{
		itemCount = itemCount - 1;
	}
	if (context::getModchip()->getSupportInfo(false).supportsLcdBacklight == false)
	{
		itemCount = itemCount - 1;
	}
	if (context::getModchip()->getSupportInfo(false).supportsLcdContrast == false)
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

		if (context::getModchip()->getLcdModeCount() > 0 && mSelectedControl == index)
		{
			uint8_t lcdMode = settingsManager::getLcdMode(false);
			lcdMode = lcdMode > 0 ? (lcdMode - 1) : (context::getModchip()->getLcdModeCount() - 1);
			settingsManager::setLcdMode(lcdMode);
			settingsManager::setLcdModel(0);
			settingsManager::setLcdAddress(0);
			return;
		}

		if (context::getModchip()->getLcdModelCount(false) > 0)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdModel = settingsManager::getLcdModel(false);
				lcdModel = lcdModel > 0 ? (lcdModel - 1) : (context::getModchip()->getLcdModelCount(false) - 1);
				settingsManager::setLcdModel(lcdModel);
				settingsManager::setLcdAddress(0);
				return;
			}
		}

		if (context::getModchip()->getLcdAddressCount(false) > 0)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdAddress = settingsManager::getLcdAddress(false);
				lcdAddress = lcdAddress > 0 ? (lcdAddress - 1) : (context::getModchip()->getLcdAddressCount(false) - 1);
				settingsManager::setLcdAddress(lcdAddress);
				return;
			}
		}
		
		if (context::getModchip()->getSupportInfo(false).supportsLcdBacklight == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdBacklight = settingsManager::getLcdBacklight(false);
				lcdBacklight = (lcdBacklight >= 4) ? (lcdBacklight - 4) : 100;
				settingsManager::setLcdBacklight(lcdBacklight);
				return;
			}
		}

		if (context::getModchip()->getSupportInfo(false).supportsLcdContrast == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdContrast = settingsManager::getLcdContrast(false);
				lcdContrast = (lcdContrast >= 4) ? (lcdContrast - 4) : 100;
				settingsManager::setLcdContrast(lcdContrast);
				return;
			}
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		int index = 0;

		if (context::getModchip()->getLcdModeCount() > 0 && mSelectedControl == index)
		{
			uint8_t lcdMode = settingsManager::getLcdMode(false);
			lcdMode = (lcdMode < (context::getModchip()->getLcdModeCount()) - 1) ? lcdMode + 1 : 0;
			settingsManager::setLcdMode(lcdMode);
			settingsManager::setLcdModel(0);
			settingsManager::setLcdAddress(0);
			return;
		}

		if (context::getModchip()->getLcdModelCount(false) > 0)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdModel = settingsManager::getLcdModel(false);
				lcdModel = (lcdModel < (context::getModchip()->getLcdModelCount(false) - 1)) ? lcdModel + 1 : 0;
				settingsManager::setLcdModel(lcdModel);
				settingsManager::setLcdAddress(0);
				return;
			}
		}

		if (context::getModchip()->getLcdAddressCount(false) > 0)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdAddress = settingsManager::getLcdAddress(false);
				lcdAddress = (lcdAddress < (context::getModchip()->getLcdAddressCount(false) - 1)) ? lcdAddress + 1 : 0;
				settingsManager::setLcdAddress(lcdAddress);
				return;
			}
		}

		if (context::getModchip()->getSupportInfo(false).supportsLcdBacklight == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdBacklight = settingsManager::getLcdBacklight(false);
				lcdBacklight = (lcdBacklight >= 100) ? 0 : (lcdBacklight + 4);
				settingsManager::setLcdBacklight(lcdBacklight);
				return;
			}
		}

		if (context::getModchip()->getSupportInfo(false).supportsLcdContrast == true)
		{
			index++;

			if (mSelectedControl == index)
			{
				uint8_t lcdContrast = settingsManager::getLcdContrast(false);
				lcdContrast = (lcdContrast >= 100) ? 0 : (lcdContrast + 4);
				settingsManager::setLcdContrast(lcdContrast);
				return;
			}
		}
	}
}

void lcdOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "LCD Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	uint8_t lcdMode = settingsManager::getLcdMode(false);
	lcdMode = min(context::getModchip()->getLcdModeCount() - 1, lcdMode);
	char* lcdModeTempString = context::getModchip()->getLcdModeString(lcdMode);
	char* lcdModeString = stringUtility::formatString("LCD Mode: %s", lcdModeTempString);
	free(lcdModeTempString);

	uint8_t lcdModel = settingsManager::getLcdModel(false);
	lcdModel = min(context::getModchip()->getLcdModelCount(false) - 1, lcdModel);
	char* lcdModelTempString = context::getModchip()->getLcdModelString(false, lcdModel);
	char* lcdModelString = stringUtility::formatString("LCD Model: %s", lcdModelTempString);
	free(lcdModelTempString);

	uint8_t lcdAddress = settingsManager::getLcdAddress(false);
	lcdAddress = min(context::getModchip()->getLcdAddressCount(false) - 1, lcdAddress);
	char* lcdAddressTempString = context::getModchip()->getLcdAddressString(false, lcdAddress);
	char* lcdAddressString = stringUtility::formatString("LCD Address: %s", lcdAddressTempString);
	free(lcdAddressTempString);

	char* lcdBacklight = stringUtility::formatString("LCD Backlight: %i%%", settingsManager::getLcdBacklight(false));
	char* lcdContrast = stringUtility::formatString("LCD Contrast: %i%%", settingsManager::getLcdContrast(false));

	pointerVector<char*>* menuItems = new pointerVector<char*>(false);

	menuItems->add(strdup(lcdModeString));
	
	if (settingsManager::getLcdMode(false) > 0)
	{
		if (context::getModchip()->getLcdModelCount(false) > 0)
		{
			menuItems->add(strdup(lcdModelString));
		}
		if (context::getModchip()->getLcdAddressCount(false) > 0)
		{
			menuItems->add(strdup(lcdAddressString));
		}
		if (context::getModchip()->getSupportInfo(false).supportsLcdBacklight == true)
		{
			menuItems->add(strdup(lcdBacklight));
		}
		if (context::getModchip()->getSupportInfo(false).supportsLcdContrast == true)
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
			char* menuText = menuItems->get(index);
			component::button(mSelectedControl == index, false, menuText, 193, yPos, 322, 30);
			yPos += 40;
		}
	}

	free(lcdContrast);
	free(lcdBacklight);
	free(lcdAddressString);
	free(lcdModelString);
	free(lcdModeString);

	delete(menuItems);

	char* options = stringUtility::formatString("\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value%s", settingsManager::shouldSave() ? " or \xC2\xA1 Apply" : "");
	drawing::drawBitmapString(context::getBitmapFontSmall(), options, theme::getFooterTextColor(), 40, theme::getFooterY());
	free(options);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}