
// Defines and Includes ---------------------------------------------------------------------------
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DepthStencilTargetView.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"

#include <d3d11.h>
#include <DXGI.h>

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

Texture::Texture( RenderContext* renderContext )
{
	m_owner = renderContext;
}

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_handle);
}

Texture2D::Texture2D( RenderContext* renderContext )
	:Texture(renderContext)
{
}

Texture2D::~Texture2D()
{

}

// Static -----------------------------------------------------------------------------------------
static uint DXBindFromUsage( uint usage ) 
{
	uint binds = 0u; 

	if (usage & TEXTURE_USAGE_TEXTURE_BIT) 
	{
		binds |= D3D11_BIND_SHADER_RESOURCE; 
	}
	if (usage & TEXTURE_USAGE_COLOR_TARGET_BIT) 
	{
		binds |= D3D11_BIND_RENDER_TARGET; 
	}
	if (usage & TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT) 
	{
		binds |= D3D11_BIND_DEPTH_STENCIL;
	}

	return binds; 
}

bool Texture2D::LoadTextureFromFile( std::string const &filename ) 
{
	Image image(filename.c_str());
	if (image.GetDimensions() == IntVec2(0, 0)) 
	{
		return false; 
	}

	return LoadTextureFromImage( image ); 
}

bool Texture2D::LoadTextureFromImage( Image const &image ) 
{
	//delete m_handle;
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device* device = m_owner->m_device; 

	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT;
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	// Setup the Texture Description (what the resource will be like on the GPU)
	D3D11_TEXTURE2D_DESC textureDescription;
	memset(&textureDescription, 0, sizeof(textureDescription));

	IntVec2 dimensions = image.GetDimensions();

	textureDescription.Width = dimensions.x;
	textureDescription.Height = dimensions.y;
	textureDescription.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	textureDescription.ArraySize = 1; // only one texture
	textureDescription.Usage = RenderBuffer::DXMemoryUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	textureDescription.BindFlags = DXBindFromUsage(m_textureUsage);   // only allowing render target for mipmap generation
	textureDescription.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	textureDescription.MiscFlags = 0U;  

	// Multi sampling
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;

	// Setup Initial Data
	// pitch is how many bytes is a single row of pixels;  
	uint pitch = dimensions.x * image.GetBytesPerPixel(); // 4 bytes for an R8G8B8A8 format;  Just sub in four if your image is always 4 bytes per channel
	D3D11_SUBRESOURCE_DATA sourceData;
	memset(&sourceData, 0, sizeof(sourceData));
	sourceData.pSysMem = image.GetImageBuffer();
	sourceData.SysMemPitch = pitch;

	// Actually create it
	ID3D11Texture2D* texture2D = nullptr; 
	HRESULT hrResult = device->CreateTexture2D( &textureDescription,
		&sourceData, 
		&texture2D );

	if (SUCCEEDED(hrResult)) 
	{
		m_dimensions = dimensions; 
		m_handle = texture2D;

		return true; 
	} 
	else 
	{
		GUARANTEE_RECOVERABLE(texture2D == nullptr, "Texture2D not set.");
		return false; 
	}
}

bool Texture2D::CreateTextureMatchingTextureForCPURead(const Texture2D& textureToMatch_)
{
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device* device = m_owner->m_device;

	m_textureUsage = 0;
	m_memoryUsage = GPU_MEMORY_USAGE_STAGING;

	// Setup the Texture Description (what the resource will be like on the GPU)
	D3D11_TEXTURE2D_DESC textureDescription;
	memset(&textureDescription, 0, sizeof(textureDescription));

	IntVec2 dimensions = textureToMatch_.m_dimensions;

	textureDescription.Width = dimensions.x;
	textureDescription.Height = dimensions.y;
	textureDescription.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	textureDescription.ArraySize = 1; // only one texture
	textureDescription.Usage = RenderBuffer::DXMemoryUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	textureDescription.BindFlags = DXBindFromUsage(m_textureUsage);   // only allowing render target for mipmap generation
	textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;                            // Determines how I can access this resource CPU side 
	textureDescription.MiscFlags = 0U;

	// Multi sampling
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D* texture2D = nullptr;
	HRESULT hrResult = device->CreateTexture2D(&textureDescription,
		nullptr,
		&texture2D);

	if (SUCCEEDED(hrResult))
	{
		m_dimensions = dimensions;
		m_handle = texture2D;

		return true;
	}
	else
	{
		GUARANTEE_RECOVERABLE(texture2D == nullptr, "Create Texture2D matching not set.");
		return false;
	}
}

void Texture2D::MakeCopyOfTexture(const Texture2D& textureToCopy_)
{
	m_owner->m_context->CopyResource(m_handle, textureToCopy_.m_handle);
}

void Texture2D::MakeImage(Image& outImage_)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D11_MAP mapType = D3D11_MAP_READ;

	HRESULT hResult;
	hResult = m_owner->m_context->Map(m_handle, 0, mapType, 0, &subResource);

	size_t size = (m_dimensions.x * m_dimensions.y) * 4;
	memcpy(outImage_.m_imageRawData, subResource.pData, size);

	m_owner->m_context->Unmap(m_handle, 0);
}

TextureView* Texture2D::CreateTextureView() const
{
	if(m_handle == nullptr)
	{
		return nullptr;
	}
	
	ID3D11Device* device = m_owner->m_device; 
	ID3D11ShaderResourceView* shaderResourceView = nullptr; 
	device->CreateShaderResourceView( m_handle, nullptr, &shaderResourceView );
	//DX_SAFE_RELEASE(device);

	if (shaderResourceView != nullptr) 
	{
		TextureView* textureView2D = new TextureView();

		textureView2D->m_view = shaderResourceView; 

		m_handle->AddRef(); 
		textureView2D->m_source = m_handle; 

		textureView2D->m_dimensions = m_dimensions;

		// done - return!
		return textureView2D; 
	} 
	else 
	{
		return nullptr; 
	}
}

bool Texture2D::CreateDepthStencilTarget( uint width, uint height )
{
	// cleanup old resources before creating new one just in case; 
	//FreeHandles(); 
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device *dd = m_owner->m_device; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT | TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof(texDesc) );

	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = RenderBuffer::DXMemoryUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);    // only allowing render target for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		m_dimensions = IntVec2(width, height); 
		m_handle = tex2D;

		return true; 

	} else {
		ASSERT( tex2D == nullptr ); // should be, just like to have the postcondition; 
		return false; 
	}
}

bool Texture2D::CreateColorTarget( uint width, uint height )
{
	// cleanup old resources before creating new one just in case; 
	//FreeHandles(); 
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device *dd = m_owner->m_device; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT | TEXTURE_USAGE_COLOR_TARGET_BIT; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof(texDesc) );

	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = RenderBuffer::DXMemoryUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);    // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		m_dimensions = IntVec2(width, height); 
		m_handle = tex2D;

		return true; 

	} else {
		ASSERT( tex2D == nullptr ); // should be, just like to have the postcondition; 
		return false; 
	}
}


DepthStencilTargetView* Texture2D::CreateDepthStencilTargetView()
{
	// if we don't have a handle, we can't create a view, so return nullptr
	ASSERT_RETURN_VALUE( m_handle != nullptr, nullptr );

	// get our device - since we're creating a resource
	ID3D11Device *dev = m_owner->m_device; 
	ID3D11DepthStencilView *dsv = nullptr; 

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	memset( &dsv_desc, 0 , sizeof(dsv_desc) );
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	dev->CreateDepthStencilView( m_handle, &dsv_desc, &dsv );

	if (dsv != nullptr) {
		// Awesome, we have one
		DepthStencilTargetView *view = new DepthStencilTargetView();

		// give it the handle to the srv (we do not AddRef, 
		// but are instead just handing this off)
		view->m_depthStencilView = dsv; 

		// Also let the view hold onto a handle to this texture
		// (so both the texture AND the view are holding onto it)
		// (hence the AddRef)
		m_handle->AddRef(); 
		view->m_source = m_handle; 

		// copy the size over for convenience
		view->m_size = m_dimensions;

		// done - return!
		return view; 

	} else {
		return nullptr; 
	}
}

STATIC Texture2D* Texture2D::CreateDepthStencilTarget( RenderContext *renderContext, uint width, uint height )
{
	Texture2D* depthTexture = new Texture2D(renderContext);
	bool result = depthTexture->CreateDepthStencilTarget(width, height);

	if(result)
	{
		return depthTexture;
	}
	else
	{
		return nullptr;
	}
}

STATIC Texture2D* Texture2D::CreateDepthStencilTargetFor( Texture2D *colorTarget )
{
	return CreateDepthStencilTarget( colorTarget->m_owner, colorTarget->m_dimensions.x, colorTarget->m_dimensions.y );
}
