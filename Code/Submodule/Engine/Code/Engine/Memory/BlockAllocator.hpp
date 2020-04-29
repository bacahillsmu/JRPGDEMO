#pragma once
#include "Engine/Memory/Allocator.hpp"


#include <mutex>

typedef unsigned int uint;

struct block_t
{
	block_t* next;
};

struct chunk_t
{
	chunk_t* next;
};

class BlockAllocator : public Allocator
{

public:

	// Takes a base allocator to sub-allocate out of,
	//  which means it can grow as long as the base can allocate
	bool init(Allocator* base, size_t block_size, size_t alignment, uint blocks_per_chunk);

	// Take a static buffer,
	//  which means it cannot grow
	bool init(void* buffer, size_t buffer_size, size_t block_size, size_t alignment);

	void deinit();

	// interface implementation
	virtual void* alloc(size_t size) final; // works as long size <= block_size
	virtual void free(void* ptr) final;

	// unique to a block allocator
	// allocates and frees a single block
	void* alloc_block();
	void free_block(void* ptr);
	block_t* pop_free_block();
	void push_free_block(block_t* block);

private:

	// allocates a single chunk of memory
	// that is divided into blocks - will fail
	// if no base allocator is provided
	bool allocate_chunk();
	void break_up_chunk(void* chunk);

private:

	Allocator* m_base = nullptr;

	block_t* m_free_blocks = nullptr;
	chunk_t* m_chunk_list = nullptr;

	size_t m_buffer_size;
	size_t m_alignment;
	size_t m_block_size;
	size_t m_blocks_per_chunk;

	// AsyncBlockAllocator
	std::mutex m_chunk_lock; // when allocating chunks;
	std::mutex m_block_lock; // when allocating a single block;
};

extern BlockAllocator* g_theBlockAllocatorOnlyForProfilerNodeT;