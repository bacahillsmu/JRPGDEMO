#pragma once
#include "Engine/Math/IntVec2.hpp"

class Sampler;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class TextureView
{
public:
	TextureView(); 
	virtual ~TextureView();

	inline void SetSampler( Sampler *sampler ) { m_sampler = sampler; }

public:
	
	ID3D11ShaderResourceView* m_view = nullptr;
	ID3D11Resource *m_source = nullptr;

	Sampler* m_sampler = nullptr;

	IntVec2 m_dimensions = IntVec2(0, 0);
}; 


