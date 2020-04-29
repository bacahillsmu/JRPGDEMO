#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Plane2;
class Segment2;

class OBB2
{

public:

	OBB2();
	OBB2(Vec2 center, Vec2 size = Vec2(0.0f, 0.0f));
	OBB2(Vec2 center, Vec2 size = Vec2(0.0f, 0.0f), float rotation = 0.0f);
	explicit OBB2(AABB2& aabb2);
	~OBB2();

	// Modification
	void Translate( Vec2 offset );
	void MoveTo( Vec2 position );

	void RotateBy( float degrees );
	void SetRotation( float degrees );

	void SetSize( Vec2 size );

	// Describing
	inline Vec2 GetRight() const		{ return m_right; }
	inline Vec2 GetUp() const			{ return m_up; }
	inline Vec2 GetCenter() const		{ return m_center; }
	Vec2 GetSize() const				{ return m_extents; }

	inline Vec2 GetBottomLeft() const	{ return m_center - m_extents.x * GetRight() - m_extents.y * GetUp(); }
	Vec2 GetBottomRight() const			{ return m_center + m_extents.x * GetRight() - m_extents.y * GetUp(); }
	Vec2 GetTopLeft() const				{ return m_center - m_extents.x * GetRight() + m_extents.y * GetUp(); }
	Vec2 GetTopRight() const			{ return m_center + m_extents.x * GetRight() + m_extents.y * GetUp(); }

	void GetCorners(Vec2 *out) const;
	void GetPlanes(Plane2 *out) const;
	void GetSides(Segment2 *out) const;

	// Collision
	Vec2 ToLocalPoint(Vec2 worldPoint) const;
	Vec2 ToWorldPoint(Vec2 localPoint) const;

	bool Contains(Vec2 worldPoint) const;
	Vec2 GetClosestPoint(Vec2 worldPoint) const;

	bool Intersects(const OBB2& other) const;

	AABB2 GetBoundingAABB2() const;

public:

	Vec2 m_right	= Vec2(1.0f, 0.0f);
	Vec2 m_up		= Vec2(0.0f, 1.0f);
	Vec2 m_center	= Vec2(0.0f, 0.0f);
	Vec2 m_extents	= Vec2(0.0f, 0.0f);

};