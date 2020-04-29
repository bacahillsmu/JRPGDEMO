#pragma once


#include <mutex>
#include <stdint.h>

typedef uint8_t byte;
typedef unsigned int uint;


struct ringbuffer_header_t
{
	uint size	: 31;
	uint ready	: 1;
};

class AsyncRingBuffer
{

public:

	~AsyncRingBuffer() { Deinit(); }

	bool Init(size_t size);
	void Deinit();

	void* TryLockWrite(size_t size);
	void* LockWrite(size_t size);
	void UnlockWrite(void* ptr);

	void* TryLockRead(size_t* out_size);
	void* LockRead(size_t* out_size);
	void UnlockRead(void* ptr);

	bool TryWrite(const void* data, size_t size);
	bool Write(const void* data, size_t size);

	// Return 0 on failure, otherwise return number of bytes read;
	size_t TryRead(void* out_data);
	size_t Read(void* out_data);

	size_t GetWritableSpace() const;


private:

	byte* m_buffer		= nullptr;
	size_t m_size		= 0;

	uint m_writeHead	= 0;
	uint m_readHead		= 0;

	std::mutex m_lock;

};