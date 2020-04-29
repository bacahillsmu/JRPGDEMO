#include "Engine/Log/Log.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Game/Framework/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Profile/Profile.hpp"

#include <iostream>
#include <fstream>
#include <stdarg.h>




// -----------------------------------------------------------------------
// Global Declaration
// -----------------------------------------------------------------------
LogSystem* g_theLogSystem = nullptr;
bool g_theLogSystemFlushRequested = false;


// -----------------------------------------------------------------------
// Static Methods Used for Callbacks
// -----------------------------------------------------------------------

static void LogTest();
static bool LogThreadTest(EventArgs& args)
{
	args;

	uint core_count = std::thread::hardware_concurrency() - 1;
	for (uint i = 0; i < core_count; ++i) 
	{
		std::thread test_thread(LogTest);
		test_thread.detach();
	}

	return true;
}

static bool LogEnableAllFilters(EventArgs& args)
{
	args;

	g_theLogSystem->LogEnableAll();

	return true;
}

static bool LogDisableAllFilters(EventArgs& args)
{
	args;

	g_theLogSystem->LogDisableAll();

	return true;
}

static bool LogEnableSingleFilter(EventArgs& args)
{
	std::string filter = args.GetValue("filter", "");
	g_theLogSystem->LogAddFilter(filter.c_str());
	g_theLogSystem->LogEnable(filter.c_str());

	return true;
}

static bool LogDisableSingleFilter(EventArgs& args)
{
	std::string filter = args.GetValue("filter", "");
	g_theLogSystem->LogAddFilter(filter.c_str());
	g_theLogSystem->LogDisable(filter.c_str());

	return true;
}

static bool LogMessageWithFilter(EventArgs& args)
{

	std::string filterandmessage = args.GetValue("fm", "");

	std::string delimiter = "::";
	std::string filter = filterandmessage.substr(0, filterandmessage.find(delimiter));
	std::string message = filterandmessage.substr(filterandmessage.find(delimiter) + 2);

	g_theLogSystem->LogAddFilter(filter.c_str());

	// however you get arguments off yoru command line
	char const* filter_c = filter.c_str();
	char const* message_c = message.c_str();

	// proper error checking
	// TODO

	// Log the message
	Logf(filter_c, message_c);

	return true;
}

static bool LogFlushTest(EventArgs& args)
{
	args;

	Logf("debug", "This message was logged.");
	LogFlush();

	return true;
}



// -----------------------------------------------------------------------
void LogSystem::LogHook(LogHookCallbackFunction cb)
{
	HookLock();
	m_hookers.push_back(cb);
	HookUnLock();
}

// -----------------------------------------------------------------------
void LogSystem::LogUnhook(LogHookCallbackFunction cb)
{
	HookLock();
	
	for(int hookCBIndex = 0; hookCBIndex < m_hookers.size(); hookCBIndex++)
	{
		if(m_hookers[hookCBIndex] == cb)
		{
			m_hookers.erase(m_hookers.begin() + hookCBIndex);
			break;
		}
	}

	HookUnLock();
}

// -----------------------------------------------------------------------
// Statics
// -----------------------------------------------------------------------
static void LogThread()
{
	// Create a log file
	FILE* file = fopen(g_theLogSystem->m_filename.c_str(), "wb");
	if(!file)
	{
		GUARANTEE_RECOVERABLE(!file, "Log file not found.");
		return;
	}

	while(g_theLogSystem->IsRunning())
	{
		g_theLogSystem->WaitForWork();

		log_message_t* msg;
		size_t size;
		while((msg = (log_message_t*)g_theLogSystem->m_asyncRingBuffer.TryLockRead(&size)) != nullptr)
		{
			for(int i = 0; i < g_theLogSystem->m_hookers.size(); i++)
			{
				g_theLogSystem->m_hookers[i](msg);
			}

			if(msg->filterSize > 0)
			{
				fwrite(msg->filter, 1, msg->filterSize - 1, file);
				fwrite(": ", 1, 2, file);
			}
			
			fwrite(msg->message, 1, msg->messageSize - 1, file);

			if(msg->callstackf)
			{
				fwrite("\n", 1, 1, file);
				fwrite("### Requested Callstack ###\n", 1, 28, file);
				std::vector<std::string> callstack = CallstackToString(msg->callstack);
				for(int i = 0; i < callstack.size(); i++)
				{
					fwrite(callstack[i].c_str(), 1, strlen(callstack[i].c_str()), file);
					fwrite("\n", 1, 1, file);
				}
			}

			fwrite("\n", 1, 1, file);

			g_theLogSystem->m_asyncRingBuffer.UnlockRead(msg);
		}

		if(g_theLogSystemFlushRequested)
		{
			fflush(file);
			g_theLogSystemFlushRequested = false;
		}
	}

	fclose(file);
}

static void LogTest()
{
	std::thread::id this_id = std::this_thread::get_id();
	size_t hash_id = std::hash<std::thread::id>{}(this_id);
	char const* format = "Thread[%llu]: Printing Message %u";

	for (uint i = 0; i < LOG_MESSAGES_PER_THREAD_TEST; ++i)
	{
// 		if (g_theRandomNumberGenerator->RandomCoinFlip())
// 		{
// 			LogCallstackf("debug", format, hash_id, i);
// 		}
// 		else 
		{
			Logf("debug", format, hash_id, i);
		}
	}
}

// -----------------------------------------------------------------------
void LogSystemInit(const char* logFile_)
{
	g_theLogSystem = new LogSystem();

	g_theLogSystem->m_asyncRingBuffer.Init(1'000'000'000);
	g_theLogSystem->m_filename = logFile_;
	g_theLogSystem->m_signal.Create(0, 1);

	g_theLogSystem->m_thread = std::thread(LogThread);

	g_theEventSystem->SubscriptionEventCallbackFunction("log_thread_test", LogThreadTest);
	g_theEventSystem->SubscriptionEventCallbackFunction("log_enable_all", LogEnableAllFilters);
	g_theEventSystem->SubscriptionEventCallbackFunction("log_disable_all", LogDisableAllFilters);
	g_theEventSystem->SubscriptionEventCallbackFunction("log_enable_filter", LogEnableSingleFilter);
	g_theEventSystem->SubscriptionEventCallbackFunction("log_disable_filter", LogDisableSingleFilter);
	g_theEventSystem->SubscriptionEventCallbackFunction("lfm_test", LogMessageWithFilter);
	g_theEventSystem->SubscriptionEventCallbackFunction("log_flush_test", LogFlushTest);
}

// -----------------------------------------------------------------------
void LogSystemShutdown()
{
	g_theLogSystem->Stop();

	Logf("", "Shutting down log system...");

	g_theLogSystem->SignalWork();
	g_theLogSystem->m_thread.join();

	delete g_theLogSystem;
	g_theLogSystem = nullptr;
}

// -----------------------------------------------------------------------
void Logf(const char* filter, const char* format, ...)
{
	if (!g_theLogSystem->IsFiltered(filter))
	{
		return;
	}

	log_message_t logMessage;

	va_list argumentList;
	va_list copyList;
	va_start(argumentList, format);
	va_copy(copyList, argumentList);

	size_t logMessageSize = sizeof(log_message_t);
	size_t messageSize = (size_t)vsnprintf(nullptr, 0, format, copyList) + 1; va_end(copyList);
	size_t filterSize = strlen(filter) + 1;

	size_t bytesNeeded = logMessageSize + messageSize + filterSize;
	logMessage.messageSize = messageSize;
	logMessage.filterSize = filterSize;

	void* ptr = g_theLogSystem->m_asyncRingBuffer.LockWrite(bytesNeeded);
	logMessage.message = (char*)ptr + logMessageSize;
	logMessage.filter = logMessage.message + messageSize;

	memcpy(ptr, &logMessage, logMessageSize);
	vsnprintf(logMessage.message, messageSize, format, argumentList); va_end(argumentList);
	memcpy(logMessage.filter, filter, filterSize);

	g_theLogSystem->m_asyncRingBuffer.UnlockWrite(ptr);

	g_theLogSystem->SignalWork();
}

// -----------------------------------------------------------------------
void LogCallstackf(const char* filter, const char* format, ...)
{
	if (!g_theLogSystem->IsFiltered(filter))
	{
		return;
	}

	log_message_t logMessage;
	logMessage.callstackf = true;

	va_list argumentList;
	va_list copyList;
	va_start(argumentList, format);
	va_copy(copyList, argumentList);

	size_t logMessageSize = sizeof(log_message_t);
	size_t messageSize = (size_t)vsnprintf(nullptr, 0, format, copyList) + 1; va_end(copyList);
	size_t filterSize = strlen(filter) + 1;

	size_t bytesNeeded = logMessageSize + messageSize + filterSize;
	logMessage.messageSize = messageSize;
	logMessage.filterSize = filterSize;

	void* ptr = g_theLogSystem->m_asyncRingBuffer.LockWrite(bytesNeeded);
	logMessage.message = (char*)ptr + logMessageSize;
	logMessage.filter = logMessage.message + messageSize;

	logMessage.callstack = GetCallstack();
	memcpy(ptr, &logMessage, logMessageSize);
	vsnprintf(logMessage.message, messageSize, format, argumentList); va_end(argumentList);
	memcpy(logMessage.filter, filter, filterSize);

	g_theLogSystem->m_asyncRingBuffer.UnlockWrite(ptr);

	g_theLogSystem->SignalWork();
}

// -----------------------------------------------------------------------
void LogReport(reporter_node_t* reporter_node, int i)
{
	if(!reporter_node)
	{
		return;
	}

	std::string t = "";
	for(int x = 1; x < i; x++)
	{
		t += "\t";
	}
	std::string args = t + "%s \n" + t + "%s %i\n" + t + "%s %f%s\n" + t + "%s %f%s";

	Logf("", args.c_str(), reporter_node->m_label,
		"calls:", reporter_node->m_callCount,
		"total:", reporter_node->m_totalTime, "s", 
		"self :", reporter_node->m_selfTime, "s");


	if(reporter_node->m_children.size() > 0)
	{
		int tab = i + 1;
		for(auto& child : reporter_node->m_children)
		{
			LogReport(&child, tab);
		}
	}
}

// -----------------------------------------------------------------------
void LogReport(std::vector<profiler_node_t*> profiler_nodes, int i)
{
	std::string t = "";
	for (int x = 1; x < i; x++)
	{
		t += "\t";
	}
	std::string args = t + "%s \n" + t + "%s %i\n" + t + "%s %f%s\n" + t + "%s %f%s";

	for(auto& profiler : profiler_nodes)
	{
		Logf("", args.c_str(), profiler->m_label,
			"calls:", profiler->callCount,
			"total:", profiler->totalTime, "s",
			"self :", profiler->selfTime, "s");
	}
}

// -----------------------------------------------------------------------
void LogFlush()
{
	g_theLogSystemFlushRequested = true;

	g_theLogSystem->SignalWork();

	while(g_theLogSystemFlushRequested)
	{
		std::this_thread::yield();
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogEnableAll()
{
	std::scoped_lock lock(m_filterLock);

	m_listMode = true;
	for(auto& filter : m_filters)
	{
		filter.second = true;
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogDisableAll()
{
	std::scoped_lock lock(m_filterLock);

	m_listMode = false;
	for (auto& filter : m_filters)
	{
		filter.second = false;
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogEnable(const char* filter)
{
	std::scoped_lock lock(m_filterLock);

	// If we enable a filter, then we want it to show;
	std::map< std::string, bool >::iterator found = m_filters.find(filter);
	if (found != m_filters.end())
	{
		found->second = true;
	}
	else
	{
		LogAddFilter(filter);
		found = m_filters.find(filter);
		if (found != m_filters.end())
		{
			found->second = true;
		}
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogDisable(const char* filter)
{
	std::scoped_lock lock(m_filterLock);

	// If we disable a filter, then we want it to not show;
	std::map< std::string, bool >::iterator found = m_filters.find(filter);
	if (found != m_filters.end())
	{
		found->second = false;
	}
	else
	{
		LogAddFilter(filter);
		found = m_filters.find(filter);
		if (found != m_filters.end())
		{
			found->second = false;
		}
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogAddFilter(const char* filter) 
{
	std::scoped_lock lock(m_filterLock);

	// If we add a filter, then we want it to default to showing;
	std::map< std::string, bool >::iterator found = m_filters.find(filter);
	if (found == m_filters.end())
	{
		m_filters[filter] = m_listMode;
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogRemoveFilter(const char* filter)
{
	std::scoped_lock lock(m_filterLock);

	std::map< std::string, bool >::iterator found = m_filters.find(filter);
	if (found != m_filters.end())
	{
		m_filters.erase(found);
	}
}

// -----------------------------------------------------------------------
void LogSystem::LogClearFilters()
{
	std::scoped_lock lock(m_filterLock);

	m_filters.clear();
}

// -----------------------------------------------------------------------
bool LogSystem::IsFiltered(const char* filter)
{
	std::scoped_lock lock(m_filterLock);

	std::map< std::string, bool >::iterator found = m_filters.find(filter);
	if (found != m_filters.end())
	{
		return found->second;
	}
	else
	{
		m_filters[filter] = m_listMode;
		return m_listMode;
	}
}
