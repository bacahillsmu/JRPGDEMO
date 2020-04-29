#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
struct Ring
{
public:
	Vec2 ringCenter;
	float ringRadius;
	float ringThickness;

public:
	
	Ring() {}
	~Ring() {}
	explicit Ring( Vec2& initialRingCenter, float initialRingRadius, float initialRingThickness );

	void SetFromText(const char* text);
};

