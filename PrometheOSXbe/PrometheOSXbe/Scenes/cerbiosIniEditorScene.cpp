#include "cerbiosIniEditorScene.h"
#include "keyboardScene.h"
#include "frontLedSelectorScene.h"
#include "sceneManager.h"
#include "scene.h"
#include "filePickerScene.h"

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
#include "..\fileSystem.h"
#include "..\cerbiosIniHelper.h"

void cerbiosIniEditorScene::onPathClosingCallback(sceneResult result, void* context, scene* scene)
{
	cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mShowingFilePicker = false;
		return;
	}

	char* path = NULL;

	if(self->mShowingFilePicker) {
		self->mShowingFilePicker = false;
		self->mNeedsSave = true;
		filePickerScene* closingScene = (filePickerScene*)scene;
		path = closingScene->getFilePath();
	}
	else
	{
		keyboardScene* closingScene = (keyboardScene*)scene;
		path = closingScene->getText();
	}

	if (self->mSelectedControl == 3)
	{
		strcpy(self->mConfig.CdPath1, path);
		free(self->mShortCdPath1);
		self->mShortCdPath1 = self->shortenString(path);
	}
	else if (self->mSelectedControl == 4)
	{
		strcpy(self->mConfig.CdPath2, path);
		free(self->mShortCdPath2);
		self->mShortCdPath2 = self->shortenString(path);
	}
	else if (self->mSelectedControl == 5)
	{
		strcpy(self->mConfig.CdPath3, path);
		free(self->mShortCdPath3);
		self->mShortCdPath3 = self->shortenString(path);
	}
	else if (self->mSelectedControl == 6)
	{
		strcpy(self->mConfig.DashPath1, path);
		free(self->mShortDashPath1);
		self->mShortDashPath1 = self->shortenString(path);
	}
	else if (self->mSelectedControl == 7)
	{
		strcpy(self->mConfig.DashPath2, path);
		free(self->mShortDashPath2);
		self->mShortDashPath2 = self->shortenString(path);
	}
	else if (self->mSelectedControl == 8)
	{
		strcpy(self->mConfig.DashPath3, path);
		free(self->mShortDashPath3);
		self->mShortDashPath3 = self->shortenString(path);
	}
	else if (self->mSelectedControl == 9)
	{
		strcpy(self->mConfig.BootAnimPath, path);
		free(self->mShortBootAnimPath);
		self->mShortBootAnimPath = self->shortenString(path);
	}

	free(path);
}

void cerbiosIniEditorScene::onFrontLedClosingCallback(sceneResult result, void* context, scene* scene)
{
	cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
	if (result == sceneResultCancelled)
	{
		return;
	}

	frontLedSelectorScene* closingScene = (frontLedSelectorScene*)scene;
	char* ledSequence = closingScene->getLedSequence();
	self->mConfig.FrontLed[0] = ledSequence[0];
	self->mConfig.FrontLed[1] = ledSequence[1];
	self->mConfig.FrontLed[2] = ledSequence[2];
	self->mConfig.FrontLed[3] = ledSequence[3];
	free(ledSequence);
}

cerbiosIniEditorScene::cerbiosIniEditorScene()
{
	mConfig = cerbiosIniHelper::loadConfig();
	mConfig.FanSpeed = mConfig.FanSpeed;
	mShortCdPath1 = shortenString(mConfig.CdPath1);
	mShortCdPath2 = shortenString(mConfig.CdPath2);
	mShortCdPath3 = shortenString(mConfig.CdPath3);
	mShortDashPath1 = shortenString(mConfig.DashPath1);
	mShortDashPath2 = shortenString(mConfig.DashPath2);
	mShortDashPath3 = shortenString(mConfig.DashPath3);
	mShortBootAnimPath = shortenString(mConfig.BootAnimPath);
	mSelectedControl = 0;
	mHasFilePicker = false;
	mNeedsSave = false;
	mShowingFilePicker = false;
	mShowingInfo = false;
	mMaxOptionCount = 18;
}

cerbiosIniEditorScene::~cerbiosIniEditorScene()
{
	free(mShortCdPath1);
	free(mShortCdPath2);
	free(mShortCdPath3);
	free(mShortDashPath1);
	free(mShortDashPath2);
	free(mShortDashPath3);
	free(mShortBootAnimPath);
}

void cerbiosIniEditorScene::update()
{
	// Exit Action

	if(mShowingInfo) {
		if (inputManager::buttonPressed(ButtonB)) {
			mShowingInfo = false;
		}
		return;
	}

	if (inputManager::buttonPressed(ButtonBlack)) {
		mShowingInfo = true;
		return;
	}

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Defaults Actions

	if (inputManager::buttonPressed(ButtonY))
	{
		cerbiosIniHelper::setConfigDefault(&mConfig);
		mNeedsSave = true;
	}

	// Save Actions

	if (mNeedsSave == true && inputManager::buttonPressed(ButtonX))
	{
		char* buffer = (char*)malloc(65536);
		cerbiosIniHelper::buildConfig(&mConfig, buffer);
		cerbiosIniHelper::saveConfig(buffer);
		free(buffer);
		mNeedsSave = false;
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < mMaxOptionCount ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : mMaxOptionCount; 
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonA) || inputManager::buttonPressed(ButtonTriggerLeft))
	{
		if (mSelectedControl == 0)
		{
			mConfig.DriveSetup = (mConfig.DriveSetup > 0) ? (mConfig.DriveSetup - 1) : 3;
		}
		else if (mSelectedControl == 1)
		{
			mConfig.AVCheck = (mConfig.AVCheck == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 2)
		{
			mConfig.Debug = (mConfig.Debug == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 3)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "CD Path 1:", mConfig.CdPath1), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 4)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "CD Path 2:", mConfig.CdPath2), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 5)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "CD Path 3:", mConfig.CdPath3), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 6)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Dash Path 1:", mConfig.DashPath1), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 7)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Dash Path 2:", mConfig.DashPath2), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 8)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Dash Path 3:", mConfig.DashPath3), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 9)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Boot Anim Path:", mConfig.BootAnimPath), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 10)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new frontLedSelectorScene(mConfig.FrontLed), "", this, onFrontLedClosingCallback));
		}
		else if (mSelectedControl == 11)
		{
			mConfig.FanSpeed = (mConfig.FanSpeed > 0) ? (mConfig.FanSpeed - 10) : 100;
		}
		else if (mSelectedControl == 12)
		{
			mConfig.UdmaModeMaster = (mConfig.UdmaModeMaster > 0) ? (mConfig.UdmaModeMaster - 1) : 6;
		}
		else if (mSelectedControl == 13)
		{
			mConfig.UdmaModeSlave = (mConfig.UdmaModeSlave > 0) ? (mConfig.UdmaModeSlave - 1) : 6;
		}
		else if (mSelectedControl == 14)
		{
			mConfig.Force480p = (mConfig.Force480p == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 15)
		{
			mConfig.ForceVGA = (mConfig.ForceVGA == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 16)
		{
			mConfig.RtcEnable = (mConfig.RtcEnable == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 17)
		{
			mConfig.BlockDashUpdate = (mConfig.BlockDashUpdate == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 18)
		{
			mConfig.ResetOnEject = (mConfig.ResetOnEject == 0) ? 1 : 0;
		}
		mNeedsSave = true;
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight))
	{
		if (mSelectedControl == 0)
		{
			mConfig.DriveSetup = (mConfig.DriveSetup >= 3) ? 0 : (mConfig.DriveSetup + 1);
		}
		else if (mSelectedControl == 1)
		{
			mConfig.AVCheck = (mConfig.AVCheck == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 2)
		{
			mConfig.Debug = (mConfig.Debug == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 3)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "CD Path 1:", mConfig.CdPath1), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 4)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "CD Path 2:", mConfig.CdPath2), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 5)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "CD Path 3:", mConfig.CdPath3), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 6)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Dash Path 1:", mConfig.DashPath1), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 7)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Dash Path 2:", mConfig.DashPath2), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 8)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Dash Path 3:", mConfig.DashPath3), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 9)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new keyboardScene(99, "Please enter path...", "Boot Anim Path:", mConfig.BootAnimPath), "", this, onPathClosingCallback));
		}
		else if (mSelectedControl == 10)
		{
			sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new frontLedSelectorScene(mConfig.FrontLed), "", this, onFrontLedClosingCallback));
		}
		else if (mSelectedControl == 11)
		{
			mConfig.FanSpeed = (mConfig.FanSpeed >= 100) ? 0 : (mConfig.FanSpeed + 10);
		}
		else if (mSelectedControl == 12)
		{
			mConfig.UdmaModeMaster = (mConfig.UdmaModeMaster >= 6) ? 0 : (mConfig.UdmaModeMaster + 1);
		}
		else if (mSelectedControl == 13)
		{
			mConfig.UdmaModeSlave = (mConfig.UdmaModeSlave >= 6) ? 0 : (mConfig.UdmaModeSlave + 1);
		}
		else if (mSelectedControl == 14)
		{
			mConfig.Force480p = (mConfig.Force480p == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 15)
		{
			mConfig.ForceVGA = (mConfig.ForceVGA == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 16)
		{
			mConfig.RtcEnable = (mConfig.RtcEnable == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 17)
		{
			mConfig.BlockDashUpdate = (mConfig.BlockDashUpdate == 0) ? 1 : 0;
		}
		else if (mSelectedControl == 18)
		{
			mConfig.ResetOnEject = (mConfig.ResetOnEject == 0) ? 1 : 0;
		}
		mNeedsSave = true;
	}

	if (mHasFilePicker && inputManager::buttonPressed(ButtonWhite)) {
		mShowingFilePicker = true;
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new filePickerScene(filePickerTypeXbe, true, true), "", this, onPathClosingCallback);
		sceneManager::pushScene(container);
	}
}

char* cerbiosIniEditorScene::getOptionInfo(int selectedControl) {
	char *infoStr;
	switch(selectedControl) {
		case 0: {
			infoStr = "Drive Setup (DriveSetup):\n"
			"\n"
			"Xbox drive setup.\n"
			"\n"
			"0 = HDD & DVD - Stock setup, DVD drive must be connected or bios\n"
			"      will fail to boot.\n"
			"1 = HDD & No DVD (Legacy) - Allows bios to boot if no DVD drive\n"
			"      is connected.\n"
			"2 = HDD & No DVD (Modern) - Removes DVD drive detect code\n"
			"      entirely, bios boots faster. DVD drive no longer detected by apps.\n"
			"3 = Dual HDD - Allows 2nd HDD to connected in-place of DVD\n"
			"      drive, HDD mode jumper must be set to slave.\n"
			"Default = 1";
		} break;

		case 1: {
			infoStr = "AV Check (AVCheck):\n"
			"\n"
			"Check for AV cable on bios boot. Flashes yellow and\n"
			"green and halts system if no AV cable is connected.\n"
			"\n"
			"True = Check for AV cable on boot\n"
			"False = Skip AV cable check on boot\n"
			"Default = True";
		} break;

		case 2: {
			infoStr = "Debug (Debug):\n"
			"\n"
			"Will check for and boot into C:\\xshell.xbe (XDK Launcher) and\n"
			"allows to debug apps. Will look for and load E:\\xbdm.dll (enables\n"
			"network communication with XDK on PC with Visual Studio).\n"
			"\n"
			"True = Allows booting into XDK Launcher and to debug apps.\n"
			"False = Will not boot C:\\xshell.xbe and will not load E:\\xbdm.dll\n"
			"Default = False";
		} break;

		case 3:
		case 4:
		case 5: {
			infoStr = "CD Path 1/2/3 (CdPath1, CdPath2, CdPath3):\n"
			"\n"
			"List of XBEs to look for and load if a DVD disc is detected in\n"
			"D: drive on bios boot. Will always fallback to D:\\default.xbe\n"
			"\n"
			"Default = (all blank)";
		} break;

		case 6:
		case 7:
		case 8: {
			infoStr = "Dash Path 1/2/3 (DashPath1, DashPath2, DashPath3):\n"
			"\n"
			"List of dashboard XBEs to look for and load on bios boot. Since\n"
			"Cerbios 2.4.2, allows dual-hdd prefixes (HDD0, HDD1), example:\n"
			"HDD0-C:\\evoxdash.xbe. Will always fallback to C:\\xboxdash.xbe\n"
			"\n"
			"Defaults:\n"
			"   DashPath1 = C:\\evoxdash.xbe\n"
			"   DashPath2 = C:\\avalaunch.xbe\n"
			"   DashPath3 = C:\\nexgen.xbe";
		} break;

		case 9: {
			infoStr = "Boot Anim Path (BootAnimPath):\n"
			"\n"
			"Optional splash animation that plays during bios boot.\n"
			"Will always fallback to C:\\BootAnims\\Xbox\\BootAnim.xbe if found.\n"
			"\n"
			"Default = C:\\BootAnims\\Xbox\\BootAnim.xbe";
		} break;

		case 10: {
			infoStr = "Front Led (FrontLed):\n"
			"\n"
			"Custom front eject button LED colors set during boot, split into\n"
			"4 phases. The pattern will continue to cycle until another application\n"
			"takes control of the LEDs.\n"
			"LED Ring Color code: G = Green, R = Red, A = Amber, O = Off\n"
			"\n"
			"Default = GGGG";
		} break;

		case 11: {
			infoStr = "Fan Speed (FanSpeed):\n"
			"\n"
			"Custom fan speed (in percent) set during boot.\n"
			"0 = Auto, 10-100 = Manual Control, Supports increments of 10\n"
			"\n"
			"Default = 0";
		} break;

		case 12:
		case 13: {
			infoStr = "UDMA Mode (UdmaMode, UdmaModeMaster, UdmaModeSlave):\n"
			"\n"
			"Set HDD UDMA mode on boot. SATA adapters always require 80-Wire\n"
			"IDE cable. Since Cerbios 2.4.2, also allows setting UDMA mode for the\n"
			"slave HDD when DriveSetup = 3 (Dual HDD mode). Modes:\n"
			"0 = Auto (Startech Adapter)\n"
			"1 = Auto (Generic Adapter)\n"
			"2 = UDMA 2 (Default / Stock)\n"
			"3 = UDMA 3 (requires 80-Wire cable)\n"
			"4 = UDMA 4 (requires 80-Wire cable)\n"
			"5 = UDMA 5 (requires 80-Wire cable)\n"
			"6 = UDMA 6 (requires 80-Wire cable, Experimental)\n"
			"Default = 2";
		} break;

		case 14: {
			infoStr = "Force 480p (Force480p):\n"
			"\n"
			"Force games and apps to use 480p HD video mode. Component cable\n"
			"required. 720p and 1080i modes are unaffected. PAL AV modes also\n"
			"unaffected. 480p option must be enabled in MS Dashboard. Only\n"
			"supported on NTSC consoles.\n"
			"\n"
			"True = Force 480p option enabled\n"
			"False = Stock setup, no forcing to 480p will happen\n"
			"Default = False";
		} break;

		case 15: {
			infoStr = "Force VGA (ForceVGA):\n"
			"\n"
			"Force games and apps to output VGA video signal (RGsB). An\n"
			"appropriate cable or adapter is required and an appropriate\n"
			"(SoG, Sync-on-Green) monitor is required, depending on the\n"
			"cable used. This option automatically enables Force480p option.\n"
			"1.6 revision consoles are unsupported.\n"
			"\n"
			"True = Force VGA video output\n"
			"False = Stock setup, no forcing VGA output will happen\n"
			"Default = False";
		} break;

		case 16: {
			infoStr = "RTC Enable (RtcEnable):\n"
			"\n"
			"Enables automatic clock syncing with an appropriate external\n"
			"battery-backed RTC module connected to the xbox's SMBUS.\n"
			"Requires Cerbios 2.4.0 or higher, or PrometheOS 1.4.0 or higher.\n"
			"Currently supported modules: DS3231\n"
			"\n"
			"True = Sync clock on bios boot\n"
			"False = Stock setup, no clock syncing will happen\n"
			"Default = False";
		} break;

		case 17: {
			infoStr = "Block Dash Update (BlockDashUpdate):\n"
			"\n"
			"Prevents some XBL games from updating the MS Dashboard, which\n"
			"can break softmod installs. Only really useful for softmods running\n"
			"Cerbios 2.4.0 or higher in BFM mode.\n"
			"\n"
			"True = Dashboard update blocking enabled\n"
			"False = Stock setup, no blocking will occur\n"
			"Default = False";
		} break;

		case 18: {
			infoStr = "Reset on Eject (ResetOnEject):\n"
			"\n"
			"Restores orignal stock functionality of resetting the xbox when the\n"
			"DVD tray is ejected. This will reload your choosen dashboard.\n"
			"\n"
			"True = Stock reset functionality enabled\n"
			"False = Ignores reset command when the tray is eject\n"
			"Default = False";
		} break;

		default: {
			infoStr = "NO INFO AVAILABLE FOR THIS OPTION";
		}
	}
	return infoStr;
}

void cerbiosIniEditorScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Cerbios INI Editor...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	const char* driveModes[] = { "HDD & DVD", "HDD & No DVD (Legacy)", "HDD & No DVD (Modern)", "Dual HDD" };
	const char* bools[] = { "False", "True" };
	const char* udmaModes[] = { "Auto (Startech Adapter)", "Auto (Generic Adapter)", "UDMA 2 (Default / Stock)", "UDMA 3 (Ultra DMA 80-Conductor)", "UDMA 4 (Ultra DMA 80-Conductor)", "UDMA 5 (Ultra DMA 80-Conductor)", "UDMA 6 (Experimental)" };
	const char* fanSpeeds[] = { "Auto", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };

	int32_t maxItems = 7;
	int32_t menuItems = mMaxOptionCount + 1;

	int32_t start = 0;
	if (menuItems >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), menuItems - maxItems);
	}

	int32_t itemCount = min(start + maxItems, menuItems) - start; 
	if (itemCount > 0)
	{
		uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		for (int32_t i = 0; i < itemCount; i++)
		{
			uint32_t index = start + i;
			if (index == 0)
			{
				component::splitButton(mSelectedControl == index, false, "Drive Setup:", 165, driveModes[mConfig.DriveSetup], 40, yPos, 640, 30);
			}
			else if (index == 1)
			{
				component::splitButton(mSelectedControl == index, false, "AV Check:", 165, bools[mConfig.AVCheck], 40, yPos, 640, 30);
			}
			else if (index == 2)
			{
				component::splitButton(mSelectedControl == index, false, "Debug:", 165, bools[mConfig.Debug], 40, yPos, 640, 30);
			}
			else if (index == 3)
			{
				component::splitButton(mSelectedControl == index, false, "CD Path 1:", 165, mShortCdPath1, 40, yPos, 640, 30);
			}
			else if (index == 4)
			{
				component::splitButton(mSelectedControl == index, false, "CD Path 2:", 165, mShortCdPath2, 40, yPos, 640, 30);
			}
			else if (index == 5)
			{
				component::splitButton(mSelectedControl == index, false, "CD Path 3:", 165, mShortCdPath3, 40, yPos, 640, 30);
			}
			else if (index == 6)
			{
				component::splitButton(mSelectedControl == index, false, "Dash Path 1:", 165, mShortDashPath1, 40, yPos, 640, 30);
			}
			else if (index == 7)
			{
				component::splitButton(mSelectedControl == index, false, "Dash Path 2:", 165, mShortDashPath2, 40, yPos, 640, 30);
			}
			else if (index == 8)
			{
				component::splitButton(mSelectedControl == index, false, "Dash Path 3:", 165, mShortDashPath3, 40, yPos, 640, 30);
			}
			else if (index == 9)
			{
				component::splitButton(mSelectedControl == index, false, "Boot Anim Path:", 165, mShortBootAnimPath, 40, yPos, 640, 30);
			}
			else if (index == 10)
			{
				component::splitButton(mSelectedControl == index, false, "Front Led:", 165, mConfig.FrontLed, 40, yPos, 640, 30);
			}
			else if (index == 11)
			{
				component::splitButton(mSelectedControl == index, false, "Fan Speed:", 165, fanSpeeds[mConfig.FanSpeed / 10], 40, yPos, 640, 30);
			}
			else if (index == 12)
			{
				component::splitButton(mSelectedControl == index, false, "Udma Master:", 165, udmaModes[mConfig.UdmaModeMaster], 40, yPos, 640, 30);
			}
			else if (index == 13)
			{
				component::splitButton(mSelectedControl == index, false, "Udma Slave:", 165, udmaModes[mConfig.UdmaModeSlave], 40, yPos, 640, 30);
			}
			else if (index == 14)
			{
				component::splitButton(mSelectedControl == index, false, "Force 480p:", 165, bools[mConfig.Force480p], 40, yPos, 640, 30);
			}
			else if (index == 15)
			{
				component::splitButton(mSelectedControl == index, false, "Force VGA:", 165, bools[mConfig.ForceVGA], 40, yPos, 640, 30);
			}
			else if (index == 16)
			{
				component::splitButton(mSelectedControl == index, false, "Rtc Enable:", 165, bools[mConfig.RtcEnable], 40, yPos, 640, 30);
			}
			else if (index == 17)
			{
				component::splitButton(mSelectedControl == index, false, "Block Dash Update:", 165, bools[mConfig.BlockDashUpdate], 40, yPos, 640, 30);
			}
			else if (index == 18)
			{
				component::splitButton(mSelectedControl == index, false, "Reset On Eject:", 165, bools[mConfig.ResetOnEject], 40, yPos, 640, 30);
			}

			yPos += 40;
		}
	}

	mHasFilePicker = false;
	switch(mSelectedControl) {
		case 3: case 4: case 5:
		case 6: case 7: case 8:
		case 9: {
			mHasFilePicker = true;
		} break;
	}

	uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	if(mShowingInfo) {
		char *infoStr = getOptionInfo(mSelectedControl);
		component::textBox(infoStr, true, false, horizAlignmentLeft, 60, yPos + 5, 600, 260, true, true);
	}

	if(!mShowingInfo) {
		const char* baseBtns = "\xC2\xA1 or \xC2\xB2\xC2\xB3 Change Value, \xC2\xA4 Defaults";
		const char* filePkrBtn = ", \xC2\xB5 Browse";
		const char* saveBtn = ", \xC2\xA3 Save";

		char* buttons = stringUtility::formatString(
			"%s%s%s",
			baseBtns,
			mHasFilePicker ? filePkrBtn : "", 
			mNeedsSave ? saveBtn : ""
		);

		drawing::drawBitmapString(context::getBitmapFontSmall(), buttons, theme::getFooterTextColor(), 40, theme::getFooterY());
		free(buttons);
	}

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mShowingInfo ? "\xC2\xA2 Back" : "\xC2\xB6 Info  \xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

char* cerbiosIniEditorScene::shortenString(const char* value)
{
	int textWidth;
	int textHeight;
	drawing::measureBitmapString(context::getBitmapFontSmall(), value, &textWidth, &textHeight);

	char* temp = strdup(value);

	int maxWidth = (640 - 165) - 20;
	if (textWidth >= maxWidth)
	{
		for (int i = 0; i < (int)strlen(value); i++)
		{
			free(temp);
			temp = stringUtility::formatString("...%s", value + i);
			drawing::measureBitmapString(context::getBitmapFontSmall(), temp, &textWidth, &textHeight);
			if (textWidth < maxWidth)
			{
				break;
			}
		}
	}

	return temp;
}