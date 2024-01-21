#include "flashingScene.h"

#include "..\drawing.h"
#include "..\ssfn.h"
#include "..\component.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\theme.h"

flashingScene::flashingScene(const char* filePath, const char* bankName, uint8_t ledColor)
{
	mFilePath = strdup(filePath);
	mBankName = strdup(bankName);
	mLedColor = ledColor;
	mProgress = strdup("");
	mStep = 0;
	mBankData = NULL;
	mBankId = 0;
	mSceneResult = sceneResultNone;
}

flashingScene::~flashingScene()
{
	free(mFilePath);
	free(mBankName);
	free(mProgress);
	delete(mBankData);
}

void flashingScene::update()
{
	// Back Action

	if (mStep == 10)
	{
		if (inputManager::buttonPressed(ButtonB))
		{
			mSceneResult = sceneResultDone;
		}
		return;
	}

	if (mStep == 0)
	{
		free(mProgress);
		mProgress = strdup("Processing");
		mStep++;
		return;
	}

	if (mStep == 1)
	{
		uint32_t fileHandle;
		if (fileSystem::fileOpen(mFilePath, fileSystem::FileModeRead, fileHandle))
		{
			uint32_t fileSize;
			if (fileSystem::fileSize(fileHandle, fileSize))
			{
				char* buffer = (char*)malloc(fileSize);
				uint32_t bytesRead;
				if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize)
				{
					mBankData = new utils::dataContainer(buffer, fileSize, fileSize);
				}
				free(buffer);
			}
			fileSystem::fileClose(fileHandle);
		}
		if (mBankData == NULL)
		{
			free(mProgress);
			mProgress = strdup("Failed to load file");
			mStep = 10;
			return;
		}
		mStep++;
		return;
	}

	if (mStep == 2)
	{
		free(mProgress);
		mProgress = strdup("Optimizing");
		mStep++;
		return;
	}

	if (mStep == 3)
	{
		uint8_t slotsNeeded = (uint8_t)(mBankData->size >> 18);
		settingsManager::optimizeBanks(slotsNeeded);
		if (settingsManager::tryGetFreeBank(slotsNeeded, mBankId) == false)
		{
			free(mProgress);
			mProgress = strdup("Not enough slots free");
			mStep = 10;
			return;
		}
		mStep++;
		return;
	}
	
	if (mStep == 4)
	{
		free(mProgress);
		mProgress = strdup("Erasing");
		mStep++;
		return;
	}
	
	if (mStep == 5)
	{
		settingsManager::eraseBank(mBankId, mBankData->size);
		mStep++;
		return;
	}
	
	if (mStep == 6)
	{
		free(mProgress);
		mProgress = strdup("Writing");
		mStep++;
		return;
	}
	
	if (mStep == 7)
	{
		settingsManager::writeBank(mBankId, mBankData, mBankName, mLedColor);
		mStep++;
		return;
	}
	
	if (mStep == 8)
	{
		free(mProgress);
		mProgress = strdup("Verifying");
		mStep++;
		return;
	}
	
	if (mStep == 9)
	{
		if (settingsManager::verifyBank(mBankId, mBankData) == false)
		{
			free(mProgress);
			mProgress = strdup("Verification failed");
			mStep = 10;
			return;
		}
		free(mProgress);
		mProgress = strdup("Done");
		mStep++;
	}
}

void flashingScene::render()
{
	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please wait...", theme::getTitleTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getTitleTextColor(), 193, yPos);

	yPos += 40;

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", mStep == 10 ? theme::getFooterTextColor() : theme::getTextDisabledColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

sceneResult flashingScene::getSceneResult()
{
	return mSceneResult;
}
