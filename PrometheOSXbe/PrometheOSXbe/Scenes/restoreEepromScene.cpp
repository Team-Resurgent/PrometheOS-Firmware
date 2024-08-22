#include "restoreEepromScene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\driveManager.h"
#include "..\fileSystem.h"
#include "..\pointerVector.h"
#include "..\settingsManager.h"
#include "..\theme.h"
#include "..\XKUtils\XKEEPROM.h"

restoreEepromScene::restoreEepromScene(const char* filePath)
{
	mFilePath = strdup(filePath);
	mProgress = strdup("Ready");
	mDone = false;
}

restoreEepromScene::~restoreEepromScene()
{
	free(mFilePath);
}

void restoreEepromScene::update()
{
	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		if (mDone == true)
		{
			utils::reboot();
		}
		else
		{
			sceneManager::popScene(sceneResultCancelled);
			return;
		}
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA) && mDone == false)
	{
		free(mProgress);
		XKEEPROM* eeprom = new XKEEPROM();
		if (eeprom->ReadFromBINFile(mFilePath, true) == true)
		{
			eeprom->WriteToXBOX();
			mProgress = strdup("Restored");
		}
		else
		{
			mProgress = strdup("Restore failed");
		}
		delete(eeprom);
		mDone = true;
	}
}

void restoreEepromScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Restore EEPROM...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (30 + 40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	char* fileName = fileSystem::getFileName(mFilePath);
	char* formattedFileName = stringUtility::formatString("EEPROM: %s", fileName);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), formattedFileName, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	free(formattedFileName);
	free(fileName);

	yPos += 30;

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 193, yPos);

	yPos += 40;

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

    if (mDone == false) 
	{
        drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Restore", theme::getFooterTextColor(), 40, theme::getFooterY());
    }
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mDone == true ?  "\xC2\xA2 Reboot" : "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}