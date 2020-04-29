#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Dbghelp.h>

#pragma comment( lib, "dbghelp.lib" )

#include <vector>
#include <string>

constexpr unsigned int STACKFRAMES = 25;
constexpr unsigned int FRAMESTOSKIP = 1;
constexpr unsigned int MAXNAMELEN = 1024;

class Callstack
{

public:

	PVOID addrs[STACKFRAMES];
	USHORT frames = 0;
	DWORD BackTraceHash = 0;	
};

Callstack GetCallstack();


//Callstack CallstackGet(unsigned int skipFrames_ = 0);

// Convert a callstack to strings
// with one string per line
// Strings should return in this format...
// filepath.cpp(line,offset): function_name
// for example
// Engine/Image.cpp(127,8): Image::LoadFromFile
std::vector<std::string> CallstackToString(const Callstack& callstack);


