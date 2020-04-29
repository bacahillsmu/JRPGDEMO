#include "Engine/Input/KeyButton.hpp"

// ------------------------------------------------------------------------------------------------
void KeyButton::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = isNowPressed;
}

// ------------------------------------------------------------------------------------------------
bool KeyButton::WasJustPressed() const
{
	if(!m_wasPressedLastFrame && m_isPressed)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ------------------------------------------------------------------------------------------------
bool KeyButton::WasJustReleased() const
{
	if(m_wasPressedLastFrame && !m_isPressed)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ------------------------------------------------------------------------------------------------
void KeyButton::Reset()
{
	m_isPressed = false;
	m_wasPressedLastFrame = false;
}
