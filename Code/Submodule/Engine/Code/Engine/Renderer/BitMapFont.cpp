
// Defines and Includes ---------------------------------------------------------------------------
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"

// ------------------------------------------------------------------
// Fixed Width;
// ------------------------------------------------------------------
BitMapFont::BitMapFont( std::string fontName_, IntVec2 spriteGridLayoutDimensions_)
{
	m_fontName = fontName_;
	m_fontTextureView = g_theRenderer->CreateOrGetTextureViewFromFile(fontName_.c_str());
	m_glyphSpriteSheet = SpriteSheet(m_fontTextureView, spriteGridLayoutDimensions_);

	int spriteSheetSize = spriteGridLayoutDimensions_.x * spriteGridLayoutDimensions_.y;
	for(int i = 0; i < spriteSheetSize; ++i)
	{
		Vec2 mins;
		Vec2 maxs;
		m_glyphSpriteSheet.GetSpriteDefinition(i).GetUVs(mins, maxs);

		GlyphData glyph;
		glyph.m_index = i;
		glyph.m_texCoordsMins = mins;
		glyph.m_texCoordsMaxs = maxs;
		glyph.m_glyphSpriteSheet = &m_glyphSpriteSheet;
		m_glyphData.push_back(glyph);
	}
}

// ------------------------------------------------------------------
// Proportional Width;
// ------------------------------------------------------------------
BitMapFont::BitMapFont(std::string fontName_)
{
	m_fontName = fontName_;
	m_fontTextureView = g_theRenderer->CreateOrGetTextureViewFromFile(fontName_.c_str());

	// Create an Image for this BitMapFont;
	Image image(fontName_.c_str());
	if (image.GetDimensions() == IntVec2(0, 0))
	{
		ERROR_AND_DIE("Unsuccessfully tried to load a bit map font.");
	}

	// We could use the Black Lines to count cells in the x-direction and y-direction...
	// ... or manually count them and know it is 16x16;
	IntVec2 spriteGridLayout = IntVec2(16, 16);

	// Setup the size of the stride for UVs on u and v;
	int xTexelsPerSpriteGrid = image.GetDimensions().x / spriteGridLayout.x;
	int yTexelsPerSpriteGrid = image.GetDimensions().y / spriteGridLayout.y;

	float uPerSpriteGrid = 1.0f / (float)spriteGridLayout.x;
	float uPerTexel = uPerSpriteGrid / (float)xTexelsPerSpriteGrid;

	// Get the area of the grid;
	int numSprites = spriteGridLayout.x * spriteGridLayout.y;
	int spritesPerRow = (int)spriteGridLayout.x;
	int spritePerColumn = (int)spriteGridLayout.y;

	// For each sprite grid, figure out the tight width;
	std::vector<AABB2> texCoordsForEachSprite;
	for (int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++)
	{
		// Get the coordinates on the sprite grid;
		int spriteGridX = spriteIndex % spritesPerRow;
		int spriteGridY = spriteIndex / spritesPerRow;

		// Get the coordinates on the texel grid (of the image);
		int texelX = spriteGridX * xTexelsPerSpriteGrid;
		int texelY = spriteGridY * yTexelsPerSpriteGrid;

		// Push the coords to the bottom right of the sprite grid;
		int pushedTexelX = texelX + xTexelsPerSpriteGrid - 1;
		int pushedTexelY = texelY + yTexelsPerSpriteGrid - 1;
		
		// Find the farthest right texel for the max u;
		int largestXTexelPosition = 0;
		int largestYTexelPosition = 0;
		for(int tIndexX = pushedTexelX - 1; tIndexX >= texelX; --tIndexX)
		{
			for(int tIndexY = pushedTexelY - 1; tIndexY > texelY; --tIndexY)
			{
				Rgba texelColor = image.GetTexelColor(tIndexX, tIndexY);
				if(texelColor.a > 0.0f)
				{
					// We found a colored texel, mark the xTexel;
					if(tIndexX > largestXTexelPosition)
					{
						largestXTexelPosition = tIndexX;
					}
					if(tIndexY > largestYTexelPosition)
					{
						largestYTexelPosition = tIndexY;
					}
				}
			}
		}

		// Make the UVs min and max values;
		// U's;
		float uAtMinX = (float)(spriteGridX) * uPerSpriteGrid;
		float uAtMaxX = uAtMinX + (((float)largestXTexelPosition - (float)texelX) * uPerTexel);

		// V's;
		float vAtMaxY = (float)spriteGridY / (float)spritePerColumn;
		float vAtMinY = vAtMaxY + (1.0f / (float)spritePerColumn);

		// Create Mins/Maxs;
		AABB2 uvBounds = AABB2::MakeFromMinsMaxs(Vec2(uAtMinX, vAtMinY), Vec2(uAtMaxX, vAtMaxY));
		texCoordsForEachSprite.push_back(uvBounds);

		if (spriteIndex == 0)
		{
			m_fontHeight = ((float)largestYTexelPosition - (float)texelY) / (float)spriteGridLayout.y;
		}
	}

	m_glyphSpriteSheet = SpriteSheet(m_fontTextureView, texCoordsForEachSprite);
	int spriteSheetSize = spriteGridLayout.x * spriteGridLayout.y;
	for (int i = 0; i < spriteSheetSize; ++i)
	{
		Vec2 mins;
		Vec2 maxs;
		m_glyphSpriteSheet.GetSpriteDefinition(i).GetUVs(mins, maxs);

		GlyphData glyph;
		glyph.m_index = i;
		glyph.m_texCoordsMins = mins;
		glyph.m_texCoordsMaxs = maxs;
		glyph.m_cellHeightFractionDuringGlyph = texCoordsForEachSprite[i].GetAspect();

		// The space character uses the aspect of the 0th cell;
		if (i == 32)
		{
			glyph.m_cellHeightFractionDuringGlyph = m_glyphData[0].m_cellHeightFractionDuringGlyph;
		}

		glyph.m_glyphSpriteSheet = &m_glyphSpriteSheet;
		m_glyphData.push_back(glyph);
	}
}

// ------------------------------------------------------------------
// Load from .fnt file;
// ------------------------------------------------------------------
BitMapFont::BitMapFont(std::string fontName_, std::string fntFilePath_)
{
	m_fontName = fontName_;
	m_fontTextureView = g_theRenderer->CreateOrGetTextureViewFromFile(fontName_.c_str());
	std::vector<AABB2> texCoordsForEachSprite;
	texCoordsForEachSprite.resize(256);
	m_glyphData.resize(256);

	tinyxml2::XMLDocument fontXMLDoc;
	fontXMLDoc.LoadFile(fntFilePath_.c_str());

	if (fontXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", fntFilePath_.c_str());
		printf("ErrorID:      %i\n", fontXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", fontXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", fontXMLDoc.ErrorName());
		ERROR_AND_DIE("Problem reading .fnt file!");
	}
	else
	{
		printf("Success with XML Doc: %s\n", fntFilePath_.c_str());

		XmlElement* fontInfoElement = fontXMLDoc.RootElement()->FirstChildElement("info");
		XmlElement* fontCommonElement = fontXMLDoc.RootElement()->FirstChildElement("common");
		//XmlElement* fontPagesElement = fontXMLDoc.RootElement()->FirstChildElement("pages");
		XmlElement* fontCharsElement = fontXMLDoc.RootElement()->FirstChildElement("chars");
		//XmlElement* fontKerningsElement = fontXMLDoc.RootElement()->FirstChildElement("kernings");

		// Getting common information about the glyph sheet;
		int scaleW = ParseXmlAttribute(*fontCommonElement, "scaleW", 0);
		int scaleH = ParseXmlAttribute(*fontCommonElement, "scaleH", 0);

		// Reading each char;
		XmlElement* charElement = fontCharsElement->FirstChildElement("char");
		m_fontHeight = ParseXmlAttribute(*fontInfoElement, "size", 0.0f) / (scaleH / 16.0f);
		float actualGlyphHeight = ParseXmlAttribute(*fontInfoElement, "size", 0.0f);
		while(charElement)
		{
			int glyphIndex = ParseXmlAttribute(*charElement, "id", 0);
			float x = ParseXmlAttribute(*charElement, "x", 0.0f);
			float y = ParseXmlAttribute(*charElement, "y", 0.0f);
			float width = ParseXmlAttribute(*charElement, "width", 0.0f);
			//float height = ParseXmlAttribute(*charElement, "height", 0.0f);
			float xoffset = ParseXmlAttribute(*charElement, "xoffset", 0.0f);
			//float yoffset = ParseXmlAttribute(*charElement, "yoffset", 0.0f);
			float xadvance = ParseXmlAttribute(*charElement, "xadvance", 0.0f);

			float minX = x / scaleW;
			float maxX = minX + (width / scaleW);
			float maxY = y / scaleH;			
			float minY = maxY + (actualGlyphHeight / scaleH);
			AABB2 texCoord = AABB2::MakeFromMinsMaxs(Vec2(minX, minY), Vec2(maxX, maxY));
			texCoordsForEachSprite[glyphIndex] = texCoord;

			GlyphData glyph;
			glyph.m_index = glyphIndex;
			glyph.m_texCoordsMins = texCoord.mins;
			glyph.m_texCoordsMaxs = texCoord.maxs;
			glyph.m_cellHeightFractionBeforeGlyph = xoffset / actualGlyphHeight;
			glyph.m_cellHeightFractionDuringGlyph = width / actualGlyphHeight;
			glyph.m_cellHeightFractionAfterGlyph = (xadvance - (fabsf(width) + fabsf(xoffset))) / actualGlyphHeight;
			m_glyphData[glyphIndex] = glyph;

			charElement = charElement->NextSiblingElement();
		}
	}

	m_glyphSpriteSheet = SpriteSheet(m_fontTextureView, texCoordsForEachSprite);
	for(GlyphData& glyph : m_glyphData)
	{
		glyph.m_glyphSpriteSheet = &m_glyphSpriteSheet;
	}
}

// ------------------------------------------------------------------
TextureView* BitMapFont::GetTextureView()
{
	return m_fontTextureView;
}

// ------------------------------------------------------------------
void BitMapFont::AddVertsForText3D( std::vector<Vertex_PCU>& textVerts, const Vec3& textPosition, float cellHeight, const std::string& text, const Rgba& tint )
{
	int numChars = static_cast<int>(text.length());
	
	Vec3 minBounds = textPosition;
	for(int charIndex = 0; charIndex < numChars; charIndex++)
	{
		//Get the UVs
		SpriteDefinition spriteDefinition = m_glyphSpriteSheet.GetSpriteDefinition(text[charIndex]);
		Vec2 minUVs = Vec2(0.0f, 0.0f);
		Vec2 maxUVs = Vec2(0.0f, 0.0f);
		spriteDefinition.GetUVs(minUVs, maxUVs);

		//Make your box
		float textWidth = cellHeight * GetGlyphsAspect(text[charIndex]);
		Vec3 maxBounds = minBounds + Vec3(textWidth, cellHeight, 0.0f);
		AABB2 box = AABB2::MakeFromMinsMaxs(Vec2(minBounds.x, minBounds.y), Vec2(maxBounds.x, maxBounds.y));

		//Now add the verts for each char
		AddVertsForAABB2D(textVerts, box, tint, minUVs, maxUVs);

		//Move the minBounds on x
		minBounds.x += textWidth;
	}
}

// ------------------------------------------------------------------
float BitMapFont::GetGlyphsAspect( int glyphCode ) const
{
	UNUSED(glyphCode);
	return 1.0f;
	// add methodology to get proper aspect
}
