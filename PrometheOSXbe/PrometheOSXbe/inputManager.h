#pragma once

#include <xtl.h>

typedef enum JoystickButtonState {
	JoystickButtonStateNone,
	JoystickButtonStatePressed
} JoystickButtonState;

typedef enum JoystickButton {
	ButtonA,
	ButtonB,
	ButtonX,
	ButtonY, 
	ButtonWhite,
	ButtonBlack,
	ButtonBack,
	ButtonStart,
	ButtonLeftThumb,
	ButtonRightThumb,
	ButtonDpadUp,
	ButtonDpadRight,
	ButtonDpadDown,
	ButtonDpadLeft,
	ButtonTriggerLeft,
	ButtonTriggerRight
} JoystickButton;

typedef struct JoystickButtonStates
{
	JoystickButtonState buttonA;
	JoystickButtonState buttonB;
	JoystickButtonState buttonX;
	JoystickButtonState buttonY; 
	JoystickButtonState buttonWhite;
	JoystickButtonState buttonBlack;
	JoystickButtonState buttonBack;
	JoystickButtonState buttonStart;
	JoystickButtonState buttonLeftThumb;
	JoystickButtonState buttonRightThumb;
	JoystickButtonState buttonDpadUp;
	JoystickButtonState buttonDpadRight;
	JoystickButtonState buttonDpadDown;
	JoystickButtonState buttonDpadLeft;
	JoystickButtonState buttonTriggerLeft;
	JoystickButtonState buttonTriggerRight;
} JoystickButtonStates;

class inputManager
{
public:
	static void processController();
	static bool buttonDown(JoystickButton button, int port = -1);
	static bool buttonPressed(JoystickButton button, int port = -1);
	static bool isMemoryUnitMounted(char letter);
};