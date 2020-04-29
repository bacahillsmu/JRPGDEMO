#pragma once
#include "Engine/Renderer/RendererTypes.hpp"

class RenderContext;
struct ID3D11SamplerState;



class Sampler
{

public:

	Sampler();
	~Sampler();

	explicit Sampler(const std::string& samplerType);

	
	void CreateStateIfDirty(RenderContext* renderContext);
	void SetFilterModes(FilterMode minFilter, FilterMode magFilter);

	ID3D11SamplerState* GetHandle();



public:

	ID3D11SamplerState *m_handle = nullptr;

	// States
	FilterMode m_minFilter = FILTER_MODE_POINT;
	FilterMode m_magFilter = FILTER_MODE_POINT;
	// FilterMode m_mipFilter = FILTER_MODE_POINT;

	// More options. Someday.
	// ...wrap.
	// ...mip clamps and bias.
	// ...compare samplers.

	bool m_isDirty = true;
};