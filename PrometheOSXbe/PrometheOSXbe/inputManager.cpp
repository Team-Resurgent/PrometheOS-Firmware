#include "inputManager.h"
#include "audioPlayer.h"
#include "stringUtility.h"

namespace
{
	bool mInitialized = false;
	HANDLE mControllerHandles[XGetPortCount()];
	CHAR mMemoryUnityHandles[XGetPortCount() * 2];
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
		for (int i = 0; i < XGetPortCount() * 2; i++)
		{
			mMemoryUnityHandles[i] = 0;
		}
		memset(&mControllerStatesPrevious, 0, sizeof(mControllerStatesPrevious));
		memset(&mControllerStatesCurrent, 0, sizeof(mControllerStatesCurrent));
	}

    DWORD insertions = 0;
	DWORD removals = 0;
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals) == TRUE)
	{
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
	}

	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insertions, &removals) == TRUE)
    {
		for (uint32_t iPort = 0; iPort < XGetPortCount(); iPort++)
		{
			for (uint32_t iSlot = 0; iSlot < 2; iSlot++ )
			{
				uint32_t mask = iPort + (iSlot ? 16 : 0);
				mask = 1 << mask;

				uint32_t index = (iPort * 2) + iSlot;
				if ((mask & removals) != 0 && mMemoryUnityHandles[index] != 0)
                {
					char* mountPoint = stringUtility::formatString("\\??\\%c:", 'H' + index);
					STRING sMountPoint = {(USHORT)strlen(mountPoint), (USHORT)strlen(mountPoint) + 1, mountPoint};

					DEVICE_OBJECT* device = MU_GetExistingDeviceObject(iPort, iSlot);
					IoDismountVolume(device);

					IoDeleteSymbolicLink(&sMountPoint);
					free(mountPoint);
					MU_CloseDeviceObject(iPort, iSlot);
                    mMemoryUnityHandles[index] = 0;
                }

				if ((mask & insertions) != 0 && mMemoryUnityHandles[index] == 0)
                {
					char szDeviceName[64];
					STRING DeviceName;
					DeviceName.Length = 0;
					DeviceName.MaximumLength = sizeof(szDeviceName) / sizeof(CHAR) - 2;
					DeviceName.Buffer = szDeviceName;
					if (MU_CreateDeviceObject(iPort, iSlot, &DeviceName) >= 0)
					{
						char* mountPoint = stringUtility::formatString("\\??\\%c:", 'H' + index);
						STRING sMountPoint = {(USHORT)strlen(mountPoint), (USHORT)strlen(mountPoint) + 1, mountPoint};
						IoCreateSymbolicLink(&sMountPoint, &DeviceName);
						free(mountPoint);
						mMemoryUnityHandles[index] = (char)('H' + index);
					}
                }
			}
		}
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
		audioPlayer::play("button-a.ogg", false);
		return true;
	}
	else if (button == ButtonB && mControllerStatesCurrent.buttonB == JoystickButtonStatePressed && mControllerStatesPrevious.buttonB == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonX && mControllerStatesCurrent.buttonX == JoystickButtonStatePressed && mControllerStatesPrevious.buttonX == JoystickButtonStateNone)
	{
		audioPlayer::play("button-x.ogg", false);
		return true;
	}
	else if (button == ButtonY && mControllerStatesCurrent.buttonY == JoystickButtonStatePressed && mControllerStatesPrevious.buttonY == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonWhite && mControllerStatesCurrent.buttonWhite == JoystickButtonStatePressed && mControllerStatesPrevious.buttonWhite == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonBlack && mControllerStatesCurrent.buttonBlack == JoystickButtonStatePressed && mControllerStatesPrevious.buttonBlack == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonBack && mControllerStatesCurrent.buttonBack == JoystickButtonStatePressed && mControllerStatesPrevious.buttonBack == JoystickButtonStateNone)
	{
		audioPlayer::play("button-back.ogg", false);
		return true;
	}
	else if (button == ButtonStart && mControllerStatesCurrent.buttonStart == JoystickButtonStatePressed && mControllerStatesPrevious.buttonStart == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonLeftThumb && mControllerStatesCurrent.buttonLeftThumb == JoystickButtonStatePressed && mControllerStatesPrevious.buttonLeftThumb == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonRightThumb && mControllerStatesCurrent.buttonRightThumb == JoystickButtonStatePressed && mControllerStatesPrevious.buttonRightThumb == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonDpadUp && mControllerStatesCurrent.buttonDpadUp == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadUp == JoystickButtonStateNone)
	{
		audioPlayer::play("button-up.ogg", false);
		return true;
	}
	else if (button == ButtonDpadRight && mControllerStatesCurrent.buttonDpadRight == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadRight == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonDpadDown && mControllerStatesCurrent.buttonDpadDown == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadDown == JoystickButtonStateNone)
	{
		audioPlayer::play("button-down.ogg", false);
		return true;
	}
	else if (button == ButtonDpadLeft && mControllerStatesCurrent.buttonDpadLeft == JoystickButtonStatePressed && mControllerStatesPrevious.buttonDpadLeft == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonTriggerLeft && mControllerStatesCurrent.buttonTriggerLeft == JoystickButtonStatePressed && mControllerStatesPrevious.buttonTriggerLeft == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	else if (button == ButtonTriggerRight && mControllerStatesCurrent.buttonTriggerRight == JoystickButtonStatePressed && mControllerStatesPrevious.buttonTriggerRight == JoystickButtonStateNone)
	{
		audioPlayer::play("button-other.ogg", false);
		return true;
	}
	return false;
}

bool inputManager::isMemoryUnitMounted(char letter)
{
	for (int i = 0; i < XGetPortCount() * 2; i++)
	{
		if (mMemoryUnityHandles[i] == letter)
		{
			return true;
		}
	}
	return false;
}