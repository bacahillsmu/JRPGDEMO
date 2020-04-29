#include "Engine/Core/NamedProperties.hpp"


// -----------------------------------------------------------------------
// FromString
// -----------------------------------------------------------------------
template <>
float FromString( char const *str, float const &def )
{
	char * e;
	errno = 0;
	float x = std::strtof(str, &e);

	if (*e != '\0' || errno != 0)
	{
		return def;
	}
	else
	{
		return x;
	}

}

template <>
int FromString( char const *str, int const &def )
{
	int x = atoi(str);
	if(x == 0 && str[0] != '0')
	{
		return def;
	}
	else
	{
		return x;
	}
}

template <>
bool FromString( char const *str, bool const &def )
{
	if(std::strcmp(str, "true") == 0)
	{
		return true;
	}
	else if(std::strcmp(str, "false") == 0)
	{
		return false;
	}
	else
	{
		return def;
	}
}

template <>
std::string FromString( char const *str, std::string const &def )
{
	if(str != nullptr)
	{
		return std::string(str);
	}
	else
	{
		return def;
	}
}

// -----------------------------------------------------------------------
// ToString
// -----------------------------------------------------------------------
template <>
std::string ToString( const float& value )
{
	return std::to_string(value);
}

template <>
std::string ToString( const int& value )
{
	return std::to_string(value);
}

template <>
std::string ToString( const bool& value )
{
	if(value)
	{
		return std::string("true");
	}
	else
	{
		return std::string("false");
	}
}

template <>
std::string ToString( const std::string& value )
{
	return value;
}




