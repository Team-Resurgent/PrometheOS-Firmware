#include "inputManager.h"
#include "audioPlayer.h"
#include "stringUtility.h"

namespace
{
	bool mInitialized = false;
	HANDLE mControllerHandles[XGetPortCount()];
	CHAR mMemoryUnityHandles[XGetPortCount() * 2];
	JoystickButtonStates mControllerStatesPrevious[XGetPortCount()];
	JoystickButtonStates mControllerStatesCurrent[XGetPortCount()];
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
			memset(&mControllerStatesPrevious[i], 0, sizeof(JoystickButtonStates));
			memset(&mControllerStatesCurrent[i], 0, sizeof(JoystickButtonStates));
		}
		for (int i = 0; i < XGetPortCount() * 2; i++)
		{
			mMemoryUnityHandles[i] = 0;
		}
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

	for (int i = 0; i < XGetPortCount(); i++)
	{

		memcpy(&mControllerStatesPrevious[i], &mControllerStatesCurrent[i], sizeof(JoystickButtonStates));
		memset(&mControllerStatesCurrent[i], 0, sizeof(JoystickButtonStates));

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
			mControllerStatesCurrent[i].buttonA = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_B] > 32)
		{
			mControllerStatesCurrent[i].buttonB = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_X] > 32)
		{
			mControllerStatesCurrent[i].buttonX = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 32)
		{
			mControllerStatesCurrent[i].buttonY = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 32)
		{
			mControllerStatesCurrent[i].buttonWhite = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 32)
		{
			mControllerStatesCurrent[i].buttonBlack = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
		{
			mControllerStatesCurrent[i].buttonBack = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_START) != 0)
		{
			mControllerStatesCurrent[i].buttonStart = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
		{
			mControllerStatesCurrent[i].buttonLeftThumb = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
		{
			mControllerStatesCurrent[i].buttonRightThumb = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
		{
			mControllerStatesCurrent[i].buttonDpadUp = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
		{
			mControllerStatesCurrent[i].buttonDpadRight = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
		{
			mControllerStatesCurrent[i].buttonDpadDown = JoystickButtonStatePressed;
		}
		if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
		{
			mControllerStatesCurrent[i].buttonDpadLeft = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 128)
		{
			mControllerStatesCurrent[i].buttonTriggerLeft = JoystickButtonStatePressed;
		}
		if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 128)
		{
			mControllerStatesCurrent[i].buttonTriggerRight = JoystickButtonStatePressed;
		}
	}
}

bool inputManager::buttonDown(JoystickButton button, int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i)
		{
			continue;
		}

		if (mControllerHandles[i] == NULL)
		{
			continue;
		}

		if (button == ButtonA && mControllerStatesCurrent[i].buttonA == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonB && mControllerStatesCurrent[i].buttonB == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonX && mControllerStatesCurrent[i].buttonX == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonY && mControllerStatesCurrent[i].buttonY == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonWhite && mControllerStatesCurrent[i].buttonWhite == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonBlack && mControllerStatesCurrent[i].buttonBlack == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonBack && mControllerStatesCurrent[i].buttonBack == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonStart && mControllerStatesCurrent[i].buttonStart == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonLeftThumb && mControllerStatesCurrent[i].buttonLeftThumb == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonRightThumb && mControllerStatesCurrent[i].buttonRightThumb == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonDpadUp && mControllerStatesCurrent[i].buttonDpadUp == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonDpadRight && mControllerStatesCurrent[i].buttonDpadRight == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonDpadDown && mControllerStatesCurrent[i].buttonDpadDown == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonDpadLeft && mControllerStatesCurrent[i].buttonDpadLeft == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonTriggerLeft && mControllerStatesCurrent[i].buttonTriggerLeft == JoystickButtonStatePressed)
		{
			return true;
		}
		else if (button == ButtonTriggerRight && mControllerStatesCurrent[i].buttonTriggerRight == JoystickButtonStatePressed)
		{
			return true;
		}
	}
	return false;
}

bool inputManager::buttonPressed(JoystickButton button, int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i)
		{
			continue;
		}

		if (mControllerHandles[i] == NULL)
		{
			continue;
		}

		if (button == ButtonA && mControllerStatesCurrent[i].buttonA == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonA == JoystickButtonStateNone)
		{
			audioPlayer::play("button-a.ogg", false);
			return true;
		}
		else if (button == ButtonB && mControllerStatesCurrent[i].buttonB == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonB == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonX && mControllerStatesCurrent[i].buttonX == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonX == JoystickButtonStateNone)
		{
			audioPlayer::play("button-x.ogg", false);
			return true;
		}
		else if (button == ButtonY && mControllerStatesCurrent[i].buttonY == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonY == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonWhite && mControllerStatesCurrent[i].buttonWhite == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonWhite == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonBlack && mControllerStatesCurrent[i].buttonBlack == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonBlack == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonBack && mControllerStatesCurrent[i].buttonBack == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonBack == JoystickButtonStateNone)
		{
			audioPlayer::play("button-back.ogg", false);
			return true;
		}
		else if (button == ButtonStart && mControllerStatesCurrent[i].buttonStart == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonStart == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonLeftThumb && mControllerStatesCurrent[i].buttonLeftThumb == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonLeftThumb == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonRightThumb && mControllerStatesCurrent[i].buttonRightThumb == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonRightThumb == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonDpadUp && mControllerStatesCurrent[i].buttonDpadUp == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonDpadUp == JoystickButtonStateNone)
		{
			audioPlayer::play("button-up.ogg", false);
			return true;
		}
		else if (button == ButtonDpadRight && mControllerStatesCurrent[i].buttonDpadRight == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonDpadRight == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonDpadDown && mControllerStatesCurrent[i].buttonDpadDown == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonDpadDown == JoystickButtonStateNone)
		{
			audioPlayer::play("button-down.ogg", false);
			return true;
		}
		else if (button == ButtonDpadLeft && mControllerStatesCurrent[i].buttonDpadLeft == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonDpadLeft == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonTriggerLeft && mControllerStatesCurrent[i].buttonTriggerLeft == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonTriggerLeft == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
		else if (button == ButtonTriggerRight && mControllerStatesCurrent[i].buttonTriggerRight == JoystickButtonStatePressed && mControllerStatesPrevious[i].buttonTriggerRight == JoystickButtonStateNone)
		{
			audioPlayer::play("button-other.ogg", false);
			return true;
		}
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