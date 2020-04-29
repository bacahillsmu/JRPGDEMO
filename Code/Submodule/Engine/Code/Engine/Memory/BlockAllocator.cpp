

#include "Engine/Memory/BlockAllocator.hpp"
#include "Engine/Log/Log.hpp"

#include <shared_mutex>



BlockAllocator* g_theBlockAllocatorOnlyForProfilerNodeT = nullptr;
static uint numberofblocks = 0;

// -----------------------------------------------------------------------
bool BlockAllocator::init(Allocator* base, size_t block_size, size_t alignment, uint blocks_per_chunk)
{
	m_base = base;
	m_block_size = block_size;
	m_alignment = alignment;
	m_blocks_per_chunk = blocks_per_chunk;

	m_free_blocks = nullptr;
	m_chunk_list = nullptr;

	allocate_chunk();

	return true;
}

// -----------------------------------------------------------------------
bool BlockAllocator::init(void* buffer, size_t buffer_size, size_t block_size, size_t alignment)
{
	m_blocks_per_chunk = buffer_size / block_size;
	m_buffer_size = buffer_size;
	m_block_size = block_size;
	m_alignment = alignment;

	m_base = nullptr;
	m_free_blocks = nullptr;

	break_up_chunk(buffer);



	return true;
}

// -----------------------------------------------------------------------
void BlockAllocator::deinit()
{
	std::scoped_lock lk(m_block_lock);
	std::scoped_lock lk2(m_chunk_lock);

	if (m_base) 
	{
		while (m_chunk_list != nullptr)
		{
			chunk_t* list = m_chunk_list;
			m_chunk_list = m_chunk_list->next;
			m_base->free(list);
		}
	}

	m_base = nullptr;
	m_free_blocks = nullptr;
	m_block_size = 0u;
	m_blocks_per_chunk = 0u;
}

// -----------------------------------------------------------------------
void* BlockAllocator::alloc(size_t size)
{
	if(size <= m_block_size)
	{
		return alloc_block();
	}

	return nullptr;
}

// -----------------------------------------------------------------------
void BlockAllocator::free(void* ptr)
{
	free_block(ptr);
}

// -----------------------------------------------------------------------
void* BlockAllocator::alloc_block()
{
	block_t* block = pop_free_block();
	while (block == nullptr) 
	{
		if (!allocate_chunk())
		{
			return nullptr;
		}

		block = pop_free_block();
	}

	return block;
}

// -----------------------------------------------------------------------
void BlockAllocator::free_block(void* ptr)
{
	block_t* block = (block_t*)ptr;
	push_free_block(block);
}

// -----------------------------------------------------------------------
block_t* BlockAllocator::pop_free_block()
{
	std::scoped_lock lk(m_block_lock);

	numberofblocks--;

	block_t* head = m_free_blocks;
	if (head != nullptr) 
	{
		m_free_blocks = head->next;
	}

	return head;
}

// -----------------------------------------------------------------------
void BlockAllocator::push_free_block(block_t* block)
{
	std::scoped_lock lk(m_block_lock);

	if (numberofblocks == 0)
	{
		LogCallstackf("filter", "asd");
	}

	numberofblocks++;

	block->next = m_free_blocks;
	m_free_blocks = block;
}

// -----------------------------------------------------------------------
bool BlockAllocator::allocate_chunk()
{
	if (m_base == nullptr) 
	{
		return false;
	}

	if (m_chunk_lock.try_lock()) 
	{
		size_t chunk_size = m_blocks_per_chunk * m_block_size + sizeof(chunk_t);

		chunk_t* chunk = (chunk_t*)m_base->alloc(chunk_size);
		if (chunk == nullptr) 
		{
			m_chunk_lock.unlock();
			return false;
		}

		// track this chunk so we can free it later
		chunk->next = m_chunk_list;
		m_chunk_list = chunk;

		// break chunk 
		break_up_chunk(chunk + 1);
		m_chunk_lock.unlock();
	}
	
	return true;
}

// -----------------------------------------------------------------------
void BlockAllocator::break_up_chunk(void* chunk)
{
	byte* buff = (byte*)chunk;
	block_t* first = (block_t*)buff;
	block_t* head = nullptr;

	for (uint i = 0; i < m_blocks_per_chunk; ++i) 
	{
		block_t* node = (block_t*)buff;
		buff += m_block_size;
		numberofblocks++;

		node->next = head;
		head = node;
	}

	{
		std::scoped_lock lk(m_block_lock);
		first->next = m_free_blocks;
		m_free_blocks = head;
	}
}
