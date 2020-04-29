#pragma once
#include "Engine/Math/IntVec2.hpp"

struct ID3D11Resource;
struct ID3D11DepthStencilView;

class DepthStencilTargetView
{

public:

	// Constructor/Deconstructor
	DepthStencilTargetView();
	~DepthStencilTargetView();


	// Gets
	float GetWidth() const;
	float GetHeight() const;

public:

	// D3D11
	ID3D11Resource* m_source					= nullptr;
	ID3D11DepthStencilView* m_depthStencilView	= nullptr;

	IntVec2 m_size								= IntVec2(0);

}; 


