#include "Engine/Renderer/VertextBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex_PCU.hpp"



VertexBuffer::VertexBuffer( RenderContext* owner )
	: RenderBuffer(owner)
{
}

VertexBuffer::~VertexBuffer()
{

}

bool VertexBuffer::CreateStaticFor( void* vertices, size_t stride, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * stride; 

	// just always create for static a new static buffer; 
	bool result = CreateBuffer( vertices, 
		sizeNeeded,        // total size needed for buffer?
		stride, // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT, 
		GPU_MEMORY_USAGE_STATIC); // probably want dynamic if we're using copy

	if (result) 
	{
		m_vertexCount = (uint)sizeNeeded; 
		return true; 
	} 
	else 
	{
		m_vertexCount = 0u; 
		return false; 
	}
}

bool VertexBuffer::CopyCPUToGPU( const void* vertices, size_t stride, uint const count )
{
	// how many bytes do we need
	size_t sizeNeeded = count * stride; 

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded > GetSize() || IsStatic()) {
		bool result = CreateBuffer( vertices, 
			sizeNeeded,        // total size needed for buffer?
			stride, // stride - size from one vertex to another
			RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		if (result) 
		{
			m_vertexCount = count; 
		} 
		else 
		{
			m_vertexCount = 0u; 
		}

		return result;
	} 
	else 
	{
		// non-static and we have enough room
		ASSERT( IsDynamic() ); 
		if (RenderBuffer::CopyCPUToGPU( vertices, sizeNeeded )) 
		{
			m_vertexCount = count; 
			return true; 
		}
	}

	return false;
}
