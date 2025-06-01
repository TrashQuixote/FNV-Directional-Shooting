#include "GameOSDepend.h"

OSInputGlobals** g_OSInputGlobals = (OSInputGlobals**)0x011F35CC;

OSGlobals** g_osGlobals = (OSGlobals**)0x011DEA0C;	//in oldWinMain, result of FormHeap_allocate ~Ax.


bool __fastcall GetXIControlPressed(UInt32 ctrlID)
{
	XINPUT_GAMEPAD_EX* curr = XINPUT_GAMEPAD_EX::GetCurrent();
	XINPUT_GAMEPAD_EX* prev = XINPUT_GAMEPAD_EX::GetPrevious();

	switch (ctrlID)
	{
	case kXboxCtrl_DPAD_UP:
		return (curr->wButtons & kXboxMask_DPAD_UP) && !(prev->wButtons & kXboxMask_DPAD_UP);
	case kXboxCtrl_DPAD_DOWN:
		return (curr->wButtons & kXboxMask_DPAD_DOWN) && !(prev->wButtons & kXboxMask_DPAD_DOWN);
	case kXboxCtrl_DPAD_RIGHT:
		return (curr->wButtons & kXboxMask_DPAD_RIGHT) && !(prev->wButtons & kXboxMask_DPAD_RIGHT);
	case kXboxCtrl_DPAD_LEFT:
		return (curr->wButtons & kXboxMask_DPAD_LEFT) && !(prev->wButtons & kXboxMask_DPAD_LEFT);
	case kXboxCtrl_BACK:
		return (curr->wButtons & kXboxMask_BACK) && !(prev->wButtons & kXboxMask_BACK);
	case kXboxCtrl_BUTTON_Y:
		return (curr->wButtons & kXboxMask_BUTTON_Y) && !(prev->wButtons & kXboxMask_BUTTON_Y);
	case kXboxCtrl_BUTTON_A:
		return (curr->wButtons & kXboxMask_BUTTON_A) && !(prev->wButtons & kXboxMask_BUTTON_A);
	case kXboxCtrl_BUTTON_B:
		return (curr->wButtons & kXboxMask_BUTTON_B) && !(prev->wButtons & kXboxMask_BUTTON_B);
	case kXboxCtrl_BUTTON_X:
		return (curr->wButtons & kXboxMask_BUTTON_X) && !(prev->wButtons & kXboxMask_BUTTON_X);
	default:
		return false;
	}
}

bool __fastcall GetXIControlHeld(UInt32 ctrlID)
{
	XINPUT_GAMEPAD_EX* curr = XINPUT_GAMEPAD_EX::GetCurrent();

	switch (ctrlID)
	{
	case kXboxCtrl_DPAD_UP:
		return (curr->wButtons & kXboxMask_DPAD_UP);
	case kXboxCtrl_DPAD_DOWN:
		return (curr->wButtons & kXboxMask_DPAD_DOWN);
	case kXboxCtrl_DPAD_RIGHT:
		return (curr->wButtons & kXboxMask_DPAD_RIGHT);
	case kXboxCtrl_DPAD_LEFT:
		return (curr->wButtons & kXboxMask_DPAD_LEFT);
	case kXboxCtrl_BUTTON_Y:
		return (curr->wButtons & kXboxMask_BUTTON_Y);
	case kXboxCtrl_BUTTON_A:
		return (curr->wButtons & kXboxMask_BUTTON_A);
	case kXboxCtrl_BUTTON_X:
		return (curr->wButtons & kXboxMask_BUTTON_X);
	case kXboxCtrl_BUTTON_LT:
		return curr->bLeftTrigger;
	default:
		return false;
	}
}

void OSInputGlobals::ResetControlState(UInt8 code)
{
	UInt8 keyBind = this->keyBinds[code];
	if (keyBind != kControlCode_NONE)
	{
		this->currKeyStates[keyBind] = 0;
	}

	keyBind = this->mouseBinds[code];
	if (keyBind != kControlCode_NONE)
	{
		this->currButtonStates[keyBind] = 0;
	}
}

void OSInputGlobals::ResetMouseButtons()
{
	for (int i = 0; i < 8; ++i)	this->currButtonStates[i] = 0;
	for (int i = 0; i < 8; ++i)	this->lastButtonStates[i] = 0;
}

ControllerState OSInputGlobals::GetControllerState(XboxControlCode button, bool includeThumbsticks)
{
	return CdeclCall<ControllerState>(0x717A40, button, includeThumbsticks);
};

void OSInputGlobals::Rumble(float left, float right, int durationMS, int pulseDuration, ERumblePattern pattern, bool a7, bool isForce)
{
	ThisStdCall(0xA255B0, this, left, right, durationMS, pulseDuration, pattern, a7, isForce);
}

void OSInputGlobals::ResetRumble()
{
	ThisStdCall(0xA25810, this);
}