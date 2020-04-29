#include "Vec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

Vec2::Vec2( float initialXY )
	: x( initialXY )
	, y( initialXY )
{
}

Vec2::Vec2( const std::string& str )
{
	SetFromText(str.c_str());
}

Vec2::Vec2( const IntVec2& intvec )
	: x((float)intvec.x)
	, y((float)intvec.y)
{
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length )
{
	float x = length * CosDegrees( directionDegrees );
	float y = length * SinDegrees( directionDegrees );
	return Vec2(x, y);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length )
{
	return MakeFromPolarDegrees( ConvertRadiansToDegrees( directionRadians ), length );
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	return sqrtf( (x * x) + ( y * y ));
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return (x * x) + ( y * y );
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetAngleDegrees() const
{
	return ATan2Degrees( y, x );
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetAngleRadians() const
{
	return (GetAngleDegrees() * MATH_PI) / 180.0f;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2( -y, x);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2( y, -x);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedDegrees( float degreesToRotate ) const
{
	float newDegrees = ATan2Degrees(y, x) + degreesToRotate;

	float newX = GetLength() * CosDegrees( newDegrees );
	float newY = GetLength() * SinDegrees( newDegrees );

	return Vec2( newX, newY );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedRadians( float radiansToRotate ) const
{
	return GetRotatedDegrees( ConvertRadiansToDegrees( radiansToRotate ) );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetClamped( float maxLength ) const
{
	if( GetLength() >= maxLength )
	{
		float newX = x * (maxLength / GetLength());
		float newY = y * (maxLength / GetLength());

		return Vec2( newX, newY);
	}

	return Vec2( x, y );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetNormalized() const
{
	float length = GetLength();
	if(length != 0)
	{
		return Vec2( x / length, y / length);
	}
	
	return *this;
}

Vec2 Vec2::GetMin( Vec2& vec2B )
{
	float min1;
	float min2;

	min1 = x < vec2B.x ? x : vec2B.x;
	min2 = y < vec2B.y ? y : vec2B.y;
	return Vec2(min1, min2);
}

Vec2 Vec2::GetMax( Vec2& vec2B )
{
	float max1;
	float max2;

	max1 = x > vec2B.x ? x : vec2B.x;
	max2 = y > vec2B.y ? y : vec2B.y;
	return Vec2(max1, max2);
}

//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength( float maxLength )
{
	if( GetLength() >= maxLength)
	{
		float newX = x * (maxLength / GetLength());
		float newY = y * (maxLength / GetLength());

		x = newX;
		y = newY;
	}
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetLength( float newLength )
{
	x = x * (newLength / GetLength());
	y = y * (newLength / GetLength());
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetAngleDegrees( float newAngleDegrees )
{
	float r = GetLength();

	x = r * CosDegrees( newAngleDegrees );
	y = r * SinDegrees( newAngleDegrees );
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetAngleRadians( float newAngleRadians )
{
	SetAngleDegrees( ConvertRadiansToDegrees( newAngleRadians ) );
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	x = newLength * CosDegrees( newAngleDegrees );
	y = newLength * SinDegrees( newAngleDegrees );
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	x = newLength * CosDegrees( ConvertRadiansToDegrees( newAngleRadians ) );
	y = newLength * SinDegrees( ConvertRadiansToDegrees( newAngleRadians ) );
}

void Vec2::Rotate90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = -oldY;
	y = oldX;
}

void Vec2::RotateMinus90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = oldY;
	y = -oldX;
}

//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float currentLength = GetLength();
	x = x / currentLength;
	y = y / currentLength;
}

//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetOldLength()
{
	float oldLength = GetLength();
	x = x / oldLength;
	y = y / oldLength;
	return oldLength;
}

void Vec2::RotateDegrees( float rotateDegrees )
{
	float r = GetLength();
	float oldDegrees = ATan2Degrees(y, x);
	float newDegrees = oldDegrees + rotateDegrees;

	float cosThetaRadians = CosDegrees( newDegrees );
	float sinThetaRadians = SinDegrees( newDegrees );

	float newX = r * cosThetaRadians;
	float newY = r * sinThetaRadians;

	x = newX;
	y = newY;
}

void Vec2::RotateRadians( float rotateRadians )
{
	float rotateDegrees = ConvertRadiansToDegrees(rotateRadians);

	float r = GetLength();
	float oldDegrees = ATan2Degrees(y, x);
	float newDegrees = oldDegrees + rotateDegrees;

	float cosThetaRadians = CosDegrees( newDegrees );
	float sinThetaRadians = SinDegrees( newDegrees );

	float newX = r * cosThetaRadians;
	float newY = r * sinThetaRadians;

	x = newX;
	y = newY;
}

Vec2 Sign( Vec2 vector )
{
	Vec2 returnVector = Vec2(1.0f, 1.0f);

	if(vector.x < 0.0f)
	{
		returnVector.x = -1.0f;
	}
	if(vector.y < 0.0f)
	{
		returnVector.y = -1.0f;
	}

	return returnVector;
}

Vec2 AbsoluteValue( Vec2 vector )
{
	Vec2 returnVector = vector;

	if(vector.x < 0.0f)
	{
		returnVector.x = vector.x * -1.0f;
	}
	if(vector.y < 0.0f)
	{
		returnVector.y = vector.y * -1.0f;
	}

	return returnVector;
}

Vec2 Fract( Vec2 vector )
{
	Vec2 returnVector = Vec2(0.0f, 0.0f);
	
	// x
	float x = vector.x;
	float wholex, fractionalx;
	fractionalx = std::modf(x, &wholex);
	returnVector.x = fractionalx;

	// y
	float y = vector.y;
	float wholey, fractionaly;
	fractionaly = std::modf(y, &wholey);
	returnVector.y = fractionaly;

	return returnVector;
}

std::string Vec2::GetAsString() const
{
	std::string returnString;

	returnString += std::to_string(x);
	returnString += ", ";
	returnString += std::to_string(y);

	return returnString;
}

void Vec2::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, ',');

	if(delimString.size() == 2 )
	{
		x = (float)atof(delimString[0].c_str());
		y = (float)atof(delimString[1].c_str());
	}
	else
	{
		ERROR_AND_DIE("Vec2 had the incorrect count of commas in it.");
	}
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


const Vec2 Vec2::operator-( const float uniformSubtract )
{
	return Vec2(x - uniformSubtract, y - uniformSubtract);
}

const Vec2 Vec2::operator+( const float uniformAddition )
{
	return Vec2(x + uniformAddition, y + uniformAddition);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}


const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2(x * vecToMultiply.x, y * vecToMultiply.y);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


const Vec2 Vec2::operator/( const Vec2& inverseVec ) const
{
	return Vec2(x / inverseVec.x, y / inverseVec.y);
}

//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x = x + vecToAdd.x;
	y = y + vecToAdd.y;
}

//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x = x - vecToSubtract.x;
	y = y - vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x = x / uniformDivisor;
	y = y / uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if (x == compare.x && y == compare.y)
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if (x != compare.x || y != compare.y)
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator<( const Vec2& compare ) const
{
	if(x < compare.x && y < compare.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator<=( const Vec2& compare ) const
{
	if(x <= compare.x && y <= compare.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator>( const Vec2& compare ) const
{
	if(x > compare.x && y > compare.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator>=( const Vec2& compare ) const
{
	if(x >= compare.x && y >= compare.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const Vec2 ALIGNED_CENTER = Vec2(0.0f, 0.0f);

// Predefined Vectors
Vec2 Vec2::LEFT			= Vec2(-1.0f, 0.0f);
Vec2 Vec2::RIGHT		= Vec2(1.0f, 0.0f);
Vec2 Vec2::DOWN			= Vec2(0.0f, -1.0f);
Vec2 Vec2::UP			= Vec2(0.0f, 1.0f);
Vec2 Vec2::ZERO			= Vec2(0.0f, 0.0f);
Vec2 Vec2::ONE			= Vec2(1.0f, 1.0f);
Vec2 Vec2::VINFINITY	= Vec2(INFINITY, INFINITY);
Vec2 Vec2::VNULL		= Vec2(NULL, NULL);