#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"


class RenderContext;

class IndexBuffer : public RenderBuffer
{
public:

	IndexBuffer( RenderContext* renderContext );

	bool CreateStaticFor( uint const *indices, uint const count );
	bool CopyCPUToGPU( uint const *indices, uint const count );

	inline uint GetIndexCount() { return m_indexCount; }


public: 

	uint m_indexCount;
};
