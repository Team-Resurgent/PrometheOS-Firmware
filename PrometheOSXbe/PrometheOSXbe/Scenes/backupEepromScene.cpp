#include "backupEepromScene.h"
#include "sceneManager.h"
#include "mainScene.h"
#include "removeScene.h"

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
#include "..\XKUtils\XKEEPROM.h"

backupEepromScene::backupEepromScene()
{
	mProgress = strdup("Ready");
	mDone = false;
}

backupEepromScene::~backupEepromScene()
{
	free(mProgress);
}

void backupEepromScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA) && mDone == false)
	{
		free(mProgress);
		if (fileSystem::directoryCreate("HDD0-E:\\PrometheOS") == true && fileSystem::directoryCreate("HDD0-E:\\PrometheOS\\Backup") == true && fileSystem::directoryCreate("HDD0-E:\\PrometheOS\\Backup\\EEPROMs") == true)
		{
			XKEEPROM* eeprom = new XKEEPROM();
			eeprom->ReadFromXBOX();
			char* serialString = xboxConfig::getSerialString();
			char* versionString = xboxConfig::getXboxVersionString(); 
			char* fileVersionString = stringUtility::replace(versionString, "/", "-");
			char* filePath = stringUtility::formatString("HDD0-E:\\PrometheOS\\Backup\\EEPROMs\\%s %s.bin", fileVersionString, serialString); 
			if (eeprom->WriteToBINFile(filePath) == true)
			{
				mProgress = strdup("Backup completed");
			}
			else
			{
				mProgress = strdup("Backup failed");
			}
			delete(eeprom);
		}
		else
		{
			mProgress = strdup("Backup failed");
		}
		mDone = true;
	}
}

void backupEepromScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Backup EEPROM...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 193, yPos);

	yPos += 40;

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

    if (mDone == false) 
	{
        drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Backup", theme::getFooterTextColor(), 40, theme::getFooterY());
    }

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
