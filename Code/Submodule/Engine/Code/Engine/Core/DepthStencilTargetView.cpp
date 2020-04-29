#include "Engine/Core/DepthStencilTargetView.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>



DepthStencilTargetView::DepthStencilTargetView()
{

}

DepthStencilTargetView::~DepthStencilTargetView()
{
	DX_SAFE_RELEASE(m_source);
	DX_SAFE_RELEASE(m_depthStencilView);
}

float DepthStencilTargetView::GetWidth() const
{
	return (float)m_size.x;
}

float DepthStencilTargetView::GetHeight() const
{
	return (float)m_size.y;
}
