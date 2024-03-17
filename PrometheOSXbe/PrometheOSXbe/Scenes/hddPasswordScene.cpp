#include "hddPasswordScene.h"
#include "sceneManager.h"

#include "..\theme.h"
#include "..\utils.h"
#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\inputManager.h"
#include "..\stringUtility.h"
#include "..\hdmiSettingsManager.h"
#include "..\hdmiDevice.h"

hddPasswordScene::hddPasswordScene()
{
	mSelectedControl = 0;
}

void hddPasswordScene::update()
{
	// Back Action

	if (inputManager::buttonPressed(ButtonB))
	{
		utils::reboot();
	}
}

void hddPasswordScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "VSC HDD Password...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	
	int yPos = (context::getBufferHeight() - (50 + 30 + 30 + 30 + 70 + 30 + 60)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "** HDD UNLOCKED, COPY THESE DETAILS AND KEEP SAFE **", theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	yPos += 50;
	char* driveModel = context::getDriveModel();
	char* driveModelString = stringUtility::formatString("Model: %s", driveModel);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), driveModelString, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	free(driveModelString);
	free(driveModel);
	yPos += 30;
	char* driveSerial = context::getDriveSerial();
	char* driveSerialString = stringUtility::formatString("Serial: %s", driveSerial);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), driveSerialString, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	free(driveSerialString);
	free(driveSerial);
	yPos += 30;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Master Password", theme::getTextColor(), 40, yPos);
	yPos += 30;
	utils::dataContainer* password1String = new utils::dataContainer(16);
	utils::dataContainer* password2String = new utils::dataContainer(16);
	if (context::getMasterPassword() != NULL)
	{
		for (int i = 0; i < 16; i++)
		{   
			uint8_t char1 = (uint8_t)context::getMasterPassword()->data[i];
			uint8_t char2 = (uint8_t)context::getMasterPassword()->data[i + 16];

			char* hex1 = stringUtility::formatString("%02x", char1);
			char* hex2 = stringUtility::formatString("%02x", char2);
			drawing::drawBitmapString(context::getBitmapFontSmall(), hex1, theme::getFooterTextColor(), 40 + (i * 22), yPos);
			drawing::drawBitmapString(context::getBitmapFontSmall(), hex2, theme::getFooterTextColor(), 40 + (i * 22), yPos + 30);
			free(hex2);
			free(hex1);

			password1String->data[i] = (char1 < 0x20 || char1 > 0x7e) ? 0x20 : (char)char1;
			password2String->data[i] = (char1 < 0x20 || char1 > 0x7e) ? 0x20 : (char)char2;
		}
		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), password1String->data, theme::getFooterTextColor(), horizAlignmentRight, 40, yPos, 640);
		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), password2String->data, theme::getFooterTextColor(), horizAlignmentRight, 40, yPos + 30, 640);
	}
	yPos += 70;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "User Password", theme::getTextColor(), 40, yPos);
	yPos += 30;
	if (context::getUserPassword() != NULL)
	{
		for (int i = 0; i < 16; i++)
		{   
			uint8_t char1 = (uint8_t)context::getUserPassword()->data[i];
			uint8_t char2 = (uint8_t)context::getUserPassword()->data[i + 16];

			char* hex1 = stringUtility::formatString("%02x", char1);
			char* hex2 = stringUtility::formatString("%02x", char2);
			drawing::drawBitmapString(context::getBitmapFontSmall(), hex1, theme::getFooterTextColor(), 40 + (i * 22), yPos);
			drawing::drawBitmapString(context::getBitmapFontSmall(), hex2, theme::getFooterTextColor(), 40 + (i * 22), yPos + 30);
			free(hex2);
			free(hex1);

			password1String->data[i] = (char1 < 0x20 || char1 > 0x7e) ? 0x20 : (char)char1;
			password2String->data[i] = (char1 < 0x20 || char1 > 0x7e) ? 0x20 : (char)char2;
		}

		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), password1String->data, theme::getFooterTextColor(), horizAlignmentRight, 40, yPos, 640);
		drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), password2String->data, theme::getFooterTextColor(), horizAlignmentRight, 40, yPos + 30, 640);
	}
	delete(password1String);
	delete(password2String);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Reboot", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
