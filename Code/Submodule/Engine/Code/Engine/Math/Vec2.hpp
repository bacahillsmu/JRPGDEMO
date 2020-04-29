#pragma once

#include "MathUtils.hpp"
#include "IntVec2.hpp"

#include <string.h>

//-----------------------------------------------------------------------------------------------
struct Vec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;

public:

	// Constructor/Deconstructor
	Vec2() {}
	~Vec2() {}
	Vec2( const std::string& str );
	Vec2( float initialXY );
	Vec2( const Vec2& copyFrom );
	Vec2( float initialX, float initialY );
	Vec2( const IntVec2& intvec );


	// Static Constructors
	static const Vec2 MakeFromPolarDegrees( float directionDegrees, float length = 1.0f );
	static const Vec2 MakeFromPolarRadians( float directionRadians, float length = 1.0f );

	// Accessors (const methods)
	float GetLength() const;
	float GetLengthSquared() const;
	float GetAngleDegrees() const;
	float GetAngleRadians() const;
	const Vec2 GetRotated90Degrees() const;
	const Vec2 GetRotatedMinus90Degrees() const;
	const Vec2 GetRotatedDegrees( float degreesToRotate ) const;
	const Vec2 GetRotatedRadians( float radiansToRotate ) const;
	const Vec2 GetClamped( float maxLength ) const;
	const Vec2 GetNormalized() const;
	Vec2 GetMin(Vec2& vec2B);
	Vec2 GetMax(Vec2& vec2B);

	// Mutators (non-const methods)
	void ClampLength( float maxLength );
	void SetLength( float newLength );
	void SetAngleDegrees( float newAngleDegrees );
	void SetAngleRadians( float newAngleRadians );
	void SetPolarDegrees( float newAngleDegrees, float newLength );
	void SetPolarRadians( float newAngleRadians, float newLength );
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void Normalize();
	float NormalizeAndGetOldLength();
	void RotateDegrees( float rotateDegrees );
	void RotateRadians ( float rotateRadians );
	
	

	std::string GetAsString() const;

	void SetFromText(const char* text);

	// Operators
	const Vec2 operator+( const Vec2& vecToAdd ) const;			// vec2 + vec2
	const Vec2 operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
	const Vec2 operator*( const Vec2& vecToMultiply ) const;	// vec2 - vec2
	const Vec2 operator*( float uniformScale ) const;			// vec2 * vec2
	const Vec2 operator/( float inverseScale ) const;			// vec2 / float
	const Vec2 operator/( const Vec2& inverseVec ) const;			// vec2 / vec2
	const Vec2 operator-(const float uniformSubtract);
	const Vec2 operator+(const float uniformAddition);
	void operator+=( const Vec2& vecToAdd );					// vec2 += vec2
	void operator-=( const Vec2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );				// vec2 *= float
	void operator/=( const float uniformDivisor );				// vec2 /= float
	void operator=( const Vec2& copyFrom );						// vec2 = vec2
	bool operator==( const Vec2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vec2& compare ) const;				// vec2 != vec2
	bool operator<( const Vec2& compare ) const;				// vec2 != vec2
	bool operator<=( const Vec2& compare ) const;				// vec2 != vec2
	bool operator>( const Vec2& compare ) const;				// vec2 != vec2
	bool operator>=( const Vec2& compare ) const;				// vec2 != vec2

	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2

	// Predefined Vectors
	static Vec2 LEFT;
	static Vec2 RIGHT;
	static Vec2 UP;
	static Vec2 DOWN;
	static Vec2 ZERO;
	static Vec2 ONE;
	static Vec2 VINFINITY;
	static Vec2 VNULL;

};

Vec2 Sign(Vec2 vector);
Vec2 AbsoluteValue(Vec2 vector);
Vec2 Fract(Vec2 vector);