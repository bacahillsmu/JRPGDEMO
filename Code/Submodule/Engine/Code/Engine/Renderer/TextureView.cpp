#include "Engine/Renderer/TextureView.hpp"

#include <d3d11.h>
#include <DXGI.h>

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

TextureView::TextureView()
{

}

TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_view);
	DX_SAFE_RELEASE(m_source);
}
