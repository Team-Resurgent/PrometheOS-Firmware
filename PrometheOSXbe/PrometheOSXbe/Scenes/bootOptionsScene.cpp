#include "bootOptionsScene.h"
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

bootOptionsScene::bootOptionsScene()
{
	mSelectedControl = 0;
}

void bootOptionsScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < 1 ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 1; 
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft) || inputManager::buttonPressed(ButtonX))
	{
		if (mSelectedControl == 0)
		{
			uint8_t splshDelay = settingsManager::getSplashDelay();
			if (splshDelay > 0)
			{
				splshDelay--;
			}
			else
			{
				splshDelay = 15;
			}
			settingsManager::setSplashDelay(splshDelay);
			return;
		}

		if (mSelectedControl == 1)
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
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		if (mSelectedControl == 0)
		{
			uint8_t splashDelay = settingsManager::getSplashDelay();
			if (splashDelay >= 15)
			{
				splashDelay = 0;
			}
			else
			{
				splashDelay++;
			}
			settingsManager::setSplashDelay(splashDelay);
			return;
		}

		if (mSelectedControl == 1)
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
	}
}

void bootOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Boot Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	char* splashDelay = stringUtility::formatString("Splash Boot Delay: %i Seconds", settingsManager::getSplashDelay());
	char* autoBootDelay = stringUtility::formatString("Auto Boot Delay: %i Seconds", settingsManager::getAutoBootDelay());
	
	pointerVector<char*>* menuItems = new pointerVector<char*>(false);
	menuItems->add(strdup(splashDelay));
	menuItems->add(strdup(autoBootDelay));

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

	free(autoBootDelay);
	free(splashDelay);

	delete(menuItems);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}