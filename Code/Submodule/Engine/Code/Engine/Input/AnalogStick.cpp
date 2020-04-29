#include "Engine/Input/AnalogStick.hpp"

// ------------------------------------------------------------------------------------------------
AnalogStick::AnalogStick( float innerDeadZoneFraction, float outerDeadZoneFraction )
	:m_innerDeadZoneFraction( innerDeadZoneFraction )
	,m_outerDeadZoneFraction( outerDeadZoneFraction )
{

}

// ------------------------------------------------------------------------------------------------
void AnalogStick::Reset()
{
	m_rawPosition = Vec2( 0.0f, 0.0f );
	m_correctedPosition = Vec2( 0.0f, 0.0f );
	m_correctedDegrees = 0.0f;
	m_correctedMagnitude = 0.0f;
}

void AnalogStick::UpdatePosition( float rawX, float rawY )
{
	rawX = RangeMap(rawX, -32767.0f, 32767.0f, -1.0f, 1.0f);
	rawY = RangeMap(rawY, -32767.0f, 32767.0f, -1.0f, 1.0f);

	m_rawPosition = Vec2(rawX, rawY);
	m_correctedPosition = Vec2(rawX, rawY);
	float originalRawLength = m_rawPosition.GetLength();

	float clampedRangeMappedRawLength = Clamp(originalRawLength, m_innerDeadZoneFraction, m_outerDeadZoneFraction);
	m_correctedMagnitude = RangeMap(clampedRangeMappedRawLength, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.0f, 1.0f); // We accounted for Dead Zones, now convert to 0 - 1

	m_correctedPosition.SetLength(m_correctedMagnitude);               // This new magnitude
	m_correctedDegrees = m_correctedPosition.GetAngleDegrees();
}
