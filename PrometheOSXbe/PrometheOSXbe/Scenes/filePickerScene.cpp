#include "filePickerScene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\driveManager.h"
#include "..\fileSystem.h"
#include "..\pointerVector.h"
#include "..\theme.h"
#include "..\Threads\driveMounter.h"

pointerVector<fileSystem::FileInfoDetail*>* filePickerScene::getFileInfoDetails()
{
	if (strlen(mCurrentPath) == 0)
	{
		pointerVector<fileSystem::FileInfoDetail*>* fileInfoDetails = new pointerVector<fileSystem::FileInfoDetail*>(true);
		for (uint32_t i = 0; i < mMountedDrives->count(); i++)
		{
			if(mHideMMU) {
				char* mountPoint = stringUtility::formatString("%s:", mMountedDrives->get(i));
				char* mountAlias = driveManager::convertMountPointToMountPointAlias(mountPoint);
				free(mountPoint);

				if (stringUtility::startsWith(mountAlias, "MMU", true)) {
					free(mountAlias);
					continue;
				}
				free(mountAlias);
			}

			char* mountPoint = mMountedDrives->get(i);
			fileSystem::FileInfoDetail* fileInfoDetail = new fileSystem::FileInfoDetail();
			fileInfoDetail->path = stringUtility::formatString("%s:", mountPoint);
			fileInfoDetail->isDirectory = true;
			fileInfoDetail->isFile = false;
			fileInfoDetails->add(fileInfoDetail);
		}
		return fileInfoDetails;
	}

	pointerVector<fileSystem::FileInfoDetail*>* result = fileSystem::fileGetFileInfoDetails(mCurrentPath);
	if (result != NULL)
	{
		for (int32_t i = (int32_t)result->count() - 1; i >= 0; i--)
		{
			fileSystem::FileInfoDetail* fileInfoDetail = result->get(i);

			bool valid = fileInfoDetail->isDirectory;
			if (fileInfoDetail->isFile)
			{
				if (mFilePickerType == filePickerTypeBios)
				{
					valid = context::getModchip()->isValidBankSize(fileInfoDetail->size);
				}
				else if (mFilePickerType == filePickerTypeEeprom)
				{
					valid = fileInfoDetail->size == 256;
				}
				else if (mFilePickerType == filePickerTypeUpdate)
				{
					valid = context::getModchip()->isValidFlashSize(false, fileInfoDetail->size);
				}
				else if (mFilePickerType == filePickerTypeUpdateRecovery)
				{
					valid = context::getModchip()->isValidFlashSize(true, fileInfoDetail->size);
				}
				else if (mFilePickerType == filePickerTypeXbe)
				{
					char* ext = fileSystem::getExtension(fileInfoDetail->path);
					valid = stricmp(ext, ".xbe") == 0;
					free(ext);
				}
			}

			if (valid == false)
			{
				result->remove(i);
			}
		}
	}

	return result;
}

filePickerScene::filePickerScene(filePickerType filePickerType, bool useDevPath, bool hideMMU)
{
	driveMounter::startThread(false);

	mHideMMU = hideMMU;
	mUseDevPath = useDevPath;
	mInitialized = false;
	mFilePickerType = filePickerType;
	mSelectedControl = 0;
	mScrollPosition = 0;
	mMountedDrives = NULL;
	mCurrentPath = NULL;
	mFileInfoDetails = NULL;
	mFilePath = NULL;
}

filePickerScene::~filePickerScene()
{
	delete(mMountedDrives);
	free(mCurrentPath);
	delete(mFileInfoDetails);
	free(mFilePath);
}

void filePickerScene::update()
{
	if (mInitialized == false)
	{
		if (driveMounter::completed() == true)
		{
			bool exists = false;
			driveMounter::closeThread();
			mMountedDrives = driveManager::getMountedDrives();

			bool biosFolderExists = false;
			mCurrentPath = strdup("");
			//mCurrentPath = fileSystem::directoryExists("C:\\Bios", biosFolderExists) && biosFolderExists == true ? strdup("C:\\Bios") : strdup("");
			mFileInfoDetails = getFileInfoDetails();
			mInitialized = true;
		}
		return;
	}

	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene(sceneResultCancelled);
		return;
	}

	// Parent Path

	if (inputManager::buttonPressed(ButtonX))
	{
		char* newPath = fileSystem::getDirectory(mCurrentPath);
		free(mCurrentPath);
		mCurrentPath = newPath;

		delete(mFileInfoDetails);
		mFileInfoDetails = getFileInfoDetails();
		mSelectedControl = 0;
		mScrollPosition = 0;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		if (mFileInfoDetails != NULL)
		{
			for (uint32_t i = 0; i < mFileInfoDetails->count(); i++)
			{
				if (mSelectedControl != i)
				{
					continue;
				}
				
				fileSystem::FileInfoDetail* fileInfoDetail = mFileInfoDetails->get(i);
				if (fileInfoDetail->isDirectory)
				{
					char* newPath = strdup(fileInfoDetail->path);
					free(mCurrentPath);
					mCurrentPath = newPath;
			
					delete(mFileInfoDetails);
					mFileInfoDetails = getFileInfoDetails();
					mSelectedControl = 0;
					mScrollPosition = 0;
					break;
				}
				else if (fileInfoDetail->isFile)
				{
					if(mUseDevPath) {
						char* drvLtr = fileSystem::getDriveLetter(fileInfoDetail->path);
						char* drvPart = driveManager::getDrivePartition(drvLtr);
						char* rootPath = fileSystem::getRootPath(fileInfoDetail->path);

						mFilePath = stringUtility::formatString("%s%s", drvPart, rootPath);

						free(rootPath);
						free(drvPart);
						free(drvLtr);
					} else {
						mFilePath = strdup(fileInfoDetail->path);
					}
					sceneManager::popScene(sceneResultDone);
					return;
				}
			}
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mFileInfoDetails != NULL)
		{
			mSelectedControl = mSelectedControl < (int)(mFileInfoDetails->count() - 1) ? mSelectedControl + 1 : 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mFileInfoDetails != NULL)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mFileInfoDetails->count() - 1); 
		}
	}
}

void filePickerScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);

	if (mFilePickerType == filePickerTypeBios)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select BIOS to flash...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mFilePickerType == filePickerTypeEeprom)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select Eeprom to flash...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mFilePickerType == filePickerTypeUpdate)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select PrometheOS to flash...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mFilePickerType == filePickerTypeUpdateRecovery)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select Recovery to flash...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mFilePickerType == filePickerTypeXbe)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select XBE...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}

	if (mInitialized == false)
	{
		component::textBox("Please Wait...", false, false, horizAlignmentCenter, 40, 218, 640, 44);
	}
	else if (mFileInfoDetails != NULL)
	{
		int32_t maxItems = 7;

		int32_t start = 0;
		if ((int32_t)mFileInfoDetails->count() >= maxItems)
		{
			start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mFileInfoDetails->count() - maxItems);
		}

		int32_t itemCount = min(start + maxItems, (int32_t)mFileInfoDetails->count()) - start; 
		if (itemCount > 0)
		{
			int32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
			yPos += theme::getCenterOffset();

			for (int32_t i = 0; i < itemCount; i++)
			{
				uint32_t index = start + i;
				fileSystem::FileInfoDetail* fileInfoDetail = mFileInfoDetails->get(index);

				if (strlen(mCurrentPath) == 0)
				{
					char* tempFileName = fileSystem::getFileName(fileInfoDetail->path);
					char* fileName = driveManager::convertMountPointToMountPointAlias(tempFileName);
					component::fileButton(mSelectedControl == index, false, fileInfoDetail->isFile, fileName, 40, yPos, 640, 30);
					free(fileName);
					free(tempFileName);	
				}
				else
				{
					char* fileName = fileSystem::getFileName(fileInfoDetail->path);
					component::fileButton(mSelectedControl == index, false, fileInfoDetail->isFile, fileName, 40, yPos, 640, 30);
					free(fileName);
				}

				yPos += 40;
			}
		}
		else
		{
			int yPos = ((context::getBufferHeight() - 44) / 2);
			yPos += theme::getCenterOffset();

			component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select \xC2\xA3 Parent", mInitialized ? theme::getFooterTextColor() : theme::getTextDisabledColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", mInitialized ? theme::getFooterTextColor() : theme::getTextDisabledColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

char* filePickerScene::getFilePath()
{
	return strdup(mFilePath);
}