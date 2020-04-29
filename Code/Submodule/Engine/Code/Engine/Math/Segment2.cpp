#include "Engine/Math/Segment2.hpp"
#include "Engine/Math/MathUtils.hpp"


Segment2::Segment2( Vec2 start, Vec2 end )
{
	m_start = start;
	m_end = end;
}

Vec2 Segment2::GetCenter() const
{
	float x = (m_start.x + m_end.x) * 0.5f;
	float y = (m_start.y + m_end.y) * 0.5f;

	return Vec2(x, y);
}

Vec2 Segment2::GetDirection() const
{
	return Vec2(0.0f, 0.0f);
}

Vec2 Segment2::GetClosestPoint( Vec2 position ) const
{
	return GetClosestPointOnLineSegment2D(position, m_start, m_end);
}

STATIC Segment2 Segment2::CreateSegment( Vec2 start, Vec2 end )
{
	Segment2 segment;
	segment.m_start = start;
	segment.m_end = end;

	return segment;
}
