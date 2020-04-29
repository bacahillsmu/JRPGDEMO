#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <vector>

typedef unsigned int uint;

struct Rgba;

class Image
{
public:
	Image(const IntVec2& size);
	Image(const IntVec2& size, const Vec3& color);
	Image(const IntVec2& size, const Rgba& color);
	Image(const char* imageFilePath);
	~Image();

	const std::string& GetImageFilePath() const;
	const Rgba& GetTexelColor(int texelX, int texelY) const;
	const Rgba& GetTexelColor(const IntVec2& texelCoords) const;
	IntVec2 GetDimensions() const;
	uint GetBytesPerPixel() const;
	unsigned char* GetImageBuffer() const;

	bool SaveImageToDisc(char const *filename);



public:

	unsigned char* m_imageRawData = nullptr;	

private:
	std::string m_imageFilePath;
	IntVec2 m_dimensions = IntVec2(0, 0);
	std::vector<Rgba> m_texels;
	

};