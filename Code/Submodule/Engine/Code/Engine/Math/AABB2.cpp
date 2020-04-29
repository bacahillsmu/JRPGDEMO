#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"

#define UNUSED(x) (void)(x);

AABB2::AABB2( const Vec2& initialCenter, const Vec2& initialOffset )
	: center(initialCenter)
	, offset(initialOffset)
{
	mins.x = center.x - offset.x;
	mins.y = center.y - offset.y;
	maxs.x = center.x + offset.x;
	maxs.y = center.y + offset.y;
}

void AABB2::SetFromText( const char* text )
{
	UNUSED(text);
	ERROR_AND_DIE("Cannot read AABB2 from text because of the change in Constructor to take a center and offset rather than min/max vecs.");
// 	std::vector<std::string> delimString = SplitStringOnDelimiter(text, ',');
// 
// 	if(delimString.size() == 4 )
// 	{
// 		mins = Vec2((float)atof(delimString[0].c_str()), (float)atof(delimString[1].c_str()));
// 		maxs = Vec2((float)atof(delimString[2].c_str()), (float)atof(delimString[3].c_str()));
// 	}
// 	else
// 	{
// 		ERROR_AND_DIE("AABB2 had the incorrect count of commas in it.");
// 	}
}

AABB2 AABB2::MakeFromMinsMaxs( const Vec2& mins, const Vec2& maxs )
{
	return AABB2((mins + maxs) * 0.5f, (maxs - mins) * 0.5f);
}

Vec2 AABB2::GetTopLeft()
{
	return Vec2(mins.x, maxs.y);
}

Vec2 AABB2::GetTopRight()
{
	return Vec2(maxs.x, maxs.y);
}

Vec2 AABB2::GetBottomLeft()
{
	return Vec2(mins.x, mins.y);
}

Vec2 AABB2::GetBottomRight()
{
	return Vec2(maxs.x, mins.y);
}

bool AABB2::IsPointInside( Vec2 point )
{
	if(point.x >= mins.x
	&& point.x <= maxs.x
	&& point.y >= mins.y
	&& point.y <= maxs.y)
	{
		return true;
	}

	return false;
}

float AABB2::GetAspect()
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;

	return fabsf(width / height);
}

Rect2::Rect2( const Vec2& initialCenter, const Vec2& initialOffset )
	: center(initialCenter)
	, offset(initialOffset)
{
	point1.x = center.x - offset.x;
	point1.y = center.y - offset.y;
	point2.x = center.x + offset.x;
	point2.y = center.y + offset.y;
}

Rect2 Rect2::MakeFromMinsMaxs( const Vec2& mins, const Vec2& maxs )
{
	return Rect2((mins + maxs) * 0.5f, (maxs - mins) * 0.5f);
}

Vec2 Rect2::GetTopLeft()
{
	return Vec2(point1.x, point2.y);
}

Vec2 Rect2::GetTopRight()
{
	return Vec2(point2.x, point2.y);
}

Vec2 Rect2::GetBottomLeft()
{
	return Vec2(point1.x, point1.y);
}

Vec2 Rect2::GetBottomRight()
{
	return Vec2(point2.x, point1.y);
}
