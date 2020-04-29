#include "Engine/Math/Plane3.hpp"


// -----------------------------------------------------------------------
Plane3::Plane3()
{

}

// -----------------------------------------------------------------------
Plane3::Plane3( Vec3 normal, float distanceToOrigin )
{
	m_normal = normal;
	m_signedDistance = distanceToOrigin;
}

// -----------------------------------------------------------------------
Plane3::~Plane3()
{

}

// -----------------------------------------------------------------------
float Plane3::GetDistance( Vec3 point ) const
{
	float distance = DotProductVec3(point, m_normal); // We dot the point with plane normal to get displacement from origin
	distance -= m_signedDistance; // Remove the distance of the plane from origin.
	return distance;
}

// -----------------------------------------------------------------------
Plane3 Plane3::AtPosition( Vec3 pointOnPlane, Vec3 normal )
{
	Plane3 plane;
	plane.m_normal = normal;
	plane.m_signedDistance = DotProductVec3(pointOnPlane, normal);

	return plane;
}

Plane3 Plane3::FromTriangleLH( Vec3& point1, Vec3& point2, Vec3& point3 )
{
	Plane3 plane;
	
	Vec3 U = point2 - point1;
	Vec3 V = point3 - point1;

	Vec3 crossed = CrossProduct(U, V);
	crossed.Normalize();
	plane.m_normal = crossed;

	plane.m_signedDistance = DotProductVec3(point1, plane.m_normal);

	return plane;
}
