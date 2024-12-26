#include "ledOptionsScene.h"
#include "sceneManager.h"

#include "..\utils.h"
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

ledOptionsScene::ledOptionsScene()
{
	mSelectedControl = 0;

	utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_SEL, MODXO_NVM_REGISTER_RGB_STATUS_PF);
	uint8_t statusFormat = utils::ioInputByte(MODXO_REGISTER_NVM_CONFIG_VAL);
	utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_SEL, MODXO_NVM_REGISTER_RGB_STRIP1_PF);
	uint8_t stripFormat = utils::ioInputByte(MODXO_REGISTER_NVM_CONFIG_VAL);

	mStatusFormat = min(statusFormat, MODXO_PIXEL_FORMAT_GBR);
	mStripFormat = min(stripFormat, MODXO_PIXEL_FORMAT_GBR);
}

void ledOptionsScene::update()
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
			mStatusFormat = mStatusFormat > 0 ? (mStatusFormat - 1) : MODXO_PIXEL_FORMAT_GBR;
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_SEL, MODXO_NVM_REGISTER_RGB_STATUS_PF);
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_VAL, mStatusFormat);
		}
		else if (mSelectedControl == 1)
		{
			mStripFormat = mStripFormat > 0 ? (mStripFormat - 1) : MODXO_PIXEL_FORMAT_GBR;
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_SEL, MODXO_NVM_REGISTER_RGB_STRIP1_PF);
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_VAL, mStripFormat);
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		if (mSelectedControl == 0)
		{
			mStatusFormat = (mStatusFormat < MODXO_PIXEL_FORMAT_GBR) ? mStatusFormat + 1 : 0;
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_SEL, MODXO_NVM_REGISTER_RGB_STATUS_PF);
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_VAL, mStatusFormat);
		}
		else if (mSelectedControl == 1)
		{
			mStripFormat = (mStripFormat < MODXO_PIXEL_FORMAT_GBR) ? mStripFormat + 1 : 0;
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_SEL, MODXO_NVM_REGISTER_RGB_STRIP1_PF);
			utils::ioOutputByte(MODXO_REGISTER_NVM_CONFIG_VAL, mStripFormat);
		}
	}
}

void ledOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "LED Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	pointerVector<char*>* menuItems = new pointerVector<char*>(false);

	char* pixel_formats[] = { "GRB", "RGB", "BRG", "RBG", "BGR", "GBR" };

	menuItems->add(stringUtility::formatString("Status Pixel Format: %s", pixel_formats[mStatusFormat]));
	menuItems->add(stringUtility::formatString("Strip Pixel Format: %s", pixel_formats[mStripFormat]));
	
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

	delete(menuItems);

	char* options = stringUtility::formatString("\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value%s", settingsManager::shouldSave() ? " or \xC2\xA1 Apply" : "");
	drawing::drawBitmapString(context::getBitmapFontSmall(), options, theme::getFooterTextColor(), 40, theme::getFooterY());
	free(options);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}