#include "inputManager.h"

namespace
{
	bool mInitialized = false;
	HANDLE mControllerHandles[XGetPortCount()];
	JoystickButtonStates mControllerStatesPrevious;
	JoystickButtonStates mControllerStatesCurrent;
}

void inputManager::processController()
{
	if (mInitialized == false)
	{
		mInitialized = true;
		XInitDevices(0, 0);
		for (int i = 0; i < XGetPortCount(); i++)
		{
			mControllerHandles[i] = 0;
		}

		memset(&mControllerStatesPrevious, 0, sizeof(mControllerStatesPrevious));
		memset(&mControllerStatesCurrent, 0, sizeof(mControllerStatesCurrent));
	}

    DWORD insertions;
	DWORD removals;
    XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals);
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if ((insertions & 1) == 1)
		{
			mControllerHandles[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
		}
		if ((removals & 1) == 1)
		{
			XInputClose(mControllerHandles[i]);
			mControllerHandles[i] = NULL;
		}
		insertions = insertions >> 1;
		removals = removals >> 1;
	}

	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insertions, &removals))
    {
	}

	memcpy(&mControllerStatesPrevious, &mControllerStatesCurrent, sizeof(JoystickButtonStates));
	memset(&mControllerStatesCurrent, 0, sizeof(mControllerStatesCurrent));

	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (mControllerHandles[i] == NULL)
		{
			continue;
		}

		XINPUT_STATE inputStates;
		if (XInputGetState(mControllerHandles[i], &inputStates) != 0) 
		{
			continue;
		}

		XINPUT_GAMEPAD gamePad;
		memcpy(&gamePad, &inputStates.Gamepad, sizeof(XINPUT_GAMEPAD));

		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_A] > 32)
		{
			mControllerStatesCurrent.buttonA = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_B] > 32)
		{
			mControllerStatesCurrent.buttonB = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_X] > 32)
		{
			mControllerStatesCurrent.buttonX = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 32)
		{
			mControllerStatesCurrent.buttonY = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 32)
		{
			mControllerStatesCurrent.buttonWhite = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 32)
		{
			mControllerStatesCurrent.buttonBlack = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
		{
			mControllerStatesCurrent.buttonBack = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_START) != 0)
		{
			mControllerStatesCurrent.buttonStart = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
		{
			mControllerStatesCurrent.buttonLeftThumb = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
		{
			mControllerStatesCurrent.buttonRightThumb = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
		{
			mControllerStatesCurrent.buttonDpadUp = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
		{
			mControllerStatesCurrent.buttonDpadRight = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
		{
			mControllerStatesCurrent.buttonDpadDown = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
		{
			mControllerStatesCurrent.buttonDpadLeft = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 128)
		{
			mControllerStatesCurrent.buttonTriggerLeft = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 128)
		{
			mControllerStatesCurrent.buttonTriggerRight = JoystickButtonStatePressed;
		}
	}
}

bool inputManager::buttonPressed(JoystickButton button)
{
	if (button == ButtonA && mControllerStatesCurrent.buttonA == JoystickButtonStatePressed && mControllerStatesPrevious.buttonA == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonB && mControllerStatesCurrent.buttonB == JoystickButtonStatePressed && mControllerStatesPrevious.buttonB == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonX && mControllerStatesCurrent.buttonX == JoystickButtonStatePressed && mControllerStatesPrevious.buttonX == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonY && mControllerStatesCurrent.buttonY == JoystickButtonStatePressed && mControllerStatesPrevious.buttonY == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonWhite && mControllerStatesCurrent.buttonWhite == JoystickButtonStatePressed && mControllerStatesPrevious.buttonWhite == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonBlack && mControllerStatesCurrent.buttonBlack == JoystickButtonStatePressed && mControllerStatesPrevious.buttonBlack == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonBack && mControllerStatesCurrent.buttonBack == JoystickButtonStatePressed && mControllerStatesPrevious.buttonBack == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonStart && mControllerStatesCurrent.buttonStart == JoystickButtonStatePressed && mControllerStatesPrevious.buttonStart == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonLeftThumb && mControllerStatesCurrent.buttonLeftThumb == JoystickButtonStatePressed && mControllerStatesPrevious.buttonLeftThumb == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonRightThumb && mControllerStatesCurrent.buttonRightThumb == JoystickButtonStatePressed && mControllerStatesPrevious.buttonRightThumb == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonDpadUp && mControllerStatesCurrent.buttonDpadUp == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadUp == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonDpadRight && mControllerStatesCurrent.buttonDpadRight == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadRight == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonDpadDown && mControllerStatesCurrent.buttonDpadDown == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadDown == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonDpadLeft && mControllerStatesCurrent.buttonDpadLeft == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadLeft == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonTriggerLeft && mControllerStatesCurrent.buttonTriggerLeft == JoystickButtonStatePressed && mControllerStatesPrevious.buttonTriggerLeft == JoystickButtonStateNone)
	{
		return true;
	}
	else if (button == ButtonTriggerRight && mControllerStatesCurrent.buttonTriggerRight == JoystickButtonStatePressed && mControllerStatesPrevious.buttonTriggerRight == JoystickButtonStateNone)
	{
		return true;
	}
	return false;
}