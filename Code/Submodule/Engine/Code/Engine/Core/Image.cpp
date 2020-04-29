#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"

#pragma warning(disable:4100) // unreferenced formal parameter
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ThirdParty/stb/stb_write.h"

Image::Image(const IntVec2& size)
{
	m_dimensions.x = size.x;
	m_dimensions.y = size.y;

	int numTexels = size.x * size.y;

	m_imageRawData = new unsigned char[numTexels * 4];
	m_texels.resize(numTexels);

	for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
	{
		m_texels[texelIndex].SetRgbaBytes(255, 255, 255, 255);
		m_imageRawData[texelIndex] = 255;
		m_imageRawData[texelIndex + 1] = 255;
		m_imageRawData[texelIndex + 2] = 255;
		m_imageRawData[texelIndex + 3] = 255;
	}
}

Image::Image( const IntVec2& size, const Vec3& color )
{
	m_dimensions.x = size.x;
	m_dimensions.y = size.y;
	Vec3 newColor = color * 255.0f;

	int numTexels = size.x * size.y;

	m_imageRawData = new unsigned char[numTexels * 4];
	m_texels.resize(numTexels);

	for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
	{
		m_texels[texelIndex].SetRgbaBytes((unsigned char)newColor.x, (unsigned char)newColor.y, (unsigned char)newColor.z, 255);
		m_imageRawData[texelIndex]		= (unsigned char)newColor.x;
		m_imageRawData[texelIndex + 1]	= (unsigned char)newColor.y;
		m_imageRawData[texelIndex + 2]	= (unsigned char)newColor.z;
		m_imageRawData[texelIndex + 3]	= 255;
	}
}

Image::Image( const IntVec2& size, const Rgba& color )
{
	m_dimensions.x = size.x;
	m_dimensions.y = size.y;
	Rgba newColor = color * 255.0f;

	int numTexels = size.x * size.y;

	m_imageRawData = new unsigned char[numTexels * 4];
	m_texels.resize(numTexels);

	for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
	{
		m_texels[texelIndex].SetRgbaBytes((unsigned char)newColor.r, (unsigned char)newColor.g, (unsigned char)newColor.b, (unsigned char)newColor.a);
		m_imageRawData[texelIndex]		= (unsigned char)newColor.r;
		m_imageRawData[texelIndex + 1]	= (unsigned char)newColor.g;
		m_imageRawData[texelIndex + 2]	= (unsigned char)newColor.b;
		m_imageRawData[texelIndex + 3]	= (unsigned char)newColor.a;
	}
}

Image::Image( const char* imageFilePath )
	:m_imageFilePath(imageFilePath)
{
	int imageTexelSizeX = 0; // Filled in for us to indicate image width
	int imageTexelSizeY = 0; // Filled in for us to indicate image height
	int numComponents = 0;   // Filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	//stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* data = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
	m_imageRawData = new unsigned char[imageTexelSizeX * imageTexelSizeY * 4];
	
	m_dimensions.x = imageTexelSizeX;
	m_dimensions.y = imageTexelSizeY;

	int numTexels = imageTexelSizeX * imageTexelSizeY;
	m_texels.resize(numTexels);

	if(numComponents == 3)
	{		
		for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
		{

			int redByteIndex = (texelIndex * numComponents) + 0;
			int greenByteIndex = (texelIndex * numComponents) + 1;
			int blueByteIndex = (texelIndex * numComponents) + 2;

			unsigned char redByte = data[redByteIndex];
			unsigned char greenByte = data[greenByteIndex];
			unsigned char blueByte = data[blueByteIndex];

			m_texels[texelIndex].SetRgbaBytes(redByte, greenByte, blueByte);
			m_imageRawData[texelIndex * 4] = redByte;
			m_imageRawData[texelIndex * 4 + 1] = greenByte;
			m_imageRawData[texelIndex * 4 + 2] = blueByte;
			m_imageRawData[texelIndex * 4 + 3] = 255;
		}
	}
	else if(numComponents == 4)
	{
		for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
		{
			int redByteIndex = (texelIndex * numComponents) + 0;
			int greenByteIndex = (texelIndex * numComponents) + 1;
			int blueByteIndex = (texelIndex * numComponents) + 2;
			int alphaByteIndex = (texelIndex * numComponents) + 3;

			unsigned char redByte = data[redByteIndex];
			unsigned char greenByte = data[greenByteIndex];
			unsigned char blueByte = data[blueByteIndex];
			unsigned char alphaByte = data[alphaByteIndex];

			// this needs to be in rgba hpp/cpp
			m_texels[texelIndex].SetRgbaBytes(redByte, greenByte, blueByte, alphaByte); 
			m_imageRawData[texelIndex * numComponents] = redByte;
			m_imageRawData[texelIndex * numComponents + 1] = greenByte;
			m_imageRawData[texelIndex * numComponents + 2] = blueByte;
			m_imageRawData[texelIndex * numComponents + 3] = alphaByte;
		}
	}
	else
	{
		ERROR_AND_DIE("Could not read image.");
	}

	STBI_FREE(data);
}





Image::~Image()
{
	delete m_imageRawData;
	m_imageRawData = nullptr;
}

const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

const Rgba& Image::GetTexelColor( int texelX, int texelY ) const
{
	int texelColorIndex = texelX + texelY * m_dimensions.x;
	return m_texels[texelColorIndex];
}

const Rgba& Image::GetTexelColor( const IntVec2& texelCoords ) const
{
	return GetTexelColor(texelCoords.x, texelCoords.y);
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

uint Image::GetBytesPerPixel() const
{
	return 4;
}

unsigned char* Image::GetImageBuffer() const
{
	return m_imageRawData;
}

bool Image::SaveImageToDisc(char const *filename)
{	
	int returnvalue = stbi_write_png(filename, m_dimensions.x, m_dimensions.y, 4, m_imageRawData, m_dimensions.x * 4);

	return returnvalue == 1;
}

