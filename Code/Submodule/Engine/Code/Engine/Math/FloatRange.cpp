#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange( float min, float max )
	: m_min( min )
	, m_max( max )
{
}

void FloatRange::SetFromText( const char* text )
{
	std::vector<std::string> delimString = SplitStringOnDelimiter(text, '~');

	if(delimString.size() == 1 )
	{
		m_min = (float)atof(delimString[0].c_str());
		m_max = (float)atof(delimString[0].c_str());
	}
	else if(delimString.size() == 2 )
	{
		m_min = (float)atof(delimString[0].c_str());
		m_max = (float)atof(delimString[1].c_str());
	}
	else
	{
		ERROR_AND_DIE("FloatRange had an imcompatible value.");
	}
}