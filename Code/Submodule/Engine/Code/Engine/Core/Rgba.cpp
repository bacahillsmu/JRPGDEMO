#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

void Rgba::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, ',');

	if(delimString.size() == 3 )
	{
		r = ((float)atof(delimString[0].c_str()) / 255.0f);
		g = ((float)atof(delimString[1].c_str()) / 255.0f);
		b = ((float)atof(delimString[2].c_str()) / 255.0f);
		a = 1.0f;
	}
	else if(delimString.size() == 4 )
	{
		r = ((float)atof(delimString[0].c_str()) / 255.0f);
		g = ((float)atof(delimString[1].c_str()) / 255.0f);
		b = ((float)atof(delimString[2].c_str()) / 255.0f);
		a = ((float)atof(delimString[3].c_str()) / 255.0f);
	}
	else
	{
		ERROR_AND_DIE("Rgba had an imcompatible value.");
	}
}

void Rgba::SetRgbaBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte /*= 255*/ )
{
	r = (float)redByte / 255.0f;
	g = (float)greenByte / 255.0f;
	b = (float)blueByte / 255.0f;
	a = (float)alphaByte / 255.0f;

}

const Rgba Rgba::operator+( const Rgba& vecToAdd ) const
{
	return Rgba( r + vecToAdd.r, 
		g + vecToAdd.g,
		b + vecToAdd.b,
		a + vecToAdd.a);
}

const Rgba Rgba::operator-( const Rgba& rgbaToSubtract ) const
{
	return Rgba( r - rgbaToSubtract.r, 
		g - rgbaToSubtract.g,
		b - rgbaToSubtract.b,
		a - rgbaToSubtract.a);
}

const Rgba Rgba::operator*( float uniformScale ) const
{
	return Rgba( r * uniformScale, 
		g * uniformScale,
		b * uniformScale,
		a * uniformScale);
}

void Rgba::operator*=( const float uniformScale )
{
	r = r * uniformScale;
	g = g * uniformScale;
	b = b * uniformScale;
	a = a * uniformScale;
}

bool Rgba::operator==( Rgba& colorToCheck )
{
	if(r == colorToCheck.r
		&& g == colorToCheck.g 
		&& b == colorToCheck.b 
		&& a == colorToCheck.a)
	{
		return true;
	}

	return false;
}

bool Rgba::operator!=( Rgba& colorToCheck )
{
	if(r == colorToCheck.r
		&& g == colorToCheck.g 
		&& b == colorToCheck.b 
		&& a == colorToCheck.a)
	{
		return false;
	}

	return true;
}

const Rgba operator*( float uniformScale, const Rgba& vecToScale )
{
	return Rgba( vecToScale.r * uniformScale,
		vecToScale.g * uniformScale,
		vecToScale.b * uniformScale,
		vecToScale.a * uniformScale);
}

// Predefined Colors
Rgba Rgba::WHITE			= Rgba(1.0f, 1.0f, 1.0f, 1.0f);
Rgba Rgba::BLACK			= Rgba(0.0f, 0.0f, 0.0f, 1.0f);
Rgba Rgba::GRAY				= Rgba(0.5f, 0.5f, 0.5f, 1.0f);
Rgba Rgba::RED				= Rgba(1.0f, 0.0f, 0.0f, 1.0f);
Rgba Rgba::GREEN			= Rgba(0.0f, 1.0f, 0.0f, 1.0f);
Rgba Rgba::BLUE				= Rgba(0.0f, 0.0f, 1.0f, 1.0f);
Rgba Rgba::MAGENTA			= Rgba(1.0f, 0.0f, 1.0f, 1.0f);
Rgba Rgba::YELLOW			= Rgba(1.0f, 1.0f, 0.0f, 1.0f);
Rgba Rgba::TEAL				= Rgba(0.0f, 1.0f, 1.0f, 1.0f);
Rgba Rgba::BROWN			= Rgba(0.65f, 0.17f, 0.17f, 1.0f);
Rgba Rgba::FFBLUE			= Rgba(0.188f, 0.188f, 0.878f, 1.0f);

Rgba Rgba::THREEQUART_FFBLUE = Rgba(0.188f, 0.188f, 0.878f, 0.75f);

Rgba Rgba::HALF_WHITE		= Rgba(1.0f, 1.0f, 1.0f, 0.5f);
Rgba Rgba::HALF_BLACK		= Rgba(0.0f, 0.0f, 0.0f, 0.5f);
Rgba Rgba::HALF_GRAY		= Rgba(0.5f, 0.5f, 0.5f, 0.5f);
Rgba Rgba::HALF_RED			= Rgba(1.0f, 0.0f, 0.0f, 0.5f);
Rgba Rgba::HALF_GREEN		= Rgba(0.0f, 1.0f, 0.0f, 0.5f);
Rgba Rgba::HALF_BLUE		= Rgba(0.0f, 0.0f, 1.0f, 0.5f);
Rgba Rgba::HALF_MAGENTA		= Rgba(1.0f, 0.0f, 1.0f, 0.5f);
Rgba Rgba::HALF_YELLOW		= Rgba(1.0f, 1.0f, 0.0f, 0.5f);
Rgba Rgba::HALF_TEAL		= Rgba(0.0f, 1.0f, 1.0f, 0.5f);
Rgba Rgba::HALF_BROWN		= Rgba(0.65f, 0.17f, 0.17f, 0.5f);
Rgba Rgba::HALF_FFBLUE		= Rgba(0.188f, 0.188f, 0.878f, 0.5f);

Rgba Rgba::QUART_WHITE		= Rgba(1.0f, 1.0f, 1.0f, 0.25f);
Rgba Rgba::QUART_BLACK		= Rgba(0.0f, 0.0f, 0.0f, 0.25f);
Rgba Rgba::QUART_GRAY		= Rgba(0.5f, 0.5f, 0.5f, 0.25f);
Rgba Rgba::QUART_RED		= Rgba(1.0f, 0.0f, 0.0f, 0.25f);
Rgba Rgba::QUART_GREEN		= Rgba(0.0f, 1.0f, 0.0f, 0.25f);
Rgba Rgba::QUART_BLUE		= Rgba(0.0f, 0.0f, 1.0f, 0.25f);
Rgba Rgba::QUART_MAGENTA	= Rgba(1.0f, 0.0f, 1.0f, 0.25f);
Rgba Rgba::QUART_YELLOW		= Rgba(1.0f, 1.0f, 0.0f, 0.25f);
Rgba Rgba::QUART_TEAL		= Rgba(0.0f, 1.0f, 1.0f, 0.25f);
Rgba Rgba::QUART_BROWN		= Rgba(0.65f, 0.17f, 0.17f, 0.25f);