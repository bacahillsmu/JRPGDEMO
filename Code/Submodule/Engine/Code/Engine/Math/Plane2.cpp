#include "Engine/Math/Plane2.hpp"


Plane2::Plane2()
{

}

Plane2::Plane2( Vec2 normal, float distanceToOrigin )
{
	m_normal = normal;
	m_distance = distanceToOrigin;
}

Plane2::~Plane2()
{

}

float Plane2::GetDistance( Vec2 point ) const
{
	float distance = DotProductVec2(point, m_normal); // We dot the point with plane normal to get displacement from origin
	distance -= m_distance; // Remove the distance of the plane from origin.
	return distance;
}

// Vec2 Plane2::GetProjectedPoint( Vec2 point ) const
// {
// 	
// }

STATIC Plane2 Plane2::AtPosition( Vec2 pointOnPlane, Vec2 normal )
{
	Plane2 plane;
	plane.m_normal = normal;
	plane.m_distance = DotProductVec2(pointOnPlane, normal);

	return plane;
}
