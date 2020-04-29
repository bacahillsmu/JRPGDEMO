#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB2.hpp"

std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue )
{
	const char* attribute;
	attribute = element.Attribute(attributeName);

	return attribute ? attribute : defaultValue;
}

std::string ParseXmlAttribute( const XmlElement& element, const char * attributeName, const char * defaultValue )
{
	const char* attribute;
	attribute = element.Attribute(attributeName);

	return attribute ? attribute : defaultValue;
}

int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	return attribute.size() > 0 ? (int)atoi(attribute.c_str()) : defaultValue;
}

char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	return attribute.size() == 1 ? attribute[0] : defaultValue;
}

bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue )
{
	return element.BoolAttribute(attributeName, defaultValue);
}

float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue));
	return attribute.size() > 0 ? (float)atof(attribute.c_str()) : defaultValue;
}

Rgba ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba& defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue.r)  + "," +  std::to_string(defaultValue.g)  + "," +  std::to_string(defaultValue.b)  + "," +  std::to_string(defaultValue.a));
	
	Rgba rgbaAttribute(0.0f, 0.0f, 0.0f, 0.0f);
	rgbaAttribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? rgbaAttribute : defaultValue;
}

Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue.x)  + "," +  std::to_string(defaultValue.y));
	
	Vec2 vec2Attribute(0.0f, 0.0f);
	vec2Attribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? vec2Attribute : defaultValue;
}

Vec3 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue.x)  + "," +  std::to_string(defaultValue.y)  + "," +  std::to_string(defaultValue.z));

	Vec3 vec3Attribute(0.0f, 0.0f, 0.0f);
	vec3Attribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? vec3Attribute : defaultValue;
}

IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue )
{
	std::string attribute = ParseXmlAttribute(element, attributeName, std::to_string(defaultValue.x)  + "," +  std::to_string(defaultValue.y));

	IntVec2 vec2Attribute(0, 0);
	vec2Attribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? vec2Attribute : defaultValue;
}

IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue )
{
	std::string attribute;
	attribute = element.Attribute(attributeName);

	IntRange intRangeAttribute(0, 0);
	intRangeAttribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? intRangeAttribute : defaultValue;
}

FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue )
{
	std::string attribute;
	attribute = element.Attribute(attributeName);

	FloatRange floatRangeAttribute(0.0f, 0.0f);
	floatRangeAttribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? floatRangeAttribute : defaultValue;
}

AABB2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue )
{
	std::string attribute;
	attribute = element.Attribute(attributeName);

	AABB2 aabb2Attribute(Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f));
	aabb2Attribute.SetFromText(attribute.c_str());
	return attribute.size() > 0 ? aabb2Attribute : defaultValue;
}

