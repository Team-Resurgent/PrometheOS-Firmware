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
		sceneManager::popScene();
		return;
	}

	int32_t itemCount = 3;
	if (context::getModchip()->supportsLcdContrast() == false)
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
			uint8_t lcdEnableType = settingsManager::getLcdEnableType();
			lcdEnableType = lcdEnableType >= 1 ? (context::getModchip()->getLcdTypeCount() - 1) : (lcdEnableType - 1);
			settingsManager::setLcdEnableType(lcdEnableType + 1);
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

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		int index = 0;

		if (mSelectedControl == index)
		{
			uint8_t lcdEnableType = settingsManager::getLcdEnableType();
			lcdEnableType = (lcdEnableType >= (context::getModchip()->getLcdTypeCount() - 1)) ? 0 : (lcdEnableType + 1);
			settingsManager::setLcdEnableType(lcdEnableType);
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

void lcdOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "LCD Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	char* lcdEnableType = stringUtility::formatString("LCD Enabled: %s", settingsManager::getLcdEnableType() > 0 ? "Yes" : "No");
	char* lcdBacklight = stringUtility::formatString("LCD Backlight: %i%%", settingsManager::getLcdBacklight());
	char* lcdContrast = stringUtility::formatString("LCD Contrast: %i%%", settingsManager::getLcdContrast());

	pointerVector<char*>* menuItems = new pointerVector<char*>(false);

	menuItems->add(strdup(lcdEnableType));
	menuItems->add(strdup(lcdBacklight));
	if (context::getModchip()->supportsLcdContrast() == true)
	{
		menuItems->add(strdup(lcdContrast));
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
	free(lcdEnableType);

	delete(menuItems);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}