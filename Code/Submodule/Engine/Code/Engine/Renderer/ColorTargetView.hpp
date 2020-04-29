#pragma once
#include "Engine/Math/IntVec2.hpp"

#include <d3d11.h>
#include <DXGI.h>

class Texture2D;

class ColorTargetView
{

public:

	ColorTargetView();
	~ColorTargetView();
	
	void CreateForInternalTexture(ID3D11Texture2D* texture, ID3D11Device* device);


	int GetWidth() const { return m_size.x; }
	int GetHeight() const { return m_size.y; }


	ID3D11RenderTargetView* m_renderTargetView = nullptr;

	IntVec2 m_size = IntVec2(0, 0);
};