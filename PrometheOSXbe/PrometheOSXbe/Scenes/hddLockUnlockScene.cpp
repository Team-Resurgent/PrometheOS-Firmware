#include "hddLockUnlockScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\audioPlayer.h"
#include "..\hdmiDevice.h"
#include "..\ftpServer.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\Threads\hddLockUnlock.h"
#include "..\Threads\hddInfo.h"

hddLockUnlockScene::hddLockUnlockScene()
{
	audioPlayer::pause(true);
	mStep = -1;
	mProgress = strdup("Please wait...");
	mIdeModel = strdup("Unknown");
	mIdeSerial = strdup("Unknown");
	mFailed = false;
	mInitialized = false;
	hddInfo::startThread();
}

hddLockUnlockScene::~hddLockUnlockScene()
{
	free(mProgress);
	free(mIdeModel);
	free(mIdeSerial);
}

void hddLockUnlockScene::update()
{
	if (mInitialized == false || mStep != -1)
	{
		return;
	}

	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		audioPlayer::pause(false);
		sceneManager::popScene();
		return;
	}

	// Lock / Unlock Action

	if (mFailed == false && inputManager::buttonPressed(ButtonA) && mStep == -1)
	{
		mStep = 0;
	}
}

void hddLockUnlockScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "HDD Lock / Unlock...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

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
			hddInfo::hddInfoResponse response = hddInfo::getResponse();
			if (response == hddInfo::hddInfoResponseFailureEeprom)
			{
				setProgress("Unable to get info");
				mFailed = true;
			}
			else if (response == hddInfo::hddInfoResponseLocked)
			{
				setProgress("Locked");
				mHddLocked = true;
			}
			else if (response == hddInfo::hddInfoResponseUnlocked)
			{
				setProgress("Unlocked");
				mHddLocked = false;
			}
			free(mIdeModel);
			free(mIdeSerial);
			mIdeModel = hddInfo::getModel();
			mIdeSerial = hddInfo::getSerial();
			hddInfo::closeThread();
			mInitialized = true;
		}
	} 
	else if (mHddLocked)
	{
		if (mStep == 0)
		{
			setProgress("Unlocking...");
			mStep = 1;
		}
		else if (mStep == 1)
		{
			hddLockUnlock::startThread(hddLockUnlock::hddLockUnlockActionUnlock);
			mStep = 2;
		}
		else if (mStep == 2)
		{
			processResponse(hddLockUnlock::getResponse());
			if (hddLockUnlock::completed() == true)
			{
				hddLockUnlock::hddLockUnlockResponse response = hddLockUnlock::getResponse(); 
				if (response == hddLockUnlock::hddLockUnlockResponseUnlockedVsc)
				{
					hddLockUnlock::closeThread();
					ftpServer::close();
					sceneManager::pushScene(sceneItemHddPasswordScene);
					return;
				}
				hddLockUnlock::closeThread();
				mStep = -1;
			}
		}
	}
	else
	{
		if (mStep == 0)
		{
			setProgress("Locking...");
			mStep = 1;
		}
		else if (mStep == 1)
		{
			hddLockUnlock::startThread(hddLockUnlock::hddLockUnlockActionLock);
			mStep = 2;
		}
		else if (mStep == 2)
		{
			processResponse(hddLockUnlock::getResponse());
			if (hddLockUnlock::completed() == true)
			{
				hddLockUnlock::closeThread();
				mStep = -1;
			}
		}
	}

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Toggle HDD Lock", mInitialized ? theme::getFooterTextColor() : theme::getTextDisabledColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", mStep == -1 ? theme::getFooterTextColor() : theme::getTextDisabledColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void hddLockUnlockScene::setProgress(const char* message)
{
	free(mProgress);
	mProgress = strdup(message);
}

void hddLockUnlockScene::processResponse(hddLockUnlock::hddLockUnlockResponse response)
{
	if (response == hddLockUnlock::hddLockUnlockResponseNone)
	{
		setProgress("");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseUnlocked)
	{
		setProgress("Unlocked");
		mHddLocked = false;
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseFailedToUnlock)
	{
		setProgress("Failed to unlock HDD");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseAlreadyUnlocked)
	{
		setProgress("Already unlocked");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseLocked)
	{
		setProgress("Locked");
		mHddLocked = true;
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseFailedToLock)
	{
		setProgress("Failed to lock HDD");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseAlreadyLocked)
	{
		setProgress("Already locked");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseLockingNotSupported)
	{
		setProgress("Locking not supported");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseFailureEeprom)
	{
		setProgress("Unable to decrypt EEPROM");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseUnlocking)
	{
		setProgress("Unlocking...");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseUnlockingVsc)
	{
		setProgress("VSC Unlocking...");
	}
	else if (response == hddLockUnlock::hddLockUnlockResponseLocking)
	{
		setProgress("Locking...");
	}
}
