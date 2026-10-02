#pragma once

#include <xtl.h>

typedef enum ButtonState {
    ButtonStateNone,
    ButtonStatePressed
} ButtonState;

typedef enum JoystickButton {
    JoystickButtonA,
    JoystickButtonB,
    JoystickButtonX,
    JoystickButtonY,
    JoystickButtonWhite,
    JoystickButtonBlack,
    JoystickButtonBack,
    JoystickButtonStart,
    JoystickButtonLeftThumb,
    JoystickButtonRightThumb,
    JoystickButtonDpadUp,
    JoystickButtonDpadRight,
    JoystickButtonDpadDown,
    JoystickButtonDpadLeft,
    JoystickButtonLeftTrigger,
    JoystickButtonRightTrigger
} JoystickButton;

typedef struct JoystickButtonStates {
    ButtonState buttonA;
    ButtonState buttonB;
    ButtonState buttonX;
    ButtonState buttonY;
    ButtonState buttonWhite;
    ButtonState buttonBlack;
    ButtonState buttonBack;
    ButtonState buttonStart;
    ButtonState buttonLeftThumb;
    ButtonState buttonRightThumb;
    ButtonState buttonDpadUp;
    ButtonState buttonDpadRight;
    ButtonState buttonDpadDown;
    ButtonState buttonDpadLeft;
    ButtonState buttonLeftTrigger;
    ButtonState buttonRightTrigger;
} JoystickButtonStates;

class inputManager {
  public:
    static void processController();
    static void injectButton(JoystickButton button);
    static bool buttonDown(JoystickButton button, int port = -1);
    static bool buttonPressed(JoystickButton button, int port = -1);
    static bool isPortConnected(int port);
    static int getConnectedPort(int index);
    static bool isMemoryUnitMounted(char letter);
};
