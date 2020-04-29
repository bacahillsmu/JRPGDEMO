
// Defines and Includes ---------------------------------------------------------------------------
#include "Engine/Renderer/SpriteDefinition.hpp"

SpriteDefinition::SpriteDefinition( const Vec2& uvAtBottomLeft, const Vec2& uvAtTopRight )
	:m_uvAtBottomLeft(uvAtBottomLeft)
	,m_uvAtTopRight(uvAtTopRight)
{
}

void SpriteDefinition::SetUVs( Vec2& in_uvAtBottomLeft, Vec2& in_uvAtTopRight )
{
	m_uvAtBottomLeft = in_uvAtBottomLeft;
	m_uvAtTopRight = in_uvAtTopRight;
}

void SpriteDefinition::GetUVs( Vec2& out_uvAtBottomLeft, Vec2& out_uvAtTopRight ) const
{
	out_uvAtBottomLeft = m_uvAtBottomLeft;
	out_uvAtTopRight = m_uvAtTopRight;
}
