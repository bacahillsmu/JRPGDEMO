#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places


#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/DevConsole.hpp"

#include "Engine/Input/InputSystem.hpp"


InputSystem* g_theInputSystem = nullptr;

// -----------------------------------------------------------------------
InputSystem::InputSystem()
{
}

// -----------------------------------------------------------------------
InputSystem::~InputSystem()
{
}

// -----------------------------------------------------------------------
void InputSystem::Startup()
{

}

// -----------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	for(int controllerID = 0; controllerID < MAX_XBOX_CONTROLLERS; controllerID++)
	{
		m_controllers[controllerID].Update();
	}

	// (0, 0) being in the top left, try to convert it so (0, 0) is bottom right, easier to understand for me;
	// This is ugly, cleanup...
	IntVec2 mousePosition = g_theWindowContext->GetClientMousePosition();
	IntVec2 clientMins = g_theWindowContext->GetClientMins();
	IntVec2 clientMaxs = g_theWindowContext->GetClientMaxs();

	float mousepositionx = (float)mousePosition.x;
	float mousepositiony = (float)mousePosition.y;

	Vec2 clientsmins = Vec2((float)clientMins.x, (float)clientMaxs.y);
	Vec2 clientsmaxs = Vec2((float)clientMaxs.x, (float)clientMins.y);

	mousepositiony = abs(mousepositiony - clientsmaxs.y);

	m_mousePosition = Vec2(mousepositionx, mousepositiony);
}

// -----------------------------------------------------------------------
void InputSystem::EndFrame()
{
	// Left Mouse Click;
	if (m_leftMouseClicked)
	{
		m_leftMouseClickedLastFrame = true;
	}
	else
	{
		m_leftMouseClickedLastFrame = false;
	}

	// Right Mouse Click;
	if (m_rightMouseClicked)
	{
		m_rightMouseClickedLastFrame = true;
	}
	else
	{
		m_rightMouseClickedLastFrame = false;
	}

	// Middle Mouse Click;
	if (m_middleMouseClicked)
	{
		m_middleMouseClickedLastFrame = true;
	}
	else
	{
		m_middleMouseClickedLastFrame = false;
	}
}

// -----------------------------------------------------------------------
void InputSystem::Shutdown()
{

}

// -----------------------------------------------------------------------
// Xbox Controller;
// -----------------------------------------------------------------------
const XboxController& InputSystem::GetXboxController( int controllerID )
{
	return m_controllers[ controllerID ];
}

// -----------------------------------------------------------------------
// Mouse Clicks;
// -----------------------------------------------------------------------
Vec2 InputSystem::GetMousePosition()
{
	return m_mousePosition;
}

// -----------------------------------------------------------------------
Vec2 InputSystem::GetGameMousePosition(Vec2 gameMapDimensions_)
{
	Vec2 mousePosition = g_theInputSystem->GetMousePosition();
	IntVec2 clientMins = g_theWindowContext->GetClientMins();
	IntVec2 clientMaxs = g_theWindowContext->GetClientMaxs();
	Vec2 clientsmins = Vec2((float)clientMins.x, (float)clientMaxs.y);
	Vec2 clientsmaxs = Vec2((float)clientMaxs.x, (float)clientMins.y);

	float xClick = mousePosition.x;
	float yClick = mousePosition.y;
	float xRange = RangeMap(xClick, clientsmins.x, clientsmaxs.x, 0.0f, gameMapDimensions_.x);
	float yRange = RangeMap(yClick, clientsmins.y, clientsmaxs.y, 0.0f, gameMapDimensions_.y);

	return Vec2(xRange, yRange);
}

// -----------------------------------------------------------------------
void InputSystem::ClickLeftMouse()
{
	m_leftMouseClicked = true;
	m_leftClickMousePosition = GetMousePosition();
}

// -----------------------------------------------------------------------
void InputSystem::ClickRightMouse()
{
	m_rightMouseClicked = true;
	m_rightClickMousePosition = GetMousePosition();
}

// -----------------------------------------------------------------------
void InputSystem::ClickMiddleMouse()
{
	m_middleMouseClicked = true;
	m_middleClickMousePosition = GetMousePosition();
}

// -----------------------------------------------------------------------
void InputSystem::ReleaseLeftMouse()
{
	m_leftMouseClicked = false;
	m_leftClickMousePosition = Vec2(0.0f, 0.0f);
}

// -----------------------------------------------------------------------
void InputSystem::ReleaseRightMouse()
{
	m_rightMouseClicked = false;
	m_rightClickMousePosition = Vec2(0.0f, 0.0f);
}

// -----------------------------------------------------------------------
void InputSystem::ReleaseMiddleMouse()
{
	m_middleMouseClicked = false;
	m_middleClickMousePosition = Vec2(0.0f, 0.0f);
}

// -----------------------------------------------------------------------
bool InputSystem::IsLeftMouseClicked()
{
	return m_leftMouseClicked;
}

// -----------------------------------------------------------------------
bool InputSystem::WasLeftMouseClickReleased()
{
	if (!m_leftMouseClicked && m_leftMouseClickedLastFrame)
	{
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------
bool InputSystem::IsRightMouseClicked()
{
	return m_rightMouseClicked;
}

// -----------------------------------------------------------------------
bool InputSystem::WasRightMouseClickReleased()
{
	if (!m_rightMouseClicked && m_rightMouseClickedLastFrame)
	{
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------
bool InputSystem::IsMiddleMouseClicked()
{
	return m_middleMouseClicked;
}

// -----------------------------------------------------------------------
bool InputSystem::WasMiddleMouseClickReleased()
{
	if (!m_middleMouseClicked && m_middleMouseClickedLastFrame)
	{
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------
// Mouse Wheel
// -----------------------------------------------------------------------
void InputSystem::StoreWheelMovement(float wheelDelta)
{
	m_wheelDelta = wheelDelta;
}

// -----------------------------------------------------------------------
int InputSystem::GetMouseWheelDirection()
{
	int direction = 0;

	if (m_wheelDelta != 0.0f)
	{
		direction = m_wheelDelta > 0.0f ? 1 : -1;
	}

	m_wheelDelta = 0.0f;
	return direction;
}

// -----------------------------------------------------------------------
// Character Keys;
// -----------------------------------------------------------------------
bool InputSystem::HandleChar(unsigned char asKey)
{
	// Let the Dev Console try to use the Character Key if it is Open;
	if (g_theDevConsole->IsOpen())
	{
		if(asKey != VirtualKey::ENTER
		&& asKey != VirtualKey::ESC
		&& asKey != VirtualKey::BACKSPACE)
		{
			std::string letter(1, asKey);
			g_theDevConsole->AddLetterToCurrentTypingText(Rgba::WHITE, letter);
			g_theDevConsole->UpdateCursorPosition(1);
			g_theDevConsole->ResetValues();

			return true;
		}
	}

	return false;
}

// -----------------------------------------------------------------------
// Virtual Keys;
// -----------------------------------------------------------------------
bool InputSystem::HandleKeyPressed(unsigned char asKey)
{	
	// Handling the Virtual Key press...
	bool virtualKeyHandled = false;

	// ...that opens the Dev Console if it is not Open;
	if (!g_theDevConsole->IsOpen())
	{
		if (asKey == VirtualKey::TILDE)
		{
			g_theDevConsole->OpenAtEndOfFrame();

			virtualKeyHandled = true;
		}
		else if(asKey == VirtualKey::F1)
		{
			m_F1 = true;

			virtualKeyHandled = true;
		}
		else if (asKey == VirtualKey::F2)
		{
			m_F2 = true;

			virtualKeyHandled = true;
		}
		else if (asKey == VirtualKey::F3)
		{
			m_F3 = true;

			virtualKeyHandled = true;
		}
		else if (asKey == VirtualKey::SPACEBAR)
		{
			m_spaceBar = true;

			virtualKeyHandled = true;
		}
	}

	// ...if the Dev Console is Open;
	else if (g_theDevConsole->IsOpen())
	{
		virtualKeyHandled = g_theDevConsole->HandleKeyPressed(asKey);
	}

	return virtualKeyHandled;
}

// -----------------------------------------------------------------------
bool InputSystem::HandleKeyReleased(unsigned char asKey)
{
	bool virtualKeyHandled = false;

	if (asKey == VirtualKey::F1)
	{
		m_F1 = false;

		virtualKeyHandled = true;
	}
	else if(asKey == VirtualKey::F2)
	{
		m_F2 = false;

		virtualKeyHandled = true;
	}
	else if (asKey == VirtualKey::F3)
	{
		m_F3 = false;

		virtualKeyHandled = true;
	}
	else if (asKey == VirtualKey::SPACEBAR)
	{
		m_spaceBar = false;

		virtualKeyHandled = true;
	}

	return virtualKeyHandled;
}

// -----------------------------------------------------------------------
// Special Static Virtual Keys;
// -----------------------------------------------------------------------
STATIC bool InputSystem::IsCtrlDown()
{
	return GetKeyState(VK_CONTROL) & 0XF0;
}

// -----------------------------------------------------------------------
STATIC bool InputSystem::IsShiftDown()
{
	return GetKeyState(VK_SHIFT) & 0XF0;
}

// -----------------------------------------------------------------------
STATIC bool InputSystem::IsAltDown()
{
	return GetKeyState(VK_MENU) & 0XF0;
}

// -----------------------------------------------------------------------
bool InputSystem::IsF1Down()
{
	return m_F1 == true;
}

// -----------------------------------------------------------------------
bool InputSystem::IsF2Down()
{
	return m_F2 == true;
}

// -----------------------------------------------------------------------
bool InputSystem::IsF3Down()
{
	return m_F3 == true;
}

bool InputSystem::IsSpaceDown()
{
	return m_spaceBar == true;
}
