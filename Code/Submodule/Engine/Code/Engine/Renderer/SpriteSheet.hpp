#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

#include <vector>

class TextureView;
struct IntVec2;
class SpriteDefinition;

class SpriteSheet
{

public:

	SpriteSheet(){}

	explicit SpriteSheet(const TextureView* texture, const IntVec2& spriteGridLayout);					// Uniform Layout;
	explicit SpriteSheet(const TextureView* texture_, const std::vector<AABB2>& texCoordsForEachSprite_);	// Non-uniform layout;

	const SpriteDefinition& GetSpriteDefinition(int spriteIndex) const;
	const TextureView& GetTextureView() const;


private:
	const TextureView* m_texture = nullptr;
	std::vector<SpriteDefinition> m_spriteDefinition;
	
	

};