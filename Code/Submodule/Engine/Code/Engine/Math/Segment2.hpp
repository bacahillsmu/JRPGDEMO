#pragma once
#include "Engine/Core/EngineCommon.hpp"


class Segment2
{

public:

	Segment2() {}
	Segment2(Vec2 start, Vec2 end);

	inline const Vec2& GetStart() const		{ return m_start; }
	inline const Vec2& GetEnd() const		{ return m_end; }
	Vec2 GetCenter() const;
	Vec2 GetDirection() const;

	// Return the closest point on line segment.
	// Voronoi regions. Either on the segment, or one of the end points
	Vec2 GetClosestPoint(Vec2 position) const;


public:

	Vec2 m_start;
	Vec2 m_end;

public:

	static Segment2 CreateSegment( Vec2 start, Vec2 end );

};

