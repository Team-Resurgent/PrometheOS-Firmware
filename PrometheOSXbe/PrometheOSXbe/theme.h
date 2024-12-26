#pragma once

#include "xboxInternals.h"
#include "alignment.h"
#include "pointerVector.h"

#define THEME_BACKGROUND_OVERLAY_AS_UNDERLAY 0
#define THEME_BACKGROUND_FRAME_DELAY 0
#define THEME_BACKGROUND_FRAME_PINGPONG 0
#define THEME_BACKGROUND_COLOR 0xff11191f
#define THEME_BACKGROUND_IMAGE_TINT 0xffffffff
#define THEME_BACKGROUND_OVERLAY_IMAGE_TINT 0xffffffff
#define THEME_PROMETHEOS_ALIGN 0
#define THEME_PROMETHEOS_Y 32
#define THEME_PROMETHEOS_COLOR 0xffffcd00
#define THEME_INSTALLER_TINT 0xffffffff
#define THEME_TEXT_COLOR 0xffffffff
#define THEME_TEXT_DISABLED_COLOR 0xff404040
#define THEME_HEADER_TEXT_COLOR 0xffffffff
#define THEME_FOOTER_TEXT_COLOR 0xffffffff

#define THEME_HEADER_ALIGN 0
#define THEME_HEADER_Y 32
#define THEME_CENTER_OFFSET 0
#define THEME_FOOTER_Y 420

#define THEME_SNAKE_WALL_COLOR 0xff808080
#define THEME_SNAKE_FOOD_COLOR 0xffff0000
#define THEME_SNAKE_HEAD_COLOR 0xff00ff00
#define THEME_SNAKE_TAIL_COLOR 0xff008000

#define THEME_INVADERS_WALL_COLOR 0xff808080
#define THEME_INVADERS_PLAYER_COLOR 0xff00ff00
#define THEME_INVADERS_BOSS_COLOR 0xffff0000
#define THEME_INVADERS_ALIEN_COLOR 0xffffd060

#define THEME_JOY_BUTTON_A_COLOR 0xff00ff00
#define THEME_JOY_BUTTON_B_COLOR 0xffff0000
#define THEME_JOY_BUTTON_X_COLOR 0xff0000ff
#define THEME_JOY_BUTTON_Y_COLOR 0xffffff00
#define THEME_JOY_BUTTON_BLACK_COLOR 0xff666666
#define THEME_JOY_BUTTON_WHITE_COLOR 0xffffffff

#define THEME_BUTTON_ACTIVE_FILL_COLOR 0xff175782
#define THEME_BUTTON_ACTIVE_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_ACTIVE_TEXT_COLOR 0xffffffff

#define THEME_BUTTON_ACTIVE_HOVER_FILL_COLOR 0xff19b3e6
#define THEME_BUTTON_ACTIVE_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_ACTIVE_HOVER_TEXT_COLOR 0xffffffff

#define THEME_BUTTON_INACTIVE_FILL_COLOR 0xff175782
#define THEME_BUTTON_INACTIVE_STROKE_COLOR 0xff000000
#define THEME_BUTTON_INACTIVE_TEXT_COLOR 0xffffffff

#define THEME_BUTTON_INACTIVE_HOVER_FILL_COLOR 0xff19b3e6
#define THEME_BUTTON_INACTIVE_HOVER_STROKE_COLOR 0xff000000
#define THEME_BUTTON_INACTIVE_HOVER_TEXT_COLOR 0xffffffff

#define THEME_PANEL_FILL_COLOR 0xff141e26
#define THEME_PANEL_STROKE_COLOR 0xff141e26

#define THEME_TEXT_PANEL_FILL_COLOR 0xff11191f
#define THEME_TEXT_PANEL_STROKE_COLOR 0xff374956
#define THEME_TEXT_PANEL_TEXT_COLOR 0xffffffff
#define THEME_TEXT_PANEL_HOVER_FILL_COLOR 0xff11191f
#define THEME_TEXT_PANEL_HOVER_STROKE_COLOR 0xffffffff
#define THEME_TEXT_PANEL_HOVER_TEXT_COLOR 0xffffffff

#define THEME_BUTTON_LED_OFF_FILL_COLOR 0xff404040
#define THEME_BUTTON_LED_OFF_STROKE_COLOR 0xff404040
#define THEME_BUTTON_LED_OFF_TEXT_COLOR 0xffffffff
#define THEME_BUTTON_LED_OFF_HOVER_FILL_COLOR 0xff202020
#define THEME_BUTTON_LED_OFF_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_OFF_HOVER_TEXT_COLOR 0xffffffff

#define THEME_BUTTON_LED_RED_FILL_COLOR 0xffd40c00
#define THEME_BUTTON_LED_RED_STROKE_COLOR 0xffd40c00
#define THEME_BUTTON_LED_RED_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_RED_HOVER_FILL_COLOR 0xffaa0a00
#define THEME_BUTTON_LED_RED_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_RED_HOVER_TEXT_COLOR 0xff000000

#define THEME_BUTTON_LED_GREEN_FILL_COLOR 0xff32c12c
#define THEME_BUTTON_LED_GREEN_STROKE_COLOR 0xff32c12c
#define THEME_BUTTON_LED_GREEN_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_GREEN_HOVER_FILL_COLOR 0xff289a23
#define THEME_BUTTON_LED_GREEN_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_GREEN_HOVER_TEXT_COLOR 0xff000000

#define THEME_BUTTON_LED_YELLOW_FILL_COLOR 0xffffcd00
#define THEME_BUTTON_LED_YELLOW_STROKE_COLOR 0xffffcd00
#define THEME_BUTTON_LED_YELLOW_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_YELLOW_HOVER_FILL_COLOR 0xffcca400
#define THEME_BUTTON_LED_YELLOW_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_YELLOW_HOVER_TEXT_COLOR 0xff000000

#define THEME_BUTTON_LED_BLUE_FILL_COLOR 0xff526eff
#define THEME_BUTTON_LED_BLUE_STROKE_COLOR 0xff526eff
#define THEME_BUTTON_LED_BLUE_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_BLUE_HOVER_FILL_COLOR 0xff4358cc
#define THEME_BUTTON_LED_BLUE_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_BLUE_HOVER_TEXT_COLOR 0xff000000

#define THEME_BUTTON_LED_PURPLE_FILL_COLOR 0xff7f4fc9
#define THEME_BUTTON_LED_PURPLE_STROKE_COLOR 0xff7f4fc9
#define THEME_BUTTON_LED_PURPLE_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_PURPLE_HOVER_FILL_COLOR 0xff663fa1
#define THEME_BUTTON_LED_PURPLE_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_PURPLE_HOVER_TEXT_COLOR 0xff000000
	
#define THEME_BUTTON_LED_TURQUOISE_FILL_COLOR 0xff00bcd9
#define THEME_BUTTON_LED_TURQUOISE_STROKE_COLOR 0xff00bcd9
#define THEME_BUTTON_LED_TURQUOISE_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR 0xff0096ae
#define THEME_BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR 0xff000000

#define THEME_BUTTON_LED_WHITE_FILL_COLOR 0xfff0f0f0
#define THEME_BUTTON_LED_WHITE_STROKE_COLOR 0xfff0f0f0
#define THEME_BUTTON_LED_WHITE_TEXT_COLOR 0xff000000
#define THEME_BUTTON_LED_WHITE_HOVER_FILL_COLOR 0xffc0c0c0
#define THEME_BUTTON_LED_WHITE_HOVER_STROKE_COLOR 0xffffffff
#define THEME_BUTTON_LED_WHITE_HOVER_TEXT_COLOR 0xff000000

class theme
{
public:

	typedef struct ThemeData {

		char     SKIN_AUTHOR[50];

		uint32_t BACKGROUND_OVERLAY_AS_UNDERLAY;
		uint32_t BACKGROUND_FRAME_DELAY;
		uint32_t BACKGROUND_FRAME_PINGPONG;
		uint32_t BACKGROUND_COLOR;
		uint32_t BACKGROUND_IMAGE_TINT;
		uint32_t BACKGROUND_OVERLAY_IMAGE_TINT;
		uint32_t PROMETHEOS_ALIGN;
		uint32_t PROMETHEOS_Y;
		uint32_t PROMETHEOS_COLOR;
		uint32_t INSTALLER_TINT;
		uint32_t TEXT_COLOR;
		uint32_t TEXT_DISABLED_COLOR;
		uint32_t HEADER_TEXT_COLOR;
		uint32_t FOOTER_TEXT_COLOR;

		uint32_t HEADER_ALIGN;
		uint32_t HEADER_Y;
		int32_t  CENTER_OFFSET;
		uint32_t FOOTER_Y;

		uint32_t SNAKE_WALL_COLOR;
		uint32_t SNAKE_FOOD_COLOR;
		uint32_t SNAKE_HEAD_COLOR;
		uint32_t SNAKE_TAIL_COLOR;

		uint32_t INVADERS_WALL_COLOR;
		uint32_t INVADERS_PLAYER_COLOR;
		uint32_t INVADERS_BOSS_COLOR;
		uint32_t INVADERS_ALIEN_COLOR;

		uint32_t JOY_BUTTON_A_COLOR;
		uint32_t JOY_BUTTON_B_COLOR;
		uint32_t JOY_BUTTON_X_COLOR;
		uint32_t JOY_BUTTON_Y_COLOR;
		uint32_t JOY_BUTTON_BLACK_COLOR;
		uint32_t JOY_BUTTON_WHITE_COLOR;

		uint32_t BUTTON_ACTIVE_FILL_COLOR;
		uint32_t BUTTON_ACTIVE_STROKE_COLOR;
		uint32_t BUTTON_ACTIVE_TEXT_COLOR;

		uint32_t BUTTON_ACTIVE_HOVER_FILL_COLOR;
		uint32_t BUTTON_ACTIVE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_ACTIVE_HOVER_TEXT_COLOR;

		uint32_t BUTTON_INACTIVE_FILL_COLOR;
		uint32_t BUTTON_INACTIVE_STROKE_COLOR;
		uint32_t BUTTON_INACTIVE_TEXT_COLOR;

		uint32_t BUTTON_INACTIVE_HOVER_FILL_COLOR;
		uint32_t BUTTON_INACTIVE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_INACTIVE_HOVER_TEXT_COLOR;

		uint32_t PANEL_FILL_COLOR;
		uint32_t PANEL_STROKE_COLOR;

		uint32_t TEXT_PANEL_FILL_COLOR;
		uint32_t TEXT_PANEL_STROKE_COLOR;
		uint32_t TEXT_PANEL_TEXT_COLOR;
		uint32_t TEXT_PANEL_HOVER_FILL_COLOR;
		uint32_t TEXT_PANEL_HOVER_STROKE_COLOR;
		uint32_t TEXT_PANEL_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_OFF_FILL_COLOR;
		uint32_t BUTTON_LED_OFF_STROKE_COLOR;
		uint32_t BUTTON_LED_OFF_TEXT_COLOR;
		uint32_t BUTTON_LED_OFF_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_OFF_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_OFF_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_RED_FILL_COLOR;
		uint32_t BUTTON_LED_RED_STROKE_COLOR;
		uint32_t BUTTON_LED_RED_TEXT_COLOR;
		uint32_t BUTTON_LED_RED_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_RED_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_RED_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_GREEN_FILL_COLOR;
		uint32_t BUTTON_LED_GREEN_STROKE_COLOR;
		uint32_t BUTTON_LED_GREEN_TEXT_COLOR;
		uint32_t BUTTON_LED_GREEN_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_GREEN_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_GREEN_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_YELLOW_FILL_COLOR;
		uint32_t BUTTON_LED_YELLOW_STROKE_COLOR;
		uint32_t BUTTON_LED_YELLOW_TEXT_COLOR;
		uint32_t BUTTON_LED_YELLOW_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_YELLOW_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_YELLOW_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_BLUE_FILL_COLOR;
		uint32_t BUTTON_LED_BLUE_STROKE_COLOR;
		uint32_t BUTTON_LED_BLUE_TEXT_COLOR;
		uint32_t BUTTON_LED_BLUE_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_BLUE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_BLUE_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_PURPLE_FILL_COLOR;
		uint32_t BUTTON_LED_PURPLE_STROKE_COLOR;
		uint32_t BUTTON_LED_PURPLE_TEXT_COLOR;
		uint32_t BUTTON_LED_PURPLE_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_PURPLE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_PURPLE_HOVER_TEXT_COLOR;
		
		uint32_t BUTTON_LED_TURQUOISE_FILL_COLOR;
		uint32_t BUTTON_LED_TURQUOISE_STROKE_COLOR;
		uint32_t BUTTON_LED_TURQUOISE_TEXT_COLOR;
		uint32_t BUTTON_LED_TURQUOISE_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_TURQUOISE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_TURQUOISE_HOVER_TEXT_COLOR;

		uint32_t BUTTON_LED_WHITE_FILL_COLOR;
		uint32_t BUTTON_LED_WHITE_STROKE_COLOR;
		uint32_t BUTTON_LED_WHITE_TEXT_COLOR;
		uint32_t BUTTON_LED_WHITE_HOVER_FILL_COLOR;
		uint32_t BUTTON_LED_WHITE_HOVER_STROKE_COLOR;
		uint32_t BUTTON_LED_WHITE_HOVER_TEXT_COLOR;

	} ThemeData;

	static uint32_t getBackgroundFrameCount();

	static char* getSkinAuthor();
	static bool getBackgroundOverlayAsUnderlay();
	static uint32_t getBackgroundFrameDelay();
	static bool getBackgroundFramePingPong();
	static uint32_t getBackgroundColor();
	static uint32_t getBackgroundImageTint();
	static uint32_t getBackgroundOverlayImageTint();
	static uint32_t getPrometheosColor();
	static horizAlignment getPrometheosAlign();
	static uint32_t getPrometheosY();
	static uint32_t getInstallerTint();
	static uint32_t getTextColor();
	static uint32_t getTextDisabledColor();
	static uint32_t getHeaderTextColor();
	static uint32_t getFooterTextColor();

	static horizAlignment getHeaderAlign();
	static uint32_t getHeaderY();
	static int32_t getCenterOffset();
	static uint32_t getFooterY();

	static uint32_t getSnakeWallColor();
	static uint32_t getSnakeFoodColor();
	static uint32_t getSnakeHeadColor();
	static uint32_t getSnakeTailColor();

	static uint32_t getInvadersWallColor();
	static uint32_t getInvadersPlayerColor();
	static uint32_t getInvadersBossColor();
	static uint32_t getInvadersAlienColor();

	static uint32_t getJoyButtonAColor();
	static uint32_t getJoyButtonBColor();
	static uint32_t getJoyButtonXColor();
	static uint32_t getJoyButtonYColor();
	static uint32_t getJoyButtonBlackColor();
	static uint32_t getJoyButtonWhiteColor();

	static uint32_t getButtonToggleFillColor();
	static uint32_t getButtonToggleStrokeColor();

	static uint32_t getButtonActiveFillColor();
	static uint32_t getButtonActiveStrokeColor();
	static uint32_t getButtonActiveTextColor();
	static uint32_t getButtonActiveHoverFillColor();
	static uint32_t getButtonActiveHoverStrokeColor();
	static uint32_t getButtonActiveHoverTextColor();

	static uint32_t getButtonInactiveFillColor();
	static uint32_t getButtonInactiveStrokeColor();
	static uint32_t getButtonInactiveTextColor();
	static uint32_t getButtonInactiveHoverFillColor();
	static uint32_t getButtonInactiveHoverStrokeColor();
	static uint32_t getButtonInactiveHoverTextColor();

	static uint32_t getPanelFillColor();
	static uint32_t getPanelStrokeColor();

	static uint32_t getTextPanelFillColor();
	static uint32_t getTextPanelStrokeColor();
	static uint32_t getTextPanelTextColor();
	static uint32_t getTextPanelHoverFillColor();
	static uint32_t getTextPanelHoverStrokeColor();
	static uint32_t getTextPanelHoverTextColor();

	static uint32_t getButtonLedOffFillColor();
	static uint32_t getButtonLedOffStrokeColor();
	static uint32_t getButtonLedOffTextColor();
	static uint32_t getButtonLedOffHoverFillColor();
	static uint32_t getButtonLedOffHoverStrokeColor();
	static uint32_t getButtonLedOffHoverTextColor();

	static uint32_t getButtonLedRedFillColor();
	static uint32_t getButtonLedRedStrokeColor();
	static uint32_t getButtonLedRedTextColor();
	static uint32_t getButtonLedRedHoverFillColor();
	static uint32_t getButtonLedRedHoverStrokeColor();
	static uint32_t getButtonLedRedHoverTextColor();

	static uint32_t getButtonLedGreenFillColor();
	static uint32_t getButtonLedGreenStrokeColor();
	static uint32_t getButtonLedGreenTextColor();
	static uint32_t getButtonLedGreenHoverFillColor();
	static uint32_t getButtonLedGreenHoverStrokeColor();
	static uint32_t getButtonLedGreenHoverTextColor();

	static uint32_t getButtonLedYellowFillColor();
	static uint32_t getButtonLedYellowStrokeColor();
	static uint32_t getButtonLedYellowTextColor();
	static uint32_t getButtonLedYellowHoverFillColor();
	static uint32_t getButtonLedYellowHoverStrokeColor();
	static uint32_t getButtonLedYellowHoverTextColor();

	static uint32_t getButtonLedBlueFillColor();
	static uint32_t getButtonLedBlueStrokeColor();
	static uint32_t getButtonLedBlueTextColor();
	static uint32_t getButtonLedBlueHoverFillColor();
	static uint32_t getButtonLedBlueHoverStrokeColor();
	static uint32_t getButtonLedBlueHoverTextColor();

	static uint32_t getButtonLedPurpleFillColor();
	static uint32_t getButtonLedPurpleStrokeColor();
	static uint32_t getButtonLedPurpleTextColor();
	static uint32_t getButtonLedPurpleHoverFillColor();
	static uint32_t getButtonLedPurpleHoverStrokeColor();
	static uint32_t getButtonLedPurpleHoverTextColor();

	static uint32_t getButtonLedTurquoiseFillColor();
	static uint32_t getButtonLedTurquoiseStrokeColor();
	static uint32_t getButtonLedTurquoiseTextColor();
	static uint32_t getButtonLedTurquoiseHoverFillColor();
	static uint32_t getButtonLedTurquoiseHoverStrokeColor();
	static uint32_t getButtonLedTurquoiseHoverTextColor();

	static uint32_t getButtonLedWhiteFillColor();
	static uint32_t getButtonLedWhiteStrokeColor();
	static uint32_t getButtonLedWhiteTextColor();
	static uint32_t getButtonLedWhiteHoverFillColor();
	static uint32_t getButtonLedWhiteHoverStrokeColor();
	static uint32_t getButtonLedWhiteHoverTextColor();

	static void loadSkin(char* skinName);
	static pointerVector<char*>* getSkins();
	static void loadRandomSkin();

	static void loadSoundPack(char* soundPackName);
	static pointerVector<char*>* getSoundPacks();
	static void loadRandomSoundPack();

private:

	static void parseConfigLine(char* param1, char* param2, char* buffer, unsigned long bufferSize);
	static void parseUnsignedNumber(char* value, uint32_t& themeValue);
	static void parseSignedNumber(char* value, int32_t& themeValue);
	static void upperCase(char* value);
	static void trimSpace(char* value);
	static void loadConfig(char* buffer, uint32_t bufferSize);
	static bool loadImage(const char* filePath, const char* imageKey);
};