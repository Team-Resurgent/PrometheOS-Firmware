#include "keypadScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "audioSettingsScene.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\theme.h"

keypadScene::keypadScene(const char* title, uint32_t address)
{
	mSelectedControl = 0;
	mTitle = strdup(title);
	if (address == 0)
	{
		mIp1 = strdup("");
		mIp2 = strdup("");
		mIp3 = strdup("");
		mIp4 = strdup("");
	}
	else
	{
		mIp1 = stringUtility::formatString("%i", address & 0xff);
		mIp2 = stringUtility::formatString("%i", (address >> 8) & 0xff);
		mIp3 = stringUtility::formatString("%i", (address >> 16) & 0xff);
		mIp4 = stringUtility::formatString("%i", (address >> 24) & 0xff);
	}
	mPart = 0;
	mCounter = 0;
}

keypadScene::~keypadScene()
{
	free(mTitle);
	free(mIp1);
	free(mIp2);
	free(mIp3);
	free(mIp4);
}


void keypadScene::update()
{
	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene(sceneResultCancelled);
		return;
	}

	// Delete Action

	if (inputManager::buttonPressed(ButtonX))
	{
		if (mPart == 0)
		{
			char* ip1 = deleteChar(mIp1);
			free(mIp1);
			mIp1 = ip1;
		}
		else if (mPart == 1)
		{
			char* ip2 = deleteChar(mIp2);
			free(mIp2);
			mIp2 = ip2;
		}
		else if (mPart == 2)
		{
			char* ip3 = deleteChar(mIp3);
			free(mIp3);
			mIp3 = ip3;
		}
		else if (mPart == 3)
		{
			char* ip4 = deleteChar(mIp4);
			free(mIp4);
			mIp4 = ip4;
		}
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		if ((mSelectedControl >= 0 && mSelectedControl <= 8) || mSelectedControl == 10)
		{
			uint32_t charIndex = mSelectedControl == 10 ? 9 : mSelectedControl;

			if (mPart == 0)
			{
				char* ip1 = addChar(mIp1, charIndex);
				if (strlen(mIp1) < 3 && strlen(ip1) == 3)
				{
					mPart = (mPart + 1) % 4;
				}
				free(mIp1);
				mIp1 = ip1;
			}
			else if (mPart == 1)
			{
				char* ip2 = addChar(mIp2, charIndex);
				if (strlen(mIp2) < 3 && strlen(ip2) == 3)
				{
					mPart = (mPart + 1) % 4;
				}
				free(mIp2);
				mIp2 = ip2;
			}
			else if (mPart == 2)
			{
				char* ip3 = addChar(mIp3, charIndex);
				if (strlen(mIp3) < 3 && strlen(ip3) == 3)
				{
					mPart = (mPart + 1) % 4;
				}
				free(mIp3);
				mIp3 = ip3;
			}
			else if (mPart == 3)
			{
				char* ip4 = addChar(mIp4, charIndex);
				if (strlen(mIp4) < 3 && strlen(ip4) == 3)
				{
					mPart = (mPart + 1) % 4;
				}
				free(mIp4);
				mIp4 = ip4;
			}
		}
		else if (mSelectedControl == 9)
		{
			if (mPart == 0)
			{
				char* ip1 = deleteChar(mIp1);
				free(mIp1);
				mIp1 = ip1;
			}
			else if (mPart == 1)
			{
				char* ip2 = deleteChar(mIp2);
				free(mIp2);
				mIp2 = ip2;
			}
			else if (mPart == 2)
			{
				char* ip3 = deleteChar(mIp3);
				free(mIp3);
				mIp3 = ip3;
			}
			else if (mPart == 3)
			{
				char* ip4 = deleteChar(mIp4);
				free(mIp4);
				mIp4 = ip4;
			}
		}
		else if (mSelectedControl == 11)
		{
			sceneManager::popScene(sceneResultDone);
			return;
		}

	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft))
	{
		mPart = mPart == 0 ? 3 : mPart - 1;
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight))
	{
		mPart = mPart == 3 ? 0 : mPart + 1;
	}
	
	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 2; 
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 5)
		{
			mSelectedControl = mSelectedControl > 3 ? mSelectedControl - 1 : 5; 
		}
		else if (mSelectedControl >= 6 && mSelectedControl <= 8)
		{
			mSelectedControl = mSelectedControl > 6 ? mSelectedControl - 1 : 8; 
		}
		else if (mSelectedControl >= 9 && mSelectedControl <= 11)
		{
			mSelectedControl = mSelectedControl > 9 ? mSelectedControl - 1 : 11; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = mSelectedControl < 2 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 5)
		{
			mSelectedControl = mSelectedControl < 5 ? mSelectedControl + 1 : 3;
		}
		else if (mSelectedControl >= 6 && mSelectedControl <= 8)
		{
			mSelectedControl = mSelectedControl < 8 ? mSelectedControl + 1 : 6;
		}
		else if (mSelectedControl >= 9 && mSelectedControl <= 11)
		{
			mSelectedControl = mSelectedControl < 11 ? mSelectedControl + 1 : 9;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 11)
		{
			mSelectedControl = mSelectedControl + 3 > 11 ? (mSelectedControl + 3) - 12 : mSelectedControl + 3;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 11)
		{
			mSelectedControl = mSelectedControl - 3 < 0 ? (mSelectedControl - 3) + 12 : mSelectedControl - 3;
		}
	}
}

void keypadScene::render()
{
	if (mCounter == 0)
	{
		mCounter = 60;
	}
	else 
	{
		mCounter--;
	}

	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), mTitle, theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	char* keys = "1234567890";

	int32_t keyboardX = 264;
	int32_t keyboardY = (context::getBufferHeight() - (((4 * 48) - 4) + 64)) / 2;
	keyboardY += theme::getCenterOffset();

	char* ip1 = mPart == 0 && mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", mIp1) : stringUtility::formatString("%s\xC2\xB0", mIp1);
	component::textBox(ip1, false, false, horizAlignmentCenter, 225 + (70 * 0), keyboardY, 66, 44);
	free(ip1);

	char* ip2 = mPart == 1 && mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", mIp2) : stringUtility::formatString("%s\xC2\xB0", mIp2);
	component::textBox(ip2, false, false, horizAlignmentCenter, 225 + (70 * 1), keyboardY, 66, 44);
	free(ip2);

	char* ip3 = mPart == 2 && mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", mIp3) : stringUtility::formatString("%s\xC2\xB0", mIp3);
	component::textBox(ip3, false, false, horizAlignmentCenter, 225 + (70 * 2),keyboardY, 66, 44);
	free(ip3);

	char* ip4 = mPart == 3 && mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", mIp4) : stringUtility::formatString("%s\xC2\xB0", mIp4);
	component::textBox(ip4, false, false, horizAlignmentCenter, 225 + (70 * 3), keyboardY, 66, 44);
	free(ip4);

	keyboardY += 64;

	keyboardButton(mSelectedControl == 0, false, keys[0], keyboardX + (66 * 0), keyboardY + (48 * 0), 62, 44);
	keyboardButton(mSelectedControl == 1, false, keys[1], keyboardX + (66 * 1), keyboardY + (48 * 0), 62, 44);
	keyboardButton(mSelectedControl == 2, false, keys[2], keyboardX + (66 * 2), keyboardY + (48 * 0), 62, 44);

	keyboardButton(mSelectedControl == 3, false, keys[3], keyboardX + (66 * 0), keyboardY + (48 * 1), 62, 44);
	keyboardButton(mSelectedControl == 4, false, keys[4], keyboardX + (66 * 1), keyboardY + (48 * 1), 62, 44);
	keyboardButton(mSelectedControl == 5, false, keys[5], keyboardX + (66 * 2), keyboardY + (48 * 1), 62, 44);

	keyboardButton(mSelectedControl == 6, false, keys[6], keyboardX + (66 * 0), keyboardY + (48 * 2), 62, 44);
	keyboardButton(mSelectedControl == 7, false, keys[7], keyboardX + (66 * 1), keyboardY + (48 * 2), 62, 44);
	keyboardButton(mSelectedControl == 8, false, keys[8], keyboardX + (66 * 2), keyboardY + (48 * 2), 62, 44);

	component::button(mSelectedControl == 9, false, "Del", keyboardX + (66 * 0), keyboardY + (48 * 3), 62, 44);
	keyboardButton(mSelectedControl == 10, false, keys[9], keyboardX + (66 * 1), keyboardY + (48 * 3), 62, 44);
	component::button(mSelectedControl == 11, false, "Done", keyboardX + (66 * 2), keyboardY + (48 * 3), 62, 44);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select \xC2\xA3 Delete", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

char* keypadScene::addChar(char* value, int charIndex)
{
	const char* keys = "1234567890";
	char* temp = stringUtility::formatString("%s%c", value, keys[charIndex]); 
	int number = stringUtility::toInt(temp);
	char* result = (number <= 255 && strlen(temp) <= 3) ? strdup(temp) : strdup(value);
	free(temp);
	return result;
}

char* keypadScene::deleteChar(char* value)
{
	return stringUtility::substr(value, 0, max(strlen(value) - 1, 0));
}

void keypadScene::keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height)
{
	char* keyString = stringUtility::formatString("%c", key);
	component::button(selected, active, keyString, x, y, width, height);
	free(keyString);
}

uint32_t keypadScene::getValue()
{
	return (stringUtility::toInt(mIp4) << 24) | (stringUtility::toInt(mIp3) << 16) | (stringUtility::toInt(mIp2) << 8) | stringUtility::toInt(mIp1);
}