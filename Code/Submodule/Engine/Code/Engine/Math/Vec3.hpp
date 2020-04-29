#pragma once
#include "MathUtils.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//-----------------------------------------------------------------------------------------------
struct Vec3
{


public:

	// Constructor/Deconstructor
	Vec3(){}
	Vec3(const std::string& str);
	Vec3( const Vec3& copyFrom );
	Vec3( float initialXYZ );
	Vec3( float initialX, float initialY, float initialZ );
	Vec3( Vec2 initialXandY, float initialZ );

	~Vec3();

	// Accessors (const methods)
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZDegrees() const;
	float GetAngleAboutZRadians() const;
	const Vec3 GetRotated90Degrees() const;
	const Vec3 GetRotatedMinus90Degrees() const;
	const Vec3 GetRotatedAboutZDegrees ( float degreesToRotateAboutZ ) const;
	const Vec3 GetRotatedAboutZRadians ( float radiansToRotateAboutZ ) const;

	std::string GetAsString() const;

	void Normalize();
	Vec3 GetNormalized();

	void SetFromText(const char* text);

	// Mutators
	void ClampLength( float maxLength );

	// Operators
	const Vec3 operator+( const Vec3& vecToAdd ) const;			// vec3 + vec3
	const Vec3 operator-( const Vec3& vecToSubtract ) const;	// vec3 - vec3
	const Vec3 operator*( float uniformScale ) const;			// vec3 * float
	const Vec3 operator*( const Vec3& vecToMultiply ) const;			// vec3 * float
	const Vec3 operator/( float inverseScale ) const;			// vec3 / float
	const Vec3 operator/( const Vec3& vecToDivide ) const;			// vec3 / float
	void operator+=( const Vec3& vecToAdd );					// vec3 += vec3
	void operator-=( const Vec3& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );				// vec3 *= float
	void operator/=( const float uniformDivisor );				// vec3 /= float
	void operator=( const Vec3& copyFrom );						// vec3 = vec3
	bool operator==( const Vec3& compare ) const;				// vec3 == vec3
	bool operator!=( const Vec3& compare ) const;				// vec3 != vec3

	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3

public: 
	// public variables
	float x;
	float y;
	float z;


	// Predefined Vectors
	static Vec3 FORWARD;
	static Vec3 BACK;
	static Vec3 LEFT;
	static Vec3 RIGHT;
	static Vec3 UP;
	static Vec3 DOWN;
	static Vec3 ZERO;
	static Vec3 VINFINITY;
	static Vec3 VNULL;
};