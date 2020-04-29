#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/Segment2.hpp"

OBB2::OBB2()
{

}

OBB2::OBB2( Vec2 center, Vec2 size /*= Vec2(0.0f, 0.0f)*/ )
{
	m_center = center;
	m_extents = size;
}

OBB2::OBB2( AABB2& aabb2 )
{
	m_right		= Vec2( 1.0f, 0.0f );
	m_up		= Vec2( 0.0f, 1.0f );
	m_center	= (aabb2.GetBottomLeft() + aabb2.GetTopRight()) * 0.5f;
	m_extents	= (aabb2.GetTopRight() - aabb2.GetBottomLeft()) * 0.5f;
}

OBB2::OBB2( Vec2 center, Vec2 size /*= Vec2(0.0f, 0.0f)*/, float rotation /*= 0.0f*/ )
{
	m_center = center;
	m_extents = size;
	SetRotation(rotation);
	m_right.Normalize();
	m_up.Normalize();
}

OBB2::~OBB2()
{

}

void OBB2::Translate( Vec2 offset )
{
	m_center += offset;
}

void OBB2::MoveTo( Vec2 position )
{
	m_center = position;
}

void OBB2::RotateBy( float degrees )
{
	m_right.RotateDegrees(degrees);
	m_up = m_right.GetRotated90Degrees();
}

void OBB2::SetRotation( float degrees )
{
	m_right = Vec2(CosDegrees(degrees), SinDegrees(degrees));
	m_up = m_right.GetRotated90Degrees();
}

void OBB2::SetSize( Vec2 size )
{
	m_extents = size;
}

void OBB2::GetCorners( Vec2 *out ) const
{
	out[0] = GetTopLeft();
	out[1] = GetTopRight();
	out[2] = GetBottomRight();
	out[3] = GetBottomLeft();
}

void OBB2::GetPlanes( Plane2 *out ) const
{
	
	out[0] = Plane2::AtPosition(GetTopLeft(), m_up);
	out[1] = Plane2::AtPosition(GetTopRight(), m_right);
	out[2] = Plane2::AtPosition(GetBottomRight(), m_up * -1.0f);
	out[3] = Plane2::AtPosition(GetBottomLeft(), m_right * -1.0f);

}

void OBB2::GetSides( Segment2 *out ) const
{
	out[0] = Segment2::CreateSegment(GetTopLeft(), GetTopRight());
	out[1] = Segment2::CreateSegment(GetTopRight(), GetBottomRight());
	out[2] = Segment2::CreateSegment(GetBottomRight(), GetBottomLeft());
	out[3] = Segment2::CreateSegment(GetBottomLeft(), GetTopLeft());
}

Vec2 OBB2::ToLocalPoint( Vec2 worldPoint ) const
{
	Vec2 disp = worldPoint - GetCenter();

	float localI = DotProductVec2( disp, GetRight() );
	float localJ = DotProductVec2( disp, GetUp() );

	return Vec2(localI, localJ);
}

Vec2 OBB2::ToWorldPoint( Vec2 localPoint ) const
{
	return localPoint.x * GetRight() + localPoint.y * GetUp() + m_center;
}

bool OBB2::Contains( Vec2 worldPoint ) const
{
	Vec2 localPoint = ToLocalPoint( worldPoint );
	Vec2 absLocalPoint = Vec2(abs(localPoint.x), abs(localPoint.y));

	return (absLocalPoint.x < m_extents.x) && (absLocalPoint.y < m_extents.y);
}

Vec2 OBB2::GetClosestPoint( Vec2 worldPoint ) const
{
	Vec2 localPoint = ToLocalPoint( worldPoint );

	Vec2 minRangeExtents = Vec2(-m_extents.x, -m_extents.y);
	Vec2 maxRangeExtents = Vec2(m_extents.x, m_extents.y);

	Vec2 clampedPoint = Clamp( localPoint, minRangeExtents, maxRangeExtents );

	return ToWorldPoint( clampedPoint );
}

bool OBB2::Intersects( const OBB2& other ) const
{
	UNUSED(other);
	// Is in GetManifold
	return false;
}

AABB2 OBB2::GetBoundingAABB2() const
{
	return AABB2();
}
