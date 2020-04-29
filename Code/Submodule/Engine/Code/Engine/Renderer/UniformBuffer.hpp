#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

class RenderContext;

class UniformBuffer : public RenderBuffer
{

public:

	UniformBuffer(RenderContext* owner);
	~UniformBuffer();

	bool CopyCPUToGPU( void const *data, size_t const size);


};
