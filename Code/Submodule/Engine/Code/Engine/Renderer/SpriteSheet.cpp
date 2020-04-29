
// Defines and Includes ---------------------------------------------
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"


// ------------------------------------------------------------------
SpriteSheet::SpriteSheet( const TextureView* texture, const IntVec2& spriteGridLayout )
	:m_texture(texture)
{
		//Setup the size of the stride for UVs on u and v
		float uPerSpriteGrid = 1.0f / static_cast<float>(spriteGridLayout.x);

		//Get the area of the grid
		int numSprites = spriteGridLayout.x * spriteGridLayout.y;
		const int& spritesPerRow = spriteGridLayout.x;

		for(int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++)
		{
			//Get the coordinates on the sprite grid
			int spriteGridX = spriteIndex % spritesPerRow;
			int spriteGridY = spriteIndex / spritesPerRow;

			//Make the UVs min and max values
			float uAtMinX = static_cast<float>(spriteGridX) * uPerSpriteGrid;
			float uAtMaxX = uAtMinX + uPerSpriteGrid;

			float vAtMaxY = (float)spriteGridY / (float)spriteGridLayout.y;
			float vAtMinY = vAtMaxY + (1.0f / (float)spriteGridLayout.y);

			Vec2 minUV = Vec2(uAtMinX + 0.0001f, vAtMinY - 0.0001f);
			Vec2 maxUV = Vec2(uAtMaxX - 0.0001f, vAtMaxY + 0.0001f);

			m_spriteDefinition.push_back(SpriteDefinition(minUV, maxUV));
		}
}

// ------------------------------------------------------------------
SpriteSheet::SpriteSheet(const TextureView* texture_, const std::vector<AABB2>& texCoordsForEachSprite_)
{
	m_texture = texture_;

	int spriteCount = (int)texCoordsForEachSprite_.size();
	for (int spriteIndex = 0; spriteIndex < spriteCount; ++spriteIndex)
	{
		m_spriteDefinition.push_back(SpriteDefinition(texCoordsForEachSprite_[spriteIndex].mins, texCoordsForEachSprite_[spriteIndex].maxs));
	}
}

// ------------------------------------------------------------------
const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_spriteDefinition[spriteIndex];
}

// ------------------------------------------------------------------
const TextureView& SpriteSheet::GetTextureView() const
{
	return *m_texture;
}


