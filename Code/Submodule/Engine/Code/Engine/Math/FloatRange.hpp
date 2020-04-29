#pragma once
#include "MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
struct FloatRange
{
public:
	float m_min;
	float m_max;

public:
	// Construction/Destruction
	FloatRange(float min, float max);
	~FloatRange() {}
	
	void SetFromText(const char* text);

};


