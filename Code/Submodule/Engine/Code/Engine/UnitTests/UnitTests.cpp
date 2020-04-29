#include "Engine/UnitTests/UnitTests.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



// ----------------------------------------------------------------------------
UnitTest::UnitTest(const char* name_, const char* category_, int priority_, test_work_cb cb_)
	:name(name_)
	,category(category_)
	,priority(priority_)
	,work_cb(cb_)
{
	g_testCount++;

	assert(g_testCount < MAX_TESTS);
	next = g_allTests;
	g_allTests = this;
}

void UnitTest::UnitTestsRunAllCategories(int priority_ /*= INT_MAX*/)
{
	int total = 0;
	int passed = 0;
	
	UnitTest* test = g_allTests;
	while(test != nullptr)
	{
		if(test->priority <= priority_)
		{
			total++;
			
			bool passCheck = test->work_cb();
			if(!passCheck)
			{
				DebuggerPrintf("Test ['%s'] Failed!\n", test->name);
			}
			else
			{
				passed++;
			}

			GUARANTEE_RECOVERABLE(total == passed, "Not passed all tests.\n");
		}		

		test = test->next;
	}

	DebuggerPrintf("%i/%i tests passed for all categories.\n", passed, total);
}

void UnitTest::UnitTestsRunCategory(const char* category_, int priority_ /*= INT_MAX*/)
{
	int total = 0;
	int passed = 0;

	UnitTest* test = g_allTests;
	while (test != nullptr)
	{
		if (test->category == category_ && test->priority <= priority_)
		{
			total++;

			bool passCheck = test->work_cb();
			if (!passCheck)
			{
				DebuggerPrintf("Test ['%s'] Failed!\n", test->name);
			}
			else
			{
				passed++;
			}

			GUARANTEE_RECOVERABLE(total == passed, "Not passed all tests!\n");
		}

		test = test->next;
	}

	DebuggerPrintf("%i/%i tests passed for category: '%s'.\n", passed, total, category_);
}

int g_testCount = 0;
UnitTest* g_allTests = nullptr;