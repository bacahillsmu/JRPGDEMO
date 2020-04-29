#pragma once


#include <string>
#include <map>

template <typename T>
void ClearAndDeleteContentsOfMap( std::map<std::string, T*>& map )
{
	auto iter = map.begin();
	auto end = map.end();
	for(/*nothing*/; iter != end; iter++)
	{
		delete iter->second;
	}
	map.clear();
}



