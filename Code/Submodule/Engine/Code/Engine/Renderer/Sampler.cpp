#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

// Required D3D11 Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )


Sampler::Sampler()
{

}


Sampler::~Sampler()
{
	DX_SAFE_RELEASE( m_handle );
}

Sampler::Sampler( const std::string& samplerType )
{
	if(samplerType == "linear")
	{
		m_minFilter = FILTER_MODE_LINEAR;
		m_magFilter = FILTER_MODE_LINEAR;
	}
	else if(samplerType == "point")
	{
		m_minFilter = FILTER_MODE_POINT;
		m_magFilter = FILTER_MODE_POINT;
	}
}

// Static -----------------------------------------------------------------------------------------
static D3D11_FILTER DXGetFilter(FilterMode minfilter, FilterMode magFilter)
{
	if(minfilter == FILTER_MODE_POINT)
	{
		switch(magFilter)
		{
		case FILTER_MODE_POINT:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;

		case FILTER_MODE_LINEAR:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		};
	}
	else
	{
		switch(magFilter)
		{
		case FILTER_MODE_POINT:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;

		case FILTER_MODE_LINEAR:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		};
	}

	GUARANTEE_RECOVERABLE(true, "Selected combination of filters was not found. Using D3D11_FILTER_MIN_MAG_MIP_POINT instead of whatever the requested.");
	
	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

void Sampler::CreateStateIfDirty( RenderContext* renderContext )
{
	if(!m_isDirty)
	{
		return;
	}

	// Release old state.
	DX_SAFE_RELEASE( m_handle );

	// Create new state.
	D3D11_SAMPLER_DESC samplerDescription;
	memset(&samplerDescription, 0, sizeof(samplerDescription));

	samplerDescription.Filter = DXGetFilter(m_minFilter, m_magFilter);
	samplerDescription.MaxAnisotropy = 1u;

	// Set texture to wrap on UV, but clamp on W (mip). 
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	samplerDescription.MinLOD = -FLT_MAX;
	samplerDescription.MaxLOD = FLT_MAX;
	samplerDescription.MipLODBias = 0.0f;

	samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ID3D11Device* d3dDevice = renderContext->m_device;
	d3dDevice->CreateSamplerState( &samplerDescription, &m_handle );

	m_isDirty = false;
	if(m_handle == nullptr)
	{
		GUARANTEE_RECOVERABLE(true, "Handle was not set when creating state in Sampler.");
	}
}

void Sampler::SetFilterModes( FilterMode minFilter, FilterMode magFilter )
{
	m_minFilter = minFilter;
	m_magFilter = magFilter;
	m_isDirty = true;
}

ID3D11SamplerState* Sampler::GetHandle()
{
	return m_handle;
}







