#pragma once
#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"


struct Rect2
{

public:

	Vec2 point1;
	Vec2 point2;
	Vec2 center;
	Vec2 offset;

public:

	explicit Rect2( const Vec2& initialCenter, const Vec2& initialOffset );
	
	static Rect2 MakeFromMinsMaxs(const Vec2& mins, const Vec2& maxs);

	Vec2 GetTopLeft();
	Vec2 GetTopRight();
	Vec2 GetBottomLeft();
	Vec2 GetBottomRight();

};

//-----------------------------------------------------------------------------------------------
struct AABB2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	Vec2 mins;
	Vec2 maxs;
	Vec2 center;
	Vec2 offset;

public:
	// Construction/Destruction
	AABB2() {}												// default constructor: do nothing (for speed)
	~AABB2() {}												// destructor: do nothing (for speed)
	explicit AABB2( const Vec2& initialCenter, const Vec2& initialOffset );		// explicit constructor (from x, y)
	
	void SetFromText(const char* text);
	static AABB2 MakeFromMinsMaxs(const Vec2& mins, const Vec2& maxs);
	
	Vec2 GetTopLeft();
	Vec2 GetTopRight();
	Vec2 GetBottomLeft();
	Vec2 GetBottomRight();

	bool IsPointInside(Vec2 point);

	float GetAspect();

	//ispointinside
	//stretchtoincludepoint
	//translate

	//getcenter
	//addpadding
	//addpaddingXY
	//getscaletofixboxwithinboxb(textSize, boxSize)
};




