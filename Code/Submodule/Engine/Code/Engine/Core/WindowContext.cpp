#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"

#include "Game/Framework/App.hpp"

static constexpr const TCHAR* GAME_WINDOW_CLASS_NAME = TEXT("GameWindowClass");
static int registerCount = 0;

WindowContext* g_theWindowContext = nullptr;

//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	WindowContext* windowContext = (WindowContext*) GetWindowLongPtr( (HWND)windowHandle, GWLP_USERDATA );
	if(!windowContext)
	{
		return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
	}

	bool keyHandled = false;

	switch(wmMessageCode)
	{
		case WM_ACTIVATE:
		{
			WORD activeWord = LOWORD(wParam);
			bool isActive = (activeWord != WA_INACTIVE);
			if(isActive)
			{
				windowContext->LockMouse();
			}
			else
			{
				windowContext->UnlockMouse();
			}

			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(windowHandle, &ps);
			EndPaint(windowHandle, &ps);

			return 0;
		}

		// Mouse Clicks;
		case WM_LBUTTONDOWN:
		{
			g_theInputSystem->ClickLeftMouse();

			return 0;
		}

		case WM_LBUTTONUP:
		{
			g_theInputSystem->ReleaseLeftMouse();

			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			g_theInputSystem->ClickRightMouse();

			return 0;
		}

		case WM_RBUTTONUP:
		{
			g_theInputSystem->ReleaseRightMouse();

			return 0;
		}

		case WM_MBUTTONDOWN:
		{
			g_theInputSystem->ClickMiddleMouse();

			return 0;
		}

		case WM_MBUTTONUP:
		{
			g_theInputSystem->ReleaseMiddleMouse();

			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			g_theInputSystem->StoreWheelMovement((float)GET_WHEEL_DELTA_WPARAM(wParam));

			return 0;
		}

		// Virtual Keys;
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char)wParam;
			keyHandled = g_theInputSystem->HandleKeyPressed(asKey);
			break;
		}

		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char)wParam;
			keyHandled = g_theInputSystem->HandleKeyReleased(asKey);
			break;
		}

		// Character Keys;
		case WM_CHAR:
		{
			unsigned char asKey = (unsigned char)wParam;
			keyHandled = g_theInputSystem->HandleChar(asKey);
			break;
		}
	}

	// Check to see if the wmMessageCode was handled/consumed by Character or Virtual Key;
	if(!keyHandled)
	{
		// If the wmMessageCode was not handled/consumed yet, let the Window Context try;
		keyHandled = windowContext->m_gameCallback(windowContext, wmMessageCode, wParam, lParam);
	}

	// Check to see if wmMessageCode was handled/consumed one more time;
	if (keyHandled)
	{
		return 0;
	}
	else
	{
		// Make sure the wmMessageCode is handled by doing nothing with it;
		return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
	}
}

// -----------------------------------------------------------------------
static void LockMouseToWindow( HWND hwnd )
{
	RECT clientRect;
	GetClientRect( hwnd, &clientRect );

	POINT clientOrigin = { 0, 0 };
	ClientToScreen( hwnd, &clientOrigin );

	clientRect.left += clientOrigin.x;
	clientRect.right += clientOrigin.x;
	clientRect.top += clientOrigin.y;
	clientRect.bottom += clientOrigin.y;

	::ClipCursor( &clientRect );
}

static void RegisterGameWindowClass()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = GAME_WINDOW_CLASS_NAME;
	RegisterClassEx( &windowClassDescription );
}


// -----------------------------------------------------------------------
WindowContext::WindowContext()
{
	m_hwnd = nullptr;

	if(registerCount == 0)
	{
		RegisterGameWindowClass();
	}

	registerCount++;
}

// -----------------------------------------------------------------------
WindowContext::~WindowContext()
{
	Close();

	registerCount--;
	if(registerCount == 0)
	{
		UnregisterClass( GAME_WINDOW_CLASS_NAME, GetModuleHandle(NULL) );
	}
}

// -----------------------------------------------------------------------
void WindowContext::Create( const std::string& name, float clientAspect, float maxClientFractionOfDesktop, WindowsProcCallback callback )
{
	ASSERT_OR_DIE(!IsOpen(), "Window is already open." );

	// #ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	//const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleFlags = WS_POPUP;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;
	//const DWORD windowStyleFlags = WS_SYSMENU;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame etc.
	RECT windowRect = clientRect;
	//AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	//WCHAR windowTitle[ 1024 ];
	//MultiByteToWideChar( GetACP(), 0, name.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
// 	HWND hwnd = CreateWindowEx(
// 		windowStyleExFlags,
// 		GAME_WINDOW_CLASS_NAME,
// 		name.c_str(),
// 		windowStyleFlags,
// 		windowRect.left,
// 		windowRect.top,
// 		windowRect.right - windowRect.left,
// 		windowRect.bottom - windowRect.top,
// 		NULL,
// 		NULL,
// 		::GetModuleHandle(NULL),
// 		NULL );
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		GAME_WINDOW_CLASS_NAME,
		name.c_str(),
		windowStyleFlags,
		0,
		0,
		w,
		h,
		NULL,
		NULL,
		::GetModuleHandle(NULL),
		NULL);
	ASSERT_OR_DIE( hwnd != NULL, "Failed to create window." );

	m_hwnd = hwnd;
	m_gameCallback = callback;
	SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)this );

	//SetWindowLong(hwnd, GWL_STYLE, 0);
	ShowWindow( hwnd, SW_SHOW );
	//SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	// Start the game with 0 mouse movement;
	m_lastFrameMousePosition = GetClientMousePosition();
	m_currentFrameMousePosition = GetClientMousePosition();
}

// -----------------------------------------------------------------------
void WindowContext::Close()
{
	if(!IsOpen())
	{
		return;
	}

	CloseWindow( (HWND)m_hwnd );
	m_hwnd = nullptr;
}

// -----------------------------------------------------------------------
void WindowContext::BeginFrame()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}

	// Update mouse position information
	m_lastFrameMousePosition = m_currentFrameMousePosition;
	m_currentFrameMousePosition = GetClientMousePosition();

	if(m_mouseMode == MOUSE_MODE_RELATIVE && HasFocus())
	{
		IntVec2 center = GetClientCenter();
		SetClientMousePosition( center );
		m_lastFrameMousePosition = GetClientMousePosition();
	}
}

// -----------------------------------------------------------------------
IntVec2 WindowContext::GetClientCenter() const
{
	RECT clientRect;
	GetClientRect( (HWND)m_hwnd, &clientRect );

	IntVec2 center;
	center.x = ( clientRect.left + clientRect.right ) / 2;
	center.y = ( clientRect.bottom + clientRect.top ) / 2;

	return center;
}

// -----------------------------------------------------------------------
IntVec2 WindowContext::GetClientMins() const
{
	RECT clientRect;
	GetClientRect( (HWND)m_hwnd, &clientRect );

	IntVec2 mins;
	mins = IntVec2(clientRect.left, clientRect.bottom);

	return mins;
}

// -----------------------------------------------------------------------
IntVec2 WindowContext::GetClientMaxs() const
{
	RECT clientRect;
	GetClientRect( (HWND)m_hwnd, &clientRect );

	IntVec2 maxs;
	maxs = IntVec2(clientRect.right, clientRect.top);

	return maxs;
}

// -----------------------------------------------------------------------
IntVec2 WindowContext::GetClientDimensions() const
{
	RECT clientRect;
	GetClientRect( (HWND)m_hwnd, &clientRect );

	IntVec2 dimensions;
	dimensions = IntVec2(clientRect.right, clientRect.bottom);

	return dimensions;
}

// -----------------------------------------------------------------------
IntVec2 WindowContext::GetClientMousePosition()
{
	POINT desktopPosition;
	if(!::GetCursorPos( &desktopPosition ))
	{
		desktopPosition.x = 0;
		desktopPosition.y = 0;
	}

	::ScreenToClient( (HWND)m_hwnd, &desktopPosition );

	return IntVec2( desktopPosition.x, desktopPosition.y );
}

// -----------------------------------------------------------------------
Vec2 WindowContext::GetClientMinsFloat() const
{
	RECT clientRect;
	GetClientRect((HWND)m_hwnd, &clientRect);

	return Vec2((float)clientRect.left, (float)clientRect.bottom);
}

// -----------------------------------------------------------------------
Vec2 WindowContext::GetClientMaxsFloat() const
{
	RECT clientRect;
	GetClientRect((HWND)m_hwnd, &clientRect);

	return Vec2((float)clientRect.right, (float)clientRect.top);
}

// -----------------------------------------------------------------------
void WindowContext::SetClientMousePosition( IntVec2& clientPosition )
{
	POINT screen = { clientPosition.x, clientPosition.y };
	::ClientToScreen( (HWND)m_hwnd, &screen );

	::SetCursorPos( screen.x, screen.y );
}

// -----------------------------------------------------------------------
IntVec2 WindowContext::GetClientMouseRelativePosition()
{
	return m_currentFrameMousePosition - m_lastFrameMousePosition;
}

// -----------------------------------------------------------------------
void WindowContext::LockMouse()
{
	m_mouseLockCount++;
	if(m_mouseLockCount > 0)
	{
		LockMouseToWindow( (HWND)m_hwnd );
	}

	// 0 out for frame
	m_lastFrameMousePosition = GetClientMousePosition();
	m_currentFrameMousePosition = GetClientMousePosition();
}

// -----------------------------------------------------------------------
void WindowContext::UnlockMouse()
{
	m_mouseLockCount--;
	if(m_mouseLockCount <= 0)
	{
		::ClipCursor(nullptr);
	}
}

// -----------------------------------------------------------------------
void WindowContext::ShowMouse()
{
	::ShowCursor(true);
}

// -----------------------------------------------------------------------
void WindowContext::HideMouse()
{
	::ShowCursor(false);
}

// -----------------------------------------------------------------------
bool WindowContext::HasFocus() const
{
	return (::GetActiveWindow() == (HWND)m_hwnd); 
}

// -----------------------------------------------------------------------
void WindowContext::SetMouseMode( MouseMode mouseMode )
{
	m_mouseMode = mouseMode;

	if(mouseMode == MOUSE_MODE_RELATIVE)
	{
		IntVec2 center = GetClientCenter();
		m_currentFrameMousePosition = center;
		m_lastFrameMousePosition = center;
		SetClientMousePosition(center);
	}
}
