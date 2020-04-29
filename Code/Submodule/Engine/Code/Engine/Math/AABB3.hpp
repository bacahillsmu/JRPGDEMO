#pragma once
#include "MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"

//-----------------------------------------------------------------------------------------------
struct AABB3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	Vec3 mins;
	Vec3 maxs;
	Vec3 center;
	Vec3 offset;

public:
	// Construction/Destruction
	AABB3() {}												// default constructor: do nothing (for speed)
	~AABB3() {}												// destructor: do nothing (for speed)
	explicit AABB3( const Vec3& initialCenter, const Vec3& initialOffset );		// explicit constructor (from x, y)
	
	void SetFromText(const char* text);
	static AABB3 MakeFromMinsMaxs(const Vec3& mins, const Vec3& maxs);

	// Corners
	Vec3 GetFrontBottomLeft();
	Vec3 GetFrontBottomRight();
	Vec3 GetFrontTopLeft();
	Vec3 GetFrontTopRight();
	Vec3 GetBackBottomLeft();
	Vec3 GetBackBottomRight();
	Vec3 GetBackTopLeft();
	Vec3 GetBackTopRight();

	void GetCorners(Vec3 out[8]);


};




