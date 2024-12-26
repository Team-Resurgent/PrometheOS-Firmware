#include "formatDriveScene.h"
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
#include "..\timeUtility.h"
#include "..\harddrive.h"
#include "..\Threads\hddFormat.h"
#include "..\Threads\hddInfo.h"
#include "..\XKUtils\XKHDD.h"

formatDriveScene::formatDriveScene(int32_t harddrive)
{
	mHarddrive = harddrive;
	mCounter = 0;
	mProgress = strdup("Please wait...");
	mIdeModel = strdup("Unknown");
	mIdeSerial = strdup("Unknown");
	mProcessing = false;
	mInitialized = false;
	mAllowFormat = false;
	hddInfo::startThread(!!harddrive);
}

formatDriveScene::~formatDriveScene()
{
	free(mProgress);
	free(mIdeModel);
	free(mIdeSerial);
}

void formatDriveScene::update()
{
	if (mInitialized == false)
	{
		return;
	}

	// Exit Action

	if (mProcessing == false && inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	if(!mAllowFormat) {
		return;
	}

	if (inputManager::buttonPressed(ButtonA))
	{
		if (mCounter >= 0 && mCounter < 1)
		{
			free(mProgress);
			mProgress = strdup("Please Confirm again...");
			mCounter++;
			return;
		}
	}

	if (mCounter == 1 && inputManager::buttonPressed(ButtonX)) {
		mCounter++;
		return;
	}

	if (mCounter == 2)
	{
		mProcessing = true;
		hddFormat::startThread(mHarddrive);
		free(mProgress);
		mProgress = strdup("Please wait...");
		mCounter++;
		return;
	}

	if (mCounter == 3 && hddFormat::completed() == true)
	{
		mProcessing = false;
		free(mProgress);
		hddFormat::hddFormatResponse response = hddFormat::getResponse();
		mProgress = strdup(response == hddFormat::hddFormatResponseFailure ? "Format failed." : "Format success.");
		mCounter = 0;
		return;
	}
}

void formatDriveScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	char* title = stringUtility::formatString("Format %s Drive...", mHarddrive == 0 ? "Primary" : "Secondary");
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), title, theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	free(title);

	int yPos = (context::getBufferHeight() - (30 + 30 + 40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	char* ideModel = stringUtility::formatString("Model: %s", mIdeModel);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), ideModel, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	free(ideModel);

	yPos += 30;

	char* ideSerial = stringUtility::formatString("Serial: %s", mIdeSerial);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), ideSerial, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	free(ideSerial);

	yPos += 30;

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getHeaderTextColor(), 193, yPos);

	yPos += 40;

	if (mInitialized == false)
	{
		if (hddInfo::completed() == true)
		{
			free(mIdeModel);
			free(mIdeSerial);
			mIdeModel = hddInfo::getModel();
			mIdeSerial = hddInfo::getSerial();
			hddInfo::closeThread();
			mInitialized = true;
			mAllowFormat = hddInfo::getResponse() != hddInfo::hddInfoResponseTimeout;

			free(mProgress);
			if(mAllowFormat) {
				mProgress = strdup("Please Confirm...");
			}
			else {
				mProgress = strdup("Unable to detect HDD");
			}
		}
	}

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	if(!mProcessing) {
		drawing::drawBitmapString(context::getBitmapFontSmall(), mCounter == 1 ? "\xC2\xA3 Confirm Again" : "\xC2\xA1 Confirm", mInitialized && mAllowFormat ? theme::getFooterTextColor() : theme::getTextDisabledColor(), 40, theme::getFooterY());
	}

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", mProcessing == false && mInitialized ? theme::getFooterTextColor() : theme::getTextDisabledColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}