#include "theme.h"
#include "fileSystem.h"
#include "stringUtility.h"
#include "drawing.h"
#include "alignment.h"
#include "pointerVector.h"
#include "utils.h"
#include "timeUtility.h"

#include <time.h>

namespace
{
	theme::ThemeData mThemeData;
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

void theme::loadSkin()
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

	mThemeData.JOY_BUTTON_A_COLOR = THEME_JOY_BUTTON_A_COLOR;
	mThemeData.JOY_BUTTON_B_COLOR = THEME_JOY_BUTTON_B_COLOR;
	mThemeData.JOY_BUTTON_X_COLOR = THEME_JOY_BUTTON_X_COLOR;
	mThemeData.JOY_BUTTON_Y_COLOR = THEME_JOY_BUTTON_Y_COLOR;

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
}
