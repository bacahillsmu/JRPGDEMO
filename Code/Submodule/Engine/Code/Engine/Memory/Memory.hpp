#pragma once
#include "Engine/Async/AsyncQueue.hpp"
#include "Game/Framework/GameCommon.hpp"
#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Engine/UnitTests/UnitTests.hpp"
#include "Engine/Callstack/Callstack.hpp"
#include "Engine/Memory/STLUntrackedAllocator.hpp"
#include "Game/EngineBuildPreferences.hpp"

#include <string>
#include <atomic>
#include <thread>
#include <chrono>

typedef unsigned int uint;

struct mem_track_info_t
{
	void* original_pointer;
	size_t byte_size;
	Callstack callstack;
};

struct output_mem_track_info_t
{
	size_t allocation;
	size_t byte_size;
	Callstack callstack;
};

#define MEM_TRACK_ALLOC_COUNT 0
#define MEM_TRACK_VERBOSE 1

// If I remember correctly, I have a memory leak in the mem tracking verbose
/*
#if defined(_DEBUG)
	#define MEM_TRACKING MEM_TRACK_VERBOSE //slow but verbose
	//#define MEM_TRACKING MEM_TRACK_ALLOC_COUNT //faster
#elif defined(_RELEASE)
	#define MEM_TRACKING MEM_TRACK_ALLOC_COUNT
#endif
*/

std::string GetSizeString(size_t byte_count);

// Allocations that mem tracking will ignore
void* UntrackedAlloc(size_t byte_count);
void UntrackedFree(void* ptr);

// Allocations that mem tracking will track
void* TrackedAlloc(size_t byte_count);
void TrackedFree(void* ptr);

// Report methods
size_t MemTrackGetLiveAllocationCount();
size_t MemTrackGetLiveByteCount();
void MemTrackLogLiveAllocations();




// #if defined(MEM_TRACKING)
// #define MEMTEST_ITER_PER_THREAD 1'000'000
// #define MEMTEST_ALLOC_BYTE_SIZE 128
// 
// static void AllocTest(AsyncQueue<void*>& mem_queue, std::atomic<uint>& running_count)
// {
// 	for (uint i = 0; i < MEMTEST_ITER_PER_THREAD; ++i) 
// 	{
// 		float coinFlip = g_theRandomNumberGenerator->GetRandomFloatZeroToOne();
// 		if (coinFlip > 0.5f)
// 		{
// 			byte* ptr = (byte*)TrackedAlloc(MEMTEST_ALLOC_BYTE_SIZE);
// 
// 			for (uint i = 0; i < MEMTEST_ALLOC_BYTE_SIZE; ++i) 
// 			{
// 				ptr[i] = (byte)i;
// 			}
// 
// 			mem_queue.Enqueue(ptr);
// 		}
// 		else 
// 		{
// 			void* ptr;
// 			if (mem_queue.Dequeue(&ptr)) 
// 			{
// 				TrackedFree(ptr);
// 			}
// 		}
// 	}
// 
// 	running_count--;
// }
//
// UNITTEST("A02", nullptr, 0)
// {
// 	size_t pre_allocations = MemTrackGetLiveAllocationCount();
// 	{
// 		AsyncQueue<void*> mem_queue;
// 		uint coreCount = std::thread::hardware_concurrency();
// 		std::atomic<uint> live_count = coreCount;
// 
// 		for (uint i = 0; i < coreCount; i++) 
// 		{
// 			std::thread test_thread(AllocTest, mem_queue, i);
// 		}
// 
// 		while (live_count.load() > 0) 
// 		{
// 			//std::this_thread::sleep_for(100ms);
// 			std::chrono::milliseconds(100);
// 		}
// 
// 		void* ptr;
// 		while (mem_queue.Dequeue(&ptr)) 
// 		{
// 			TrackedFree(ptr);
// 		}
// 	}
// 
// 	// check we're back to where we started; 
// 	size_t post_allocations = MemTrackGetLiveAllocationCount();
// 
// 	// if done right, allocations at the start
// 	// should be allocations at the end; 
// 	return (pre_allocations == post_allocations);
//}
//#endif



extern thread_local size_t t_allocCount;
extern thread_local size_t t_allocBytes;
extern thread_local size_t t_freeCount;
extern thread_local size_t t_freeBytes;


