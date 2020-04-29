#include "Engine/Renderer/UniformBuffer.hpp"

UniformBuffer::UniformBuffer( RenderContext* owner )
	: RenderBuffer(owner)
{
}

UniformBuffer::~UniformBuffer()
{

}

bool UniformBuffer::CopyCPUToGPU( void const *data, size_t const byteSize)
{
	if (byteSize > GetSize() || IsStatic()) 
	{
		bool result = CreateBuffer( data, 
			byteSize,        // total size needed for buffer?
			byteSize,        // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_UNIFORMS_BIT, 
			GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		return result; 
	} 
	else 
	{
		// non-static and we have enough room, so call down 
		// to our base classes CopyGPUToGPU that assumes
		// enough room is already allocated; 
		ASSERT( IsDynamic() ); 
		if (RenderBuffer::CopyCPUToGPU( data, byteSize )) 
		{
			return true; 
		}
	}

	return false; 
}
