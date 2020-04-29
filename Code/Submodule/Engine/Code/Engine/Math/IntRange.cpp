#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
IntRange::IntRange()
{

}

//-----------------------------------------------------------------------------------------------
IntRange::IntRange( int min, int max )
	: m_min( min )
	, m_max( max )
{
}

//-----------------------------------------------------------------------------------------------
IntRange::IntRange( int minmax )
	:m_min(minmax)
	,m_max(minmax)
{

}

//-----------------------------------------------------------------------------------------------
IntRange::~IntRange()
{
}

//-----------------------------------------------------------------------------------------------
void IntRange::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, '~');

	if(delimString.size() == 1 )
	{
		m_min = (int)atoi(delimString[0].c_str());
		m_max = (int)atoi(delimString[0].c_str());
	}
	else if(delimString.size() == 2 )
	{
		m_min = (int)atoi(delimString[0].c_str());
		m_max = (int)atoi(delimString[1].c_str());
	}
	else
	{
		ERROR_AND_DIE("InRange had an imcompatible value.");
	}
}

//-----------------------------------------------------------------------------------------------
bool IntRange::IsZero()
{
	return m_min != 0 && m_max != 0;
}

