#pragma once

struct Rgba
{

public:
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;


public:
	// Construction/Destruction
	Rgba(){}
	~Rgba(){}

	explicit Rgba( float initialR, float initialG, float initialB, float initialA = 1.0f)
		: r( initialR )
		, g( initialG )
		, b( initialB )
		, a( initialA )
	{
	};

	void SetFromText(const char* text);
	void SetRgbaBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);

	const Rgba operator+( const Rgba& vecToAdd ) const;
	const Rgba operator-( const Rgba& vecToSubtract ) const;	// Vec4 - Vec4
	const Rgba operator*( float uniformScale ) const;			// Vec4 * float
	void operator*=( const float uniformScale );				// Vec4 *= float
	bool operator==( Rgba& colorToCheck );				// Vec4 *= float
	bool operator!=( Rgba& colorToCheck );				// Vec4 *= float
	
	friend const Rgba operator*( float uniformScale, const Rgba& vecToScale );	// float * Vec4

	// Predefined Colors
	static Rgba WHITE;
	static Rgba BLACK;
	static Rgba GRAY;
	static Rgba RED;
	static Rgba GREEN;
	static Rgba BLUE;
	static Rgba MAGENTA;
	static Rgba YELLOW;
	static Rgba TEAL;
	static Rgba BROWN;
	static Rgba FFBLUE;

	static Rgba THREEQUART_FFBLUE;

	static Rgba HALF_WHITE;
	static Rgba HALF_BLACK;
	static Rgba HALF_GRAY;
	static Rgba HALF_RED;
	static Rgba HALF_GREEN;
	static Rgba HALF_BLUE;
	static Rgba HALF_MAGENTA;
	static Rgba HALF_YELLOW;
	static Rgba HALF_TEAL;
	static Rgba HALF_BROWN;
	static Rgba HALF_FFBLUE;

	static Rgba QUART_WHITE;
	static Rgba QUART_BLACK;
	static Rgba QUART_GRAY;
	static Rgba QUART_RED;
	static Rgba QUART_GREEN;
	static Rgba QUART_BLUE;
	static Rgba QUART_MAGENTA;
	static Rgba QUART_YELLOW;
	static Rgba QUART_TEAL;
	static Rgba QUART_BROWN;
};


