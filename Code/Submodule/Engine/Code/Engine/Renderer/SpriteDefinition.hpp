#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

class SpriteDefinition
{

public:

	SpriteDefinition(){}
	SpriteDefinition(const Vec2& uvAtBottomLeft, const Vec2& uvAtTopRight);
	void GetUVs(Vec2& out_uvAtBottomLeft, Vec2& out_uvAtTopRight) const;
	void SetUVs(Vec2& in_uvAtBottomLeft, Vec2& in_uvAtTopRight);

public:

	Vec2 m_uvAtBottomLeft = Vec2(0.0f, 1.0f);
	Vec2 m_uvAtTopRight = Vec2(1.0f, 1.0f);

	

};