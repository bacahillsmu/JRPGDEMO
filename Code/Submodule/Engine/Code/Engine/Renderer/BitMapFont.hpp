#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"


#include <vector>

class TextureView;
struct Vertex_PCU;

struct GlyphData
{
	int m_index = -1;
	SpriteSheet* m_glyphSpriteSheet = nullptr;
	Vec2 m_texCoordsMins = Vec2::ZERO;
	Vec2 m_texCoordsMaxs = Vec2::ONE;
	float m_cellHeightFractionBeforeGlyph = 0.0f;	// A in TTF ABC terminology;
	float m_cellHeightFractionDuringGlyph = 1.0f;	// B;
	float m_cellHeightFractionAfterGlyph  = 0.0f;	// C;
};

class BitMapFont
{
	friend class RenderContext;

private:

	BitMapFont(std::string fontName_, IntVec2 spriteGridLayoutDimensions_);
	BitMapFont(std::string fontName_);
	BitMapFont(std::string fontName_, std::string fntFilePath_);

public:
	TextureView* GetTextureView();
	
	template<typename ...Types>
	void AddVertsForText2D(std::vector<Vertex_PCU>& textVerts, 
						   const Vec2& textPosition, 
						   float displayHeight,
						   const std::string& text, 
						   const Rgba& tint = Rgba::WHITE, 
						   Types... args)
	{
		// Calculating apparent font height;
		displayHeight /= m_fontHeight;

		std::string newText = "";
		newText = Stringf(text.c_str(), args...);
		
		int numChars = static_cast<int>(newText.length());
		Vec2 minBounds = textPosition;
		for(int charIndex = 0; charIndex < numChars; charIndex++)
		{
			// Get the Glyph information;
			int index = newText[charIndex];
			GlyphData glyph = m_glyphData[index];

			// Get the UVs of the glyph;
			Vec2 mins = glyph.m_texCoordsMins;
			Vec2 maxs = glyph.m_texCoordsMaxs;

			// Get the A, B, C values of the glyph;
			float before = displayHeight * glyph.m_cellHeightFractionBeforeGlyph;
			float during = displayHeight * glyph.m_cellHeightFractionDuringGlyph;
			float after = displayHeight * glyph.m_cellHeightFractionAfterGlyph;

			// Move the minBounds on X by A;
			minBounds.x += before;

			// Create the bounding box for this Glyph;			
			AABB2 glyphBoundingBox = AABB2::MakeFromMinsMaxs(minBounds, (minBounds + Vec2(during, displayHeight)));

			// Move the minBounds on X by B;
			minBounds.x += during;

			// Add the verts of glyph bounding box to the textVerts;
			AddVertsForAABB2D(textVerts, glyphBoundingBox, tint, mins, maxs);

			// Move the minBounds on X by C;
			minBounds.x += after;
		}
	}
	
	void AddVertsForText3D(std::vector<Vertex_PCU>& textVerts, const Vec3& textPosition, float cellHeight,
		const std::string& text, const Rgba& tint = Rgba::WHITE/*, float cellAspect = 1.0f*/);
	

public:

	float GetGlyphsAspect(int glyphCode) const;

	std::string m_fontName;
	TextureView* m_fontTextureView = nullptr;
	SpriteSheet m_glyphSpriteSheet;
	std::vector<GlyphData> m_glyphData;
	float m_fontHeight = 1.0f;
	
	
};