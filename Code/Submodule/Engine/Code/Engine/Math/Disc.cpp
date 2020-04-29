#include "Engine/Math/Disc.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"


Disc::Disc( const Vec2& initialDiscCenter, float initialDiscRadius )
	: discCenter(initialDiscCenter)
	, discRadius(initialDiscRadius)
{
}

void Disc::SetFromText( const char* text )
{
	text = "";
}