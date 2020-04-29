#pragma once
#include "Engine/Input/XboxController.hpp"

constexpr int MAX_XBOX_CONTROLLERS = 4;

namespace VirtualKey
{
	constexpr unsigned char ESC = 0x1B;
	constexpr unsigned char TILDE = 0xC0;
	constexpr unsigned char ENTER = 0x0D;
	constexpr unsigned char SPACEBAR = 0x20;
	constexpr unsigned char BACKSPACE = 0x08;
	constexpr unsigned char DEL = 0x2E;
	constexpr unsigned char SHIFT = 0x10;

	constexpr unsigned char UPARROW = 0x26;
	constexpr unsigned char LEFTARROW = 0x25;
	constexpr unsigned char DOWNARROW = 0x28;
	constexpr unsigned char RIGHTARROW = 0x27;

	constexpr unsigned char F1 = 0x70;
	constexpr unsigned char F2 = 0x71;
	constexpr unsigned char F3 = 0x72;
	constexpr unsigned char F4 = 0x73;
	constexpr unsigned char F5 = 0x74;
	constexpr unsigned char F6 = 0x75;
	constexpr unsigned char F7 = 0x76;
	constexpr unsigned char F8 = 0x77;
	constexpr unsigned char F9 = 0x78;
	constexpr unsigned char F10 = 0x79;
	constexpr unsigned char F11 = 0x7A;
	constexpr unsigned char F12 = 0x7B;

	// OEM
	constexpr unsigned char PERIOD = 0xBE;

	// Other
	constexpr unsigned char LEFTBRACKET = 219;
	constexpr unsigned char RIGHTBRACKET = 221;
};

class InputSystem
{

public:

	InputSystem();
	~InputSystem();

	// Flow;
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	// Xbox Controller;
	const XboxController& GetXboxController( int controllerID );

	// Mouse Clicks;
	Vec2 GetMousePosition();
	Vec2 GetGameMousePosition(Vec2 gameMapDimensions_);
	void ClickLeftMouse();
	void ClickRightMouse();
	void ClickMiddleMouse();
	void ReleaseLeftMouse();
	void ReleaseRightMouse();
	void ReleaseMiddleMouse();
	bool IsLeftMouseClicked();
	bool WasLeftMouseClickReleased();
	bool IsRightMouseClicked();
	bool WasRightMouseClickReleased();
	bool IsMiddleMouseClicked();
	bool WasMiddleMouseClickReleased();

	// Mouse Wheel Scrolling;
	void StoreWheelMovement(float wheelDelta);
	int GetMouseWheelDirection();

	// Character Keys;
	bool HandleChar(unsigned char asKey);

	// Virtual Keys;
	bool HandleKeyPressed(unsigned char asKey);
	bool HandleKeyReleased(unsigned char asKey);

	// Special Static Virtual Keys;
	static bool IsCtrlDown();
	static bool IsShiftDown();
	static bool IsAltDown();

	bool IsF1Down();
	bool IsF2Down();
	bool IsF3Down();

	bool IsSpaceDown();

private:

	// Xbox Controller;
	XboxController m_controllers[MAX_XBOX_CONTROLLERS] = 
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3),
	};

	// Mouse Input;
	Vec2 m_mousePosition = Vec2(0.0f, 0.0f);

	Vec2 m_leftClickMousePosition = Vec2(0.0f, 0.0f);
	Vec2 m_rightClickMousePosition = Vec2(0.0f, 0.0f);
	Vec2 m_middleClickMousePosition = Vec2(0.0f, 0.0f);

	bool m_leftMouseClicked = false;
	bool m_leftMouseClickedLastFrame = false;
	bool m_rightMouseClicked = false;
	bool m_rightMouseClickedLastFrame = false;
	bool m_middleMouseClicked = false;
	bool m_middleMouseClickedLastFrame = false;

	float m_wheelDelta = 0.0f;

	bool m_F1 = false;
	bool m_F2 = false;
	bool m_F3 = false;

	bool m_spaceBar = false;
};

extern InputSystem* g_theInputSystem;
