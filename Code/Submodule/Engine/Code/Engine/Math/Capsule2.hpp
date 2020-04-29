#pragma once
#include "Engine/Core/EngineCommon.hpp"



class Capsule2
{

public:
	Capsule2();
	Capsule2( Vec2 position );				// Equivalent to a point;
	Capsule2( Vec2 center, float radius );	// Equivalent to a disc;
	Capsule2( Vec2 point0, Vec2 point1, float radius );

	// Modification
	void SetPosition( Vec2 position );
	void SetPositions( Vec2 point0, Vec2 point1 );
	void Translate( Vec2 offset );

	void RotateBy( float degrees );

	// Helpers for describing it;
	Vec2 GetStart() const;
	Vec2 GetEnd() const;

	// Collision Utility
	bool Contains( Vec2 worldPoint );
	Vec2 GetClosestPoint( Vec2 worldPoint );

	AABB2 GetBoundingAABB2() const;


public:

	Vec2 m_start	= Vec2(0.0f, 0.0f);
	Vec2 m_end		= Vec2(0.0f, 0.0f);
	float m_radius	= 0.0f;



};



