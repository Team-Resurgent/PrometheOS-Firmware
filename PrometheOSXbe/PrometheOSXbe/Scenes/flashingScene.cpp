#include "flashingScene.h"

#include "..\drawing.h"
#include "..\ssfn.h"
#include "..\component.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\theme.h"
#include "..\flash.h"

flashingScene::flashingScene(const char* filePath, const char* bankName, uint8_t ledColor)
{
	mProgress = strdup("");
	mDone = false;
	mSceneResult = sceneResultNone;
	flash::startThread(filePath, bankName, ledColor);
}

flashingScene::~flashingScene()
{
	free(mProgress);
}

void flashingScene::update()
{
	// Back Action

	if (mDone == true && inputManager::buttonPressed(ButtonB))
	{
		mSceneResult = sceneResultDone;
	}

	if (mDone == false)
	{
		processResponse(flash::getResponse());
		if (flash::completed() == true)
		{
			flash::closeThread();
			mDone = true;
		}
	}
}

void flashingScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please wait...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 193, yPos);

	yPos += 40;

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", mDone == true ? theme::getFooterTextColor() : theme::getTextDisabledColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

sceneResult flashingScene::getSceneResult()
{
	return mSceneResult;
}

void flashingScene::setProgress(const char* message)
{
	free(mProgress);
	mProgress = strdup(message);
}

void flashingScene::processResponse(flash::flashResponse response)
{
	if (response == flash::flashResponseNone)
	{
		setProgress("");
	}
	else if (response == flash::flashProcessing)
	{
		setProgress("Processing");
	}
	else if (response == flash::flashFailedToLoadFile)
	{
		setProgress("Failed to load file");
	}
	else if (response == flash::flashOptimizing)
	{
		setProgress("Optimizing");
	}
	else if (response == flash::flashNotEnoughSlots)
	{
		setProgress("Not enough slots");
	}
	else if (response == flash::flashErasing)
	{
		setProgress("Erasing");
	}
	else if (response == flash::flashWriting)
	{
		setProgress("Writing");
	}
	else if (response == flash::flashVerifying)
	{
		setProgress("Verifying");
	}
	else if (response == flash::flashVerificationFailed)
	{
		setProgress("Verification failed");
	}
	else if (response == flash::flashDone)
	{
		setProgress("Done");
	}
}