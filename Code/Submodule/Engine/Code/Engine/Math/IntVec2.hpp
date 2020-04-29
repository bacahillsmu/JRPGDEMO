#pragma once

#include "MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x;
	int y;

public:

	// Constructor/Deconstructor
	IntVec2();
	IntVec2( const IntVec2& copyFrom );
	IntVec2( int initialXY );
	IntVec2( int initialX, int initialY );
	~IntVec2();

	void SetFromText(const char* text);

	// Accessors (const methods)
	int GetLengthSquared() const;
	const IntVec2 GetRotated90Degrees() const;
	const IntVec2 GetRotatedMinus90Degrees() const;

	bool IsZero();

	

	// Mutators (non-const methods)
	void Rotate90Degrees();
	void RotateMinus90Degrees();

	// Operators
	const IntVec2 operator+( const IntVec2& vecToAdd ) const;			// vec2 + vec2
	const IntVec2 operator-( const IntVec2& vecToSubtract ) const;	// vec2 - vec2
	const IntVec2 operator*( int uniformScale ) const;			// vec2 * int
	void operator+=( const IntVec2& vecToAdd );					// vec2 += vec2
	void operator-=( const IntVec2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const int uniformScale );				// vec2 *= int
	void operator=( const IntVec2& copyFrom );						// vec2 = vec2
	bool operator==( const IntVec2& compare ) const;				// vec2 == vec2
	bool operator!=( const IntVec2& compare ) const;				// vec2 != vec2

	friend const IntVec2 operator*( int uniformScale, IntVec2& vecToScale );	// int * vec2



};

IntVec2 Sign(IntVec2 intVector);
