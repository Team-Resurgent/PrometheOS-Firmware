#include "rtcScene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\inputManager.h"
#include "..\ssfn.h"
#include "..\xboxConfig.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\theme.h"
#include "..\rtcManager.h"

rtcScene::rtcScene()
{
	bool doRtc = settingsManager::getRtcEnable() == true && xboxConfig::getHasRtcExpansion() == true;
	mSelectedControl = 0;
	mHasRtcExpansion = doRtc;
	mShouldApply = false;
	mDateTime = doRtc ? rtcManager::getDateTime() : rtcManager::getXboxDateTime();
}

void rtcScene::update()
{
	// Back Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		if (mHasRtcExpansion == true)
		{
			rtcManager::setDateTime(mDateTime);
		}
		rtcManager::setXboxDateTime(mDateTime);
		mDateTime = mHasRtcExpansion ? rtcManager::getDateTime() : rtcManager::getXboxDateTime();
		mShouldApply = false;
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft) || inputManager::buttonPressed(ButtonX))
	{
		if (mSelectedControl == 0)
		{
			mDateTime.dotw = mDateTime.dotw > 0 ? mDateTime.dotw - 1 : 6; 
			mShouldApply = true;
		}
		else if (mSelectedControl == 1)
		{
			mDateTime.day = mDateTime.day > 1 ? mDateTime.day - 1 : 31; 
			mShouldApply = true;
		}
		else if (mSelectedControl == 2)
		{
			mDateTime.month = mDateTime.month > 1 ? mDateTime.month - 1 : 12; 
			mShouldApply = true;
		}
		else if (mSelectedControl == 3)
		{
			mDateTime.year = mDateTime.year > 0 ? mDateTime.year - 1 : 255; 
			mShouldApply = true;
		}
		else if (mSelectedControl == 4)
		{
			mDateTime.hour = mDateTime.hour > 0 ? mDateTime.hour - 1 : 23; 
			mShouldApply = true;
		}
		else if (mSelectedControl == 5)
		{
			mDateTime.min = mDateTime.min > 0 ? mDateTime.min - 1 : 59; 
			mShouldApply = true;
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		if (mSelectedControl == 0)
		{
			mDateTime.dotw = mDateTime.dotw < 6 ? mDateTime.dotw + 1 : 0;
			mShouldApply = true;
		}
		else if (mSelectedControl == 1)
		{
			mDateTime.day = mDateTime.day < 31 ? mDateTime.day + 1 : 1;
			mShouldApply = true;
		}
		else if (mSelectedControl == 2)
		{
			mDateTime.month = mDateTime.month < 12 ? mDateTime.month + 1 : 1;
			mShouldApply = true;
		}
		else if (mSelectedControl == 3)
		{
			mDateTime.year = mDateTime.year < 255 ? mDateTime.year + 1 : 0;
			mShouldApply = true;
		}
		else if (mSelectedControl == 4)
		{
			mDateTime.hour = mDateTime.hour < 23 ? mDateTime.hour + 1 : 0;
			mShouldApply = true;
		}
		else if (mSelectedControl == 5)
		{
			mDateTime.min = mDateTime.min < 59 ? mDateTime.min + 1 : 0;
			mShouldApply = true;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < 5 ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 5; 
	}
}

void rtcScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), mHasRtcExpansion ? "RTC Expansion settings..." : "Date Time settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int xPos = 117;
	int yPos = (context::getBufferHeight() - ((6 * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	const char* daysOfWeek[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	char* dayOfWeekString = stringUtility::formatString("Day Of Week: %s", daysOfWeek[mDateTime.dotw]);
	component::button(mSelectedControl == 0, false, dayOfWeekString, 193, yPos, 322, 30);
	free(dayOfWeekString);
	yPos += 40;

	char* dayString = stringUtility::formatString("Day: %i", mDateTime.day);
	component::button(mSelectedControl == 1, false, dayString, 193, yPos, 322, 30);
	free(dayString);
	yPos += 40;

	const char* months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	char* monthString = stringUtility::formatString("Month: %s", months[mDateTime.month - 1]);
	component::button(mSelectedControl == 2, false, monthString, 193, yPos, 322, 30);
	free(monthString);
	yPos += 40;

	char* yearString = stringUtility::formatString("Year: %i", 2000 + mDateTime.year);
	component::button(mSelectedControl == 3, false, yearString, 193, yPos, 322, 30);
	free(yearString);
	yPos += 40;

	char* hourString = stringUtility::formatString("Hour (24h): %i", mDateTime.hour);
	component::button(mSelectedControl == 4, false, hourString, 193, yPos, 322, 30);
	free(hourString);
	yPos += 40;

	char* minString = stringUtility::formatString("Minutes: %i", mDateTime.min);
	component::button(mSelectedControl == 5, false, minString, 193, yPos, 322, 30);
	free(minString);
	yPos += 40;
	
	drawing::drawBitmapString(context::getBitmapFontSmall(), mShouldApply ? "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value \xC2\xA1 Apply" : "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
