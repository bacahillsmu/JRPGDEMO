#pragma once
#include "Engine/Async/AsyncQueue.hpp"
#include "Engine/Semaphore/Semaphore.hpp"
#include "Engine/Async/AsyncRingBuffer.hpp"
#include "Engine/Callstack/Callstack.hpp"

#include <functional>
#include <thread>
#include <mutex>
#include <map>

#define LOG_MESSAGES_PER_THREAD_TEST 1024

class AsyncRingBuffer;
class reporter_node_t;
struct profiler_node_t;

struct log_message_t
{
	char* filter;
	char* message;
	Callstack callstack;

	size_t filterSize;
	size_t messageSize;

	bool callstackf = false;
};
using LogHookCallbackFunction = void(*)(log_message_t* logt);


class LogSystem
{

public:

	std::thread m_thread;
	std::string m_filename;

	AsyncRingBuffer m_asyncRingBuffer;
	Semaphore m_signal;

	inline void WaitForWork() { m_signal.Acquire(); }
	inline void SignalWork() { m_signal.Release(1); }

	inline bool IsRunning() { return m_isRunning; }
	inline void Stop() { m_isRunning = false; }

	inline void HookLock() { cdHookLock.lock(); }
	inline void HookUnLock() { cdHookLock.unlock(); }

public:

	bool m_isRunning = true;
	std::mutex cdHookLock;

	// Filtering
	void LogEnableAll();					// All messages log;
	void LogDisableAll();					// Not messages log;
	void LogEnable(const char* filter);		// This filter will start to appear in the log;
	void LogDisable(const char* filter);	// This filter will no longer appear in the log;
	void LogAddFilter(const char* filter);
	void LogRemoveFilter(const char* filter);
	void LogClearFilters();
	bool IsFiltered(const char* filter);
	std::map<std::string, bool> m_filters;
	bool m_isWhiteList = true;
	bool m_listMode = true;
	std::mutex m_filterLock;

	// Listener Hooks
	// Allow other systems to get message events from the logger thread
	//  so it can do something with them;
	// Exmaple would be DebuggerPrintf - or enable the DevConsole to print them;

	// This message is only valid for the lifetime of the callback
	//  and is not guaranteed to be valid after your hook returns, so be sure
	//  to copy the data you need;
	std::vector<LogHookCallbackFunction> m_hookers;

	void LogHook(LogHookCallbackFunction cb);
	void LogUnhook(LogHookCallbackFunction cb);

	
};

extern LogSystem* g_theLogSystem;
extern bool g_theLogSystemFlushRequested;



void LogSystemInit(const char* logFile);
void LogSystemShutdown(); // A final flush of the log file and properly close down;

// Commands expose;
// Log
// LogEnableAll
// LogDisableAll
// LogEnable
// LogDisable
// LogFlushTest
// LogThreadTest
// LogHookDevConsole

// Logs a message with the given filter;
void Logf(const char* filter, const char* format, ...);

// Logs a message with the given filter and includes the callstack on the next lines;
void LogCallstackf(const char* filter, const char* format, ...);

void LogReport(reporter_node_t* reporter_node, int i = 1);
void LogReport(std::vector<profiler_node_t*> profiler_node, int i = 1);

// Confirms all messages have been processed (committed to disk)
void LogFlush();





