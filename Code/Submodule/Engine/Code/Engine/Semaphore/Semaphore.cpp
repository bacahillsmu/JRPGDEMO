#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

#include "Engine/Semaphore/Semaphore.hpp"

// -----------------------------------------------------------------------
void Semaphore::Create(unsigned int initialCount, unsigned int maxCount)
{
	m_semaphore = ::CreateSemaphore(
		nullptr,		// Security attributes;
		initialCount,	// Count this starts at;
		maxCount,		// Max count this semaphore can reach;
		nullptr			// Name if used across processes;
	);

	m_maxCount = maxCount;
}

// -----------------------------------------------------------------------
void Semaphore::Destroy()
{
	if(m_semaphore != nullptr)
	{
		::CloseHandle(m_semaphore);
		m_semaphore = nullptr;
	}
}

// -----------------------------------------------------------------------
void Semaphore::Acquire()
{
	::WaitForSingleObject(
		m_semaphore,	// Object to wait on;
		INFINITE		// Time to wait in milliseconds, we will wait till we get it;
	);
}

// -----------------------------------------------------------------------
bool Semaphore::TryAcquire()
{
	// May not succeed,
	//  if return true, the counter was decremented
	//  if returns false, the counter was 0 and unable to be decremented
	DWORD result = ::WaitForSingleObject(
		m_semaphore,	// Object to wait on;
		0				// Time to wait, it is 0 because we want to try only once;
	);

	return result == WAIT_OBJECT_0;
}

// -----------------------------------------------------------------------
void Semaphore::Release(unsigned int count /*= 1*/)
{
	// Releases the semaphore - adds the counter up to max;
	::ReleaseSemaphore(
		m_semaphore,	// Object to release;
		count,			// How many to add to the semaphore;
		nullptr			// Out, for previous count;
	);
}

// -----------------------------------------------------------------------
void Semaphore::ReleaseAll()
{
	// Releases the semaphore - adds the counter up to max;
	::ReleaseSemaphore(
		m_semaphore,	// Object to release;
		m_maxCount,			// How many to add to the semaphore;
		nullptr			// Out, for previous count;
	);
}
