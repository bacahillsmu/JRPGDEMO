#include "Engine/Math/Ray.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Core/EngineCommon.hpp"


// -----------------------------------------------------------------------
Ray3::Ray3()
{

}

// -----------------------------------------------------------------------
Ray3::Ray3( Vec3 start, Vec3 direction )
{
	m_start = start;
	m_direction = direction;
}

// -----------------------------------------------------------------------
STATIC Ray3 Ray3::FromPoints( Vec3 start, Vec3 end )
{
	Vec3 direction = end - start;
	direction.Normalize();

	return Ray3(start, direction);
}

// -----------------------------------------------------------------------
uint Raycast( float *out, Ray3* ray, Sphere& sphere )
{
	Vec3 i = ray->m_start - sphere.center;
	
	float a = 1.0f;
	float b = DotProductVec3((2.0f * i), ray->m_direction) ;
	float c = DotProductVec3(i, i) - (sphere.radius * sphere.radius);

	uint solutions = Quadratic(out, a, b, c);

	return solutions;
}

// -----------------------------------------------------------------------
uint Raycast( float *out, Ray3* ray, Capsule3& capsule )
{
	float validTimes[6];
	int currentIndex = 0;
	
	Vec3 capsuleDirection = capsule.end - capsule.start;
	capsuleDirection.Normalize();

	float raycastResults[2];
	uint cylinderHits = Raycast(raycastResults, ray, capsule.start, capsuleDirection, capsule.radius);
	

	// Hits on infinite cylinder;
	for(uint i = 0; i < cylinderHits; i++)
	{
		Vec3 point = ray->PointAtTime(raycastResults[i]);

		Vec3 ps = capsule.start - point;
		Vec3 pe = capsule.end - point;

		float x = DotProductVec3(ps, capsuleDirection);
		float y = DotProductVec3(pe, capsuleDirection);

		if(x < 0.0f && y > 0.0f)
		{
			validTimes[currentIndex++] = raycastResults[i];
		}
	}

	// Hits on sphere;
	Sphere sphere;
	sphere.center = capsule.start;
	sphere.radius = capsule.radius;
	uint sphereHits = Raycast(raycastResults, ray, sphere);
	for(uint j = 0; j < sphereHits; j++)
	{
		validTimes[currentIndex++] = raycastResults[j];
	}

	sphere.center = capsule.end;
	sphereHits = Raycast(raycastResults, ray, sphere);
	for(uint j = 0; j < sphereHits; j++)
	{
		validTimes[currentIndex++] = raycastResults[j];
	}

	// Early outs;
	if(currentIndex == 0)
	{
		return 0;
	}
	if(currentIndex == 1)
	{
		out[0] = validTimes[0];
		return 1;
	}

	// Get min and max of times;
	float min = validTimes[0];
	float max = validTimes[0];

	for(int k = 0; k < currentIndex; k++)
	{
		min = GetMin(min, validTimes[k]);
		max = GetMax(max, validTimes[k]);
	}

	out[0] = min;
	out[1] = max;

	return 2;
}

// -----------------------------------------------------------------------
uint Raycast( float *out, Ray3* ray, Plane3& plane )
{
	float denominator = DotProductVec3(ray->m_direction, plane.m_normal);

	if(denominator == 0.0f)
	{
		return 0;
	}

	float numerator = plane.m_signedDistance - DotProductVec3(ray->m_start, plane.m_normal);
	out[0] = numerator / denominator;
	return 1;
}

// -----------------------------------------------------------------------
uint Raycast( float* out, Ray3* ray, Vec3& point, Vec3& line, float radius )
{
	Vec3 a = (DotProductVec3(ray->m_direction, line) * line) - ray->m_direction;
	Vec3 b = point + ((DotProductVec3(ray->m_start, line) * line) - (DotProductVec3(point, line) * line)) - ray->m_start;
	float A = DotProductVec3(a, a);
	float B = 2.0f * DotProductVec3(a, b);
	float C = DotProductVec3(b, b) - (radius * radius);

	return Quadratic(out, A, B, C);

}

// -----------------------------------------------------------------------
uint Raycast(float* out, const Vec2& rayDir, const Vec2& rayOrigin, Disc& disc)
{
	Vec2 i = rayOrigin - disc.discCenter;

	float a = 1.0f;
	float b = DotProductVec2((2.0f * i), rayDir);
	float c = DotProductVec2(i, i) - disc.discRadius * disc.discRadius;

	uint solutions = Quadratic(out, a, b, c);

	return solutions;
}

// -----------------------------------------------------------------------
// uint Raycast(float* out, Ray3* ray, ConvexPoly2D& convexPoly2D)
// {
// 	for(int i = 0; i < convexPoly2D.m_verts.size(); ++i)
// 	{
// 
// 	}
// 
// 	return 0;
// }
