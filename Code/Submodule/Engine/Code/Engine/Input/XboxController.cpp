#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Input/XboxController.hpp"
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility
#include "Engine/Core/ErrorWarningAssert.hpp"

// ------------------------------------------------------------------------------------------------
XboxController::XboxController( int controllerID )
	:m_controllerID( controllerID )
{

}

// ------------------------------------------------------------------------------------------------
XboxController::~XboxController()
{

}

const KeyButton& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );
	if( errorStatus == ERROR_SUCCESS )
	{
		m_isConnected = true;

		// Left/Right Trigger
		UpdateTrigger(m_leftTriggerValue, xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTriggerValue, xboxControllerState.Gamepad.bRightTrigger);

		// Left/Right Analog Stick
		UpdateAnalogStick(m_leftAnalogStick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateAnalogStick(m_rightAnalogStick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);

		// Update Every Button's State
		UpdateButton(XBOX_BUTTON_ID_A, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_ID_B, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_ID_X, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_ID_Y, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_Y);

		UpdateButton(XBOX_BUTTON_ID_BACK, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_ID_START, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_START);

		UpdateButton(XBOX_BUTTON_ID_LSHOULDER, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_RSHOULDER, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);

		UpdateButton(XBOX_BUTTON_ID_LTHUMB, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_RTHUMB, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);

		UpdateButton(XBOX_BUTTON_ID_DPAD_RIGHT, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_UP, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_ID_LEFT, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_ID_DOWN, xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
	}
	else if( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		Reset();
		//DebuggerPrintf( "Xbox controller #%i is not connected. \n", m_controllerID );
	}
	else
	{
		Reset();
	}
	//DebuggerPrintf( "\n" );
}

void XboxController::Reset()
{
	m_isConnected = false;
	ResetTrigger();
	ResetAnalogStick();
	ResetButton();
}

void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	// the trigger value is the actual left/right float variable, just set it
	triggerValue = RangeMap(rawValue, 0.0f, 255.0f, 0.0f, 1.0f);
}

void XboxController::UpdateAnalogStick( AnalogStick& analogStick, short rawX, short rawY )
{
	analogStick.UpdatePosition( rawX, rawY );
}

void XboxController::UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag )
{
	// if this is true, then button is down.
	bool isButtonDown = (buttonFlags & buttonFlag) != 0;
	m_buttonStates[buttonID].UpdateStatus(isButtonDown);	
}

void XboxController::ResetTrigger()
{
	m_leftTriggerValue = 0.0f;
	m_rightTriggerValue = 0.0f;
}

void XboxController::ResetAnalogStick()
{
	m_leftAnalogStick.Reset();
	m_rightAnalogStick.Reset();
}

void XboxController::ResetButton()
{
	for(int i = 0; i < NUM_XBOX_BUTTONS; i++)
	{
		m_buttonStates[i].Reset();
	}		
}

