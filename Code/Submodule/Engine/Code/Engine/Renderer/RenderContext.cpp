
// Defines and Includes ---------------------------------------------------------------------------
#include "Engine/Renderer/RenderContext.hpp"
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/EngineCommon.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/VertextBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Core/DepthStencilTargetView.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Core/NamedStrings.hpp"
//#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Core/Utils.hpp"
#include "Engine/Profile/Profile.hpp"
#include "Engine/Job/MakeImageFromTextureJob.hpp"
#include "Engine/Job/SaveImageJob.hpp"



/*
This is gl stuff we no longer need
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
*/

//---------------------------------------------------------------------------
// D3D11 STUFF
//---------------------------------------------------------------------------

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>

// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

//#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

template<typename D3D11Type>
void DX_SAFE_RELEASE2(D3D11Type*& ptr)
{
	if (ptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

// ------------------------------------------------------------------------------------------------
RenderContext* g_theRenderer = nullptr;


// Callbacks
static bool DirectionalLightColorEvent(EventArgs& args)
{
	std::string colorName = args.GetValue("color", "white");	

	if(args.GetValue("color", "white") == "white")
	{
		g_theRenderer->m_cpuLightBuffer.lights[0].color = Rgba::WHITE;
		g_theRenderer->m_lightBufferDirty = true;
	}
	else if(args.GetValue("color", "white") == "red")
	{
		g_theRenderer->m_cpuLightBuffer.lights[0].color = Rgba::RED;
		g_theRenderer->m_lightBufferDirty = true;
	}
	else if(args.GetValue("color", "white") == "green")
	{
		g_theRenderer->m_cpuLightBuffer.lights[0].color = Rgba::GREEN;
		g_theRenderer->m_lightBufferDirty = true;
	}
	else if(args.GetValue("color", "white") == "yellow")
	{
		g_theRenderer->m_cpuLightBuffer.lights[0].color = Rgba::YELLOW;
		g_theRenderer->m_lightBufferDirty = true;
	}

	return true;
}

static bool AmbientLightColorEvent(EventArgs& args)
{
	std::string colorName = args.GetValue("color", "white");	

	if(args.GetValue("color", "white") == "white")
	{
		g_theRenderer->m_cpuLightBuffer.ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		g_theRenderer->m_lightBufferDirty = true;
	}
	else if(args.GetValue("color", "white") == "red")
	{
		g_theRenderer->m_cpuLightBuffer.ambient = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		g_theRenderer->m_lightBufferDirty = true;
	}
	else if(args.GetValue("color", "white") == "green")
	{
		g_theRenderer->m_cpuLightBuffer.ambient = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		g_theRenderer->m_lightBufferDirty = true;
	}
	else if(args.GetValue("color", "white") == "yellow")
	{
		g_theRenderer->m_cpuLightBuffer.ambient = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
		g_theRenderer->m_lightBufferDirty = true;
	}

	return true;
}

static bool DirectionalLightDirectionEvent(EventArgs& args)
{
	// Forward
	if(args.GetValue("direction", Vec3(0.0f, 0.0f, 1.0f)) == Vec3(0.0f, 0.0f, 1.0f))
	{
		g_theRenderer->m_cpuLightBuffer.lights[0].direction = Vec3(0.0f, 0.0f, 1.0f);
		g_theRenderer->m_lightBufferDirty = true;
	}
	else
	{
		g_theRenderer->m_cpuLightBuffer.lights[0].direction = args.GetValue("direction", Vec3(0.0f, 0.0f, 1.0f));
		g_theRenderer->m_lightBufferDirty = true;
	}	

	return true;
}



// ------------------------------------------------------------------------------------------------
RenderContext::RenderContext( WindowContext* windowContext )
{
	D3D11Setup(windowContext);

	m_immediateVBO		= new VertexBuffer(this);
	m_immediateMesh		= new GPUMesh(this);
	m_timeBuffer		= new UniformBuffer(this);
	m_modelBuffer		= new UniformBuffer(this);
}

// ------------------------------------------------------------------------------------------------
RenderContext::~RenderContext()
{
	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_immediateMesh;
	m_immediateMesh = nullptr;

	delete m_timeBuffer;
	m_timeBuffer = nullptr;

	delete m_modelBuffer;
	m_modelBuffer = nullptr;

	delete m_gpuLightBuffer;
	m_gpuLightBuffer = nullptr;
}

// ------------------------------------------------------------------------------------------------
void RenderContext::Init()
{
	ProfileTimer profileTimer("RenderContext Init: ", 0.0);

	CreateDefaultModelBuffer();

	LoadDefaultSamplers();

	LoadDefaultTextureViews();
}

// ------------------------------------------------------------------------------------------------
void RenderContext::Startup()
{
	// Subscribe;
	g_theEventSystem->SubscriptionEventCallbackFunction( "dl_color", DirectionalLightColorEvent );
	g_theEventSystem->SubscriptionEventCallbackFunction( "al_color", AmbientLightColorEvent );
	g_theEventSystem->SubscriptionEventCallbackFunction( "dl_dir", DirectionalLightDirectionEvent );

	// Creating the RenderContext light buffer
	m_cpuLightBuffer = light_buffer_t();
	m_cpuLightBuffer.ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_cpuLightBuffer.specFactor = 1.0f;
	m_cpuLightBuffer.specPower = 64.0f;

	// Create a basic light; Setting all values to blank/nothing/empty
	light_t basicLight;
	basicLight.color = Rgba::BLACK;
	basicLight.position = Vec3(0.0f, 0.0f, 0.0f);
	basicLight.direction = Vec3(0.0f, 0.0f, 0.0f);
	basicLight.diffuseAttenuation = Vec3( 0.0f, 1.0f, 0.0f );
	basicLight.specularAttenuation = Vec3( 0.0f, 1.0f, 0.0f );

	// Add all our basic lights to our light buffer;
	// Keep in mind, position 0 is the directional light;
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		m_cpuLightBuffer.lights[i] = basicLight;
	}	

	m_effectCamera = new Camera();
}

ColorTargetView* RenderContext::GetFrameColorTarget()
{
	//return m_renderContextColorTargetView;
	return m_defaultColorTargetView;
}

void RenderContext::CopyTextureToBackBuffer( Texture2D* source )
{
	ID3D11Texture2D *back_buffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);	

	m_context->CopyResource((ID3D11Resource*)back_buffer, source->m_handle);

	DX_SAFE_RELEASE(back_buffer);
}

// -----------------------------------------------------------------------
void RenderContext::CopyTextureToTexture( Texture2D* destination, Texture2D* source )
{
	m_context->CopyResource(destination->m_handle, source->m_handle);
}

// -----------------------------------------------------------------------
void RenderContext::ApplyEffect( Material* material )
{
	m_effectCamera->SetOrthographicProjection(Vec2(0.0f, 0.0f), Vec2((float)m_effectsColorTargetView->m_size.x, (float)m_effectsColorTargetView->m_size.y));
	m_effectCamera->SetColorTargetView( m_effectsColorTargetView );
	BeginCamera( m_effectCamera );

	// bind texture
	TextureView* textureView = m_defaultColorTexture->CreateTextureView();
	material->SetTextureView(0u, textureView);
	BindMaterial( material );

	Draw(3);

	EndCamera();

	CopyTextureToTexture(m_defaultColorTexture, m_effectsColorTexture);
	BindTextureView(0u, nullptr);

	delete textureView;
	textureView = nullptr;
}

// -----------------------------------------------------------------------
void RenderContext::ApplyEffect( ColorTargetView* destination, Texture2D* source, Material* material )
{
	UNUSED(destination);
	UNUSED(source);
	UNUSED(material);
}

// -----------------------------------------------------------------------
void RenderContext::CheckAndSaveScreenshot()
{
	if(!m_saveScreenshot)
	{
		return;
	}

	Texture2D* texture = new Texture2D(this);	
	texture->CreateTextureMatchingTextureForCPURead(*m_defaultColorTexture);
	texture->MakeCopyOfTexture(*m_defaultColorTexture);
	Image* image = new Image(texture->m_dimensions);

	MakeImageFromTextureJob* makeImageJob = new MakeImageFromTextureJob(texture, image);
	SaveImageJob* saveImageJob = new SaveImageJob(image, m_screenshotPathWithName);
	
	makeImageJob->AddSuccessor(saveImageJob);
	g_theJobSystem->Run(makeImageJob);
	g_theJobSystem->Run(saveImageJob);

	m_saveScreenshot = false;
	m_screenshotPathWithName = "";
}

// -----------------------------------------------------------------------
void RenderContext::RequestScreenshot(std::string screenshotPathWithName_)
{
	m_saveScreenshot = true;
	m_screenshotPathWithName = screenshotPathWithName_;
}

// -----------------------------------------------------------------------
// Texture View
// -----------------------------------------------------------------------
void RenderContext::LoadDefaultTextureViews()
{
	// Create a blank texture view for white, black, and flat;
	Image smallWhite = Image(IntVec2(1, 1), Rgba::WHITE);
	m_blankWhiteTexture = CreateTextureViewFromImage(smallWhite);

	Image smallBlack = Image(IntVec2(1, 1), Rgba::BLACK);
	m_blankBlackTexture = CreateTextureViewFromImage(smallBlack);

	Image smallBlue = Image(IntVec2(1, 1), Rgba::BLUE);
	m_blankBlueTexture = CreateTextureViewFromImage(smallBlue);

	Image smallRed = Image(IntVec2(1, 1), Rgba::RED);
	m_blankRedTexture = CreateTextureViewFromImage(smallRed);

	Image smallGreen = Image(IntVec2(1, 1), Rgba(0.0f, 1.0f, 0.0f, 0.2f));
	m_blankGreenTexture = CreateTextureViewFromImage(smallGreen);

	Image smallYellow = Image(IntVec2(1, 1), Rgba::YELLOW);
	m_blankYellowTexture = CreateTextureViewFromImage(smallYellow);

	Image smallFlat = Image(IntVec2(1, 1), Rgba(0.5f, 0.5f, 1.0f, 1.0f));
	m_blankFlatTexture = CreateTextureViewFromImage(smallFlat);
}

// -----------------------------------------------------------------------
void RenderContext::BindTextureView( uint slot, TextureView* textureView )
{
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	if(textureView != nullptr)
	{
		shaderResourceView = textureView->m_view;
	}
	else
	{
		shaderResourceView = m_blankWhiteTexture->m_view;
	}

	m_context->PSSetShaderResources( slot, 1u, &shaderResourceView );
}

// -----------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, TextureView* textureView )
{
	BindTextureView(slot, textureView);

	if(textureView != nullptr)
	{
		BindSampler(slot, textureView->m_sampler);
	}
	else
	{
		BindSampler(slot, nullptr);
	}
}

// -----------------------------------------------------------------------
TextureView* RenderContext::CreateTextureViewFromImage(Image& image)
{
	TextureView* textureView = nullptr;
	Texture2D* texture = new Texture2D(this);
	bool isSuccessfulTextureLoad = texture->LoadTextureFromImage(image);

	// Try to load the texture. Return nullptr if cannot load.
	if(isSuccessfulTextureLoad)
	{
		textureView = texture->CreateTextureView();
	}

	delete texture;
	texture = nullptr;
	return textureView;
}

// -----------------------------------------------------------------------
TextureView* RenderContext::CreateOrGetTextureViewFromFile( const char* filename )
{
	TextureView* textureView = nullptr;

	// If it already exists then return it
	std::map<std::string, TextureView*>::const_iterator textureViewMapPair = m_cachedTextureViews.find(filename);
	if(textureViewMapPair != m_cachedTextureViews.end())
	{
		return textureViewMapPair->second;
	}

	Texture2D* texture = new Texture2D(this);

	bool isSuccessfulTextureLoad = texture->LoadTextureFromFile(filename);

	// Try to load the texture. Return nullptr if cannot load.
	if(!isSuccessfulTextureLoad)
	{
		delete texture;
		texture = nullptr;
		m_cachedTextureViews[filename] = nullptr;
		return nullptr;
	}
	else
	{
		textureView = texture->CreateTextureView();

		delete texture;
		texture = nullptr;
		m_cachedTextureViews[filename] = textureView;
		return textureView;
	}	
}

// -----------------------------------------------------------------------
TextureView* RenderContext::CreateOrGetTextureViewFromFile(std::string& filename)
{
	TextureView* textureView = nullptr;

	// If it already exists then return it
	std::map<std::string, TextureView*>::const_iterator textureViewMapPair = m_cachedTextureViews.find(filename);
	if (textureViewMapPair != m_cachedTextureViews.end())
	{
		return textureViewMapPair->second;
	}

	Texture2D* texture = new Texture2D(this);

	bool isSuccessfulTextureLoad = texture->LoadTextureFromFile(filename);

	// Try to load the texture. Return nullptr if cannot load.
	if (!isSuccessfulTextureLoad)
	{
		delete texture;
		texture = nullptr;
		m_cachedTextureViews[filename] = nullptr;
		return nullptr;
	}
	else
	{
		textureView = texture->CreateTextureView();

		delete texture;
		texture = nullptr;
		m_cachedTextureViews[filename] = textureView;
		return textureView;
	}
}

// -----------------------------------------------------------------------
TextureView* RenderContext::CreateOrGetTextureViewFromFile(const std::string& filename)
{
	std::string f = filename;

	return CreateOrGetTextureViewFromFile(f);
}

// -----------------------------------------------------------------------
// Shader
// -----------------------------------------------------------------------
void RenderContext::BindShader( const std::string& shaderFile )
{
	BindShader(GetOrCreateShader(shaderFile.c_str()));
}

// -----------------------------------------------------------------------
void RenderContext::BindShader( Shader* shader )
{
	m_currentShader = shader;

	// Binding a shader for now is just binding
	// these two programmable shaders
	// This method will eventually do a *lot* more
	// as it sets up nearly every part of the pipeline; 
	m_context->VSSetShader( shader->m_vertexStage.m_vs, nullptr, 0u ); 
	m_context->PSSetShader( shader->m_fragmentStage.m_ps, nullptr, 0u );

	// **NEW** - before a draw can happen, 
	// We need to describe the input to the shader
	// TODO: only create an input layout if the vertex type changes; 
	// TODO: When different vertex types come on-line, look at the current bound
	//       input streams (VertexBuffer) for the layout
	// TODO: m_currentShader->CreateInputLayoutFor( VertexPCU::LAYOUT );	

	shader->UpdateBlendStateIfDirty(this);
	shader->UpdateDepthStateIfDirty(this);
	shader->UpdateRasterStateIfDirty(this);

	float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
	m_context->OMSetBlendState(shader->m_blendState,
		black,
		0xffffffff);

	m_context->OMSetDepthStencilState( shader->m_depthStencilState, 1u );

	m_context->RSSetState( shader->m_rasterState );
}

// -----------------------------------------------------------------------
Shader* RenderContext::GetOrCreateShader( const char* shaderFileName )
{
	std::map<std::string, Shader*>::const_iterator mapPair = m_loadedShaders.find(shaderFileName);
	if(mapPair == m_loadedShaders.end())
	{
		Shader* newShader = CreateShaderFromFile(shaderFileName);
		m_loadedShaders[shaderFileName] = newShader;
		return newShader;
	}

	return mapPair->second;
}

// -----------------------------------------------------------------------
// Material
// -----------------------------------------------------------------------
Material* RenderContext::GetOrCreateMaterial(const std::string& filename)
{
	std::map<std::string, Material*>::const_iterator materialMapPair = m_materialDatabase.find(filename);
	if(materialMapPair == m_materialDatabase.end())
	{
		Material* material = new Material(this);
		material->CreateMaterialFromFile(filename.c_str());

		m_materialDatabase[filename] = material;

		return material;
	}

	return materialMapPair->second;
}

// -----------------------------------------------------------------------
// Mesh
// -----------------------------------------------------------------------
GPUMesh* RenderContext::GetOrCreateMesh( const std::string& filename )
{
	std::map<std::string, GPUMesh*>::const_iterator meshMapPair = m_meshDatabase.find(filename);
	if(meshMapPair == m_meshDatabase.end())
	{
		GPUMesh* mesh = new GPUMesh(this);
		mesh->CreateMeshFromFile(filename.c_str());

		m_meshDatabase[filename] = mesh;

		return mesh;
	}

	return meshMapPair->second;
}

// -----------------------------------------------------------------------
void RenderContext::CreateAndRegisterGPUMesh( CPUMesh* cpuMesh, const std::string& filename )
{
	std::map<std::string, GPUMesh*>::const_iterator meshMapPair = m_meshDatabase.find(filename);
	if(meshMapPair == m_meshDatabase.end())
	{
		GPUMesh* mesh = new GPUMesh(this);
		mesh->CreateFromCPUMesh(cpuMesh);

		m_meshDatabase[filename] = mesh;
	}
}

// -----------------------------------------------------------------------
// Sampler
// -----------------------------------------------------------------------
void RenderContext::LoadDefaultSamplers()
{
	// Set up sampler point
	Sampler* pointSampler = new Sampler();
	pointSampler->SetFilterModes(FILTER_MODE_LINEAR, FILTER_MODE_POINT);
	m_cachedSamplers[SAMPLE_MODE_POINT] = pointSampler;

	// Set up sampler linear
	Sampler* linearSampler = new Sampler();
	linearSampler->SetFilterModes(FILTER_MODE_LINEAR, FILTER_MODE_LINEAR);
	m_cachedSamplers[SAMPLE_MODE_LINEAR] = linearSampler;
}

// -----------------------------------------------------------------------
Sampler* RenderContext::GetCachedSampler( SampleMode sampleMode )
{
	return m_cachedSamplers[sampleMode];
}

// -----------------------------------------------------------------------
void RenderContext::BindSampler( uint slot, Sampler* sampler )
{
	if(sampler == nullptr)
	{
		sampler = m_cachedSamplers[SAMPLE_MODE_DEFAULT];
	}

	// Create the DX Handle
	sampler->CreateStateIfDirty(this);

	ID3D11SamplerState* handle = sampler->GetHandle();
	m_context->PSSetSamplers(slot, 1u, &handle);
}

// -----------------------------------------------------------------------
// Vertex Stream
// -----------------------------------------------------------------------
void RenderContext::BindVertexStream( VertexBuffer* vertexBuffer )
{
	ID3D11Buffer* handle = vertexBuffer->GetHandle();

	// Bind the input stream; 
	uint stride = (uint)vertexBuffer->m_elementSize; /*unsigned int vertexBuffer->m_elementSize;*/ // Colin said I'd fuck this up. Change this value to something...
	uint offset = 0u;
	m_context->IASetVertexBuffers( 0,    // Start slot index
		1,                            // Number of buffers we're binding
		&handle, // Array of buffers
		&stride,                // Stride (read: vertex size, or amount we move forward each vertex) for each buffer
		&offset );             // Offset into each buffer (array - we are only passing one. 
}

// -----------------------------------------------------------------------
// Index Stream
// -----------------------------------------------------------------------
void RenderContext::BindIndexStream( IndexBuffer *indexBuffer ) 
{
	ID3D11Buffer* handle = nullptr; 
	if (indexBuffer != nullptr) 
	{
		handle = indexBuffer->GetHandle(); 
	}

	m_context->IASetIndexBuffer( 
		handle, 
		DXGI_FORMAT_R32_UINT,   // 32-bit indices;            
		0 );					// byte offset 
}

// -----------------------------------------------------------------------
// Uniform Buffer
// -----------------------------------------------------------------------
void RenderContext::BindUniformBuffer( uint slot, UniformBuffer* uniformBuffer )
{
	// The API allows us to bind multiple constant buffers at once
	// and binds to each stage separately.  For simplicity, we'll
	// just assume a slot is uniform for the entire pipeline
	ID3D11Buffer* buffer = (uniformBuffer != nullptr) ? uniformBuffer->GetHandle() : nullptr; 
	m_context->VSSetConstantBuffers( slot, 1u, &buffer ); 
	m_context->PSSetConstantBuffers( slot, 1u, &buffer ); 
}

//---------------------------------------------------------------------------
// D3D11 FUNCTIONS
//---------------------------------------------------------------------------
bool RenderContext::D3D11Setup( WindowContext* windowContext )
{
	HWND hwnd = (HWND)windowContext->m_hwnd;

	// Creation Flags
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=device_flags+%7C%3D+D3D11_CREATE_DEVICE_DEBUG%3B
	uint device_flags = 0U;
#define RENDER_DEBUG
#if defined(RENDER_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

	// Setup our Swap Chain
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=DXGI_SWAP_CHAIN_DESC

	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset( &swap_desc, 0, sizeof(swap_desc) );

	// fill the swap chain description struct
	swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;                                  
	swap_desc.OutputWindow = hwnd;                                // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1;                               // how many multi-samples (1 means no multi sampling)

																  // Get window size information
	RECT client_rect; 
	::GetClientRect( hwnd, &client_rect ); 
	uint width = client_rect.right - client_rect.left; 
	uint height = client_rect.bottom - client_rect.top; 

	// Default options.
	swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	swap_desc.BufferDesc.Width = width;
	swap_desc.BufferDesc.Height = height;


	// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain( 
		nullptr,					// Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,	// Driver Type - We want to use the GPU (HARDWARE)
		nullptr,					// Software Module - DLL that implements software mode (we do not use)
		device_flags,				// device creation options
		nullptr,					// feature level (use default)
		0U,							// number of feature levels to attempt
		D3D11_SDK_VERSION,			// SDK Version to use
		&swap_desc,					// Description of our swap chain
		&m_swapChain,				// Swap Chain we're creating
		&m_device,					// [out] The device created
		nullptr,					// [out] Feature Level Acquired
		&m_context );				// Context that can issue commands on this pipe.

		// SUCCEEDED & FAILED are macros provided by Windows to checking
		// the results.  Almost every D3D call will return one - be sure to check it.

		// Setting the rasterizer state. We are changing whether we think counter clockwise vs. clockwise drawing.
		// Default clockwise, but we want counter clockwise.
		//CreateAndSetDefaultRasterState();

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return SUCCEEDED(hr);
}


void RenderContext::D3D11Cleanup()
{
	DX_SAFE_RELEASE2(m_swapChain);
	DX_SAFE_RELEASE2(m_context);
	DX_SAFE_RELEASE2(m_device);
}



void RenderContext::BindModelMatrix( const Matrix4x4 &model ) 
{
	model_buffer_t buffer; 
	buffer.model = model; 

	m_modelBuffer->CopyCPUToGPU( &buffer, sizeof(buffer));
	BindUniformBuffer( UNIFORM_SLOT_MODEL, m_modelBuffer );
}

void RenderContext::SetAmbientLight( const Rgba& color, float intensity )
{
	m_ambientLight = Vec4(color.r, color.g, color.b, intensity);

	m_cpuLightBuffer.ambient = m_ambientLight;
	m_lightBufferDirty = true;
}

void RenderContext::SetSpecFactor( float specFactor )
{
	m_specFactor = specFactor;

	m_cpuLightBuffer.specFactor = m_specFactor;
	m_lightBufferDirty = true;
}

void RenderContext::SetSpecPower( float specPower )
{
	m_specPower = specPower;

	m_cpuLightBuffer.specPower = m_specPower;
	m_lightBufferDirty = true;
}

void RenderContext::SetEmissiveFactor( float emissiveFactor )
{
	m_emissiveFactor = emissiveFactor;

	m_cpuLightBuffer.emissiveFactor = m_emissiveFactor;
	m_lightBufferDirty = true;
}

void RenderContext::TurnOnDirectionalLight()
{
	m_cpuLightBuffer.lights[0].color.a = 1.0f;
	m_lightBufferDirty = true;
}

void RenderContext::TurnOffDirectionalLight()
{
	m_cpuLightBuffer.lights[0].color.a = 0.0f;
	m_lightBufferDirty = true;
}

void RenderContext::UpdateDirectionalLightIntensity( float intensity )
{
	m_cpuLightBuffer.lights[0].color.a = intensity;
	m_lightBufferDirty = true;
}

Vec3 RenderContext::UpdateAndReturnDirectionalLightDirection( float deltaSeconds )
{
	Vec3 dynamicLightDirection = Vec3(0.0f, 0.0f, SinDegrees(deltaSeconds * 200.0f));
	
	m_cpuLightBuffer.lights[0].direction = dynamicLightDirection;
	m_lightBufferDirty = true;

	return dynamicLightDirection;
}

void RenderContext::UpdatePointLightColor( uint slot, const Rgba& color )
{
	ASSERT_OR_DIE(slot > 0u, "The Point Light requested to change color was the Directional Light Slot of 0.");

	m_cpuLightBuffer.lights[slot].color = color;
}

void RenderContext::CreateLight( uint slot, const light_t& info )
{
	if((int)slot == 0)
	{
		m_directionalLightColor = info.color;
	}	

	m_cpuLightBuffer.lights[slot] = info;
	m_lightBufferDirty = true;
}

void RenderContext::DisableLight( uint slot )
{
	m_cpuLightBuffer.lights[slot].color.a = 0.0f;
	m_lightBufferDirty = true;
}

void RenderContext::DisableAllPointLights()
{
	for(int i = 1; i < MAX_LIGHTS; i++)
	{
		m_cpuLightBuffer.lights[i].color.a = 0.0f;
	}
	m_lightBufferDirty = true;
}

void RenderContext::BindMaterial( Material* material )
{
	// Bind the Shader;
	BindShader(material->m_shader);

	// Bind the TextureView;
	for(int slot = 0; slot < material->m_textures.size(); slot++)
	{
		BindTextureView(slot, material->m_textures[slot]);
	}

	// Bind the Sampler;
	for(int slot = 0; slot < material->m_samplers.size(); slot++)
	{
		BindSampler(slot, material->m_samplers[slot]);
	}

	// Bind the material (user) constant buffer if available
	if(material->m_materialBuffer)
	{
		BindUniformBuffer(UNIFORM_SLOT_MATERIAL, material->m_materialBuffer);
	}
}

DepthStencilTargetView* RenderContext::GetFrameDepthStencilTarget()
{
	return m_defaultDepthStencilView;
}

void RenderContext::CreateDefaultModelBuffer()
{
	model_buffer_t buffer; 
	buffer.model = Matrix4x4::IDENTITY;
	m_modelBuffer = new UniformBuffer( this ); 
	m_modelBuffer->CopyCPUToGPU( &buffer, sizeof(buffer));
}

Shader* RenderContext::CreateShaderFromFile( const char* shaderFileName )
{
	std::string shaderFileNameString = shaderFileName;
	
	Shader* shader = new Shader();

	if(shaderFileNameString.find(".shader"))
	{
		shader->CreateFromFile(shaderFileName, g_theRenderer);
	}
	else
	{
		char* outData = nullptr;
		unsigned long bufferSize = CreateFileBuffer(shaderFileName, &outData);
		
		shader->m_vertexStage.LoadShaderFromSource(this, shaderFileName, outData, bufferSize, SHADER_STAGE_VERTEX);
		shader->m_fragmentStage.LoadShaderFromSource(this, shaderFileName, outData, bufferSize, SHADER_STAGE_FRAGMENT);

		delete[] outData;		
	}

	m_loadedShaders[shaderFileName] = shader;
	return shader;

	
}

BitMapFont* RenderContext::CreateOrGetBitmapFontFixedWidth16x16( const char* bitmapFontName )
{
	std::map<std::string, BitMapFont*>::const_iterator mapPair = m_loadedFonts.find(bitmapFontName);
	if(mapPair == m_loadedFonts.end())
	{
		BitMapFont* newBitMapFont = CreateBitMapFontFixedWidth16x16(bitmapFontName);
		m_loadedFonts[bitmapFontName] = newBitMapFont;
		return newBitMapFont;
	}

	return mapPair->second;
}

// ------------------------------------------------------------------------------------------------
BitMapFont* RenderContext::CreateOrGetBitmapFontProportionalWidth(const char* bitmapFontName)
{
	std::map<std::string, BitMapFont*>::const_iterator mapPair = m_loadedFonts.find(bitmapFontName);
	if (mapPair == m_loadedFonts.end())
	{
		BitMapFont* newBitMapFont = CreateBitMapFontProportionalWidth(bitmapFontName);
		m_loadedFonts[bitmapFontName] = newBitMapFont;
		return newBitMapFont;
	}

	return mapPair->second;
}

// ------------------------------------------------------------------------------------------------
BitMapFont* RenderContext::CreateOrGetBitmapFontLoadFromFNTFile(const char* bitmapFontName)
{
	std::map<std::string, BitMapFont*>::const_iterator mapPair = m_loadedFonts.find(bitmapFontName);
	if (mapPair == m_loadedFonts.end())
	{
		BitMapFont* newBitMapFont = CreateBitMapFontLoadFromFNTFile(bitmapFontName);
		m_loadedFonts[bitmapFontName] = newBitMapFont;
		return newBitMapFont;
	}

	return mapPair->second;
}

// ------------------------------------------------------------------------------------------------
void RenderContext::CreateTextureViewFromImage( Image* image, std::string& imageName )
{
	
	// If it already exists then return it
	std::map<std::string, TextureView*>::const_iterator textureViewMapPair = m_cachedTextureViews.find(imageName);
	if(textureViewMapPair != m_cachedTextureViews.end())
	{
		return;
	}

	TextureView* textureView = nullptr;
	Texture2D* texture = new Texture2D(this);

	bool isSuccessfulTextureLoad = texture->LoadTextureFromImage( *image ); 

	// Try to load the texture. Return nullptr if cannot load.
	if(!isSuccessfulTextureLoad)
	{
		delete texture;
		texture = nullptr;
		m_cachedTextureViews[imageName] = nullptr;
	}
	else
	{
		textureView = texture->CreateTextureView();

		delete texture;
		texture = nullptr;
		m_cachedTextureViews[imageName] = textureView;
	}

}

// ------------------------------------------------------------------------------------------------
BitMapFont* RenderContext::CreateBitMapFontFixedWidth16x16( std::string fontName )
{
	std::string pngFilePath = "Data/Fonts/" + fontName + ".png";
	return new BitMapFont(pngFilePath, IntVec2(16, 16));
}

// ------------------------------------------------------------------------------------------------
BitMapFont* RenderContext::CreateBitMapFontProportionalWidth(std::string fontName)
{
	std::string pngFilePath = "Data/Fonts/" + fontName + ".png";
	return new BitMapFont(pngFilePath);
}

// ------------------------------------------------------------------------------------------------
BitMapFont* RenderContext::CreateBitMapFontLoadFromFNTFile(std::string fontName)
{
	std::string pngFilePath = "Data/Fonts/" + fontName + ".png";
	std::string fntFilePath = "Data/Fonts/" + fontName + ".fnt";
	
	return new BitMapFont(pngFilePath, fntFilePath);
}

// ------------------------------------------------------------------------------------------------
Texture2D* RenderContext::CreateOrGetBitMapFontTextureFromFile(const std::string& fontName_)
{
	// If it already exists then return it
	auto textureMapPair = m_cachedTextures.find(fontName_);
	if (textureMapPair != m_cachedTextures.end())
	{
		return textureMapPair->second;
	}

	Texture2D* texture = new Texture2D(this);

	// Try to load the texture, return nullptr if cannot load;
	bool isSuccessfulTextureLoad = texture->LoadTextureFromFile(fontName_);
	if (!isSuccessfulTextureLoad)
	{
		delete texture;
		texture = nullptr;
		m_cachedTextures[fontName_] = nullptr;
		return nullptr;
	}
	else
	{
		m_cachedTextures[fontName_] = texture;
		return texture;
	}
}

// ------------------------------------------------------------------------------------------------
Texture2D* RenderContext::CreateOrGetBitMapFontTextureFromImage(const Image& image_)
{
	// If it already exists then return it
	auto textureMapPair = m_cachedTextures.find(image_.GetImageFilePath());
	if (textureMapPair != m_cachedTextures.end())
	{
		return textureMapPair->second;
	}

	Texture2D* texture = new Texture2D(this);

	// Try to load the texture, return nullptr if cannot load;
	bool isSuccessfulTextureLoad = texture->LoadTextureFromImage(image_);
	if (!isSuccessfulTextureLoad)
	{
		delete texture;
		texture = nullptr;
		m_cachedTextures[image_.GetImageFilePath()] = nullptr;
		return nullptr;
	}
	else
	{
		m_cachedTextures[image_.GetImageFilePath()] = texture;
		return texture;
	}
}

// ------------------------------------------------------------------------------------------------
void RenderContext::BeginFrame()
{
	// Get the back buffer;
	ID3D11Texture2D *back_buffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Default CTV;
	m_renderContextColorTargetView = new ColorTargetView();
	m_renderContextColorTargetView->CreateForInternalTexture(back_buffer, m_device);
	
	// Default DSTV;
	m_defaultDepthTex = new Texture2D(this);
	m_defaultDepthTex->CreateDepthStencilTarget(m_renderContextColorTargetView->m_size.x, m_renderContextColorTargetView->m_size.y);
	m_defaultDepthStencilView = m_defaultDepthTex->CreateDepthStencilTargetView();

	// Other CTV;
	m_defaultColorTexture = new Texture2D(this);
	m_defaultColorTexture->CreateColorTarget(m_renderContextColorTargetView->m_size.x, m_renderContextColorTargetView->m_size.y);
	m_defaultColorTargetView = new ColorTargetView();
	m_defaultColorTargetView->CreateForInternalTexture((ID3D11Texture2D*)m_defaultColorTexture->m_handle, m_device);

	// Effects CTV;
	m_effectsColorTexture = new Texture2D(this);
	m_effectsColorTexture->CreateColorTarget(m_renderContextColorTargetView->m_size.x, m_renderContextColorTargetView->m_size.y);
	m_effectsColorTargetView = new ColorTargetView();
	m_effectsColorTargetView->CreateForInternalTexture((ID3D11Texture2D*)m_effectsColorTexture->m_handle, m_device);

	frame_buffer_t frameBuffer;
	float thisFrameTime = (float)GetCurrentTimeSeconds();

	frameBuffer.time = thisFrameTime;
	frameBuffer.halfTime = thisFrameTime * 0.5f;
	frameBuffer.cosTime = cos(thisFrameTime);
	frameBuffer.sinTime = sin(thisFrameTime);
	
	// Copy the CPU to the GPU (will create or update the buffer)
	m_timeBuffer->CopyCPUToGPU(&frameBuffer, sizeof(frameBuffer));
	BindUniformBuffer(UNIFORM_SLOT_FRAME, m_timeBuffer);

	DX_SAFE_RELEASE( back_buffer ); // I'm done using this - so release my hold on it (does not destroy it!)

}

// ------------------------------------------------------------------------------------------------
void RenderContext::ClearColorTargets( const Rgba& color )
{
	ID3D11RenderTargetView* renderTargetView = m_currentCamera->m_cameraColorTargetView->m_renderTargetView;
	
	float clearColor[4] = {color.r, color.g, color.b, color.a};
	m_context->ClearRenderTargetView( renderTargetView, clearColor );
}

void RenderContext::ClearDepthStencilTarget( float depth /*= 1.0f*/, uint8_t stencil /*= 0U */ )
{
	DepthStencilTargetView* depthStencilTargetView = m_currentCamera->GetDepthStencilTargetView(); 

	// If the Current Camera does not have a DepthStencilTargetView, then use default
	if (depthStencilTargetView == nullptr)
	{
		depthStencilTargetView = m_defaultDepthStencilView;
	}

	m_context->ClearDepthStencilView( depthStencilTargetView->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil );
}

// ------------------------------------------------------------------------------------------------
void RenderContext::EndFrame()
{
	CopyTextureToBackBuffer(m_defaultColorTexture);

	m_swapChain->Present( 0, 0 );

	CheckAndSaveScreenshot();

	delete m_effectsColorTargetView;
	m_effectsColorTargetView = nullptr;

	delete m_effectsColorTexture;
	m_effectsColorTexture = nullptr;

	delete m_renderContextColorTargetView;
	m_renderContextColorTargetView = nullptr;

	delete m_defaultDepthStencilView;
	m_defaultDepthStencilView = nullptr;

	delete m_defaultColorTargetView;
	m_defaultColorTargetView = nullptr;

	delete m_defaultDepthTex;
	m_defaultDepthTex = nullptr;

	delete m_defaultColorTexture;
	m_defaultColorTexture = nullptr;

	
}

// ------------------------------------------------------------------------------------------------
void RenderContext::Shutdown()
{
	delete m_effectCamera;
	m_effectCamera = nullptr;

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_immediateMesh;
	m_immediateMesh = nullptr;

	delete m_timeBuffer;
	m_timeBuffer = nullptr;

	delete m_renderContextColorTargetView;
	m_renderContextColorTargetView = nullptr;

	delete m_defaultColorTargetView;
	m_defaultColorTargetView = nullptr;

	delete m_blankWhiteTexture;
	m_blankWhiteTexture = nullptr;

	delete m_blankBlackTexture;
	m_blankBlackTexture = nullptr;

	delete m_blankFlatTexture;
	m_blankFlatTexture = nullptr;

	delete m_blankBlueTexture;
	m_blankBlueTexture = nullptr;

	delete m_blankRedTexture;
	m_blankRedTexture = nullptr;

	delete m_blankGreenTexture;
	m_blankGreenTexture = nullptr;

	delete m_blankYellowTexture;
	m_blankYellowTexture = nullptr;

	for(int i = 0; i < SAMPLE_MODE_COUNT; i++)
	{
		delete m_cachedSamplers[i];
		m_cachedSamplers[i] = nullptr;
	}

	ClearAndDeleteContentsOfMap(m_loadedShaders);
	ClearAndDeleteContentsOfMap(m_loadedFonts);
	ClearAndDeleteContentsOfMap(m_cachedTextureViews);
	ClearAndDeleteContentsOfMap(m_materialDatabase);
	ClearAndDeleteContentsOfMap(m_meshDatabase);

	ID3D11Debug* debugObject = nullptr;
	HRESULT hr = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugObject);

	D3D11Cleanup();

	if(SUCCEEDED(hr))
	{
		// Uncomment when debugging
		//debugObject->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		// Attach this to D3D11 Devices to give them names, it helps identify which resources have not been released
		//m_handle->SetPrivateData(WKPDID_D3DDebugObjectName, debugName.size(), debugName.c_str());

		DX_SAFE_RELEASE(debugObject);
	}
}

// ------------------------------------------------------------------------------------------------
void RenderContext::BeginCamera( Camera* camera )
{
	m_currentCamera = camera;

	//ID3D11RenderTargetView* cameraRenderTargetView = camera->m_cameraColorTargetView->m_renderTargetView;
	ColorTargetView* cameraColorTargetView = camera->m_cameraColorTargetView;
	//ID3D11RenderTargetView* renderContextRenderTargetView = m_renderContextColorTargetView->m_renderTargetView;
	ID3D11RenderTargetView* renderContextRenderTargetView = m_defaultColorTargetView->m_renderTargetView;

	ID3D11RenderTargetView* renderTargetViewToUse = nullptr; 
	uint colorCount = 0u; 
	
	// If the Camera has a Render Target View, use that one;
	// Else, use the Render Context's Render Target View
	if (cameraColorTargetView != nullptr) 
	{
		renderTargetViewToUse = cameraColorTargetView->m_renderTargetView; 
		colorCount = 1u;
	}
	else
	{
		renderTargetViewToUse = renderContextRenderTargetView;
		colorCount = 1u;
	}

	DepthStencilTargetView* cameraDepthStencilTargetView = camera->m_depthStencilTargetView;

	ID3D11DepthStencilView* depthStencilTargetViewToUse = nullptr; 

	// If the Camera has a Depth Stencil TargetView, use that one;
	// Else, use the Render Context's default
	if(cameraDepthStencilTargetView != nullptr)
	{
		depthStencilTargetViewToUse = cameraDepthStencilTargetView->m_depthStencilView;
	}
	else
	{
		depthStencilTargetViewToUse = m_defaultDepthStencilView->m_depthStencilView;
	}

	m_context->OMSetRenderTargets( colorCount, &renderTargetViewToUse, depthStencilTargetViewToUse );

	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = 0u;
	viewport.TopLeftY = 0u;
	viewport.Width = (float)cameraColorTargetView->GetWidth();
	viewport.Height = (float)cameraColorTargetView->GetHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &viewport);

	camera->UpdateUniformBuffer( this ); 
	BindUniformBuffer( UNIFORM_SLOT_CAMERA, camera->m_cameraUniformBuffer ); 

	// Bind model matrix; 
	BindModelMatrix( Matrix4x4::IDENTITY ); 
	BindUniformBuffer( UNIFORM_SLOT_MODEL, m_modelBuffer ); 
}

// ------------------------------------------------------------------------------------------------
void RenderContext::EndCamera()
{
	m_context->OMSetRenderTargets(0, nullptr, nullptr);
	m_currentCamera = nullptr;
}

// ------------------------------------------------------------------------------------------------
void RenderContext::DrawMesh( GPUMesh* gpuMesh )
{

	if(m_gpuLightBuffer == nullptr)
	{
		m_gpuLightBuffer = new UniformBuffer(this);
	}

	if(m_lightBufferDirty)
	{
		m_lightBufferDirty = false;
		m_gpuLightBuffer->CopyCPUToGPU( &m_cpuLightBuffer, sizeof(m_cpuLightBuffer));
	}

	BindUniformBuffer(UNIFORM_SLOT_LIGHT, m_gpuLightBuffer);

	bool result = m_currentShader->CreateOrUpdateInputLayout(gpuMesh->GetLayout()); 

	if (result) 
	{
		m_context->IASetInputLayout( m_currentShader->m_inputLayout );
	} 
	else 
	{
		GUARANTEE_RECOVERABLE(true, "Shader could not create input layer for gpu mesh.")
	}

	BindVertexStream( gpuMesh->m_vertexBuffer ); 	

	if (gpuMesh->UsesIndexBuffer()) 
	{
		BindIndexStream( gpuMesh->m_indexBuffer ); 
		DrawIndexed( gpuMesh->GetElementCount() ); 
	} 
	else 
	{
		Draw( gpuMesh->GetElementCount() ); 
	}
}

// ------------------------------------------------------------------------------------------------
void RenderContext::DrawModel( Model* model )
{
	g_theRenderer->DrawMesh(model->m_mesh);
}

// ------------------------------------------------------------------------------------------------
void RenderContext::DrawIndexed( uint elementCount ) 
{
	m_context->DrawIndexed( elementCount, 0, 0 );
}

// ------------------------------------------------------------------------------------------------
void RenderContext::Draw( uint vertexCount, uint byteOffset /*= 0u*/ )
{
	m_context->Draw( vertexCount, byteOffset ); 
}

// ------------------------------------------------------------------------------------------------
void RenderContext::DrawVertexArray( int count, const Vertex_PCU* vertices )
{
	BufferLayout buff = BufferLayout(Vertex_PCU::LAYOUT, sizeof(Vertex_PCU), Vertex_PCU::CopyFromMaster);

	bool result = m_currentShader->CreateOrUpdateInputLayout(&buff); 
	if (result) 
	{
		m_context->IASetInputLayout( m_currentShader->m_inputLayout );
	} 
	else 
	{
		GUARANTEE_RECOVERABLE(true, "Shader could not create input layer for vertex pcu.")
	}

	// copy to a vertex buffer
	m_immediateVBO->CopyCPUToGPU( vertices, sizeof(Vertex_PCU), count );

	// bind that vertex buffer
	BindVertexStream( m_immediateVBO ); 

	Draw( count );
 }


 void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexs )
 {
	DrawVertexArray((int)vertexs.size(), &vertexs[0]);
 }

 void RenderContext::DrawVertexArray( Vertex_PCU const* vertices, uint count )
 {
	 UNUSED(vertices);
	 UNUSED(count);
 }


