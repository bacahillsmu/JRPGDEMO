#pragma once
#include "Engine/Core/Common.hpp"
#include "limits.h"
#include <cassert>

constexpr int MAX_TESTS = 1024;
extern int g_testCount;

typedef bool(*test_work_cb)();

class UnitTest
{
public:

	UnitTest(const char* name_, const char* category_, int priority_, test_work_cb cb_);

	const char* name;
	const char* category;
	int priority;
	test_work_cb work_cb;

	UnitTest* next;

	static void UnitTestsRunAllCategories(int priority_ = INT_MAX);
	static void UnitTestsRunCategory(const char* category_, int priority_ = INT_MAX);

};

extern UnitTest* g_allTests;

#define UNITTEST( name, cat, pri ) 	\
static bool MACRO_COMBINE(__UnitTest_,__LINE__)(); 	\
static UnitTest MACRO_COMBINE(__UnitTestObj_, __LINE__)(name, cat, pri, MACRO_COMBINE(__UnitTest_, __LINE__)); \
static bool MACRO_COMBINE(__UnitTest_, __LINE__)()