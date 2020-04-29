#pragma once
#include "Engine/Renderer/RendererTypes.hpp"

#include <d3d11.h>

typedef unsigned int uint;



class RenderContext;
struct ID3D11Buffer; 

class RenderBuffer
{

public:
	RenderBuffer(RenderContext* owner);
	~RenderBuffer();

	static uint DXBufferUsageFromBufferUsage( eRenderBufferUsageBits bufferUsage );
	static D3D11_USAGE DXMemoryUsageFromMemoryUsage( GPUMemoryUsage memoryUsage );

	size_t GetSize() const;
	bool IsStatic() const;
	bool IsDynamic() const;
	ID3D11Buffer* GetHandle();

	RenderContext* m_owner = nullptr;
	eRenderBufferUsageBits m_bufferUsage;
	GPUMemoryUsage m_memoryUsage;

	size_t m_bufferSize = 0;
	size_t m_elementSize = 0;

	ID3D11Buffer* m_handle = nullptr;

protected:
	
	bool CreateBuffer(void const* initialData,
		size_t bufferSize,
		size_t elementSize,
		eRenderBufferUsageBits bufferUsage,
		GPUMemoryUsage memoryUsage);
	bool CopyCPUToGPU(void const* data, size_t const byteSize);



};