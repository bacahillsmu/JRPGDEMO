#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Math/IntVec2.hpp"



class RenderContext;
class TextureView;
struct ID3D11Resource;
class Image;
class DepthStencilTargetView;
class ColorTargetView;

class Texture
{
	

public:
	Texture( RenderContext* renderContext );
	virtual ~Texture();

	RenderContext* m_owner = nullptr;

	ID3D11Resource* m_handle = nullptr;
	GPUMemoryUsage m_memoryUsage;
	TextureUsageBits m_textureUsage;
};

class Texture2D : public Texture
{
public:
	Texture2D( RenderContext* renderContext );
	virtual ~Texture2D(); 

	bool LoadTextureFromFile( std::string const &filename );
	bool LoadTextureFromImage( const Image &image );
	bool CreateTextureMatchingTextureForCPURead(const Texture2D& textureToMatch_);
	void MakeCopyOfTexture(const Texture2D& textureToCopy_);

	void MakeImage(Image& outImage_);

	TextureView* CreateTextureView() const;  
	DepthStencilTargetView* CreateDepthStencilTargetView();  // A04

	bool CreateDepthStencilTarget( uint width, uint height );	
	bool CreateColorTarget(uint width, uint height);

	static Texture2D* CreateDepthStencilTarget( RenderContext *renderContext, uint width, uint height );   // A04
	static Texture2D* CreateDepthStencilTargetFor( Texture2D *colorTarget );
	
	//static Texture2D* CreateMatchingColorTarget(Texture2D* otherTexture);

public:
	IntVec2 m_dimensions = IntVec2(0, 0); 
};