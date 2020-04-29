#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <ctime>

RandomNumberGenerator* g_theRandomNumberGenerator = nullptr;

//-----------------------------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator( unsigned int seed )
	: m_seed(seed)
	, m_position(0)
{
}

//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntLessThan( int maxNotInclusive )
{
	unsigned int randomUint = Get1dNoiseUint(m_position, m_seed);
	m_position++;

	return randomUint % maxNotInclusive;
}

//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntInRange( int minInclusive, int maxInclusive )
{
	unsigned int randomUint = Get1dNoiseUint(m_position, m_seed);
	m_position++;

	return randomUint%((maxInclusive - minInclusive) + 1) + minInclusive; 
}

//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
	float randomUint = Get1dNoiseZeroToOne(m_position, m_seed);
	m_position++;

	return randomUint;
}

//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatInRange( float minInclusive, float maxInclusive )
{
	unsigned int randomUint = GetRandomIntInRange(0, (int)RAND_MAX);
	m_position++;

	return minInclusive + float(randomUint) / (float(RAND_MAX / (maxInclusive - minInclusive)));
}

//-----------------------------------------------------------------------------------------------
Vec2 RandomNumberGenerator::GetRandomVec2InRange(Vec2 minInclusive, Vec2 maxInclusive)
{
	float randomXpoint = GetRandomFloatInRange(minInclusive.x, maxInclusive.x);
	float randomYpoint = GetRandomFloatInRange(minInclusive.y, maxInclusive.y);

	return Vec2(randomXpoint, randomYpoint);
}

//-----------------------------------------------------------------------------------------------
void RandomNumberGenerator::NewSeed( unsigned int newSeed )
{
	m_seed = newSeed;
	m_position = 0;
}

//-----------------------------------------------------------------------------------------------
void RandomNumberGenerator::JumpToPosition( int newPosition )
{
	m_position = newPosition;
}

//-----------------------------------------------------------------------------------------------
bool RandomNumberGenerator::RandomCoinFlip()
{
	return GetRandomFloatZeroToOne() > 0.5f ? true : false;
}

unsigned int RandomNumberGenerator::GetCurrentPosition()
{
	return m_position;
}
