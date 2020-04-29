#pragma once

#include "Engine/Math/Vec2.hpp"

class RandomNumberGenerator
{

public:

	RandomNumberGenerator(unsigned int seed = 0);

	int GetRandomIntLessThan( int maxNotInclusive );
	int GetRandomIntInRange( int minInclusive, int maxInclusive );
	float GetRandomFloatZeroToOne();
	float GetRandomFloatInRange( float minInclusive, float maxInclusive );
	Vec2 GetRandomVec2InRange( Vec2 minInclusive, Vec2 maxInclusive );

	void NewSeed(unsigned int newSeed);
	void JumpToPosition(int newPosition);

	unsigned int GetCurrentPosition();
	bool RandomCoinFlip();

protected:

	unsigned int m_seed = 0;
	unsigned int m_position = 0;
};

extern RandomNumberGenerator* g_theRandomNumberGenerator;