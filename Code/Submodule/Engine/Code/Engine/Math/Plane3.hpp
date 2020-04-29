#pragma once
//#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"


class Plane3
{

public:

	Plane3();
	Plane3( Vec3 normal, float signedDistanceToOrigin );
	~Plane3();

	float GetDistance( Vec3 point ) const;
	inline bool IsInFront( Vec3 point ) const { return GetDistance(point) > 0.0f; }
	inline bool IsBehind( Vec3 point) const { return GetDistance(point) < 0.0f; }



public:

	Vec3 m_normal;
	float m_signedDistance;


public:

	// Named Constructor;
	static Plane3 AtPosition( Vec3 pointOnPlane, Vec3 normal );
	static Plane3 FromTriangleLH(Vec3& point1, Vec3& point2, Vec3& point3);
};