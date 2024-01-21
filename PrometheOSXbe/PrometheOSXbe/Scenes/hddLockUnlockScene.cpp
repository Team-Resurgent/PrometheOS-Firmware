#include "hddLockUnlockScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\XKUtils\XKEEPROM.h"
#include "..\XKUtils\XKHDD.h"

hddLockUnlockScene::hddLockUnlockScene()
{
	mSelectedControl = 0;
	mStep = -1;
	getIdeInfo();
}

hddLockUnlockScene::~hddLockUnlockScene()
{
	free(mProgress);
	free(mIdeModel);
	free(mIdeSerial);
}

void hddLockUnlockScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::openScene(sceneItemUtilitiesScene);
	}

	// Lock / Unlock Action

	if (inputManager::buttonPressed(ButtonA) && mStep == -1)
	{
		mStep = 0;
	}
}

void hddLockUnlockScene::render()
{
	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "HDD Lock / Unlock...", theme::getTitleTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

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

	drawing::drawBitmapString(context::getBitmapFontMedium(), "Status", theme::getTitleTextColor(), 193, yPos);

	yPos += 40;

	if (mHDDLocked)
	{
		if (mStep == 0)
		{
			setProgress("Unlocking...");
			mStep = 1;
		}
		else if (mStep == 1)
		{
			unlockHDD();
			mStep = -1;
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
			lockHDD();
			mStep = -1;
		}
	}

	component::textBox(mProgress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Toggle HDD Lock", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void hddLockUnlockScene::setProgress(const char* message)
{
	free(mProgress);
	mProgress = strdup(message);
}

void hddLockUnlockScene::getIdeInfo()
{
	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();
	if (eeprom->Decrypt())
	{
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 512;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
		char* ideModel = XKHDD::GetIDEModel(hddcommand.DATA_BUFFER);
		mIdeModel = stringUtility::trim(ideModel, ' ');
		free(ideModel);
		char* ideSerial = XKHDD::GetIDESerial(hddcommand.DATA_BUFFER);
		mIdeSerial = stringUtility::trim(ideSerial, ' ');
		free(ideSerial);
		DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
		mHDDLocked = (SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED);
		mProgress = strdup(mHDDLocked ? "Locked" : "Unlocked");
	}
	else 
	{
		mHDDLocked = false;
		mProgress = strdup("Unknown");
		mIdeModel = strdup("Unknown");
		mIdeSerial = strdup("Unknown");
	}
	delete(eeprom);
}

void hddLockUnlockScene::lockHDD()
{
	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();

	XKHDD::ATA_COMMAND_OBJ hddcommand;
	UCHAR HddPass[32];
	UCHAR MasterPassword[13] = "TEAMASSEMBLY";
	
	XBOX_VERSION xver = eeprom->GetXBOXVersion();

	if (eeprom->Decrypt())
	{
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 0;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

		XKEEPROM::EEPROMDATA tmpData;
		eeprom->GetEEPROMData(&tmpData);
		XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);

		DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
		if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
		{
			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
			LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
			LPDWORD pMastr = (LPDWORD) HDDP;
			*pMastr = 0x0001;
			memcpy(HDDP+2, MasterPassword, 13);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
			SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
			
			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
			memcpy(HDDP+2, HddPass, 20);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			int retrycnt = 0;
			do
			{
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 512;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
				SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				retrycnt++;
			} while (!((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED)) && (retrycnt < 3));

			if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
			{
				setProgress("Locked");
				mHDDLocked = true;
			}
			else
			{
				setProgress("Failed to lock HDD");
			}
		}
		else if (!(SecStatus & IDE_SECURITY_SUPPORTED))
		{
			setProgress("Locking not supported");
		}
		else
		{
			setProgress("Already locked");
		}
	}
	else
	{
		setProgress("Unable to decrypt EEPROM");
	}

	delete(eeprom);
}

void hddLockUnlockScene::unlockHDD()
{
	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();

	XBOX_VERSION xver = eeprom->GetXBOXVersion();

	XKHDD::ATA_COMMAND_OBJ hddcommand;
	UCHAR HddPass[32];

	if (eeprom->Decrypt())
	{
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 0;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

		XKEEPROM::EEPROMDATA tmpData;
		eeprom->GetEEPROMData(&tmpData);
		XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);
		
		DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
		if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
		{
			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_UNLOCK;
			LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
			memcpy(HDDP+2, HddPass, 20);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_DISABLE;
			memcpy(HDDP+2, HddPass, 20);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			int retrycnt = 0;
			do
			{
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 512;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
				SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				retrycnt++;
			} while (!((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED)) && (retrycnt < 3));

			if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
			{
				setProgress("Unlocked");
				mHDDLocked = false;
			}
			else
			{
				setProgress("Failed to unlock HDD");
			}
		}
		else
		{
			setProgress("Already unlocked");
		}
	}
	else
	{
		setProgress("Unable to decrypt EEPROM");
	}

	delete(eeprom);
}

