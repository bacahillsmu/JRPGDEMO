#include "Engine/Async/AsyncRingBuffer.hpp"
#include "Engine/Memory/Memory.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

// -----------------------------------------------------------------------
bool AsyncRingBuffer::Init(size_t size)
{
	if(m_buffer != nullptr)
	{
		Deinit();
	}

	m_buffer = (byte*)UntrackedAlloc(size);
	m_size = size;

	return true;
}

// -----------------------------------------------------------------------
void AsyncRingBuffer::Deinit()
{
	if (m_buffer != nullptr)
	{
		TrackedFree(m_buffer);
		m_buffer = nullptr;
		m_size = 0u;
	}
}

// -----------------------------------------------------------------------
void* AsyncRingBuffer::LockRead(size_t* out_size)
{
	void* ptr = TryLockRead(out_size);

	while(ptr == nullptr)
	{
		std::this_thread::yield();
		ptr = TryLockRead(out_size);
	}

	return ptr;
}

// -----------------------------------------------------------------------
void* AsyncRingBuffer::LockWrite(size_t size)
{
	void* ptr = TryLockWrite(size);

	while (ptr == nullptr)
	{
		std::this_thread::yield();
		ptr = TryLockWrite(size);
	}

	return ptr;
}

// -----------------------------------------------------------------------
void* AsyncRingBuffer::TryLockWrite(size_t size)
{
	GUARANTEE_OR_DIE(size < (1 << 31), "Trying to write something bigger than the availible size in header.");

	size_t headerSize = sizeof(ringbuffer_header_t);
	size_t totalSize = 2 * headerSize + size;

	std::scoped_lock lk(m_lock);
	if(GetWritableSpace() < totalSize)
	{
		return nullptr;
	}

	uint newHead = m_writeHead + (uint)totalSize;
	if(newHead > m_size)
	{
		// Skip case;
		ringbuffer_header_t* skipHeader = (ringbuffer_header_t*)(m_buffer + m_writeHead);
		skipHeader->size = 0u;
		skipHeader->ready = 1u;

		m_writeHead = 0u;
	}

	if(GetWritableSpace() < totalSize)
	{
		return nullptr;
	}

	byte* currentBuffer = m_buffer + m_writeHead;

	ringbuffer_header_t* head = (ringbuffer_header_t*)currentBuffer;
	head->size = (uint)size;
	head->ready = 0u;

	m_writeHead += (uint)totalSize;

	return head + 1;
}

// -----------------------------------------------------------------------
void AsyncRingBuffer::UnlockWrite(void* ptr)
{
	ringbuffer_header_t* writeHead = (ringbuffer_header_t*)ptr;
	writeHead--;

	writeHead->ready = 1u;
}

// -----------------------------------------------------------------------
void* AsyncRingBuffer::TryLockRead(size_t* out_size)
{
	std::scoped_lock lk(m_lock);

	while(true)
	{
		if(m_readHead == m_writeHead)
		{
			return nullptr;
		}

		ringbuffer_header_t* readHeader = (ringbuffer_header_t*)(m_buffer + m_readHead);
		if(readHeader->ready)
		{
			if(readHeader->size == 0u)
			{
				// Skip case;
				m_readHead = 0u;
			}
			else
			{
				// Valid case;
				*out_size = readHeader->size;
				void* returnBuffer = readHeader + 1;
				return returnBuffer;
			}
		}
		else
		{
			return nullptr;
		}
	}
}

// -----------------------------------------------------------------------
void AsyncRingBuffer::UnlockRead(void* ptr)
{
	std::scoped_lock lk(m_lock);

	ringbuffer_header_t* readHeader = (ringbuffer_header_t*)ptr;
	readHeader--;

	GUARANTEE_OR_DIE((m_buffer + m_readHead) == (void*)readHeader, "Out of sync when reading.");

	m_readHead += sizeof(ringbuffer_header_t*) + readHeader->size;
}

// -----------------------------------------------------------------------
size_t AsyncRingBuffer::GetWritableSpace() const
{
	size_t remaining = 0;
	if(m_writeHead >= m_readHead)
	{
		remaining = m_size - m_writeHead;
		remaining += m_readHead;
	}
	else
	{
		remaining = m_readHead - m_writeHead;
	}

	return remaining;
}





