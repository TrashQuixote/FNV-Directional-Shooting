#pragma once
#include "Utilities.h"

// keeping this in a separate file so we don't need to include dinput/dsound everywhere

#define DIRECTINPUT_VERSION 0x0800
//#define DIRECTSOUND_VERSION 0x0800
#include <dinput.h>
//#include <dsound.h>

class NiCamera;

enum
{
	kMaxControlBinds = 0x1C,
};

enum XboxControlCode
{
	kXboxCtrl_DPAD_UP = 0x1,
	kXboxCtrl_DPAD_DOWN = 0x2,
	kXboxCtrl_DPAD_LEFT = 0x3,
	kXboxCtrl_DPAD_RIGHT = 0x4,
	kXboxCtrl_START = 0x5,
	kXboxCtrl_BACK = 0x6,
	kXboxCtrl_LS_BUTTON = 0x7,
	kXboxCtrl_RS_BUTTON = 0x8,
	kXboxCtrl_BUTTON_A = 0x9,
	kXboxCtrl_BUTTON_B = 0xA,
	kXboxCtrl_BUTTON_X = 0xB,
	kXboxCtrl_BUTTON_Y = 0xC,
	kXboxCtrl_BUTTON_LT = 0xD,
	kXboxCtrl_BUTTON_RT = 0xE,
	kXboxCtrl_LB = 0xF,
	kXboxCtrl_RB = 0x10,
	kXboxCtrl_L3 = 0x11,
	kXboxCtrl_R3 = 0x12,
};

enum XboxButtonMask
{
	kXboxMask_DPAD_UP = 1,
	kXboxMask_DPAD_DOWN = 2,
	kXboxMask_DPAD_LEFT = 4,
	kXboxMask_DPAD_RIGHT = 8,
	kXboxMask_START = 0x10,
	kXboxMask_BACK = 0x20,
	kXboxMask_LS_BUTTON = 0x40,
	kXboxMask_RS_BUTTON = 0x80,
	kXboxMask_LB = 0x100,
	kXboxMask_RB = 0x200,
	kXboxMask_GUIDE = 0x400,
	kXboxMask_BUTTON_A = 0x1000,
	kXboxMask_BUTTON_B = 0x2000,
	kXboxMask_BUTTON_X = 0x4000,
	kXboxMask_BUTTON_Y = 0x8000,
};

enum InputGlobalsController
{
	kThumbLX = 0x7,
	kThumbLY = 0x8,
	kThumbRX = 0x9,
	kThumbRY = 0xA,
};

enum KeyState
{
	isHeld = 0x0,
	isPressed = 0x1,
	isDepressed = 0x2,
	isChanged = 0x3,
};

enum ControllerState
{
	kControllerState_NotHeld = -1,
	kControllerState_IsHeld = 0,
	kControllerState_IsDepressed = 1,
	kControllerState_IsPressed = 2,
};

enum ControlCode
{
	Forward = 0x0,
	Backward = 0x1,
	Left = 0x2,
	Right = 0x3,
	Attack = 0x4,
	Activate = 0x5,
	Aim = 0x6,
	ReadyItem = 0x7,
	Crouch = 0x8,
	Run = 0x9,
	AlwaysRun = 0xA,
	AutoMove = 0xB,
	Jump = 0xC,
	TogglePOV = 0xD,
	MenuMode = 0xE,
	Rest = 0xF,
	VATS_ = 0x10,
	Hotkey1 = 0x11,
	AmmoSwap = 0x12,
	Hotkey3 = 0x13,
	Hotkey4 = 0x14,
	Hotkey5 = 0x15,
	Hotkey6 = 0x16,
	Hotkey7 = 0x17,
	Hotkey8 = 0x18,
	QuickSave = 0x19,
	QuickLoad = 0x1A,
	Grab = 0x1B,
	Escape_ = 0x1C,
	Console = 0x1D,
	Screenshot = 0x1E,
	kControlCode_NONE = 255,
};

enum Scancodes
{
	_Escape = 0x1,
	One = 0x2,
	Two = 0x3,
	Three = 0x4,
	Four = 0x5,
	Five = 0x6,
	Six = 0x7,
	Seven = 0x8,
	Eight = 0x9,
	Nine = 0xA,
	Zero = 0xB,
	Minus = 0xC,
	Equals = 0xD,
	Backspace = 0xE,
	Tab = 0xF,
	Q = 0x10,
	W = 0x11,
	E = 0x12,
	R = 0x13,
	T = 0x14,
	Y = 0x15,
	U = 0x16,
	I = 0x17,
	O = 0x18,
	P = 0x19,
	LeftBracket = 0x1A,
	RightBracket = 0x1B,
	Enter = 0x1C,
	LeftControl = 0x1D,
	A = 0x1E,
	S = 0x1F,
	D = 0x20,
	F = 0x21,
	G = 0x22,
	H = 0x23,
	J = 0x24,
	K = 0x25,
	L = 0x26,
	SemiColon = 0x27,
	Apostrophe = 0x28,
	Tilde = 0x29,
	LeftShift = 0x2A,
	BackSlash = 0x2B,
	Z = 0x2C,
	X = 0x2D,
	C = 0x2E,
	V = 0x2F,
	B = 0x30,
	N = 0x31,
	M = 0x32,
	Comma = 0x33,
	Period = 0x34,
	ForwardSlash = 0x35,
	RightShift = 0x36,
	NumStar = 0x37,
	LeftAlt = 0x38,
	SpaceBar = 0x39,
	CapsLock = 0x3A,
	F1 = 0x3B,
	F2 = 0x3C,
	F3 = 0x3D,
	F4 = 0x3E,
	F5 = 0x3F,
	F6 = 0x40,
	F7 = 0x41,
	F8 = 0x42,
	F9 = 0x43,
	F10 = 0x44,
	NumLock = 0x45,
	ScrollLock = 0x46,
	Num7 = 0x47,
	Num8 = 0x48,
	Num9 = 0x49,
	NumMinus = 0x4A,
	Num4 = 0x4B,
	Num5 = 0x4C,
	Num6 = 0x4D,
	NumPlus = 0x4E,
	Num1 = 0x4F,
	Num2 = 0x50,
	Num3 = 0x51,
	Num0 = 0x52,
	NumPeriod = 0x53,
	F11 = 0x57,
	F12 = 0x58,
	NumEnter = 0x9C,
	RightControl = 0x9D,
	NumSlash = 0xB5,
	PrintScrn = 0xB7,
	RightAlt = 0xB8,
	Home = 0xC7,
	UpArrow = 0xC8,
	PageUp = 0xC9,
	LeftArrow = 0xCB,
	RightArrow = 0xCD,
	End_ = 0xCF,
	DownArrow = 0xD0,
	PageDown = 0xD1,
	Delete_ = 0xD3,
	LeftMouse = 0x100,
	RightMouse = 0x101,
	MiddleMouse = 0x102,
	MouseButton3 = 0x103,
	MouseButton4 = 0x104,
	MouseButton5 = 0x105,
	MouseButton6 = 0x106,
	MouseButton7 = 0x107,
	MouseWheelUp = 0x108,
	MouseWheelDown = 0x109,
};

enum MouseButtonNames
{
	kLeftClick = 0,
	kRightClick
};

enum ERumblePattern
{
	eRumblePattern_Constant = 0x0,
	eRumblePattern_Square = 0x1,
	eRumblePattern_Triangle = 0x2,
	eRumblePattern_Sawtooth = 0x3,
};

// 1C04
class OSInputGlobals
{
public:
	enum
	{
		kFlag_HasJoysticks = 1 << 0,
		kFlag_HasMouse = 1 << 1,
		kFlag_HasKeyboard = 1 << 2,
		kFlag_BackgroundMouse = 1 << 3,
	};

	// Have not verified nothing has changed here so commenting out (no controllers to test with currently)
#if 0
	enum
	{
		kMaxDevices = 8,
	};

	OSInputGlobals();
	~OSInputGlobals();

	// 244
	class Joystick
	{
	public:
		Joystick();
		~Joystick();

		UInt32	unk000[0x244 >> 2];
	};

	struct JoystickObjectsInfo
	{
		enum
		{
			kHasXAxis = 1 << 0,
			kHasYAxis = 1 << 1,
			kHasZAxis = 1 << 2,
			kHasXRotAxis = 1 << 3,
			kHasYRotAxis = 1 << 4,
			kHasZRotAxis = 1 << 5
		};

		UInt32	axis;
		UInt32	buttons;
	};

	// 2C
	struct Unk1AF4
	{
		UInt32	bufLen;
		UInt8	unk04[0x2C - 4];
	};

	// 28
	struct Unk1B20
	{
		UInt32	unk00;
		UInt32	unk04;
		UInt32	unk08;
		UInt32	unk0C;
		UInt32	unk10;
		UInt32	unk14;
		UInt32	unk18;
		UInt32	unk1C;
		UInt32	unk20;
		UInt32	unk24;
	};
#endif

	enum Device
	{
		kKeyboard = 0x0,
		kMouse = 0x1,
		kJoystick = 0x2,
		kController = 0x3,
	};

	enum
	{
		kNumKeybinds = 0x28
	};

	struct Bind
	{
		UInt8 controller;
		UInt8 mouse;
		UInt8 key;
		UInt8 pad03;
	};
	STATIC_ASSERT(sizeof(Bind) == 0x4);

	struct VibrationStates
	{
		UInt32 motorStrength;
		UInt32 unk04;
		UInt32 durationMS;
		UInt32 unk0C;
		UInt32 unk10;
		UInt32 unk14;
		UInt16 unk18[2];
		UInt32 unk1C;
		UInt16 unk20[2];
	};

	struct ControllerVibration
	{
		UInt32 controllerIndex;
		UInt32 unk04;
		UInt32 unk08;
		UInt32 durationLeft;
		UInt32 unk10;
		UInt32 unk14;
		UInt32 unk18;
		UInt32 unk1C;
		UInt32 durationRight;
		UInt32 unk24;
		UInt32 unk28;
		UInt32 unk2C;
		UInt32 unk30;
		UInt32 unk34;
		UInt32 time38;
		UInt32 unk3C;
	};

	UInt8			isControllerDisabled;	// 0000
	UInt8			byte0001;				// 0001
	UInt8			byte0002;				// 0002
	UInt8			byte0003;				// 0003
	UInt32			flags;					// 0004
	IDirectInput8* directInput;			// 0008
	UInt32			unk000C;				// 000C 0xA230F3
	UInt32			unk0010;				// 0010
	UInt32			unk0014;				// 0014
	UInt32			unk0018;				// 0018
	UInt32			unk001C;				// 001C
	UInt32			unk0020;				// 0020
	UInt32			unk0024;				// 0024
	UInt32			unk0028;				// 0028
	void* unk002C;				// 002C 0xA237BC, 0xA2315F
	void* unk0030;				// 0030 0xA23201, 0xA23C0C

	// joystick
	UInt32			unk0034[320];			// 0034 0xA234D9 0xA23612
	UInt32			unk534[1264];			// 0534 0xA236C0
	UInt32			unk18F4;				// 18F4 0xA24896

	UInt8			currKeyStates[256];		// 18F8
	UInt8			lastKeyStates[256];		// 19F8
	UInt32			unk1AF8[11];			// 1AF8 0xA23A3A
	int				xDelta;					// 1B24
	int				yDelta;					// 1B28
	int				mouseWheelScroll;		// 1B2C
	UInt8			currButtonStates[8];	// 1B30
	int				lastxDelta;				// 1B38
	int				lastyDelta;				// 1B3C
	int				lastMouseWheelScroll;	// 1B40
	UInt8			lastButtonStates[8];	// 1B44
	UInt32			ltrtButtonState;		// 1B4C
	UInt8			mouseSensitivity;		// 1B50 0x711D6A 0xA22821
	UInt8			byte1B51;				// 1B51 
	UInt8			byte1B52;				// 1B52 
	UInt8			byte1B53;				// 1B53 
	UInt32			doubleClickTime;		// 1B54
	UInt8			buttonStates1B58[8];	// 1B58
	UInt32			unk1B60[8];				// 1B60
	ControllerVibration* controllerVibration;// 1B80
	VibrationStates* vibrationStates;				// 1B84
	UInt8			isControllerEnabled;	// 1B88 0xA257D7
	UInt8			byte1B89;				// 1B89
	UInt8			byte1B8A;				// 1B8A
	UInt8			byte1B8B;				// 1B8B
	UInt32			unk1B8C;				// 1B8C
	UInt8			byte1B90;				// 1B90
	UInt8			byte1B91;				// 1B91
	UInt16			overrideFlags;			// 1B92 bit 3 used for adding a debounce to the MenuMode control
	UInt8			keyBinds[28];			// 1B94
	UInt8			mouseBinds[28];			// 1BB0
	UInt8			joystickBinds[28];		// 1BCC
	UInt8			controllerBinds[28];	// 1BE8

	void ResetMouseButtons();
	void ResetPressedControls() { ThisStdCall(0xA253D0, this); };
	void ResetControlState(UInt8 code);

	bool GetControlState(ControlCode code, KeyState state) { return ThisStdCall<bool>(0xA24660, this, code, state); };
	void SetControlHeld(ControlCode code) { ThisStdCall(0xA24280, this, code); };
	bool GetMouseState(MouseButtonNames buttonID, KeyState state) { return ThisStdCall<bool>(0xA23A50, this, buttonID, state); };
	bool GetKeyState(int key, KeyState state) { return ThisStdCall<bool>(0xA24180, this, key, state); };
	signed int GetControllerValue(int a1, int key) { return ThisStdCall<signed int>(0xA23390, this, a1, key); };
	signed int GetControllerValueRaw(int key, int state) { return ThisStdCall<signed int>(0xA23E10, this, key, state); };
	void SetDebounceMenuMode(bool isSet = true)
	{
		if (isSet)
		{
			overrideFlags |= 8;
		}
		else
		{
			overrideFlags &= ~8;
		}
	}
	bool IsDebounceMenuMode()
	{
		return overrideFlags & 8;
	}

	void SetBind(ControlCode code, Bind bind)
	{
		keyBinds[code] = bind.key;
		mouseBinds[code] = bind.mouse;
		controllerBinds[code] = bind.controller;
	}

	void Unbind(ControlCode code)
	{
		Bind unbound;
		unbound.key = 0xFF;
		unbound.mouse = 0xFF;
		unbound.controller = 0xFF;
		unbound.pad03 = 0xFF;
		SetBind(code, unbound);
	}

	static OSInputGlobals* GetSingleton() { return *(OSInputGlobals**)(0x11F35CC); }
	static ControllerState GetControllerState(XboxControlCode button, bool includeThumbsticks = 1);
	void ResetRumble();
	void Rumble(float left, float right, int durationMS, int pulseDuration, ERumblePattern pattern, bool a7, bool isForce);
};
STATIC_ASSERT(sizeof(OSInputGlobals) == 0x1C04);

extern OSInputGlobals** g_OSInputGlobals;

#if 0
#include "GameTypes.h"

class TESGameSound;
class NiAVObject;

// 58
class TESGameSound
{
public:
	TESGameSound();
	~TESGameSound();

	UInt32			unk00[3];	// 00
	UInt32			hashKey;	// 0C
	UInt32			unk10[4];	// 10
	float			x;			// 20
	float			y;			// 24
	float			z;			// 28
	UInt32			unk2C[4];	// 2C
	float			unk3C;		// 3C
	UInt32			unk40[3];	// 40
	const char* name;		// 4C
	UInt32			unk50;		// 50
	UInt32			unk54;		// 54
};

// 328
class OSSoundGlobals
{
public:
	OSSoundGlobals();
	~OSSoundGlobals();

	enum
	{
		kFlags_HasDSound = 1 << 0,
		kFlags_HasHardware3D = 1 << 2,
	};

	typedef NiTPointerMap <TESGameSound>	TESGameSoundMap;
	typedef NiTPointerMap <NiAVObject>		NiAVObjectMap;

	UInt32					unk000;						// 000
	UInt32					unk004;						// 004
	IDirectSound8* dsoundInterface;			// 008
	IDirectSoundBuffer8* primaryBufferInterface;	// 00C
	DSCAPS					soundCaps;					// 010
	UInt32					unk070;						// 070
	UInt32					unk074;						// 074
	IDirectSound3DListener* listenerInterface;		// 078
	UInt32					unk07C[(0x0A4 - 0x07C) >> 2];	// 07C
	UInt8					unk0A4;						// 0A4
	UInt8					unk0A5;						// 0A5
	UInt8					unk0A6;						// 0A6
	UInt8					pad0A7;						// 0A7
	UInt32					unk0A8;						// 0A8
	UInt32					flags;						// 0AC - flags?
	UInt32					unk0B0;						// 0B0
	float					unk0B4;						// 0B4
	float					masterVolume;				// 0B8
	float					footVolume;					// 0BC
	float					voiceVolume;				// 0C0
	float					effectsVolume;				// 0C4
	UInt32					unk0C8;						// 0C8 - time
	UInt32					unk0CC;						// 0CC - time
	UInt32					unk0D0;						// 0D0 - time
	UInt32					unk0D4[(0x0DC - 0x0D4) >> 2];	// 0D4
	UInt32					unk0DC;						// 0DC
	UInt32					unk0E0[(0x2F0 - 0x0E0) >> 2];	// 0E0
	float					musicVolume;				// 2F0
	UInt32					unk2F4;						// 2F4
	float					musicVolume2;				// 2F8
	UInt32					unk2FC;						// 2FC
	TESGameSoundMap* gameSoundMap;				// 300
	NiAVObjectMap* niObjectMap;				// 304
	NiTPointerList <void>* soundMessageMap;			// 308 - AudioManager::SoundMessage *
	UInt32					unk30C[(0x320 - 0x30C) >> 2];	// 30C
	void* soundMessageList;			// 320
	UInt32					unk324;						// 324
};

STATIC_ASSERT(sizeof(OSSoundGlobals) == 0x328);
#endif

class OSSoundGlobals {
};

// A4
class OSGlobals
{
public:
	OSGlobals();
	~OSGlobals();

	UInt8			oneMore;			// 00
	UInt8			quitGame;			// 01	The seven are initialized to 0, this one is set by QQQ
	UInt8			exitToMainMenu;		// 02
	UInt8			isWindowActive;		// 03
	UInt8			unk04;				// 04
	UInt8			unk05;				// 05
	UInt8			isFlyCam;			// 06	This looks promising as TFC status byte
	bool			freezeTime;			// 07
	HWND			window;				// 08
	HINSTANCE		procInstance;		// 0C
	UInt32			mainThreadID;		// 10
	HANDLE			mainThreadHandle;	// 14
	UInt32* unk18;				// 18	ScrapHeapManager::Buffer*
	UInt32			unk1C;				// 1C
	OSInputGlobals* input;				// 20
	OSSoundGlobals* sound;				// 24
	UInt32			unk28;				// 28	relates to unk18
	UInt32 unk2C;
	UInt32 unk30;
	UInt32 unk34;
	UInt32 unk38;
	UInt32 unk3C;
	UInt32 unk40;
	UInt32 unk44;
	UInt32 unk48;
	UInt32 unk4C;
	UInt32 unk50;
	UInt32 unk54;
	UInt32 unk58;
	UInt32 unk5C;
	UInt32 unk60;
	UInt32 unk64;
	UInt32 unk68;
	UInt32 unk6C;
	UInt32 unk70;
	UInt32 unk74;
	UInt32 unk78;
	UInt32 unk7C;
	UInt32 unk80;
	UInt32 unk84;
	UInt32 unk88;
	UInt32 ptr8C;
	UInt32 unk90;
	UInt32 unk94;
	UInt32 unk98;
	UInt8 byte9C;
	UInt8 gap9D[3];
	NiCamera* pFirstPersonCamera;

	static OSGlobals* GetSingleton() { return *(OSGlobals**)0x011DEA0C; };
	static bool IsCollisionDisabled() { return *(UInt8*)0x11C3C0D; };
	static const char* GetConfigDirectory() { return (const char*)0x1202FA0; };
};
STATIC_ASSERT(sizeof(OSGlobals) == 0x0A4);	// found in oldWinMain 0x0086AF4B

extern OSGlobals** g_osGlobals;

// credits to Luthien
struct XINPUT_GAMEPAD_EX
{
	UInt32		eventCount;		// 00
	UInt16		wButtons;		// 04
	UInt8		bLeftTrigger;	// 06
	UInt8		bRightTrigger;	// 07
	SInt16		sThumbLX;		// 08
	SInt16		sThumbLY;		// 0A
	SInt16		sThumbRX;		// 0C
	SInt16		sThumbRY;		// 0E

	static XINPUT_GAMEPAD_EX* GetCurrent() { return (XINPUT_GAMEPAD_EX*)0x11F35A8; };
	static XINPUT_GAMEPAD_EX* GetPrevious() { return (XINPUT_GAMEPAD_EX*)0x11F35B8; };
};

bool __fastcall GetXIControlPressed(UInt32 ctrlID);
bool __fastcall GetXIControlHeld(UInt32 ctrlID);

struct XINPUT_STATE
{
	DWORD dwPacketNumber;
	XINPUT_GAMEPAD_EX Gamepad;
};