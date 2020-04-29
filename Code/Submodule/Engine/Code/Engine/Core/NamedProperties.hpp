#pragma once
#include "Engine/Core/XmlUtils.hpp"

#include <string>
#include <map>

class BaseProperty
{

public:

	virtual std::string ToString() const = 0;
};

// -----------------------------------------------------------------------
// FromString
// -----------------------------------------------------------------------
template <typename T>
T FromString( char const *str, const T& def )
{
	if(str != nullptr)
	{
		return T(str);
	}
	else
	{
		return def;
	}
}

template <>
float FromString( char const *str, float const &def );

template <>
int FromString( char const *str, int const &def );

template <>
bool FromString( char const *str, bool const &def );

template <>
std::string FromString( char const *str, std::string const &def );

// -----------------------------------------------------------------------
// ToString
// -----------------------------------------------------------------------
template <typename T>
std::string ToString( const T& value )
{
	return value.GetAsString();
}

template <>
std::string ToString( const float& value );

template <>
std::string ToString( const int& value );

template <>
std::string ToString( const bool& value );

template <>
std::string ToString( const std::string& value );

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
template <typename T>
class TypedProperty : public BaseProperty
{

public:

	TypedProperty(const T& value)
		: m_value(value)
	{}

	virtual std::string ToString() const override
	{
		return ::ToString(m_value);
	}

public:

	T m_value;

};

// std::string ToString(void const* ptr)
// {
// 	return "";
// }

class NamedProperties
{

public:

	template <typename T>
	T GetValue(const std::string& key, const T& def)
	{
		auto ew = m_properties.find(key);
		if(ew == m_properties.end())
		{
			return def;
		}

		BaseProperty* property = ew->second;

		TypedProperty<T>* typed_prop = dynamic_cast<TypedProperty<T>*>(property);
		if(typed_prop == nullptr)
		{
			std::string str = property->ToString();
			return FromString(str.c_str(), def);
		}
		else
		{
			return typed_prop->m_value;
		}
	}

	template <typename T>
	void SetValue(const std::string& key, const T& val)
	{
		TypedProperty<T>* property = new TypedProperty<T>(val);

		auto itr = m_properties.find(key);
		if(itr != m_properties.end())
		{
			delete itr->second;
		}

		m_properties[key] = property;
	}

	void SetValue(const std::string& key, const char* str)
	{
		SetValue(key, std::string(str));
	}

	std::string GetValue(const std::string& key, const char* def)
	{
		return GetValue<std::string>(key, def);
	}

	template <typename T>
	void SetValue(const std::string& key, T* ptr)
	{
		SetValue<T*>(key, ptr);
	}

	template <typename T>
	T* GetValue(const std::string& key, T* def)
	{
		auto ew = m_properties.find(key);
		if(ew == m_properties.end())
		{
			return def;
		}

		BaseProperty* property = ew->second;
		TypedProperty<T*> typed_prop = dynamic_cast<TypedProperty<T*>*>(property);
		if(typed_prop == nullptr)
		{
			return def;
		}
		else
		{
			return typed_prop->m_value;
		}
	}


public:

	std::map<std::string, BaseProperty*> m_properties; 

};