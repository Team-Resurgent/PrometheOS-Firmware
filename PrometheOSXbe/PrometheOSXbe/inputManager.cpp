#include "inputManager.h"
#include "audioPlayer.h"
#include "stringUtility.h"
#include "irRemote.h"

namespace {
bool mInitialized = false;
HANDLE mControllerHandles[XGetPortCount()];
HANDLE mRemoteHandles[XGetPortCount()];
CHAR mMemoryUnityHandles[XGetPortCount() * 2];
BYTE mRemoteStatesPrevious[XGetPortCount()];
BYTE mRemoteStatesCurrent[XGetPortCount()];
JoystickButtonStates mControllerStatesPrevious[XGetPortCount()];
JoystickButtonStates mControllerStatesCurrent[XGetPortCount()];
JoystickButtonStates mInjectedStatesPrevious;
JoystickButtonStates mInjectedStatesCurrent;
int mPendingInjectButton = -1;

static ButtonState getButtonState(const JoystickButtonStates* states, JoystickButton button) {
    switch (button) {
        case JoystickButtonA:
            return states->buttonA;
        case JoystickButtonB:
            return states->buttonB;
        case JoystickButtonX:
            return states->buttonX;
        case JoystickButtonY:
            return states->buttonY;
        case JoystickButtonWhite:
            return states->buttonWhite;
        case JoystickButtonBlack:
            return states->buttonBlack;
        case JoystickButtonBack:
            return states->buttonBack;
        case JoystickButtonStart:
            return states->buttonStart;
        case JoystickButtonLeftThumb:
            return states->buttonLeftThumb;
        case JoystickButtonRightThumb:
            return states->buttonRightThumb;
        case JoystickButtonDpadUp:
            return states->buttonDpadUp;
        case JoystickButtonDpadRight:
            return states->buttonDpadRight;
        case JoystickButtonDpadDown:
            return states->buttonDpadDown;
        case JoystickButtonDpadLeft:
            return states->buttonDpadLeft;
        case JoystickButtonLeftTrigger:
            return states->buttonLeftTrigger;
        case JoystickButtonRightTrigger:
            return states->buttonRightTrigger;
        default:
            return ButtonStateNone;
    }
}

static void setButtonState(JoystickButtonStates* states, JoystickButton button, ButtonState state) {
    switch (button) {
        case JoystickButtonA:
            states->buttonA = state;
            break;
        case JoystickButtonB:
            states->buttonB = state;
            break;
        case JoystickButtonX:
            states->buttonX = state;
            break;
        case JoystickButtonY:
            states->buttonY = state;
            break;
        case JoystickButtonWhite:
            states->buttonWhite = state;
            break;
        case JoystickButtonBlack:
            states->buttonBlack = state;
            break;
        case JoystickButtonBack:
            states->buttonBack = state;
            break;
        case JoystickButtonStart:
            states->buttonStart = state;
            break;
        case JoystickButtonLeftThumb:
            states->buttonLeftThumb = state;
            break;
        case JoystickButtonRightThumb:
            states->buttonRightThumb = state;
            break;
        case JoystickButtonDpadUp:
            states->buttonDpadUp = state;
            break;
        case JoystickButtonDpadRight:
            states->buttonDpadRight = state;
            break;
        case JoystickButtonDpadDown:
            states->buttonDpadDown = state;
            break;
        case JoystickButtonDpadLeft:
            states->buttonDpadLeft = state;
            break;
        case JoystickButtonLeftTrigger:
            states->buttonLeftTrigger = state;
            break;
        case JoystickButtonRightTrigger:
            states->buttonRightTrigger = state;
            break;
        default:
            break;
    }
}

static void playButtonSound(JoystickButton button) {
    if (button == JoystickButtonA) {
        audioPlayer::play("button-a.ogg", false);
    } else if (button == JoystickButtonX) {
        audioPlayer::play("button-x.ogg", false);
    } else if (button == JoystickButtonBack) {
        audioPlayer::play("button-back.ogg", false);
    } else if (button == JoystickButtonDpadUp) {
        audioPlayer::play("button-up.ogg", false);
    } else if (button == JoystickButtonDpadDown) {
        audioPlayer::play("button-down.ogg", false);
    } else {
        audioPlayer::play("button-other.ogg", false);
    }
}

static void setPendingInjectButton(JoystickButton button) {
    mPendingInjectButton = (int)button;
}
} // namespace

void inputManager::injectButton(JoystickButton button) {
    setPendingInjectButton(button);
}

void inputManager::processController() {
    if (mInitialized == false) {
        mInitialized = true;
        XInitDevices(0, 0);
        for (int i = 0; i < XGetPortCount(); i++) {
            mControllerHandles[i] = 0;
            memset(&mRemoteStatesPrevious[i], 0, sizeof(BYTE));
            memset(&mRemoteStatesCurrent[i], 0, sizeof(BYTE));
            memset(&mControllerStatesPrevious[i], 0, sizeof(JoystickButtonStates));
            memset(&mControllerStatesCurrent[i], 0, sizeof(JoystickButtonStates));
        }
        memset(&mInjectedStatesPrevious, 0, sizeof(JoystickButtonStates));
        memset(&mInjectedStatesCurrent, 0, sizeof(JoystickButtonStates));
        mPendingInjectButton = -1;
        for (int i = 0; i < XGetPortCount() * 2; i++) {
            mMemoryUnityHandles[i] = 0;
        }
    }

    DWORD insertions = 0;
    DWORD removals = 0;
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals) == TRUE) {
        for (int i = 0; i < XGetPortCount(); i++) {
            if ((insertions & 1) == 1) {
                mControllerHandles[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
            }
            if ((removals & 1) == 1) {
                XInputClose(mControllerHandles[i]);
                mControllerHandles[i] = NULL;
            }
            insertions = insertions >> 1;
            removals = removals >> 1;
        }
    }

    if (XGetDeviceChanges(XDEVICE_TYPE_IR_REMOTE, &insertions, &removals) == TRUE) {
        for (int i = 0; i < XGetPortCount(); i++) {
            if ((insertions & 1) == 1) {
                mRemoteHandles[i] = XInputOpen(XDEVICE_TYPE_IR_REMOTE, i, XDEVICE_NO_SLOT, NULL);
            }
            if ((removals & 1) == 1) {
                XInputClose(mRemoteHandles[i]);
                mRemoteHandles[i] = NULL;
            }
            insertions = insertions >> 1;
            removals = removals >> 1;
        }
    }

    if (XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insertions, &removals) == TRUE) {
        for (uint32_t iPort = 0; iPort < XGetPortCount(); iPort++) {
            for (uint32_t iSlot = 0; iSlot < 2; iSlot++) {
                uint32_t mask = iPort + (iSlot ? 16 : 0);
                mask = 1 << mask;

                uint32_t index = (iPort * 2) + iSlot;
                if ((mask & removals) != 0 && mMemoryUnityHandles[index] != 0) {
                    char* mountPoint = stringUtility::formatString("\\??\\%c:", 'H' + index);
                    STRING sMountPoint = {(USHORT)strlen(mountPoint), (USHORT)strlen(mountPoint) + 1, mountPoint};

                    DEVICE_OBJECT* device = MU_GetExistingDeviceObject(iPort, iSlot);
                    IoDismountVolume(device);

                    IoDeleteSymbolicLink(&sMountPoint);
                    free(mountPoint);
                    MU_CloseDeviceObject(iPort, iSlot);
                    mMemoryUnityHandles[index] = 0;
                }

                if ((mask & insertions) != 0 && mMemoryUnityHandles[index] == 0) {
                    char szDeviceName[64];
                    STRING DeviceName;
                    DeviceName.Length = 0;
                    DeviceName.MaximumLength = sizeof(szDeviceName) / sizeof(CHAR) - 2;
                    DeviceName.Buffer = szDeviceName;
                    if (MU_CreateDeviceObject(iPort, iSlot, &DeviceName) >= 0) {
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

    for (int i = 0; i < XGetPortCount(); i++) {
        memcpy(&mRemoteStatesPrevious[i], &mRemoteStatesCurrent[i], sizeof(BYTE));
        memset(&mRemoteStatesCurrent[i], 0, sizeof(BYTE));

        if (mRemoteHandles[i] == NULL) {
            continue;
        }

        XINPUT_STATEEX inputStates;
        memset(&inputStates, 0, sizeof(XINPUT_STATEEX));
        if (XInputGetState(mRemoteHandles[i], (XINPUT_STATE*)&inputStates) != 0) {
            continue;
        }

        if (inputStates.firstEvent == 0x00) {
            continue;
        }

        mRemoteStatesCurrent[i] = inputStates.wButtons;
    }

    for (int i = 0; i < XGetPortCount(); i++) {
        memcpy(&mControllerStatesPrevious[i], &mControllerStatesCurrent[i], sizeof(JoystickButtonStates));
        memset(&mControllerStatesCurrent[i], 0, sizeof(JoystickButtonStates));

        if (mControllerHandles[i] == NULL) {
            continue;
        }

        XINPUT_STATE inputStates;
        if (XInputGetState(mControllerHandles[i], &inputStates) != 0) {
            continue;
        }

        XINPUT_GAMEPAD gamePad;
        memcpy(&gamePad, &inputStates.Gamepad, sizeof(XINPUT_GAMEPAD));

        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_A] > 32) {
            mControllerStatesCurrent[i].buttonA = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_B] > 32) {
            mControllerStatesCurrent[i].buttonB = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_X] > 32) {
            mControllerStatesCurrent[i].buttonX = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 32) {
            mControllerStatesCurrent[i].buttonY = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 32) {
            mControllerStatesCurrent[i].buttonWhite = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 32) {
            mControllerStatesCurrent[i].buttonBlack = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_BACK) != 0) {
            mControllerStatesCurrent[i].buttonBack = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_START) != 0) {
            mControllerStatesCurrent[i].buttonStart = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0) {
            mControllerStatesCurrent[i].buttonLeftThumb = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0) {
            mControllerStatesCurrent[i].buttonRightThumb = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0) {
            mControllerStatesCurrent[i].buttonDpadUp = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0) {
            mControllerStatesCurrent[i].buttonDpadRight = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0) {
            mControllerStatesCurrent[i].buttonDpadDown = ButtonStatePressed;
        }
        if ((gamePad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0) {
            mControllerStatesCurrent[i].buttonDpadLeft = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 128) {
            mControllerStatesCurrent[i].buttonLeftTrigger = ButtonStatePressed;
        }
        if (gamePad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 128) {
            mControllerStatesCurrent[i].buttonRightTrigger = ButtonStatePressed;
        }
    }

    memcpy(&mInjectedStatesPrevious, &mInjectedStatesCurrent, sizeof(JoystickButtonStates));
    memset(&mInjectedStatesCurrent, 0, sizeof(JoystickButtonStates));

    if (mPendingInjectButton >= JoystickButtonA && mPendingInjectButton <= JoystickButtonRightTrigger) {
        setButtonState(&mInjectedStatesCurrent, (JoystickButton)mPendingInjectButton, ButtonStatePressed);
        mPendingInjectButton = -1;
    }
}

bool inputManager::buttonDown(JoystickButton button, int port) {
    if ((port < 0 || port == 0) && getButtonState(&mInjectedStatesCurrent, button) == ButtonStatePressed) {
        return true;
    }

    for (int i = 0; i < XGetPortCount(); i++) {
        if (port >= 0 && port != i) {
            continue;
        }

        if (mControllerHandles[i] == NULL) {
            continue;
        }

        if (button == JoystickButtonA && mControllerStatesCurrent[i].buttonA == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonB && mControllerStatesCurrent[i].buttonB == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonX && mControllerStatesCurrent[i].buttonX == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonY && mControllerStatesCurrent[i].buttonY == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonWhite && mControllerStatesCurrent[i].buttonWhite == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonBlack && mControllerStatesCurrent[i].buttonBlack == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonBack && mControllerStatesCurrent[i].buttonBack == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonStart && mControllerStatesCurrent[i].buttonStart == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonLeftThumb &&
            mControllerStatesCurrent[i].buttonLeftThumb == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonRightThumb &&
            mControllerStatesCurrent[i].buttonRightThumb == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonDpadUp && mControllerStatesCurrent[i].buttonDpadUp == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonDpadRight &&
            mControllerStatesCurrent[i].buttonDpadRight == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonDpadDown &&
            mControllerStatesCurrent[i].buttonDpadDown == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonDpadLeft &&
            mControllerStatesCurrent[i].buttonDpadLeft == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonLeftTrigger &&
            mControllerStatesCurrent[i].buttonLeftTrigger == ButtonStatePressed) {
            return true;
        } else if (button == JoystickButtonRightTrigger &&
            mControllerStatesCurrent[i].buttonRightTrigger == ButtonStatePressed) {
            return true;
        }
    }
    return false;
}

bool inputManager::buttonPressed(JoystickButton button, int port) {
    if ((port < 0 || port == 0) && getButtonState(&mInjectedStatesCurrent, button) == ButtonStatePressed &&
        getButtonState(&mInjectedStatesPrevious, button) == ButtonStateNone) {
        playButtonSound(button);
        return true;
    }

    // Controller processing

    for (int i = 0; i < XGetPortCount(); i++) {
        if (port >= 0 && port != i) {
            continue;
        }

        if (mControllerHandles[i] == NULL) {
            continue;
        }

        if (button == JoystickButtonA && mControllerStatesCurrent[i].buttonA == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonA == ButtonStateNone) {
            audioPlayer::play("button-a.ogg", false);
            return true;
        } else if (button == JoystickButtonB && mControllerStatesCurrent[i].buttonB == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonB == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonX && mControllerStatesCurrent[i].buttonX == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonX == ButtonStateNone) {
            audioPlayer::play("button-x.ogg", false);
            return true;
        } else if (button == JoystickButtonY && mControllerStatesCurrent[i].buttonY == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonY == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonWhite && mControllerStatesCurrent[i].buttonWhite == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonWhite == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonBlack && mControllerStatesCurrent[i].buttonBlack == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonBlack == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonBack && mControllerStatesCurrent[i].buttonBack == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonBack == ButtonStateNone) {
            audioPlayer::play("button-back.ogg", false);
            return true;
        } else if (button == JoystickButtonStart && mControllerStatesCurrent[i].buttonStart == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonStart == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonLeftThumb &&
            mControllerStatesCurrent[i].buttonLeftThumb == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonLeftThumb == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonRightThumb &&
            mControllerStatesCurrent[i].buttonRightThumb == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonRightThumb == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadUp && mControllerStatesCurrent[i].buttonDpadUp == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonDpadUp == ButtonStateNone) {
            audioPlayer::play("button-up.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadRight &&
            mControllerStatesCurrent[i].buttonDpadRight == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonDpadRight == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadDown &&
            mControllerStatesCurrent[i].buttonDpadDown == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonDpadDown == ButtonStateNone) {
            audioPlayer::play("button-down.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadLeft &&
            mControllerStatesCurrent[i].buttonDpadLeft == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonDpadLeft == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonLeftTrigger &&
            mControllerStatesCurrent[i].buttonLeftTrigger == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonLeftTrigger == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonRightTrigger &&
            mControllerStatesCurrent[i].buttonRightTrigger == ButtonStatePressed &&
            mControllerStatesPrevious[i].buttonRightTrigger == ButtonStateNone) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        }
    }

    // Remote processing

    for (int i = 0; i < XGetPortCount(); i++) {
        if (port >= 0 && port != i) {
            continue;
        }

        if (mRemoteHandles[i] == NULL) {
            continue;
        }

        if (button == JoystickButtonA && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_A1 &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-a.ogg", false);
            return true;
        }
        if (button == JoystickButtonA && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_A2 &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-a.ogg", false);
            return true;
        } else if (button == JoystickButtonB && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_B1 &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonX && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_X1 &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-x.ogg", false);
            return true;
        } else if (button == JoystickButtonX && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_X2 &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-x.ogg", false);
            return true;
        } else if (button == JoystickButtonY && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_Y1 &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonWhite && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_WHITE &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonBlack && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_BLACK &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonLeftThumb && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_LEFT_THUMB &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonRightThumb && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_RIGHT_THUMB &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadUp && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_DPAD_UP &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-up.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadRight && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_DPAD_RIGHT &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadDown && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_DPAD_DOWN &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-down.ogg", false);
            return true;
        } else if (button == JoystickButtonDpadLeft && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_DPAD_LEFT &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonLeftTrigger && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_LEFT_TRIGGER &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        } else if (button == JoystickButtonRightTrigger && mRemoteStatesCurrent[i] == XINPUT_IR_REMOTE_RIGHT_TRIGGER &&
            mRemoteStatesPrevious[i] == 0) {
            audioPlayer::play("button-other.ogg", false);
            return true;
        }
    }
    return false;
}

bool inputManager::isPortConnected(int port) {
    if (port < 0 || port >= XGetPortCount()) {
        return false;
    }
    return mControllerHandles[port] != NULL;
}

int inputManager::getConnectedPort(int index) {
    if (index < 0) {
        return -1;
    }

    int found = 0;
    for (int i = 0; i < XGetPortCount(); i++) {
        if (mControllerHandles[i] != NULL) {
            if (found == index) {
                return i;
            }
            found++;
        }
    }

    return -1;
}

bool inputManager::isMemoryUnitMounted(char letter) {
    for (int i = 0; i < XGetPortCount() * 2; i++) {
        if (mMemoryUnityHandles[i] == letter) {
            return true;
        }
    }
    return false;
}
