#include "flashBackupScene.h"
#include "sceneManager.h"

#include "..\drawing.h"
#include "..\ssfn.h"
#include "..\component.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\theme.h"
#include "..\stringUtility.h"
#include "..\Threads\flashBackup.h"

flashBackupScene::flashBackupScene(bool recovery)
{
	mProgress = strdup("");
	mDone = false;
	flashBackup::startThread(recovery);
}

flashBackupScene::~flashBackupScene()
{
	free(mProgress);
}

void flashBackupScene::update()
{
	// Back Action

	if (mDone == true)
	{
		if (inputManager::buttonPressed(ButtonB))
		{
			sceneManager::popScene(sceneResultDone);
		}
		return;
	}

	processResponse(flashBackup::getResponse());
	if (flashBackup::completed() == true)
	{
		flashBackup::closeThread();
		mDone = true;
	}
}

void flashBackupScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please wait...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 193, yPos);

	yPos += 40;

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	if (mDone == true)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
	}
}

void flashBackupScene::setProgress(const char* message)
{
	free(mProgress);
	mProgress = strdup(message);
}

void flashBackupScene::processResponse(flashBackup::flashBackupResponse response)
{
	uint32_t currentStep;
	uint32_t totalSteps;
	flashBackup::getProgress(currentStep, totalSteps);

	if (response == flashBackup::flashBackupResponseNone)
	{
		setProgress("");
	}
	else if (response == flashBackup::flashBackupProcessing)
	{
		setProgress("Processing");
	}
	else if (response == flashBackup::flashBackupFailedToCreateDirectory)
	{
		setProgress("Failed to create file");
	}
	else if (response == flashBackup::flashBackupFailedToCreateFile)
	{
		setProgress("Failed to create file");
	}
	else if (response == flashBackup::flashBackupFailedToWriteFile)
	{
		setProgress("Failed to write to file");
	}
	else if (response == flashBackup::flashBackupReading)
	{
		char* progress = stringUtility::formatString("Step %i of %i - Reading", currentStep, totalSteps);
		setProgress(progress);
		free(progress);
	}
	else if (response == flashBackup::flashBackupWriting)
	{
		char* progress = stringUtility::formatString("Step %i of %i - Writing", currentStep, totalSteps);
		setProgress(progress);
		free(progress);
	}
	else if (response == flashBackup::flashBackupDone)
	{
		setProgress("Done");
	}
}