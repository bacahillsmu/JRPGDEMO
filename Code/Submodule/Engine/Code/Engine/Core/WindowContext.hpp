#pragma once
#include "Engine/Math/IntVec2.hpp"

class WindowContext;

typedef bool (*WindowsProcCallback)( WindowContext*, unsigned int msg, uintptr_t wparam, uintptr_t lparam );

enum MouseMode
{
	MOUSE_MODE_ABSOLUTE,
	MOUSE_MODE_RELATIVE
};

class WindowContext
{

public:

	// Constructor/Deconstructor
	WindowContext();
	~WindowContext();

	// Game Flow
	void Create( const std::string& name, float clientAspect, float maxClientFractionOfDesktop, WindowsProcCallback callback );
	void BeginFrame();
	void Close();

	// Client Calls
	void SetClientMousePosition(IntVec2& clientPosition);
	IntVec2 GetClientMouseRelativePosition();
	IntVec2 GetClientCenter() const;
	IntVec2 GetClientMins() const;
	IntVec2 GetClientMaxs() const;
	IntVec2 GetClientDimensions() const;
	IntVec2 GetClientMousePosition();
	Vec2 GetClientMinsFloat() const;
	Vec2 GetClientMaxsFloat() const;
	
	// Mouse
	void SetMouseMode(MouseMode mouseMode);
	void ShowMouse();
	void HideMouse();
	void LockMouse();
	void UnlockMouse();	
	inline bool IsMouseLocked() const { return m_mouseLockCount > 0; }

	inline bool IsOpen() const { return m_hwnd != nullptr; }
	bool HasFocus() const; 


public:

	int m_mouseLockCount				= 0;	

	IntVec2 m_lastFrameMousePosition	= IntVec2(0, 0);
	IntVec2 m_currentFrameMousePosition = IntVec2(0, 0);
	
	MouseMode m_mouseMode				= MOUSE_MODE_ABSOLUTE;

	void* m_hwnd						= nullptr;

	WindowsProcCallback m_gameCallback;
};

extern WindowContext* g_theWindowContext;

