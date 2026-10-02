#include "durationSelectorScene.h"
#include "sceneManager.h"
#include "integerInputScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\stringUtility.h"
#include "..\theme.h"

#define DURATION_REPEAT_INITIAL_DELAY_MS 400
#define DURATION_REPEAT_INTERVAL_MS      80

#define DURATION_FIELD_COUNT 4

durationSelectorScene::durationSelectorScene(const char* title, const char* label, unsigned int initialSeconds, unsigned int minSeconds, unsigned int maxSeconds, const char* zeroLabel)
{
	mSelectedControl = 0;
	mMinSeconds = minSeconds;
	mMaxSeconds = initialSeconds > maxSeconds ? initialSeconds : maxSeconds;
	mMaxDays = mMaxSeconds / 86400;
	mTitle = strdup(title != NULL ? title : "");
	mLabel = strdup(label != NULL ? label : "");
	mZeroLabel = zeroLabel != NULL ? strdup(zeroLabel) : NULL;
	mRepeatNextUp = 0;
	mRepeatNextDown = 0;

	mDays = initialSeconds / 86400;
	mHours = (initialSeconds % 86400) / 3600;
	mMinutes = (initialSeconds % 3600) / 60;
	mSeconds = initialSeconds % 60;
}

durationSelectorScene::~durationSelectorScene()
{
	free(mTitle);
	free(mLabel);
	free(mZeroLabel);
}

void durationSelectorScene::onKeypadClosingCallback(sceneResult result, void* context, scene* caller)
{
	if (result != sceneResultDone) {
		return;
	}

	durationSelectorScene* self = (durationSelectorScene*)context;
	integerInputScene* input = (integerInputScene*)caller;
	self->setFieldValue(self->mSelectedControl, input->getValue());
}

unsigned int durationSelectorScene::getFieldValue(int field)
{
	if (field == 0) {
		return mDays;
	}
	if (field == 1) {
		return mHours;
	}
	if (field == 2) {
		return mMinutes;
	}
	return mSeconds;
}

void durationSelectorScene::setFieldValue(int field, unsigned int value)
{
	unsigned int fieldMax = getFieldMax(field);
	if (value > fieldMax) {
		value = fieldMax;
	}

	if (field == 0) {
		mDays = value;
	} else if (field == 1) {
		mHours = value;
	} else if (field == 2) {
		mMinutes = value;
	} else {
		mSeconds = value;
	}
}

unsigned int durationSelectorScene::getFieldMax(int field)
{
	if (field == 0) {
		return mMaxDays;
	}
	if (field == 1) {
		return 23;
	}
	return 59;
}

const char* durationSelectorScene::getFieldLabel(int field)
{
	if (field == 0) {
		return "Days:";
	}
	if (field == 1) {
		return "Hours:";
	}
	if (field == 2) {
		return "Minutes:";
	}
	return "Seconds:";
}

void durationSelectorScene::adjustField(int field, int direction)
{
	unsigned int fieldMax = getFieldMax(field);
	unsigned int value = getFieldValue(field);

	if (direction > 0) {
		value = value >= fieldMax ? 0 : value + 1;
	} else {
		value = value == 0 ? fieldMax : value - 1;
	}

	setFieldValue(field, value);
}

unsigned int durationSelectorScene::getRawTotalSeconds()
{
	unsigned __int64 total =
		((unsigned __int64)mDays * 86400) +
		((unsigned __int64)mHours * 3600) +
		((unsigned __int64)mMinutes * 60) +
		(unsigned __int64)mSeconds;

	if (total > (unsigned __int64)mMaxSeconds) {
		return mMaxSeconds;
	}
	return (unsigned int)total;
}

unsigned int durationSelectorScene::getTotalSeconds()
{
	unsigned int total = getRawTotalSeconds();
	if (total != 0 && total < mMinSeconds) {
		return mMinSeconds;
	}
	return total;
}

void durationSelectorScene::update()
{
	if (inputManager::buttonPressed(JoystickButtonB)) {
		sceneManager::popScene(sceneResultDone);
		return;
	}

	if (inputManager::buttonPressed(JoystickButtonY)) {
		mDays = 0;
		mHours = 0;
		mMinutes = 0;
		mSeconds = 0;
	}

	if (inputManager::buttonPressed(JoystickButtonA)) {
		sceneManager::pushScene(new sceneContainer(sceneItemGenericScene,
			new integerInputScene("Please enter value...", getFieldLabel(mSelectedControl), getFieldValue(mSelectedControl), getFieldMax(mSelectedControl)),
			"Integer Input", this, &onKeypadClosingCallback));
		return;
	}

	if (inputManager::buttonPressed(JoystickButtonDpadLeft)) {
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : DURATION_FIELD_COUNT - 1;
	}

	if (inputManager::buttonPressed(JoystickButtonDpadRight)) {
		mSelectedControl = mSelectedControl < DURATION_FIELD_COUNT - 1 ? mSelectedControl + 1 : 0;
	}

	DWORD now = GetTickCount();

	if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
		mRepeatNextUp = now + DURATION_REPEAT_INITIAL_DELAY_MS;
		adjustField(mSelectedControl, 1);
	} else if (inputManager::buttonDown(JoystickButtonDpadUp) && (int)(now - mRepeatNextUp) >= 0) {
		mRepeatNextUp = now + DURATION_REPEAT_INTERVAL_MS;
		adjustField(mSelectedControl, 1);
	}

	if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
		mRepeatNextDown = now + DURATION_REPEAT_INITIAL_DELAY_MS;
		adjustField(mSelectedControl, -1);
	} else if (inputManager::buttonDown(JoystickButtonDpadDown) && (int)(now - mRepeatNextDown) >= 0) {
		mRepeatNextDown = now + DURATION_REPEAT_INTERVAL_MS;
		adjustField(mSelectedControl, -1);
	}
}

char* durationSelectorScene::buildTotalText()
{
	unsigned int raw = getRawTotalSeconds();
	unsigned int effective = getTotalSeconds();

	if (effective == 0) {
		if (mZeroLabel != NULL && strlen(mZeroLabel) > 0) {
			return stringUtility::formatString("Total: 0 - %s", mZeroLabel);
		}
		return strdup("Total: 0");
	}

	if (raw != effective) {
		return stringUtility::formatString("Total: %u Seconds (Raised To Minimum)", effective);
	}

	return stringUtility::formatString("Total: %u Seconds", effective);
}

void durationSelectorScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), mTitle, theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t fieldY = (context::getBufferHeight() - 44) / 2;
	fieldY += theme::getCenterOffset();

	if (mLabel != NULL && strlen(mLabel) > 0) {
		component::text(mLabel, false, horizAlignmentCenter, 40, fieldY - 96, 640, 30);
	}

	int32_t fieldX = 105;
	int32_t fieldStep = 130;
	int32_t fieldWidth = 120;

	component::text("Days", false, horizAlignmentCenter, fieldX + (fieldStep * 0), fieldY - 32, fieldWidth, 30);
	component::text("Hours", false, horizAlignmentCenter, fieldX + (fieldStep * 1), fieldY - 32, fieldWidth, 30);
	component::text("Minutes", false, horizAlignmentCenter, fieldX + (fieldStep * 2), fieldY - 32, fieldWidth, 30);
	component::text("Seconds", false, horizAlignmentCenter, fieldX + (fieldStep * 3), fieldY - 32, fieldWidth, 30);

	char* daysText = stringUtility::formatString("%u", mDays);
	char* hoursText = stringUtility::formatString("%02u", mHours);
	char* minutesText = stringUtility::formatString("%02u", mMinutes);
	char* secondsText = stringUtility::formatString("%02u", mSeconds);

	component::button(mSelectedControl == 0, false, daysText, fieldX + (fieldStep * 0), fieldY, fieldWidth, 44);
	component::button(mSelectedControl == 1, false, hoursText, fieldX + (fieldStep * 1), fieldY, fieldWidth, 44);
	component::button(mSelectedControl == 2, false, minutesText, fieldX + (fieldStep * 2), fieldY, fieldWidth, 44);
	component::button(mSelectedControl == 3, false, secondsText, fieldX + (fieldStep * 3), fieldY, fieldWidth, 44);

	free(daysText);
	free(hoursText);
	free(minutesText);
	free(secondsText);

	char* totalText = buildTotalText();
	component::textBox(totalText, false, false, horizAlignmentCenter, 120, fieldY + 64, 480, 44, false, false, false);
	free(totalText);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Enter Value \xC2\xA4 Default", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
