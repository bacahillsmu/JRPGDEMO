#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"


#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxguid.lib")
#pragma comment( lib, "DXGI.lib" )

// Convert a buffer usage bitfield to a DX specific version; 
STATIC uint RenderBuffer::DXBufferUsageFromBufferUsage( eRenderBufferUsageBits bufferUsage )
{
	uint ret = 0u;

	if (bufferUsage & RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT) {
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (bufferUsage & RENDER_BUFFER_USAGE_INDEX_STREAM_BIT) {
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if (bufferUsage & RENDER_BUFFER_USAGE_UNIFORMS_BIT) {
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

// translate external enum to D3D11 specific options; 
// These will be used by texture as well,
// so may want to move this function to a common include; 
STATIC D3D11_USAGE RenderBuffer::DXMemoryUsageFromMemoryUsage( GPUMemoryUsage memoryUsage )
{
	switch (memoryUsage)
	{
		case GPU_MEMORY_USAGE_GPU:      
			return D3D11_USAGE_DEFAULT;

		case GPU_MEMORY_USAGE_STATIC:
			return D3D11_USAGE_IMMUTABLE;

		case GPU_MEMORY_USAGE_DYNAMIC:  
			return D3D11_USAGE_DYNAMIC;

		case GPU_MEMORY_USAGE_STAGING:  
			return D3D11_USAGE_STAGING;

		default: ASSERT_RETURN_VALUE(false, D3D11_USAGE_DYNAMIC); 
	}
}

RenderBuffer::RenderBuffer( RenderContext* owner )
{
	m_owner = owner;
}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);
}

size_t RenderBuffer::GetSize() const
{
	return m_bufferSize;
}

bool RenderBuffer::IsStatic() const
{
	return m_memoryUsage == GPU_MEMORY_USAGE_STATIC;
}

bool RenderBuffer::IsDynamic() const
{
	return m_memoryUsage == GPU_MEMORY_USAGE_DYNAMIC;
}

ID3D11Buffer* RenderBuffer::GetHandle()
{
	return m_handle;
}

bool RenderBuffer::CreateBuffer( void const* initialData, 
	size_t bufferSize, 
	size_t elementSize, 
	eRenderBufferUsageBits bufferUsage, 
	GPUMemoryUsage memoryUsage)
{
	// Free the old handle
	DX_SAFE_RELEASE(m_handle);

	// can not create a 0 sized buffer; 
	ASSERT_RETURN_VALUE( (bufferSize > 0u) && (elementSize > 0u), false );

	// static buffers MUST be supplied data.
	ASSERT_RETURN_VALUE( (memoryUsage != GPU_MEMORY_USAGE_STATIC) || (initialData != nullptr), false );

	// Setup the buffer. When creating a D3D object, we setup
	// DESC object to describe it.
	D3D11_BUFFER_DESC bufferDescription;
	memset( &bufferDescription, 0, sizeof(bufferDescription) );

	bufferDescription.ByteWidth = (UINT) bufferSize;
	bufferDescription.StructureByteStride = (UINT) elementSize;
	bufferDescription.Usage = DXMemoryUsageFromMemoryUsage(memoryUsage);
	bufferDescription.BindFlags = DXBufferUsageFromBufferUsage(bufferUsage);

	// give us write access to dynamic buffers
	// and read/write access to staging buffers; 
	bufferDescription.CPUAccessFlags = 0u;

	if (memoryUsage == GPU_MEMORY_USAGE_DYNAMIC) 
	{
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} 
	else if (memoryUsage == GPU_MEMORY_USAGE_STAGING) 
	{
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	// Map initial data if relevant (something was passed in)
	D3D11_SUBRESOURCE_DATA subresourceData;
	D3D11_SUBRESOURCE_DATA* subresourceData_ptr = nullptr;
	if (initialData != nullptr) 
	{
		memset( &subresourceData, 0, sizeof(subresourceData) );
		subresourceData.pSysMem = initialData;
		subresourceData_ptr = &subresourceData;
	}

	// Create it - devices create resources; 
	ID3D11Device *device = m_owner->m_device; 
	HRESULT hrResult = device->CreateBuffer( &bufferDescription, subresourceData_ptr, &m_handle );

	if (SUCCEEDED(hrResult)) 
	{
		// save off options; 
		m_bufferUsage = bufferUsage;
		m_memoryUsage = memoryUsage; 
		m_bufferSize = bufferSize; 
		m_elementSize = elementSize; 
		return true;
	} 
	else 
	{
		return false;
	}


}



bool RenderBuffer::CopyCPUToGPU(void const* data, size_t const byteSize)
{
	// staging or dynamic only & we better have room; 
	ASSERT( !IsStatic() ); 
	ASSERT( byteSize <= m_bufferSize );

	// Map and copy
	// This is a command, so runs using the context
	ID3D11DeviceContext *context = m_owner->m_context;

	// Map (ie, lock and get a writable pointer)
	// Be sure to ONLY write to what you locked
	D3D11_MAPPED_SUBRESOURCE resource; 
	HRESULT hrResult = context->Map( m_handle, 
		0,						  // resource index (for arrays/map layers/etc...)
		D3D11_MAP_WRITE_DISCARD,  // says to discard (don't care) about the memory that was already there
		0u,						  // option to allow this to fail if the resource is in use, 0u means we'll wait...
		&resource );

	if (SUCCEEDED(hrResult)) 
	{
		// we're mapped!  Copy over
		memcpy( resource.pData, data, byteSize ); 

		// unlock the resource (we're done writing)
		context->Unmap( GetHandle(), 0 ); 

		return true; 
	} 
	else 
	{
		return false; 
	}
}

