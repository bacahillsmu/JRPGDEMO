#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}

//------------------------------------------------------------------------
Vec3::Vec3( float initialXYZ )
	: x( initialXYZ)
	, y( initialXYZ)
	, z( initialXYZ)
{
}


//------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}

Vec3::Vec3( Vec2 initialXandY, float initialZ )
	: x( initialXandY.x)
	, y( initialXandY.y)
	, z( initialZ )
{
}

Vec3::Vec3( const std::string& str )
{
	SetFromText(str.c_str());
}

//------------------------------------------------------------------------
Vec3::~Vec3()
{

}

//------------------------------------------------------------------------
float Vec3::GetLength() const
{
	return sqrtf( (x * x) + ( y * y ) + ( z * z ) );
}


//------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
	return sqrtf( (x * x) + ( y * y ) );
}


//------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
	return (x * x) + ( y * y ) + ( z * z );
}


//------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
	return (x * x) + ( y * y );
}


//------------------------------------------------------------------------
float Vec3::GetAngleAboutZDegrees() const
{
	return ATan2Degrees( y, x );
}


//------------------------------------------------------------------------
float Vec3::GetAngleAboutZRadians() const
{
	return (GetAngleAboutZDegrees() * MATH_PI) / 180.0f;
}

const Vec3 Vec3::GetRotated90Degrees() const
{
	return Vec3( -y, x, z);
}

const Vec3 Vec3::GetRotatedMinus90Degrees() const
{
	return Vec3( y, -x, z);
}

//------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZDegrees( float degreesToRotateAboutZ ) const
{
	float newDegrees = ATan2Degrees(y, x) + degreesToRotateAboutZ;

	float newX = GetLengthXY() * CosDegrees( newDegrees );
	float newY = GetLengthXY() * SinDegrees( newDegrees );

	return Vec3( newX, newY, z );
}

//------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZRadians( float radiansToRotateAboutZ ) const
{
	return GetRotatedAboutZDegrees( ConvertRadiansToDegrees( radiansToRotateAboutZ ) );
}

std::string Vec3::GetAsString() const
{
	std::string returnString;

	returnString += std::to_string(x);
	returnString += ", ";
	returnString += std::to_string(y);
	returnString += ", ";
	returnString += std::to_string(z);

	return returnString;
}

void Vec3::Normalize()
{
	float currentLength = GetLength();
	x = x / currentLength;
	y = y / currentLength;
	z = z / currentLength;
}

Vec3 Vec3::GetNormalized()
{
	float currentLength = GetLength();
	float newX = x / currentLength;
	float newY = y / currentLength;
	float newZ = z / currentLength;

	return Vec3(newX, newY, newZ);
}

void Vec3::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, ',');

	if(delimString.size() == 3 )
	{
		x = (float)atof(delimString[0].c_str());
		y = (float)atof(delimString[1].c_str());
		z = (float)atof(delimString[2].c_str());
	}
	else
	{
		ERROR_AND_DIE("Vec3 had the incorrect count of commas in it.");
	}
}
//-----------------------------------------------------------------------------------------------
void Vec3::ClampLength( float maxLength )
{
	if( GetLength() >= maxLength)
	{
		float newX = x * (maxLength / GetLength());
		float newY = y * (maxLength / GetLength());
		float newZ = z * (maxLength / GetLength());

		x = newX;
		y = newY;
		z = newZ;
	}
}

//------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, 
		         y + vecToAdd.y,
		         z + vecToAdd.z );
}

//------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, 
	             y - vecToSubtract.y,
		         z - vecToSubtract.z );
}

//------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, 
				 y * uniformScale,
		         z * uniformScale );
}

const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( x * vecToMultiply.x,
				 y * vecToMultiply.y,
				 z * vecToMultiply.z );
}

//------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale,
		         y / inverseScale,
		         z / inverseScale );
}

const Vec3 Vec3::operator/( const Vec3& vecToDivide ) const
{
	return Vec3( x / vecToDivide.x,
				 y / vecToDivide.y,
				 z / vecToDivide.z );
}

//------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x = x + vecToAdd.x;
	y = y + vecToAdd.y;
	z = z + vecToAdd.z;
}

//------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x = x - vecToSubtract.x;
	y = y - vecToSubtract.y;
	z = z - vecToSubtract.z;
}

//------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
	z = z * uniformScale;
}

//------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x = x / uniformDivisor;
	y = y / uniformDivisor;
	z = z / uniformDivisor;
}

//------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

//------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale,
		         vecToScale.y * uniformScale,
		         vecToScale.z * uniformScale );
}

//------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if (x == compare.x && y == compare.y && z == compare.z)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if (x != compare.x || y != compare.y || z != compare.z)
		return true;
	else
		return false;
}

// Predefined Vectors
Vec3 Vec3::FORWARD		= Vec3(0.0f, 0.0f, -1.0f);
Vec3 Vec3::BACK			= Vec3(0.0f, 0.0f, 1.0f);
Vec3 Vec3::LEFT			= Vec3(-1.0f, 0.0f, 0.0f);
Vec3 Vec3::RIGHT		= Vec3(1.0f, 0.0f, 0.0f);
Vec3 Vec3::DOWN			= Vec3(0.0f, -1.0f, 0.0f);
Vec3 Vec3::UP			= Vec3(0.0f, 1.0f, 0.0f);
Vec3 Vec3::ZERO			= Vec3(0.0f, 0.0f, 0.0f);
Vec3 Vec3::VINFINITY	= Vec3(INFINITY, INFINITY, INFINITY);
Vec3 Vec3::VNULL		= Vec3(INFINITY, INFINITY, INFINITY);
