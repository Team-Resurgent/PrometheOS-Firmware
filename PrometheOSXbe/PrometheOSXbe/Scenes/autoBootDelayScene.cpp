#include "autoBootDelayScene.h"
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
#include "..\fileSystem.h"

autoBootDelayScene::autoBootDelayScene()
{
	mSelectedControl = settingsManager::getAutoBootDelay();
	mMenuItems = new pointerVector(false);

	for (int i = 0; i < 6; i++)
	{	
		mMenuItems->add(stringUtility::formatString("%i Seconds", i));
	}
}

autoBootDelayScene::~autoBootDelayScene()
{
	delete(mMenuItems);
}

void autoBootDelayScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::openScene(sceneItemPrometheOsSettingsScene);
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		settingsManager::setAutoBootDelay((uint8_t)mSelectedControl);
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < (int)(mMenuItems->count() - 1) ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mMenuItems->count() - 1); 
	}
}

void autoBootDelayScene::render()
{
	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Auto Boot Delay option...", theme::getTitleTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)mMenuItems->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mMenuItems->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)mMenuItems->count()) - start; 

	int32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	for (int32_t i = 0; i < itemCount; i++)
	{
		uint32_t index = start + i;
		char* delay = (char*)mMenuItems->get(index);
		component::button(mSelectedControl == index, false, delay, 193, yPos, 322, 30);
		yPos += 40;
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}