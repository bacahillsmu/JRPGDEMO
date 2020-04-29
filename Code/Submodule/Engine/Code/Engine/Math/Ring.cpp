#include "Engine/Math/Ring.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"


Ring::Ring( Vec2& initialRingCenter, float initialRingRadius, float initialRingThickness )
	: ringCenter(initialRingCenter)
	, ringRadius(initialRingRadius)
	, ringThickness(initialRingThickness)
{
}

void Ring::SetFromText( const char* text )
{
	text = "";
}