#pragma once


class Semaphore
{

public:

	void Create(unsigned int initialCount, unsigned int maxCount);
	void Destroy();
	void Acquire();
	bool TryAcquire();
	void Release(unsigned int count = 1);
	void ReleaseAll();

	// To make this work and appear like a normal scope lock;
	inline void lock() { Acquire(); }
	inline bool try_lock() { return TryAcquire(); }
	inline void unlock() { Release(1); }

	void* m_semaphore;
	unsigned int m_maxCount;
};