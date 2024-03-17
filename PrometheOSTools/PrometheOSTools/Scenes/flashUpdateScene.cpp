#include "flashUpdateScene.h"
#include "sceneManager.h"

#include "..\drawing.h"
#include "..\ssfn.h"
#include "..\component.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\theme.h"
#include "..\stringUtility.h"
#include "..\Threads\flashUpdate.h"

flashUpdateScene::flashUpdateScene(bool recovery, const char* filePath)
{
	mProgress = strdup("Ready");
	mStarted = false;
	mDone = false;
	mFailed = false;
	mSceneResult = sceneResultNone;
	mFilePath = strdup(filePath);
	mRecovery = recovery;
}

flashUpdateScene::~flashUpdateScene()
{
	free(mProgress);
	free(mFilePath);
}

void flashUpdateScene::update()
{
	if (mStarted == false)
	{
		if (inputManager::buttonPressed(ButtonB))
		{
			mSceneResult = sceneResultCancelled;
		}
		if (inputManager::buttonPressed(ButtonX))
		{
			mStarted = true;
			flashUpdate::startThread(mRecovery, mFilePath);
		}
		return;
	}

	// Back Action

	if (mDone == true)
	{
		if (mFailed == false && inputManager::buttonPressed(ButtonB))
		{
			if (mRecovery == true)
			{
				mSceneResult = sceneResultDone;
			}
			else
			{
				utils::reboot();
			}
		}
		else if (mFailed == true && inputManager::buttonPressed(ButtonB))
		{
			mDone = false;
			mFailed = false;
			flashUpdate::startThread(mRecovery, mFilePath);
		}
		return;
	}


	if (mDone == false)
	{
		processResponse(flashUpdate::getResponse());
		if (flashUpdate::completed() == true)
		{
			flashUpdate::closeThread();
			mDone = true;
		}
	}
}

void flashUpdateScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please wait...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 193, yPos);

	yPos += 40;

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	if (mStarted == false)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA3 To Confirm Update", theme::getFooterTextColor(), horizAlignmentLeft, 40, theme::getFooterY(), 640);
		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
	}
	else if (mDone == true)
	{
		if (mRecovery == true)
		{
			drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mFailed == true ? "\xC2\xA2 Retry" : "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
		}
		else
		{
			drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mFailed == true ? "\xC2\xA2 Retry" : "\xC2\xA2 Reboot", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
		}
	}
}

sceneResult flashUpdateScene::getSceneResult()
{
	return mSceneResult;
}

void flashUpdateScene::setProgress(const char* message)
{
	free(mProgress);
	mProgress = strdup(message);
}

void flashUpdateScene::processResponse(flashUpdate::flashUpdateResponse response)
{
	uint32_t currentStep;
	uint32_t totalSteps;
	flashUpdate::getProgress(currentStep, totalSteps);

	if (response == flashUpdate::flashUpdateResponseNone)
	{
		setProgress("");
	}
	else if (response == flashUpdate::flashUpdateProcessing)
	{
		setProgress("Processing");
	}
	else if (response == flashUpdate::flashUpdateFailedToLoadFile)
	{
		setProgress("Failed to load file");
	}
	else if (response == flashUpdate::flashUpdateErasing)
	{
		char* progress = stringUtility::formatString("Step %i of %i - Erasing", currentStep, totalSteps);
		setProgress(progress);
		free(progress);
	}
	else if (response == flashUpdate::flashUpdateWriting)
	{
		char* progress = stringUtility::formatString("Step %i of %i - Writing", currentStep, totalSteps);
		setProgress(progress);
		free(progress);
	}
	else if (response == flashUpdate::flashUpdateVerifying)
	{
		char* progress = stringUtility::formatString("Step %i of %i - Verifying", currentStep, totalSteps);
		setProgress(progress);
		free(progress);
	}
	else if (response == flashUpdate::flashUpdateVerificationFailed)
	{
		mFailed = true;
		char* progress = stringUtility::formatString("Step %i of %i - Failed", currentStep, totalSteps);
		setProgress(progress);
		free(progress);
	}
	else if (response == flashUpdate::flashUpdateDone)
	{
		setProgress("Done");
	}
}