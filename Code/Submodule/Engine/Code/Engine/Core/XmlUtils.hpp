#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.hpp"

#include <string>

struct Rgba;
struct Vec2;
struct Vec3;
struct IntRange;
struct FloatRange;
struct IntVec2;
struct AABB2;

typedef tinyxml2::XMLElement XmlElement;
typedef tinyxml2::XMLAttribute XmlAttribute;
typedef tinyxml2::XMLNode XmlNode;

std::string ParseXmlAttribute(const XmlElement& element, const char* attributeName, const std::string& defaultValue);
std::string ParseXmlAttribute(const XmlElement& element, const char* attributeName, const char* defaultValue = nullptr);

int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue );
char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue );
Rgba ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba& defaultValue );
Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue );
Vec3 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue );
IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue );
IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue );
AABB2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue );



