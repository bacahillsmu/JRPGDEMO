#include "Engine/Math/Capsule2.hpp"



Capsule2::Capsule2()
{

}

Capsule2::Capsule2( Vec2 position )
{
	m_start = position;
	m_end = position;
	m_radius = 0.0f;
}

Capsule2::Capsule2( Vec2 center, float radius )
{
	m_start = center;
	m_end = center;
	m_radius = radius;
}

Capsule2::Capsule2( Vec2 point0, Vec2 point1, float radius )
{
	m_start = point0;
	m_end = point1;
	m_radius = radius;
}

void Capsule2::SetPosition( Vec2 position )
{
	m_start = position;
	m_end = position;
}

void Capsule2::SetPositions( Vec2 point0, Vec2 point1 )
{
	m_start = point0;
	m_end = point1;
}

void Capsule2::Translate( Vec2 offset )
{
	m_start += offset;
	m_end += offset;
}

Vec2 Capsule2::GetStart() const
{
	return m_start;
}

Vec2 Capsule2::GetEnd() const
{
	return m_end;
}
