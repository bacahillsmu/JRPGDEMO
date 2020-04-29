#include "Engine/Memory/Memory.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Memory/Allocator.hpp"


//-----------------------------------------------------------------------------------------------
std::atomic<size_t> memtrack_allocationCount = 0;
std::atomic<size_t> memtrack_byteCount = 0;

thread_local size_t t_allocCount = 0;
thread_local size_t t_allocBytes = 0;
thread_local size_t t_freeCount = 0;
thread_local size_t t_freeBytes = 0;

size_t GetMemAllocCount()
{
	return memtrack_allocationCount;
}

// std::mutex gTrackerLock;
// 
// std::map<void*,      // key
// 	mem_track_info_t, // value
// 	std::less<void*>, // less to use for map
// 	UntrackedAllocator<std::pair<void* const, mem_track_info_t>> // allocator  
// > gMemTrackers;


// ------------------------------------------------------------------------------------------------------------------------------
std::mutex& GetMemTrackerLock()
{
    static std::mutex trackerLock;
    return trackerLock;
}

// ------------------------------------------------------------------------------------------------------------------------------
std::map<void*, mem_track_info_t, std::less<void*>, STLUntrackedAllocator<std::pair<void* const, mem_track_info_t>>>& GetMemTrakingMap()
{
    static std::map<void*, mem_track_info_t, std::less<void*>, STLUntrackedAllocator<std::pair<void* const, mem_track_info_t>>> memTrackerMap;
    return memTrackerMap;
}


//-----------------------------------------------------------------------------------------------
std::string GetSizeString(size_t byte_count)
{
	uint numBytes = (uint)byte_count;
	std::string returnString = "";
	if (numBytes < 1024)
	{
		returnString += Stringf("%u B", numBytes);
	}
	else if (numBytes > 1024 && numBytes < 1024 * 1024)
	{
		returnString += Stringf("%.3f KiB", (float)numBytes / 1024);
	}
	else if (numBytes > 1024 * 1024 && numBytes < 1024 * 1024 * 1024)
	{
		returnString += Stringf("%.3f MiB", (float)numBytes / (1024 * 1024));
	}
	else if (numBytes > (1024 * 1024 * 1024))
	{
		returnString += Stringf("%.3f GiB", (float)numBytes / (1024 * 1024 * 1024));
	}

	return returnString;
}

//-----------------------------------------------------------------------------------------------
void* UntrackedAlloc(size_t byte_count)
{
	// Allocate memory off the free-store
	return ::malloc(byte_count);
}

//-----------------------------------------------------------------------------------------------
void UntrackedFree(void* ptr)
{
	// Return memory to the free-store
	return ::free(ptr);
}

//-----------------------------------------------------------------------------------------------
#if defined(MEM_TRACKING) 
	#if MEM_TRACKING == MEM_TRACK_VERBOSE
		static void TrackAllocation(void* allocation, size_t byte_count)
		{
			memtrack_byteCount += byte_count;

			Callstack callstack = GetCallstack();

			mem_track_info_t memtrack_info;
			memtrack_info.original_pointer = allocation;
			memtrack_info.byte_size = byte_count;
			memtrack_info.callstack = callstack;

			GetMemTrackerLock().lock();
			GetMemTrakingMap()[allocation] = memtrack_info;
			GetMemTrackerLock().unlock();
		}
	#endif
#endif

//-----------------------------------------------------------------------------------------------
#if defined(MEM_TRACKING) 
	#if MEM_TRACKING == MEM_TRACK_VERBOSE
		static void UntrackAllocation(void* allocation)
		{
			// This can be nullptr because we may delete a nullptr, its not a real problem...
			// ... except when we try to find a nullptr in our memorymap
			if (allocation)
			{
				GetMemTrackerLock().lock();
				auto it = GetMemTrakingMap().find(allocation);
				if (it == GetMemTrakingMap().end())
				{
					__debugbreak();
				}

				memtrack_byteCount -= it->second.byte_size;
				t_freeBytes += it->second.byte_size;

				GetMemTrakingMap().erase(it);
				GetMemTrackerLock().unlock();
			}
		}
	#endif
#endif

//-----------------------------------------------------------------------------------------------
void* TrackedAlloc(size_t byte_count)
{
#if !defined(MEM_TRACKING)
	return UntrackedAlloc(byte_count);
#else
	#if MEM_TRACKING == MEM_TRACK_ALLOC_COUNT

		memtrack_allocationCount++;
		t_allocCount++;
		t_allocBytes += byte_count;
		return UntrackedAlloc(byte_count);

	#elif MEM_TRACKING == MEM_TRACK_VERBOSE
		memtrack_allocationCount++;
		t_allocCount++;
		t_allocBytes += byte_count;
		void* allocation = UntrackedAlloc(byte_count);
		TrackAllocation(allocation, byte_count);
		return allocation;
	#endif
#endif
}

void TrackedFree(void* ptr)
{
#if !defined(MEM_TRACKING)
	return UntrackedFree(ptr);
#else
		
	#if MEM_TRACKING == MEM_TRACK_ALLOC_COUNT
		memtrack_allocationCount--;
		t_freeCount++;
		UntrackedFree(ptr);
	#elif MEM_TRACKING == MEM_TRACK_VERBOSE
		memtrack_allocationCount--;
		t_freeCount++;
		UntrackAllocation(ptr);
		UntrackedFree(ptr);
	#endif

#endif
}

size_t MemTrackGetLiveAllocationCount()
{
	return memtrack_allocationCount;
}

size_t MemTrackGetLiveByteCount()
{
	return memtrack_byteCount;
}

struct 
{
	bool operator()(output_mem_track_info_t a, output_mem_track_info_t b) const
	{
		return a.byte_size < b.byte_size;
	}
} VecCompareFunction;

void MemTrackLogLiveAllocations()
{
	std::map<void*, mem_track_info_t, std::less<void*>, STLUntrackedAllocator<std::pair<void* const, mem_track_info_t>>> secretMemTrackerMap;
	
	// Deep copy the mem tracker map
	GetMemTrackerLock().lock();
	auto copyIter = GetMemTrakingMap().begin();
	while(copyIter != GetMemTrakingMap().end())
	{
		secretMemTrackerMap[copyIter->first] = copyIter->second;
		copyIter++;
	}
	GetMemTrackerLock().unlock();

	// Group things by callstack
	size_t totalByte = 0;
	size_t totalAllocCount = 0;

	std::map<DWORD, output_mem_track_info_t> condensedSecretMap;
	auto backTraceHashIter = secretMemTrackerMap.begin();
	while(backTraceHashIter != secretMemTrackerMap.end())
	{
		DWORD backtracehash = backTraceHashIter->second.callstack.BackTraceHash;

		auto it = condensedSecretMap.find(backtracehash);
		if(it == condensedSecretMap.end())
		{
			output_mem_track_info_t output_memtrack;
			output_memtrack.allocation = 1;
			output_memtrack.byte_size = backTraceHashIter->second.byte_size;
			output_memtrack.callstack = backTraceHashIter->second.callstack;

			condensedSecretMap[backtracehash] = output_memtrack;

			totalByte += output_memtrack.byte_size;
			totalAllocCount++;
		}
		else
		{
			it->second.allocation++;
			it->second.byte_size += backTraceHashIter->second.byte_size;

			totalByte += it->second.byte_size;
			totalAllocCount++;
		}

		backTraceHashIter++;
	}

	// Sort by byte size
	std::vector<output_mem_track_info_t> logVector;
	auto logIter = condensedSecretMap.begin();
	while (logIter != condensedSecretMap.end())
	{
		logVector.emplace_back(logIter->second);

		logIter++;
	}

	std::sort(logVector.begin(), logVector.end(), VecCompareFunction);

	// Actually Log the things
	DebuggerPrintf("===========\n");
	DebuggerPrintf("\n\n\n=== LOG ===\n");
	std::string formattedBytes = GetSizeString(totalByte);
	DebuggerPrintf("===========\n");
	DebuggerPrintf("%s", formattedBytes.c_str());
	DebuggerPrintf(" from %d allocations\n", totalAllocCount);
	DebuggerPrintf("===========\n\n");

	for(int i = 0; i < logVector.size(); i++)
	{
		formattedBytes = GetSizeString(logVector[i].byte_size);
		DebuggerPrintf("\n\n======================\n");
		DebuggerPrintf("%s", formattedBytes.c_str());
		DebuggerPrintf(" from %d allocations\n", logVector[i].allocation);
		DebuggerPrintf("======================\n");
		CallstackToString(logVector[i].callstack);
	}
	
	DebuggerPrintf("\n=== END OF LOG ===\n\n\n");
}

// Overload new and delete to use tracked allocations
void* operator new(size_t size)
{
	return TrackedAlloc(size);
}

void operator delete(void* ptr)
{
	TrackedFree(ptr);
}

void* operator new(size_t size, Allocator& pool)
{
	return pool.alloc(size);
}

void operator delete(void* ptr, Allocator& pool) 
{ 
	pool.free(ptr);
}

UNITTEST("New Delete", "BasicBitchTests", 0)
{
	int* i = new int(5);
	delete i;

	return true;
}