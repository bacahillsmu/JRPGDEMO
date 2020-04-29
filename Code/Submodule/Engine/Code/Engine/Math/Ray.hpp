#pragma once
#include "Engine/Math/Vec3.hpp"
//#include "Game/Shapes/ConvexPoly.hpp"
#include "Engine/Math/Disc.hpp"

typedef unsigned int uint;

class Plane3;

struct Ray3
{

public:

	Ray3();
	Ray3( Vec3 start, Vec3 direction);

	Vec3 m_start = Vec3::ZERO;
	Vec3 m_direction = Vec3::FORWARD;

	// Helper;
	static Ray3 FromPoints( Vec3 start, Vec3 end );

	inline Vec3 PointAtTime(float time) { return m_start + m_direction * time; }
};


// "Engine/Match/Sphere.hpp"
struct Sphere
{

public:

	Vec3 center;
	float radius;
};

// "Engine/Match/Capsule.hpp"
struct Capsule3
{

public:

	Vec3 start;
	Vec3 end;
	float radius;
};

// Functions;
// All return intersections (including negatives);
uint Raycast( float* out, Ray3* ray, Sphere& sphere);		// Sphere
uint Raycast( float* out, Ray3* ray, Capsule3& capsule);	// Capsule
uint Raycast( float* out, Ray3* ray, Plane3& plane);		// Plane
uint Raycast( float* out, Ray3* ray, Vec3& start, Vec3& direction, float radius);
uint Raycast( float* out, const Vec2& rayDir, const Vec2& rayOrigin, Disc& disc);

//uint Raycast(float* out, Ray3* ray, ConvexPoly2D& convexPoly2D);
//uint Raycast( float *out, Ray3 ray, const OBB3& bounds);		// OBB