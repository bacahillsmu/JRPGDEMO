#pragma once
#include "MathUtils.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Matrix4x4;
enum 
{ 
	Ix, Iy, Iz, Iw,  
	Jx, Jy, Jz, Jw,  
	Kx, Ky, Kz, Kw,  
	Tx, Ty, Tz, Tw 
};

//-----------------------------------------------------------------------------------------------
struct Vec4
{
public: 
	// public variables
	float x;
	float y;
	float z;
	float w;

public:
	// public members
	~Vec4() {}												// destructor: do nothing (for speed)
	Vec4() {}												// default constructor: do nothing (for speed)
	Vec4( const Vec4& copyFrom );							// copy constructor (from another vec3)
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y)
	Vec4( Vec3 initialXandYandZ, float initialW );
																			// Accessors (const methods)
	void SetFromText(const char* text);

	// Operators
	const Vec4 operator+( const Vec4& vecToAdd ) const;			// Vec4 + Vec4
	const Vec4 operator-( const Vec4& vecToSubtract ) const;	// Vec4 - Vec4
	const Vec4 operator*( float uniformScale ) const;			// Vec4 * float
	const Vec4 operator*( const Vec4& vecToMultiply ) const;			// Vec4 * float
	const Vec4 operator/( float inverseScale ) const;			// Vec4 / float
	const Vec4 operator/( const Vec4& vecToDivide ) const;			// Vec4 / float
	void operator+=( const Vec4& vecToAdd );					// Vec4 += Vec4
	void operator-=( const Vec4& vecToSubtract );				// Vec4 -= Vec4
	void operator*=( const float uniformScale );				// Vec4 *= float
	void operator*=( const Matrix4x4 matrix );
	void operator/=( const float uniformDivisor );				// Vec4 /= float
	void operator=( const Vec4& copyFrom );						// Vec4 = Vec4
	bool operator==( const Vec4& compare ) const;				// Vec4 == Vec4
	bool operator!=( const Vec4& compare ) const;				// Vec4 != Vec4

	friend const Vec4 operator*( float uniformScale, const Vec4& vecToScale );	// float * Vec4

};