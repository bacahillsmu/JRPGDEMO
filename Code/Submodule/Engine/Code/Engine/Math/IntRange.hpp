#pragma once
#include "MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
struct IntRange
{
public:
	int m_min;
	int m_max;

public:
	// Construction/Destruction
	IntRange();
	IntRange(int minmax);
	IntRange(int min, int max);
	~IntRange();
	
	void SetFromText(const char* text);
	bool IsZero();
};


