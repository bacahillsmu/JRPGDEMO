#pragma once

#include <limits>
#include <algorithm>
#undef max

template <typename T>
struct STLUntrackedAllocator 
{
	using value_type = T;

	STLUntrackedAllocator() = default;
	template <class U>
	STLUntrackedAllocator(const STLUntrackedAllocator<U>&) {}

	T* allocate(std::size_t n) 
	{
		if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) 
		{
			if (auto ptr = std::malloc(n * sizeof(T))) 
			{
				return static_cast<T*>(ptr);
			}
		}
		throw std::bad_alloc();
	}
	void deallocate(T* ptr, std::size_t n) 
	{
		n;
		std::free(ptr);
	}
};

template <typename T, typename U>
inline bool operator == (const STLUntrackedAllocator<T>&, const STLUntrackedAllocator<U>&) 
{
	return true;
}

template <typename T, typename U>
inline bool operator != (const STLUntrackedAllocator<T>& a, const STLUntrackedAllocator<U>& b) 
{
	return !(a == b);
}