#include "Engine/Core/Tags.hpp"
#include "Engine/Core/StringUtils.hpp"


Tags::Tags( const std::string& commaSeparatedTagNames )
{
	SetOrRemoveTags(commaSeparatedTagNames);
}

Tags::Tags()
{

}

void Tags::SetOrRemoveTags( const std::string& commaSeparatedTagNames )
{
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commaSeparatedTagNames, ',');

	for(int stringIndex = 0; stringIndex < (int)(splitStrings.size()); stringIndex++)
	{
		if(splitStrings[stringIndex][0] == '!')
		{
			RemoveTag(splitStrings[stringIndex]);
			continue;
		}
		else
		{
			SetTag(splitStrings[stringIndex]);
			continue;
		}
	}
}

void Tags::ClearTagsAndSetNewTags( const std::string& commaSeparatedTagNames )
{
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commaSeparatedTagNames, ',');

	RemoveAllTags();

	for(int stringIndex = 0; stringIndex < (int)(splitStrings.size()); stringIndex++)
	{
		SetTag(splitStrings[stringIndex]);
	}
}

bool Tags::HasTags( const std::string& commaSeparatedTagNames )
{
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commaSeparatedTagNames, ',');
	for(int stringIndex = 0; stringIndex < (int)(splitStrings.size()); stringIndex++)
	{
		HasTag(splitStrings[stringIndex]) ? true : false;
	}

	return true;
}

void Tags::SetTag( const std::string& tagName )
{
	std::vector<std::string>::iterator tagIterator;
	tagIterator = find(m_tags.begin(), m_tags.end(), tagName);
	if(tagIterator == m_tags.end())
	{
		m_tags.push_back(tagName);
	}
}

void Tags::RemoveTag( const std::string& tagName )
{
	std::string tagNameRemoveFirstElement = tagName;
	tagNameRemoveFirstElement = tagNameRemoveFirstElement.replace(0,1,"");
	std::vector<std::string>::iterator tagIterator;
	tagIterator = find(m_tags.begin(), m_tags.end(), tagNameRemoveFirstElement);
	if(tagIterator != m_tags.end())
	{
		m_tags.erase(tagIterator);
	}
}

void Tags::RemoveAllTags()
{
	m_tags.clear();
}

bool Tags::HasTag( const std::string& tagName )
{
	std::vector<std::string>::iterator tagIterator;
	tagIterator = find(m_tags.begin(), m_tags.end(), tagName);
	if(tagIterator != m_tags.end())
	{
		bool hasTag = false;
		tagName[0] == '!' ? hasTag = false : hasTag = true;
		return hasTag;
	}
	else
	{
		bool hasTag = false;
		tagName[0] == '!' ? hasTag = true : hasTag = false;
		return hasTag;
	}
}

const std::string Tags::GetAllTags() const
{
	std::string allTags = "";

	for(int tagIndex = 0; tagIndex < (int)(m_tags.size()); tagIndex++)
	{
		allTags += m_tags[tagIndex];
	}

	return allTags;
}
