
//-----------------------------------------------------------------------------------------------
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"

NamedStrings::NamedStrings()
{

}

NamedStrings::~NamedStrings()
{

}

void NamedStrings::PopulateFromXmlElementAttributes( const XmlElement& element )
 {
 	const XmlAttribute* attributeElement = element.FirstAttribute();
	while(attributeElement)
	{
		std::string attributeName = attributeElement->Name();
		std::string attributeValue = attributeElement->Value();
		SetValue(attributeName, attributeValue);

		attributeElement = attributeElement->Next();
	}
 }

void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);
	
	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}	
	
	std::string mapPairBool = mapPair->second;
	if(mapPairBool == "true" || mapPairBool == "True")
	{
		return true;
	}
	else if(mapPairBool == "false" || mapPairBool == "False")
	{
		return false;
	}
	else
	{
		return false;
	}
}

int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	int mapPairInt = atoi(mapPair->second.c_str());
	return mapPairInt;
}

float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	float mapPairFloat = (float)atof(mapPair->second.c_str());
	return mapPairFloat;
}

std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	return mapPair->second;
}

std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	return mapPair->second;
}

Rgba NamedStrings::GetValue( const std::string& keyName, Rgba& defaultValue )
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Rgba mapPairRgba;
	mapPairRgba.SetFromText(mapPair->second.c_str());
	return mapPairRgba;
}

Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Vec2 mapPairVec2;
	mapPairVec2.SetFromText(mapPair->second.c_str());
	return mapPairVec2;
}


Vec3 NamedStrings::GetValue( const std::string& keyName, const Vec3& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Vec3 mapPairVec3;
	mapPairVec3.SetFromText(mapPair->second.c_str());
	return mapPairVec3;
}


IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	IntVec2 mapPairIntVec2;
	mapPairIntVec2.SetFromText(mapPair->second.c_str());
	return mapPairIntVec2;
}

FloatRange NamedStrings::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	FloatRange mapPairFloatRange(0.0f, 0.0f);
	mapPairFloatRange.SetFromText(mapPair->second.c_str());
	return mapPairFloatRange;
}
 
IntRange NamedStrings::GetValue( const std::string& keyName, const IntRange& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator mapPair = m_keyValuePairs.find(keyName);

	if(mapPair == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	IntRange mapPairIntRange(0, 0);
	mapPairIntRange.SetFromText(mapPair->second.c_str());
	return mapPairIntRange;
}

