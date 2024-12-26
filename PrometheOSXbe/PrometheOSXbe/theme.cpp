#include "theme.h"
#include "fileSystem.h"
#include "stringUtility.h"
#include "drawing.h"
#include "alignment.h"
#include "pointerVector.h"
#include "utils.h"
#include "audioPlayer.h"
#include "timeUtility.h"

#include <time.h>

namespace
{
	theme::ThemeData mThemeData;
	uint32_t mBackgroundFrameCount = 0;
}

char* theme::getSkinAuthor()
{
	return strdup(&mThemeData.SKIN_AUTHOR[0]);
}

uint32_t theme::getBackgroundFrameCount()
{
	return mBackgroundFrameCount;
}

bool theme::getBackgroundOverlayAsUnderlay()
{
	return mThemeData.BACKGROUND_OVERLAY_AS_UNDERLAY != 0;
}

uint32_t theme::getBackgroundFrameDelay()
{
	return mThemeData.BACKGROUND_FRAME_DELAY;
}

bool theme::getBackgroundFramePingPong()
{
	return mThemeData.BACKGROUND_FRAME_PINGPONG != 0;
}

uint32_t theme::getBackgroundColor()
{
	return mThemeData.BACKGROUND_COLOR;
}

uint32_t theme::getBackgroundImageTint()
{
	return mThemeData.BACKGROUND_IMAGE_TINT;
}

uint32_t theme::getBackgroundOverlayImageTint()
{
	return mThemeData.BACKGROUND_OVERLAY_IMAGE_TINT;
}

uint32_t theme::getPrometheosColor()
{
	return mThemeData.PROMETHEOS_COLOR;
}

horizAlignment theme::getPrometheosAlign()
{
	if (mThemeData.PROMETHEOS_ALIGN == 1)
	{
		return horizAlignmentCenter;
	}
	else if (mThemeData.PROMETHEOS_ALIGN == 2)
	{
		return horizAlignmentRight;
	}
	return horizAlignmentLeft;
}

uint32_t theme::getPrometheosY()
{
	return mThemeData.PROMETHEOS_Y;
}

uint32_t theme::getInstallerTint()
{
	return mThemeData.INSTALLER_TINT;
}

uint32_t theme::getTextColor()
{
	return mThemeData.TEXT_COLOR;
}

uint32_t theme::getTextDisabledColor()
{
	return mThemeData.TEXT_DISABLED_COLOR;
}

uint32_t theme::getHeaderTextColor()
{
	return mThemeData.HEADER_TEXT_COLOR;
}

uint32_t theme::getFooterTextColor()
{
	return mThemeData.FOOTER_TEXT_COLOR;
}

horizAlignment theme::getHeaderAlign()
{
	if (mThemeData.HEADER_ALIGN == 1)
	{
		return horizAlignmentCenter;
	}
	else if (mThemeData.HEADER_ALIGN == 2)
	{
		return horizAlignmentRight;
	}
	return horizAlignmentLeft;
}

uint32_t theme::getHeaderY()
{
	return mThemeData.HEADER_Y;
}

int32_t theme::getCenterOffset()
{
	return mThemeData.CENTER_OFFSET;
}

uint32_t theme::getFooterY()
{
	return mThemeData.FOOTER_Y;
}

uint32_t theme::getSnakeWallColor()
{
	return mThemeData.SNAKE_WALL_COLOR;
}

uint32_t theme::getSnakeFoodColor()
{
	return mThemeData.SNAKE_FOOD_COLOR;
}

uint32_t theme::getSnakeHeadColor()
{
	return mThemeData.SNAKE_HEAD_COLOR;
}

uint32_t theme::getSnakeTailColor()
{
	return mThemeData.SNAKE_TAIL_COLOR;
}

uint32_t theme::getInvadersWallColor()
{
	return mThemeData.INVADERS_WALL_COLOR;
}

uint32_t theme::getInvadersPlayerColor()
{
	return mThemeData.INVADERS_PLAYER_COLOR;
}

uint32_t theme::getInvadersBossColor()
{
	return mThemeData.INVADERS_BOSS_COLOR;
}

uint32_t theme::getInvadersAlienColor()
{
	return mThemeData.INVADERS_ALIEN_COLOR;
}

uint32_t theme::getJoyButtonAColor()
{
	return mThemeData.JOY_BUTTON_A_COLOR;
}

uint32_t theme::getJoyButtonBColor()
{
	return mThemeData.JOY_BUTTON_B_COLOR;
}

uint32_t theme::getJoyButtonXColor()
{
	return mThemeData.JOY_BUTTON_X_COLOR;
}

uint32_t theme::getJoyButtonYColor()
{
	return mThemeData.JOY_BUTTON_Y_COLOR;
}

uint32_t theme::getJoyButtonBlackColor()
{
	return mThemeData.JOY_BUTTON_BLACK_COLOR;
}

uint32_t theme::getJoyButtonWhiteColor()
{
	return mThemeData.JOY_BUTTON_WHITE_COLOR;
}

uint32_t theme::getButtonActiveFillColor() 
{
	return mThemeData.BUTTON_ACTIVE_FILL_COLOR;
}

uint32_t theme::getButtonActiveStrokeColor() 
{
	return mThemeData.BUTTON_ACTIVE_STROKE_COLOR;
}

uint32_t theme::getButtonActiveTextColor() 
{
	return mThemeData.BUTTON_ACTIVE_TEXT_COLOR;
}

uint32_t theme::getButtonActiveHoverFillColor() 
{
	return mThemeData.BUTTON_ACTIVE_HOVER_FILL_COLOR;
}

uint32_t theme::getButtonActiveHoverStrokeColor() 
{
	return mThemeData.BUTTON_ACTIVE_HOVER_STROKE_COLOR;
}

uint32_t theme::getButtonActiveHoverTextColor() 
{
	return mThemeData.BUTTON_ACTIVE_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonInactiveFillColor() 
{
	return mThemeData.BUTTON_INACTIVE_FILL_COLOR;
}

uint32_t theme::getButtonInactiveStrokeColor()
{
	return mThemeData.BUTTON_INACTIVE_STROKE_COLOR;
}

uint32_t theme::getButtonInactiveTextColor()
{
	return mThemeData.BUTTON_INACTIVE_TEXT_COLOR;
}

uint32_t theme::getButtonInactiveHoverFillColor() 
{
	return mThemeData.BUTTON_INACTIVE_HOVER_FILL_COLOR;
}

uint32_t theme::getButtonInactiveHoverStrokeColor() 
{
	return mThemeData.BUTTON_INACTIVE_HOVER_STROKE_COLOR;
}

uint32_t theme::getButtonInactiveHoverTextColor() 
{
	return mThemeData.BUTTON_INACTIVE_HOVER_TEXT_COLOR;
}

uint32_t theme::getPanelFillColor()
{
	return mThemeData.PANEL_FILL_COLOR;
}

uint32_t theme::getPanelStrokeColor()
{
	return mThemeData.PANEL_STROKE_COLOR;
}

uint32_t theme::getTextPanelFillColor()
{
	return mThemeData.TEXT_PANEL_FILL_COLOR;
}
uint32_t theme::getTextPanelStrokeColor()
{
	return mThemeData.TEXT_PANEL_STROKE_COLOR;
}
uint32_t theme::getTextPanelTextColor()
{
	return mThemeData.TEXT_PANEL_TEXT_COLOR;
}
uint32_t theme::getTextPanelHoverFillColor()
{
	return mThemeData.TEXT_PANEL_HOVER_FILL_COLOR;
}
uint32_t theme::getTextPanelHoverStrokeColor()
{
	return mThemeData.TEXT_PANEL_HOVER_STROKE_COLOR;
}
uint32_t theme::getTextPanelHoverTextColor()
{
	return mThemeData.TEXT_PANEL_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedOffFillColor()
{ 
	return mThemeData.BUTTON_LED_OFF_FILL_COLOR;
}
uint32_t theme::getButtonLedOffStrokeColor()
{ 
	return mThemeData.BUTTON_LED_OFF_STROKE_COLOR;
}
uint32_t theme::getButtonLedOffTextColor()
{ 
	return mThemeData.BUTTON_LED_OFF_TEXT_COLOR;
}
uint32_t theme::getButtonLedOffHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_OFF_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedOffHoverStrokeColor()
{
	return mThemeData.BUTTON_LED_OFF_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedOffHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_OFF_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedRedFillColor()
{ 
	return mThemeData.BUTTON_LED_RED_FILL_COLOR;
}
uint32_t theme::getButtonLedRedStrokeColor()
{ 
	return mThemeData.BUTTON_LED_RED_STROKE_COLOR;
}
uint32_t theme::getButtonLedRedTextColor()
{ 
	return mThemeData.BUTTON_LED_RED_TEXT_COLOR;
}
uint32_t theme::getButtonLedRedHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_RED_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedRedHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_RED_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedRedHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_RED_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedGreenFillColor()
{ 
	return mThemeData.BUTTON_LED_GREEN_FILL_COLOR;
}
uint32_t theme::getButtonLedGreenStrokeColor()
{ 
	return mThemeData.BUTTON_LED_GREEN_STROKE_COLOR;
}
uint32_t theme::getButtonLedGreenTextColor()
{ 
	return mThemeData.BUTTON_LED_GREEN_TEXT_COLOR;
}
uint32_t theme::getButtonLedGreenHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_GREEN_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedGreenHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_GREEN_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedGreenHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_GREEN_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedYellowFillColor()
{ 
	return mThemeData.BUTTON_LED_YELLOW_FILL_COLOR;
}
uint32_t theme::getButtonLedYellowStrokeColor()
{ 
	return mThemeData.BUTTON_LED_YELLOW_STROKE_COLOR;
}
uint32_t theme::getButtonLedYellowTextColor()
{ 
	return mThemeData.BUTTON_LED_YELLOW_TEXT_COLOR;
}
uint32_t theme::getButtonLedYellowHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_YELLOW_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedYellowHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_YELLOW_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedYellowHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_YELLOW_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedBlueFillColor()
{ 
	return mThemeData.BUTTON_LED_BLUE_FILL_COLOR;
}
uint32_t theme::getButtonLedBlueStrokeColor()
{ 
	return mThemeData.BUTTON_LED_BLUE_STROKE_COLOR;
}
uint32_t theme::getButtonLedBlueTextColor()
{ 
	return mThemeData.BUTTON_LED_BLUE_TEXT_COLOR;
}
uint32_t theme::getButtonLedBlueHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_BLUE_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedBlueHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_BLUE_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedBlueHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_BLUE_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedPurpleFillColor()
{ 
	return mThemeData.BUTTON_LED_PURPLE_FILL_COLOR;
}
uint32_t theme::getButtonLedPurpleStrokeColor()
{ 
	return mThemeData.BUTTON_LED_PURPLE_STROKE_COLOR;
}
uint32_t theme::getButtonLedPurpleTextColor()
{ 
	return mThemeData.BUTTON_LED_PURPLE_TEXT_COLOR;
}
uint32_t theme::getButtonLedPurpleHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_PURPLE_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedPurpleHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_PURPLE_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedPurpleHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_PURPLE_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedTurquoiseFillColor()
{ 
	return mThemeData.BUTTON_LED_TURQUOISE_FILL_COLOR;
}
uint32_t theme::getButtonLedTurquoiseStrokeColor()
{ 
	return mThemeData.BUTTON_LED_TURQUOISE_STROKE_COLOR;
}
uint32_t theme::getButtonLedTurquoiseTextColor()
{ 
	return mThemeData.BUTTON_LED_TURQUOISE_TEXT_COLOR;
}
uint32_t theme::getButtonLedTurquoiseHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedTurquoiseHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedTurquoiseHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR;
}

uint32_t theme::getButtonLedWhiteFillColor()
{ 
	return mThemeData.BUTTON_LED_WHITE_FILL_COLOR;
}
uint32_t theme::getButtonLedWhiteStrokeColor()
{ 
	return mThemeData.BUTTON_LED_WHITE_STROKE_COLOR;
}
uint32_t theme::getButtonLedWhiteTextColor()
{ 
	return mThemeData.BUTTON_LED_WHITE_TEXT_COLOR;
}
uint32_t theme::getButtonLedWhiteHoverFillColor()
{ 
	return mThemeData.BUTTON_LED_WHITE_HOVER_FILL_COLOR;
}
uint32_t theme::getButtonLedWhiteHoverStrokeColor()
{ 
	return mThemeData.BUTTON_LED_WHITE_HOVER_STROKE_COLOR;
}
uint32_t theme::getButtonLedWhiteHoverTextColor()
{ 
	return mThemeData.BUTTON_LED_WHITE_HOVER_TEXT_COLOR;
}

void theme::loadSkin(char* skinName)
{
	memset(&mThemeData.SKIN_AUTHOR[0], 0, sizeof(mThemeData.SKIN_AUTHOR));

	mThemeData.BACKGROUND_OVERLAY_AS_UNDERLAY = THEME_BACKGROUND_OVERLAY_AS_UNDERLAY;
	mThemeData.BACKGROUND_FRAME_DELAY = THEME_BACKGROUND_FRAME_DELAY;
	mThemeData.BACKGROUND_FRAME_PINGPONG = THEME_BACKGROUND_FRAME_PINGPONG;
	mThemeData.BACKGROUND_COLOR = THEME_BACKGROUND_COLOR;
	mThemeData.BACKGROUND_IMAGE_TINT = THEME_BACKGROUND_IMAGE_TINT;
	mThemeData.BACKGROUND_OVERLAY_IMAGE_TINT = THEME_BACKGROUND_OVERLAY_IMAGE_TINT;
	mThemeData.PROMETHEOS_COLOR = THEME_PROMETHEOS_COLOR;
	mThemeData.PROMETHEOS_ALIGN = THEME_PROMETHEOS_ALIGN;
	mThemeData.PROMETHEOS_Y = THEME_PROMETHEOS_Y;
	mThemeData.INSTALLER_TINT = THEME_INSTALLER_TINT;
	mThemeData.TEXT_COLOR = THEME_TEXT_COLOR;
	mThemeData.TEXT_DISABLED_COLOR = THEME_TEXT_DISABLED_COLOR;
	mThemeData.HEADER_TEXT_COLOR = THEME_HEADER_TEXT_COLOR;
	mThemeData.FOOTER_TEXT_COLOR = THEME_FOOTER_TEXT_COLOR;

	mThemeData.HEADER_ALIGN = THEME_HEADER_ALIGN;
	mThemeData.HEADER_Y = THEME_HEADER_Y;
	mThemeData.CENTER_OFFSET = THEME_CENTER_OFFSET;
	mThemeData.FOOTER_Y = THEME_FOOTER_Y;

	mThemeData.SNAKE_WALL_COLOR = THEME_SNAKE_WALL_COLOR;
	mThemeData.SNAKE_FOOD_COLOR = THEME_SNAKE_FOOD_COLOR;
	mThemeData.SNAKE_HEAD_COLOR = THEME_SNAKE_HEAD_COLOR;
	mThemeData.SNAKE_TAIL_COLOR = THEME_SNAKE_TAIL_COLOR;

	mThemeData.INVADERS_WALL_COLOR = THEME_INVADERS_WALL_COLOR;
	mThemeData.INVADERS_PLAYER_COLOR = THEME_INVADERS_PLAYER_COLOR;
	mThemeData.INVADERS_BOSS_COLOR = THEME_INVADERS_BOSS_COLOR;
	mThemeData.INVADERS_ALIEN_COLOR = THEME_INVADERS_ALIEN_COLOR;

	mThemeData.JOY_BUTTON_A_COLOR = THEME_JOY_BUTTON_A_COLOR;
	mThemeData.JOY_BUTTON_B_COLOR = THEME_JOY_BUTTON_B_COLOR;
	mThemeData.JOY_BUTTON_X_COLOR = THEME_JOY_BUTTON_X_COLOR;
	mThemeData.JOY_BUTTON_Y_COLOR = THEME_JOY_BUTTON_Y_COLOR;
	mThemeData.JOY_BUTTON_BLACK_COLOR = THEME_JOY_BUTTON_BLACK_COLOR;
	mThemeData.JOY_BUTTON_WHITE_COLOR = THEME_JOY_BUTTON_WHITE_COLOR;

	mThemeData.BUTTON_ACTIVE_FILL_COLOR = THEME_BUTTON_ACTIVE_FILL_COLOR;
	mThemeData.BUTTON_ACTIVE_STROKE_COLOR = THEME_BUTTON_ACTIVE_STROKE_COLOR;
	mThemeData.BUTTON_ACTIVE_TEXT_COLOR = THEME_BUTTON_ACTIVE_TEXT_COLOR;

	mThemeData.BUTTON_ACTIVE_HOVER_FILL_COLOR = THEME_BUTTON_ACTIVE_HOVER_FILL_COLOR;
	mThemeData.BUTTON_ACTIVE_HOVER_STROKE_COLOR = THEME_BUTTON_ACTIVE_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_ACTIVE_HOVER_TEXT_COLOR = THEME_BUTTON_ACTIVE_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_INACTIVE_FILL_COLOR = THEME_BUTTON_INACTIVE_FILL_COLOR;
	mThemeData.BUTTON_INACTIVE_STROKE_COLOR = THEME_BUTTON_INACTIVE_STROKE_COLOR;
	mThemeData.BUTTON_INACTIVE_TEXT_COLOR = THEME_BUTTON_INACTIVE_TEXT_COLOR;

	mThemeData.BUTTON_INACTIVE_HOVER_FILL_COLOR = THEME_BUTTON_INACTIVE_HOVER_FILL_COLOR;
	mThemeData.BUTTON_INACTIVE_HOVER_STROKE_COLOR = THEME_BUTTON_INACTIVE_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_INACTIVE_HOVER_TEXT_COLOR = THEME_BUTTON_INACTIVE_HOVER_TEXT_COLOR;

	mThemeData.PANEL_FILL_COLOR = THEME_PANEL_FILL_COLOR;
	mThemeData.PANEL_STROKE_COLOR = THEME_PANEL_STROKE_COLOR;

	mThemeData.TEXT_PANEL_FILL_COLOR = THEME_TEXT_PANEL_FILL_COLOR;
	mThemeData.TEXT_PANEL_STROKE_COLOR = THEME_TEXT_PANEL_STROKE_COLOR;
	mThemeData.TEXT_PANEL_TEXT_COLOR = THEME_TEXT_PANEL_TEXT_COLOR;
	mThemeData.TEXT_PANEL_HOVER_FILL_COLOR = THEME_TEXT_PANEL_HOVER_FILL_COLOR;
	mThemeData.TEXT_PANEL_HOVER_STROKE_COLOR = THEME_TEXT_PANEL_HOVER_STROKE_COLOR;
	mThemeData.TEXT_PANEL_HOVER_TEXT_COLOR = THEME_TEXT_PANEL_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_OFF_FILL_COLOR = THEME_BUTTON_LED_OFF_FILL_COLOR;
	mThemeData.BUTTON_LED_OFF_STROKE_COLOR = THEME_BUTTON_LED_OFF_STROKE_COLOR;
	mThemeData.BUTTON_LED_OFF_TEXT_COLOR = THEME_BUTTON_LED_OFF_TEXT_COLOR;
	mThemeData.BUTTON_LED_OFF_HOVER_FILL_COLOR = THEME_BUTTON_LED_OFF_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_OFF_HOVER_STROKE_COLOR = THEME_BUTTON_LED_OFF_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_OFF_HOVER_TEXT_COLOR = THEME_BUTTON_LED_OFF_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_RED_FILL_COLOR = THEME_BUTTON_LED_RED_FILL_COLOR;
	mThemeData.BUTTON_LED_RED_STROKE_COLOR = THEME_BUTTON_LED_RED_STROKE_COLOR;
	mThemeData.BUTTON_LED_RED_TEXT_COLOR = THEME_BUTTON_LED_RED_TEXT_COLOR;
	mThemeData.BUTTON_LED_RED_HOVER_FILL_COLOR = THEME_BUTTON_LED_RED_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_RED_HOVER_STROKE_COLOR = THEME_BUTTON_LED_RED_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_RED_HOVER_TEXT_COLOR = THEME_BUTTON_LED_RED_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_GREEN_FILL_COLOR = THEME_BUTTON_LED_GREEN_FILL_COLOR;
	mThemeData.BUTTON_LED_GREEN_STROKE_COLOR = THEME_BUTTON_LED_GREEN_STROKE_COLOR;
	mThemeData.BUTTON_LED_GREEN_TEXT_COLOR = THEME_BUTTON_LED_GREEN_TEXT_COLOR;
	mThemeData.BUTTON_LED_GREEN_HOVER_FILL_COLOR = THEME_BUTTON_LED_GREEN_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_GREEN_HOVER_STROKE_COLOR = THEME_BUTTON_LED_GREEN_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_GREEN_HOVER_TEXT_COLOR = THEME_BUTTON_LED_GREEN_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_YELLOW_FILL_COLOR = THEME_BUTTON_LED_YELLOW_FILL_COLOR;
	mThemeData.BUTTON_LED_YELLOW_STROKE_COLOR = THEME_BUTTON_LED_YELLOW_STROKE_COLOR;
	mThemeData.BUTTON_LED_YELLOW_TEXT_COLOR = THEME_BUTTON_LED_YELLOW_TEXT_COLOR;
	mThemeData.BUTTON_LED_YELLOW_HOVER_FILL_COLOR = THEME_BUTTON_LED_YELLOW_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_YELLOW_HOVER_STROKE_COLOR = THEME_BUTTON_LED_YELLOW_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_YELLOW_HOVER_TEXT_COLOR = THEME_BUTTON_LED_YELLOW_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_BLUE_FILL_COLOR = THEME_BUTTON_LED_BLUE_FILL_COLOR;
	mThemeData.BUTTON_LED_BLUE_STROKE_COLOR = THEME_BUTTON_LED_BLUE_STROKE_COLOR;
	mThemeData.BUTTON_LED_BLUE_TEXT_COLOR = THEME_BUTTON_LED_BLUE_TEXT_COLOR;
	mThemeData.BUTTON_LED_BLUE_HOVER_FILL_COLOR = THEME_BUTTON_LED_BLUE_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_BLUE_HOVER_STROKE_COLOR = THEME_BUTTON_LED_BLUE_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_BLUE_HOVER_TEXT_COLOR = THEME_BUTTON_LED_BLUE_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_PURPLE_FILL_COLOR = THEME_BUTTON_LED_PURPLE_FILL_COLOR;
	mThemeData.BUTTON_LED_PURPLE_STROKE_COLOR = THEME_BUTTON_LED_PURPLE_STROKE_COLOR;
	mThemeData.BUTTON_LED_PURPLE_TEXT_COLOR = THEME_BUTTON_LED_PURPLE_TEXT_COLOR;
	mThemeData.BUTTON_LED_PURPLE_HOVER_FILL_COLOR = THEME_BUTTON_LED_PURPLE_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_PURPLE_HOVER_STROKE_COLOR = THEME_BUTTON_LED_PURPLE_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_PURPLE_HOVER_TEXT_COLOR = THEME_BUTTON_LED_PURPLE_HOVER_TEXT_COLOR;
		
	mThemeData.BUTTON_LED_TURQUOISE_FILL_COLOR = THEME_BUTTON_LED_TURQUOISE_FILL_COLOR;
	mThemeData.BUTTON_LED_TURQUOISE_STROKE_COLOR = THEME_BUTTON_LED_TURQUOISE_STROKE_COLOR;
	mThemeData.BUTTON_LED_TURQUOISE_TEXT_COLOR = THEME_BUTTON_LED_TURQUOISE_TEXT_COLOR;
	mThemeData.BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR = THEME_BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR = THEME_BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR = THEME_BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR;

	mThemeData.BUTTON_LED_WHITE_FILL_COLOR = THEME_BUTTON_LED_WHITE_FILL_COLOR;
	mThemeData.BUTTON_LED_WHITE_STROKE_COLOR = THEME_BUTTON_LED_WHITE_STROKE_COLOR;
	mThemeData.BUTTON_LED_WHITE_TEXT_COLOR = THEME_BUTTON_LED_WHITE_TEXT_COLOR;
	mThemeData.BUTTON_LED_WHITE_HOVER_FILL_COLOR = THEME_BUTTON_LED_WHITE_HOVER_FILL_COLOR;
	mThemeData.BUTTON_LED_WHITE_HOVER_STROKE_COLOR = THEME_BUTTON_LED_WHITE_HOVER_STROKE_COLOR;
	mThemeData.BUTTON_LED_WHITE_HOVER_TEXT_COLOR = THEME_BUTTON_LED_WHITE_HOVER_TEXT_COLOR;

	for (uint32_t i = 0; i < mBackgroundFrameCount; i++)
	{
		char* backgroundName = stringUtility::formatString("background:%i", i);
		drawing::removeImage(backgroundName);
		free(backgroundName);
	}
	drawing::removeImage("background-overlay");
	mBackgroundFrameCount = 0;

	if (strlen(skinName) == 0)
	{
		return;
	}
	else if (stringUtility::equals(skinName, "*", false) == true)
	{
		loadRandomSkin();
		return;
	}

	char* skinPath = stringUtility::formatString("HDD0-E:\\PrometheOS\\Skins\\%s", skinName);

	uint32_t fileHandle;

	char* iniPath = fileSystem::combinePath(skinPath, "theme.ini");
	if (fileSystem::fileOpen(iniPath, fileSystem::FileModeRead, fileHandle))
	{
		uint32_t fileSize;
		if (fileSystem::fileSize(fileHandle, fileSize))
		{
			char* buffer = (char*)malloc(fileSize);
			uint32_t bytesRead;
			if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize)
			{
				loadConfig(buffer, fileSize);
			}
			free(buffer);
		}
		fileSystem::fileClose(fileHandle);
	}
	free(iniPath);

	uint64_t totalMemUsed = 0;

	char* backgroundSkinPath = fileSystem::combinePath(skinPath, "backgrounds");
	pointerVector<fileSystem::FileInfoDetail*>* result = fileSystem::fileGetFileInfoDetails(backgroundSkinPath);
	if (result != NULL)
	{
		for (uint32_t i = 0; i < result->count(); i++)
		{
			if (totalMemUsed > (20 * 1024 * 1024))
			{
				utils::debugPrint("Skipping loading any remaining textures, due to mem use.");
				break;
			}
			fileSystem::FileInfoDetail* fileInfoDetail = result->get(i);
			char* backgroundName = stringUtility::formatString("background:%i", mBackgroundFrameCount);
			if (loadImage(fileInfoDetail->path, backgroundName))
			{
				totalMemUsed += drawing::getImageMemUse(backgroundName);
				mBackgroundFrameCount++;
			}
			free(backgroundName);
		}
	}
	delete(result);
	free(backgroundSkinPath);

	char* overlaySkinPath = fileSystem::combinePath(skinPath, "background-overlay.png");
	loadImage(overlaySkinPath, "background-overlay");

	free(skinPath);
}

void theme::parseConfigLine(char* param1, char* param2, char* buffer, unsigned long bufferSize)
{
    char* params[2];
    params[0] = param1;
    params[1] = param2;

	uint32_t paramLengths[2];
    paramLengths[0] = 0;
    paramLengths[1] = 0;

    uint32_t paramIndex = 0;
    for (uint32_t i = 0; i < bufferSize; i++) {
        char currentChar = buffer[i];
        if (paramIndex > 1) {
            return;
        }
        else if (currentChar == '=') {
            params[paramIndex][paramLengths[paramIndex]] = 0;
            paramIndex++;
            continue;
        }
        else if (currentChar == ';') {
            break;
        }
        if (paramLengths[paramIndex] > 255) {
            continue;
        }
        params[paramIndex][paramLengths[paramIndex]] = currentChar;
        paramLengths[paramIndex] = paramLengths[paramIndex] + 1;
    }
    params[paramIndex][paramLengths[paramIndex]] = 0;

    if (paramIndex != 1) {
        return;
    }

    trimSpace(params[0]);
    upperCase(params[0]);

    trimSpace(params[1]);

	if (strcmp(params[0], "SKIN_AUTHOR") == 0) {
		strncpy(&mThemeData.SKIN_AUTHOR[0], params[1], 49);
	} else if (strcmp(params[0], "BACKGROUND_OVERLAY_AS_UNDERLAY") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BACKGROUND_OVERLAY_AS_UNDERLAY);
	} else if (strcmp(params[0], "BACKGROUND_FRAME_DELAY") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BACKGROUND_FRAME_DELAY);
	} else if (strcmp(params[0], "BACKGROUND_FRAME_PINGPONG") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BACKGROUND_FRAME_PINGPONG);
	} else if (strcmp(params[0], "BACKGROUND_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BACKGROUND_COLOR);
	} else if (strcmp(params[0], "BACKGROUND_IMAGE_TINT") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BACKGROUND_IMAGE_TINT);
	} else if (strcmp(params[0], "BACKGROUND_OVERLAY_IMAGE_TINT") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BACKGROUND_OVERLAY_IMAGE_TINT);
    } else if (strcmp(params[0], "PROMETHEOS_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.PROMETHEOS_COLOR);
    } else if (strcmp(params[0], "PROMETHEOS_ALIGN") == 0) {
        parseUnsignedNumber(params[1], mThemeData.PROMETHEOS_ALIGN);
    } else if (strcmp(params[0], "PROMETHEOS_Y") == 0) {
        parseUnsignedNumber(params[1], mThemeData.PROMETHEOS_Y);
    } else if (strcmp(params[0], "INSTALLER_TINT") == 0 || strcmp(params[0], "INSTALLER_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.INSTALLER_TINT);
    } else if (strcmp(params[0], "TEXT_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.TEXT_COLOR);
    } else if (strcmp(params[0], "TEXT_DISABLED_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.TEXT_DISABLED_COLOR);
    } else if (strcmp(params[0], "HEADER_TEXT_COLOR") == 0 || strcmp(params[0], "TITLE_TEXT_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.HEADER_TEXT_COLOR);
    } else if (strcmp(params[0], "FOOTER_TEXT_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.FOOTER_TEXT_COLOR);
    } else if (strcmp(params[0], "HEADER_ALIGN") == 0) {
        parseUnsignedNumber(params[1], mThemeData.HEADER_ALIGN);
    } else if (strcmp(params[0], "HEADER_Y") == 0) {
        parseUnsignedNumber(params[1], mThemeData.HEADER_Y);
    } else if (strcmp(params[0], "CENTER_OFFSET") == 0) {
        parseSignedNumber(params[1], mThemeData.CENTER_OFFSET);
    } else if (strcmp(params[0], "FOOTER_Y") == 0) {
        parseUnsignedNumber(params[1], mThemeData.FOOTER_Y);
    } else if (strcmp(params[0], "SNAKE_WALL_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.SNAKE_WALL_COLOR);
    } else if (strcmp(params[0], "SNAKE_FOOD_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.SNAKE_FOOD_COLOR);
    } else if (strcmp(params[0], "SNAKE_HEAD_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.SNAKE_HEAD_COLOR);
    } else if (strcmp(params[0], "SNAKE_TAIL_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.SNAKE_TAIL_COLOR);
    } else if (strcmp(params[0], "INVADERS_WALL_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.INVADERS_WALL_COLOR);
    } else if (strcmp(params[0], "INVADERS_PLAYER_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.INVADERS_PLAYER_COLOR);
    } else if (strcmp(params[0], "INVADERS_BOSS_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.INVADERS_BOSS_COLOR);
    } else if (strcmp(params[0], "INVADERS_ALIEN_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.INVADERS_ALIEN_COLOR);
	} else if (strcmp(params[0], "JOY_BUTTON_A_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.JOY_BUTTON_A_COLOR);
	} else if (strcmp(params[0], "JOY_BUTTON_B_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.JOY_BUTTON_B_COLOR);
	} else if (strcmp(params[0], "JOY_BUTTON_X_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.JOY_BUTTON_X_COLOR);
	} else if (strcmp(params[0], "JOY_BUTTON_Y_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.JOY_BUTTON_Y_COLOR);
	} else if (strcmp(params[0], "JOY_BUTTON_BLACK_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.JOY_BUTTON_BLACK_COLOR);
	} else if (strcmp(params[0], "JOY_BUTTON_WHITE_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.JOY_BUTTON_WHITE_COLOR);
    } else if (strcmp(params[0], "BUTTON_ACTIVE_FILL_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BUTTON_ACTIVE_FILL_COLOR);
    } else if (strcmp(params[0], "BUTTON_ACTIVE_STROKE_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BUTTON_ACTIVE_STROKE_COLOR);
    } else if (strcmp(params[0], "BUTTON_ACTIVE_TEXT_COLOR") == 0) {
        parseUnsignedNumber(params[1], mThemeData.BUTTON_ACTIVE_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_ACTIVE_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_ACTIVE_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_ACTIVE_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_ACTIVE_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_ACTIVE_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_ACTIVE_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_INACTIVE_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_INACTIVE_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_INACTIVE_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_INACTIVE_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_INACTIVE_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_INACTIVE_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_INACTIVE_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_INACTIVE_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_INACTIVE_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_INACTIVE_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_INACTIVE_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_INACTIVE_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "PANEL_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.PANEL_FILL_COLOR);
	} else if (strcmp(params[0], "PANEL_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.PANEL_STROKE_COLOR);
	} else if (strcmp(params[0], "TEXT_PANEL_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.TEXT_PANEL_FILL_COLOR);
	} else if (strcmp(params[0], "TEXT_PANEL_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.TEXT_PANEL_STROKE_COLOR);
	} else if (strcmp(params[0], "TEXT_PANEL_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.TEXT_PANEL_TEXT_COLOR);
	} else if (strcmp(params[0], "TEXT_PANEL_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.TEXT_PANEL_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "TEXT_PANEL_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.TEXT_PANEL_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "TEXT_PANEL_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.TEXT_PANEL_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_OFF_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_OFF_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_OFF_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_OFF_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_OFF_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_OFF_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_OFF_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_OFF_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_OFF_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_OFF_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_OFF_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_OFF_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_RED_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_RED_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_RED_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_RED_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_RED_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_RED_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_RED_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_RED_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_RED_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_RED_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_RED_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_RED_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_GREEN_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_GREEN_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_GREEN_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_GREEN_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_GREEN_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_GREEN_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_GREEN_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_GREEN_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_GREEN_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_GREEN_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_GREEN_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_GREEN_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_YELLOW_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_YELLOW_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_YELLOW_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_YELLOW_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_YELLOW_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_YELLOW_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_YELLOW_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_YELLOW_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_YELLOW_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_YELLOW_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_YELLOW_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_YELLOW_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_BLUE_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_BLUE_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_BLUE_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_BLUE_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_BLUE_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_BLUE_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_BLUE_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_BLUE_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_BLUE_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_BLUE_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_BLUE_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_BLUE_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_PURPLE_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_PURPLE_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_PURPLE_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_PURPLE_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_PURPLE_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_PURPLE_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_PURPLE_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_PURPLE_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_PURPLE_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_PURPLE_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_PURPLE_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_PURPLE_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_TURQUOISE_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_TURQUOISE_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_TURQUOISE_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_TURQUOISE_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_TURQUOISE_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_TURQUOISE_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_WHITE_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_WHITE_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_WHITE_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_WHITE_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_WHITE_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_WHITE_TEXT_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_WHITE_HOVER_FILL_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_WHITE_HOVER_FILL_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_WHITE_HOVER_STROKE_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_WHITE_HOVER_STROKE_COLOR);
	} else if (strcmp(params[0], "BUTTON_LED_WHITE_HOVER_TEXT_COLOR") == 0) {
		parseUnsignedNumber(params[1], mThemeData.BUTTON_LED_WHITE_HOVER_TEXT_COLOR);
    }
}

void theme::parseUnsignedNumber(char* value, uint32_t& themeValue)
{
    uint32_t result = 0;

    trimSpace(value);
    upperCase(value);

	if (strlen(value) == 10 && strncmp(value, "0X", 2) == 0) 
	{
		result = (((stringUtility::hexCharToInt(value[2]) << 4) | stringUtility::hexCharToInt(value[3])) << 24) |
		         (((stringUtility::hexCharToInt(value[4]) << 4) | stringUtility::hexCharToInt(value[5])) << 16) |
		       	 (((stringUtility::hexCharToInt(value[6]) << 4) | stringUtility::hexCharToInt(value[7])) << 8) |
		   	     ((stringUtility::hexCharToInt(value[8]) << 4) | stringUtility::hexCharToInt(value[9]));
    }
	else
	{
		result = stringUtility::toInt(value);
	}
    themeValue = result;
}


void theme::parseSignedNumber(char* value, int32_t& themeValue)
{
    trimSpace(value);

	int32_t	result = stringUtility::toInt(value);
    themeValue = result;
}

void theme::upperCase(char* value)
{
    for (uint32_t i = 0; i < (int)strlen(value); i++) 
	{
        value[i] = (char)(value[i] >= 97 && value[i] <= 122 ? value[i] - 32 : value[i]);
    }
}

void theme::trimSpace(char* value)
{
    char* pos = value;
    uint32_t length = strlen(pos);
    while (length > 0 && isspace(pos[length - 1])) {
        pos[length - 1] = 0;
        length--;
    }
    while (length > 0 && *pos && isspace(*pos)) {
        ++pos;
        --length;
    }
    memmove(value, pos, length + 1);
}

void theme::loadConfig(char* buffer, uint32_t bufferSize)
{
    char* lineBuffer = (char*)malloc(1024);
    char* param1 = (char*)lineBuffer + 512;
    char* param2 = (char*)param1 + 256;
    bool skip = false;

    uint32_t lineLength = 0;
    for (uint32_t i = 0; i < bufferSize; i++) 
    {
        char currentChar = buffer[i];
        if (currentChar == '\t') 
        {
            continue;
        }
		else if (currentChar == ';' || currentChar == '#') 
        {
            skip = true;
        }
        else if (currentChar == '\r' || currentChar == '\n') 
        {
            if (lineLength > 0) 
            {
                parseConfigLine(param1, param2, lineBuffer, lineLength);
                lineLength = 0;
            }
            skip = false;
            continue;
        }
        else if (lineLength < 512 && skip == FALSE) 
        {
            lineBuffer[lineLength] = currentChar;
            lineLength++;
        }
    }

    if (lineLength > 0) 
    {
        parseConfigLine(param1, param2, lineBuffer, lineLength);
    }

    free(lineBuffer);
}

pointerVector<char*>* theme::getSkins()
{
	pointerVector<char*>* result = new pointerVector<char*>(false);
	
	pointerVector<fileSystem::FileInfoDetail*>* skins = fileSystem::fileGetFileInfoDetails("HDD0-E:\\PrometheOS\\Skins");
	if (skins != NULL)
	{
		for (uint32_t i = 0; i < skins->count(); i++)
		{
			fileSystem::FileInfoDetail* fileInfoDetail = skins->get(i);
			if (fileInfoDetail->isDirectory)
			{
				char* skinName = fileSystem::getFileName(fileInfoDetail->path);
				result->add(skinName);
			}
		}
	}
	delete(skins);

	return result;
}

void theme::loadRandomSkin()
{
	pointerVector<char*>* skins = getSkins();

	srand((uint32_t)timeUtility::getMillisecondsNow());
	int index = rand() % (skins->count() + 1);

	if (index == 0)
	{
		loadSkin("");
	}
	else
	{
		loadSkin(skins->get(index - 1));
	}

	delete(skins);
}

bool theme::loadImage(const char* filePath, const char* imageKey)
{
	bool result = false;
	uint32_t fileHandle;

	if (fileSystem::fileOpen(filePath, fileSystem::FileModeRead, fileHandle))
	{
		uint32_t fileSize;
		if (fileSystem::fileSize(fileHandle, fileSize))
		{
			char* buffer = (char*)malloc(fileSize);
			uint32_t bytesRead;
			if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize)
			{
				result = drawing::loadImage(buffer, fileSize, imageKey);
			}
			free(buffer);
		}
		fileSystem::fileClose(fileHandle);
	}

	return result;
}

void theme::loadSoundPack(char* soundPackName)
{
	audioPlayer::stop(context::getMusicId());
	
	if (strlen(soundPackName) == 0)
	{
		context::setSoundPackPath("");
		return;
	}
	else if (stringUtility::equals(soundPackName, "*", false) == true)
	{
		loadRandomSoundPack();
		return;
	}

	char* soundPackPath = stringUtility::formatString("HDD0-E:\\PrometheOS\\SoundPacks\\%s", soundPackName);
	
	context::setSoundPackPath(soundPackPath);
	context::setMusicId(audioPlayer::play("background-music.ogg", true));

	free(soundPackPath);
}


pointerVector<char*>* theme::getSoundPacks()
{
	pointerVector<char*>* result = new pointerVector<char*>(false);
	
	pointerVector<fileSystem::FileInfoDetail*>* soundPacks = fileSystem::fileGetFileInfoDetails("HDD0-E:\\PrometheOS\\SoundPacks");
	if (soundPacks != NULL)
	{
		for (uint32_t i = 0; i < soundPacks->count(); i++)
		{
			fileSystem::FileInfoDetail* fileInfoDetail = soundPacks->get(i);
			if (fileInfoDetail->isDirectory)
			{
				char* soundPackName = fileSystem::getFileName(fileInfoDetail->path);
				result->add(soundPackName);
			}
		}
	}
	delete(soundPacks);

	return result;
}

void theme::loadRandomSoundPack()
{
	pointerVector<char*>* soundPacks = getSoundPacks();

	srand((uint32_t)timeUtility::getMillisecondsNow());
	int index = rand() % (soundPacks->count() + 1);

	if (index == 0)
	{
		loadSoundPack("");
	}
	else
	{
		loadSoundPack(soundPacks->get(index - 1));
	}

	delete(soundPacks);
}