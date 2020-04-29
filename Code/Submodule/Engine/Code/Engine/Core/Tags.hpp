#pragma once

#include <vector>

class Tags
{
public:

	explicit Tags(const std::string& commaSeparatedTagNames);
	explicit Tags();

	// These support comma-separated strings of 0+ tag names, with “!” (NOT) tag names to remove
	void SetOrRemoveTags( const std::string& commaSeparatedTagNames ); // "blessed,!poisoned"
	bool HasTags( const std::string& commaSeparatedTagNames ); // "good,!cursed"
	void ClearTagsAndSetNewTags(const std::string& commaSeperatedTags);

	// These each take exactly one tag name, and do not support commas nor “!” (NOT) tags
	void SetTag( const std::string& tagName );
	void RemoveTag( const std::string& tagName );
	void RemoveAllTags();
	bool HasTag( const std::string& tagName );
	const std::string GetAllTags() const;

protected:
	std::vector<std::string> m_tags;
};
