#include "Engine/Math/Vec2.hpp"


class AnalogStick
{

	friend class XboxController;


public:
	explicit AnalogStick( float innerDeadZoneFraction, float outerDeadZoneFraction );

	Vec2 GetPosition() const { return m_correctedPosition; }
	float GetMagnitude() const { return m_correctedMagnitude; }
	float GetAngleDegrees() const { return m_correctedDegrees; }
	Vec2 GetRawPosition() const { return m_rawPosition; }
	float GetInnerDeadZoneFraction() const { return m_innerDeadZoneFraction; }
	float GetOuterDeadZoneFraction() const { return m_outerDeadZoneFraction; }
	void Reset();


private:
	void UpdatePosition( float rawX, float rawY );


private:
	const float m_innerDeadZoneFraction; // Because construction explicit, don't default
	const float m_outerDeadZoneFraction; // Because construction explicit, don't default
	Vec2 m_rawPosition = Vec2( 0.0f, 0.0f );
	Vec2 m_correctedPosition = Vec2( 0.0f, 0.0f );
	float m_correctedDegrees = 0.0f;
	float m_correctedMagnitude = 0.0f;

};