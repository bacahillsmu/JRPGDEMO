#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec3.hpp"

#define UNUSED(x) (void)(x);

AABB3::AABB3( const Vec3& initialCenter, const Vec3& initialOffset )
	: center(initialCenter)
	, offset(initialOffset)
{
	mins.x = center.x - offset.x;
	mins.y = center.y - offset.y;
	mins.z = center.z - offset.z;

	maxs.x = center.x + offset.x;
	maxs.y = center.y + offset.y;
	maxs.z = center.z + offset.z;
}

void AABB3::SetFromText( const char* text )
{
	UNUSED(text);
	ERROR_AND_DIE("Cannot read AABB3 from text because of the change in Constructor to take a center and offset rather than min/max vecs.");
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

AABB3 AABB3::MakeFromMinsMaxs( const Vec3& mins, const Vec3& maxs )
{
	return AABB3((mins + maxs)/2, (maxs - mins)/ 2);
}

Vec3 AABB3::GetFrontBottomLeft()
{
	Vec3 FBL = center;
	FBL.x -= offset.x;
	FBL.y -= offset.y;
	FBL.z -= offset.z;

	return FBL;
}

Vec3 AABB3::GetFrontBottomRight()
{
	Vec3 FBL = center;
	FBL.x += offset.x;
	FBL.y -= offset.y;
	FBL.z -= offset.z;

	return FBL;
}

Vec3 AABB3::GetFrontTopLeft()
{
	Vec3 FBL = center;
	FBL.x -= offset.x;
	FBL.y += offset.y;
	FBL.z -= offset.z;

	return FBL;
}

Vec3 AABB3::GetFrontTopRight()
{
	Vec3 FBL = center;
	FBL.x += offset.x;
	FBL.y += offset.y;
	FBL.z -= offset.z;

	return FBL;
}

Vec3 AABB3::GetBackBottomLeft()
{
	Vec3 FBL = center;
	FBL.x -= offset.x;
	FBL.y -= offset.y;
	FBL.z += offset.z;

	return FBL;
}

Vec3 AABB3::GetBackBottomRight()
{
	Vec3 FBL = center;
	FBL.x += offset.x;
	FBL.y -= offset.y;
	FBL.z += offset.z;

	return FBL;
}

Vec3 AABB3::GetBackTopLeft()
{
	Vec3 FBL = center;
	FBL.x -= offset.x;
	FBL.y += offset.y;
	FBL.z += offset.z;

	return FBL;
}

Vec3 AABB3::GetBackTopRight()
{
	Vec3 FBL = center;
	FBL.x += offset.x;
	FBL.y += offset.y;
	FBL.z += offset.z;

	return FBL;
}

void AABB3::GetCorners(Vec3 out[8])
{
	//    6-----7
	//   /|    /|
	//  2-----3 |
	//  | 4---|-5
	//  |/    |/
	//  0-----1

	out[0] = GetFrontBottomLeft();
	out[1] = GetFrontBottomRight();
	out[2] = GetFrontTopLeft();
	out[3] = GetFrontTopRight();
	out[4] = GetBackBottomLeft();
	out[5] = GetBackBottomRight();
	out[6] = GetBackTopLeft();
	out[7] = GetBackTopRight();
}

