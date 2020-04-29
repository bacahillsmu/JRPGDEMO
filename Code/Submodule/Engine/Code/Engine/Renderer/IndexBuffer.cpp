#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"


IndexBuffer::IndexBuffer( RenderContext* renderContext )
	: RenderBuffer(renderContext)
{
	
}

bool IndexBuffer::CreateStaticFor( uint const* indices, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(uint); 

	// just always create for static a new static buffer; 
	bool result = CreateBuffer( indices, 
		sizeNeeded,        // total size needed for buffer?
		sizeof(uint), // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_INDEX_STREAM_BIT, 
		GPU_MEMORY_USAGE_STATIC); // probably want dynamic if we're using copy

	if (result) 
	{
		m_indexCount = (uint)sizeNeeded; 
		return true; 
	} 
	else 
	{
		m_indexCount = 0u; 
		return false; 
	}
}

bool IndexBuffer::CopyCPUToGPU( uint const *indices, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(uint); 

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded > GetSize() || IsStatic()) {
		bool result = CreateBuffer( indices, 
			sizeNeeded,        // total size needed for buffer?
			sizeof(uint), // stride - size from one vertex to another
			RENDER_BUFFER_USAGE_INDEX_STREAM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		if (result) 
		{
			m_indexCount = count; 
		} 
		else 
		{
			m_indexCount = 0u; 
		}

		return result;
	} 
	else 
	{
		// non-static and we have enough room
		ASSERT( IsDynamic() ); 
		if (RenderBuffer::CopyCPUToGPU( indices, sizeNeeded )) 
		{
			m_indexCount = count; 
			return true; 
		}
	}

	return false;
}
