#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Plane2
{

public:

	Plane2();
	Plane2( Vec2 normal, float distanceToOrigin);
	~Plane2();

	// Positive -> In front of the plane;
	// Negative -> Behind the plane;
	float GetDistance( Vec2 point ) const;
	inline bool IsInFront( Vec2 point ) const { return GetDistance(point) > 0.0f; }
	inline bool IsBehind( Vec2 point) const { return GetDistance(point) < 0.0f; }

	// Would be nice to have at some point
	//Vec2 GetProjectedPoint( Vec2 point ) const;


public:

	Vec2 m_normal;
	float m_distance; // Distance along the normal to get to the origin;


public:

	// Name Constructors
	static Plane2 AtPosition( Vec2 pointOnPlane, Vec2 normal );
	//static Plane2 FromPoints( Vec2 point0, Vec2 point1 );

};



