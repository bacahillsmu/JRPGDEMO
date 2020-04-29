#pragma once
#include "Engine/Memory/Memory.hpp"

// Abstract base class;
class Allocator
{

public:

	// Get Memory;
	virtual void* alloc(size_t size) = 0;

	// Free Memory;
	virtual void free(void* ptr) = 0;


	template <typename T, typename ...ARGS>
	T* create(ARGS&& ...args)
	{
		void* mem = this->alloc(sizeof(T));
		if (mem != nullptr)
		{
			new(mem) T(args...);
		}

		return (T*)mem;
	}

	template <typename T> 
	void destroy(T* obj)
	{
		if(obj != nullptr)
		{
			obj->~T();
			this->free(obj);
		}
	}
};

class TrackedAllocator : public Allocator
{

public:

	virtual void* alloc(size_t size) final { return TrackedAlloc(size); }
	virtual void free(void* ptr) final { TrackedFree(ptr); }

	static TrackedAllocator s_instance;
};

class UntrackedAllocator : public Allocator
{

public:

	virtual void* alloc(size_t size) final { return UntrackedAlloc(size); }
	virtual void free(void* ptr) final { UntrackedFree(ptr); }

	static UntrackedAllocator s_instance;
};