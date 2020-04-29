#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Matrix44.hpp"


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( const Vec4& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
	, w( copy.w )
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}

Vec4::Vec4( Vec3 initialXandYandZ, float initialW )
	: x( initialXandYandZ.x )
	, y( initialXandYandZ.y )
	, z( initialXandYandZ.z )
	, w( initialW )
{
}

void Vec4::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, ',');

	if(delimString.size() == 4 )
	{
		x = (float)atof(delimString[0].c_str());
		y = (float)atof(delimString[1].c_str());
		z = (float)atof(delimString[2].c_str());
		w = (float)atof(delimString[3].c_str());
	}
	else
	{
		ERROR_AND_DIE("Vec4 had the incorrect count of commas in it.");
	}
}

//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator + ( const Vec4& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, 
		y + vecToAdd.y,
		z + vecToAdd.z,
		w + vecToAdd.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
	return Vec4( x - vecToSubtract.x, 
		y - vecToSubtract.y,
		z - vecToSubtract.z,
		w - vecToSubtract.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( float uniformScale ) const
{
	return Vec4( x * uniformScale, 
		y * uniformScale,
		z * uniformScale,
		w * uniformScale);
}


const Vec4 Vec4::operator*( const Vec4& vecToMultiply ) const
{
	return Vec4( x * vecToMultiply.x,
		y * vecToMultiply.y,
		z * vecToMultiply.z,
		w * vecToMultiply.w);
}

//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/( float inverseScale ) const
{
	return Vec4( x / inverseScale,
		y / inverseScale,
		z / inverseScale,
		w / inverseScale);
}


const Vec4 Vec4::operator/( const Vec4& vecToDivide ) const
{
	return Vec4( x / vecToDivide.x,
		y / vecToDivide.y,
		z / vecToDivide.z,
		w / vecToDivide.w);
}

void Vec4::operator*=( const Matrix4x4 matrix )
{
	float newX = x * matrix.m_values[Ix] + y * matrix.m_values[Jx] + z * matrix.m_values[Kx] + w * matrix.m_values[Tx];
	float newY = x * matrix.m_values[Iy] + y * matrix.m_values[Jy] + z * matrix.m_values[Ky] + w * matrix.m_values[Ty];
	float newZ = x * matrix.m_values[Iz] + y * matrix.m_values[Jz] + z * matrix.m_values[Kz] + w * matrix.m_values[Tz];
	float newW = x * matrix.m_values[Iw] + y * matrix.m_values[Jw] + z * matrix.m_values[Kw] + w * matrix.m_values[Tw];

	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

//-----------------------------------------------------------------------------------------------
void Vec4::operator+=( const Vec4& vecToAdd )
{
	x = x + vecToAdd.x;
	y = y + vecToAdd.y;
	z = z + vecToAdd.z;
	w = w + vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=( const Vec4& vecToSubtract )
{
	x = x - vecToSubtract.x;
	y = y - vecToSubtract.y;
	z = z - vecToSubtract.z;
	w = w - vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=( const float uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
	z = z * uniformScale;
	w = w * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=( const float uniformDivisor )
{
	x = x / uniformDivisor;
	y = y / uniformDivisor;
	z = z / uniformDivisor;
	w = w / uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( const Vec4& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
const Vec4 operator*( float uniformScale, const Vec4& vecToScale )
{
	return Vec4( vecToScale.x * uniformScale,
		vecToScale.y * uniformScale,
		vecToScale.z * uniformScale,
		vecToScale.w * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( const Vec4& compare ) const
{
	if (x == compare.x && y == compare.y && z == compare.z && w == compare.w)
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=( const Vec4& compare ) const
{
	if (x != compare.x || y != compare.y || z != compare.z || w != compare.w)
		return true;
	else
		return false;
}

