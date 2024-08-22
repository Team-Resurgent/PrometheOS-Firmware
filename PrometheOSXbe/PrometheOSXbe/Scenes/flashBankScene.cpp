#include "flashBankScene.h"
#include "sceneManager.h"

#include "..\drawing.h"
#include "..\ssfn.h"
#include "..\component.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\theme.h"
#include "..\Threads\flashBank.h"

flashBankScene::flashBankScene(const char* filePath, const char* bankName, uint8_t ledColor)
{
	mProgress = strdup("");
	mDone = false;
	flashBank::startThread(filePath, bankName, ledColor);
}

flashBankScene::~flashBankScene()
{
	free(mProgress);
}

void flashBankScene::update()
{
	// Back Action

	if (mDone == true && inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene(sceneResultDone);
		return;
	}

	if (mDone == false)
	{
		processResponse(flashBank::getResponse());
		if (flashBank::completed() == true)
		{
			flashBank::closeThread();
			mDone = true;
		}
	}
}

void flashBankScene::render()
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

void flashBankScene::setProgress(const char* message)
{
	free(mProgress);
	mProgress = strdup(message);
}

void flashBankScene::processResponse(flashBank::flashBankResponse response)
{
	if (response == flashBank::flashBankResponseNone)
	{
		setProgress("");
	}
	else if (response == flashBank::flashBankProcessing)
	{
		setProgress("Processing");
	}
	else if (response == flashBank::flashBankFailedToLoadFile)
	{
		setProgress("Failed to load file");
	}
	else if (response == flashBank::flashBankOptimizing)
	{
		setProgress("Optimizing");
	}
	else if (response == flashBank::flashBankNotEnoughSlots)
	{
		setProgress("Not enough slots");
	}
	else if (response == flashBank::flashBankErasing)
	{
		setProgress("Erasing");
	}
	else if (response == flashBank::flashBankWriting)
	{
		setProgress("Writing");
	}
	else if (response == flashBank::flashBankVerifying)
	{
		setProgress("Verifying");
	}
	else if (response == flashBank::flashBankVerificationFailed)
	{
		setProgress("Verification failed");
	}
	else if (response == flashBank::flashBankDone)
	{
		setProgress("Done");
	}
}