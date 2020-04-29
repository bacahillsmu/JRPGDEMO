#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"


class RenderContext;
struct Vertex_PCU;

class VertexBuffer : public RenderBuffer
{

public:
	VertexBuffer(RenderContext* owner);
	~VertexBuffer();

	bool CreateStaticFor(void* vertices, size_t stride, uint const count);
	bool CopyCPUToGPU(const void* vertices, size_t stride, uint const count);

	inline uint GetVertexCount() { return m_vertexCount; }

	uint m_vertexCount = 0u;
	// BufferLayout const* m_bufferLayout;

	// need to store a stride in here


};



