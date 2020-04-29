#include "Engine/Math/Line.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Line::Line( const Vec2& initialLineStart, const Vec2& initialLineEnd, float initialLineThickness /*= 0.0f*/ )
	: lineStart(initialLineStart)
	, lineEnd(initialLineEnd)
	, lineThickness(initialLineThickness)
{
	m_surfaceNormal = lineStart - lineEnd;
	m_surfaceNormal.Normalize();
	m_surfaceNormal.Rotate90Degrees();
}

void Line::SetFromText( const char* text )
{
	text = "";
}