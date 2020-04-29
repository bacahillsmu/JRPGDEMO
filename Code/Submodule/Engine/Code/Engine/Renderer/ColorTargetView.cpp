#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/Texture.hpp"



#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }


ColorTargetView::ColorTargetView()
{

}

ColorTargetView::~ColorTargetView()
{
	DX_SAFE_RELEASE( m_renderTargetView );
}

void ColorTargetView::CreateForInternalTexture( ID3D11Texture2D* texture, ID3D11Device* device )
{
	DX_SAFE_RELEASE(m_renderTargetView);

	// NOTE:  This could be cached off and stored instead of creating
	// a new one each frame.  It is fairly cheap to do though.
	HRESULT hr = device->CreateRenderTargetView( 
		texture, 
		nullptr, 
		&m_renderTargetView );

	GUARANTEE_OR_DIE( SUCCEEDED(hr), "HRESULT was not created in ColorTargetView::CreateForInternalTexture. See output - DX will usually tell you why." );

	// Cache off size from texture description.
	D3D11_TEXTURE2D_DESC texture2DDesc;
	texture->GetDesc(&texture2DDesc);

	m_size = IntVec2(texture2DDesc.Width, texture2DDesc.Height);
}
