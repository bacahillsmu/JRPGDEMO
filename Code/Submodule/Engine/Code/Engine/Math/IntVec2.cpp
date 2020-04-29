#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



//------------------------------------------------------------------------
IntVec2::IntVec2()
{

}

//------------------------------------------------------------------------
IntVec2::IntVec2( const IntVec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}

//------------------------------------------------------------------------
IntVec2::IntVec2( int initialXY )
	: x( initialXY )
	, y( initialXY )
{
}

//------------------------------------------------------------------------
IntVec2::IntVec2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{
}

//------------------------------------------------------------------------
IntVec2::~IntVec2()
{

}

//------------------------------------------------------------------------
void IntVec2::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, ',');

	if(delimString.size() == 2 )
	{
		x = (int)atoi(delimString[0].c_str());
		y = (int)atoi(delimString[1].c_str());
	}
	else
	{
		ERROR_AND_DIE("IntVec2 had the incorrect count of commas in it.");
	}
}

//------------------------------------------------------------------------
int IntVec2::GetLengthSquared() const
{
	return (x * x) + ( y * y );
}

//------------------------------------------------------------------------
const IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2( -y, x);
}


//------------------------------------------------------------------------
const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2( y, -x);
}

bool IntVec2::IsZero()
{
	return x == 0 && y == 0;
}

//------------------------------------------------------------------------
IntVec2 Sign( IntVec2 intVector )
{
	IntVec2 returnVector = IntVec2(1, 1);

	if(intVector.x < 0)
	{
		returnVector.x = -1;
	}
	if(intVector.y < 0)
	{
		returnVector.y = -1;
	}

	return returnVector;
}

//------------------------------------------------------------------------
void IntVec2::Rotate90Degrees()
{
	int oldX = x;
	int oldY = y;
	x = -oldY;
	y = oldX;
}

//------------------------------------------------------------------------
void IntVec2::RotateMinus90Degrees()
{
	int oldX = x;
	int oldY = y;
	x = oldY;
	y = -oldX;
}

//------------------------------------------------------------------------
const IntVec2 IntVec2::operator + ( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}


//------------------------------------------------------------------------
const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

//------------------------------------------------------------------------
const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( x * uniformScale, y * uniformScale );
}

//------------------------------------------------------------------------
void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x = x + vecToAdd.x;
	y = y + vecToAdd.y;
}

//------------------------------------------------------------------------
void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x = x - vecToSubtract.x;
	y = y - vecToSubtract.y;
}

//------------------------------------------------------------------------
void IntVec2::operator*=( const int uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
}

//------------------------------------------------------------------------
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

//------------------------------------------------------------------------
const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
	return IntVec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}

//------------------------------------------------------------------------
bool IntVec2::operator==( const IntVec2& compare ) const
{
	if (x == compare.x && y == compare.y)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------
bool IntVec2::operator!=( const IntVec2& compare ) const
{
	if (x != compare.x || y != compare.y)
		return true;
	else
		return false;
}

