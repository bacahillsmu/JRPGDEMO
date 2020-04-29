#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Matrix44.hpp"

#include <string>
#include <map>

class WindowContext;
class Texture;
class Texture2D;
class TextureView;
class Sampler;
class BitMapFont;
class Shader;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class Image;
class DepthStencilTargetView;
class GPUMesh;
class CPUMesh;
class Material;
class Model;

typedef unsigned int uint;

class RenderContext
{

	friend class Shader;


public:

	explicit RenderContext( WindowContext* windowContext );
	~RenderContext();

	void Init();
	void Startup();
	void BeginFrame();
	void BeginCamera( Camera* camera );
	void EndCamera();
	void EndFrame();
	void Shutdown();

	// D3D11
	bool D3D11Setup( WindowContext* windowContext );
	void D3D11Cleanup();

	// Target View
	void ClearColorTargets( const Rgba& clearColor );
	void ClearDepthStencilTarget( float depth = 1.0f, uint8_t stencil = 0U );
	
	DepthStencilTargetView* GetFrameDepthStencilTarget();

	// Model Buffer;
	void CreateDefaultModelBuffer();

	// Sampler;
	void LoadDefaultSamplers();

	// TextureViews;
	void LoadDefaultTextureViews();

	// Shaders, Textures, TextureView, Sampler
	void BindTextureView( uint slot, TextureView* textureView );
	void BindTextureView( uint slot, std::string const &name );
	void BindTextureViewWithSampler( uint slot, TextureView* textureView ); 
	void BindSampler( SampleMode mode );
	void BindSampler( uint slot, Sampler* sampler );
	void BindShader(Shader* shader);
	void BindShader(const std::string& shaderFile);
	Shader* CreateShaderFromFile(const char* filename);
	void BindTexture( Texture* texture );
	Shader* GetOrCreateShader(const char* filename);
	TextureView* CreateTextureViewFromImage(Image& image);
	TextureView* CreateOrGetTextureViewFromFile(const char* filename);
	TextureView* CreateOrGetTextureViewFromFile(std::string& filename);
	TextureView* CreateOrGetTextureViewFromFile(const std::string& filename);
	Texture* CreateOrGetTextureFromFile( const char* imageFilePath );
	Texture* CreateTextureFromFile(const char* imageFilePath);
	BitMapFont* CreateOrGetBitmapFontFixedWidth16x16( const char* bitmapFontName );
	BitMapFont* CreateOrGetBitmapFontProportionalWidth( const char* bitmapFontName );
	BitMapFont* CreateOrGetBitmapFontLoadFromFNTFile( const char* bitmapFontName);
	
	void CreateTextureViewFromImage(Image* image, std::string& imageName);
	
	// Draw
	void Draw(uint vertexCount, uint byteOffset = 0u);
	void DrawIndexed(uint elementCount);
	void DrawMesh( GPUMesh* gpuMesh);
	void DrawModel(Model* model);

	void DrawVertexArray( Vertex_PCU const* vertices, uint count ); 
	void DrawVertexArray( int count, const Vertex_PCU* vertices );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexs );

	// Mesh;
	GPUMesh* GetOrCreateMesh(const std::string& filename);
	void CreateAndRegisterGPUMesh(CPUMesh* cpuMesh, const std::string& filename);
	
	void SetBlendMode(BlendMode blendMode);	
	
	// Stream Data
	void BindVertexStream( VertexBuffer* vertexBuffer ); 
	void BindIndexStream( IndexBuffer* indexBuffer );

	// Uniform/Constant Data
	void BindUniformBuffer( uint slot, UniformBuffer* uniformBuffer ); 

	void BindModelMatrix(const Matrix4x4 &model);

	// Lighting;
	void SetAmbientLight( const Rgba& color, float intensity );
	void SetSpecFactor( float specFactor );
	void SetSpecPower( float specPower );
	void SetEmissiveFactor( float emissiveFactor );
	void TurnOnDirectionalLight();
	void TurnOffDirectionalLight();
	void UpdateDirectionalLightIntensity( float intensity );
	Vec3 UpdateAndReturnDirectionalLightDirection( float deltaSeconds );
	void UpdatePointLightColor( uint slot, const Rgba& color );
	void CreateLight( uint slot, const light_t& info );
	void DisableLight( uint slot );
	void DisableAllPointLights();

	// Material
	void BindMaterial( Material* material );
	Material* GetOrCreateMaterial(const std::string& filename);
	std::map<std::string, Material*> m_materialDatabase;

	// Light Buffer
	UniformBuffer* m_gpuLightBuffer = nullptr;
	light_buffer_t m_cpuLightBuffer;
	bool m_lightBufferDirty = false;

	// *new* members
	VertexBuffer* m_immediateVBO = nullptr;
	GPUMesh* m_immediateMesh = nullptr;
	
	UniformBuffer* m_timeBuffer = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;

	Camera* m_currentCamera = nullptr;
	Camera* m_effectCamera = nullptr;
	ColorTargetView* m_renderContextColorTargetView = nullptr;
	
	
	UniformBuffer* m_modelBuffer = nullptr;

	Shader* m_currentShader = nullptr;


	Vec4 m_ambientLight = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float m_ambientLightIntensity = 1.0f;
	float m_specFactor = 1.0f;
	float m_specPower = 64.0f;
	float m_emissiveFactor = 1.0f;

	Rgba m_directionalLightColor = Rgba::WHITE;
	TextureView* m_blankWhiteTexture = nullptr;
	TextureView* m_blankBlackTexture = nullptr;
	TextureView* m_blankBlueTexture = nullptr;
	TextureView* m_blankRedTexture = nullptr;
	TextureView* m_blankGreenTexture = nullptr;
	TextureView* m_blankYellowTexture = nullptr;
	TextureView* m_blankFlatTexture = nullptr;
	
	Sampler* GetCachedSampler(SampleMode sampleMode);


	ColorTargetView* GetFrameColorTarget();
	void CopyTextureToBackBuffer(Texture2D* source);
	void CopyTextureToTexture(Texture2D* destination, Texture2D* source);
	void ApplyEffect(Material* material);
	void ApplyEffect(ColorTargetView* destination, Texture2D* source, Material* material);
	
	DepthStencilTargetView* m_defaultDepthStencilView = nullptr;
	Texture2D* m_defaultDepthTex = nullptr;

	ColorTargetView* m_defaultColorTargetView = nullptr;
	Texture2D* m_defaultColorTexture = nullptr;

	ColorTargetView* m_effectsColorTargetView = nullptr;
	Texture2D* m_effectsColorTexture = nullptr;

	void CheckAndSaveScreenshot();
	void RequestScreenshot(std::string screenshotPathWithName_);
	bool m_saveScreenshot = false;
	std::string m_screenshotPathWithName = "";

private:

	
	//Texture* CreateTextureFromFile( const char* imageFilePath );
	BitMapFont* CreateBitMapFontFixedWidth16x16( std::string fontName );
	BitMapFont* CreateBitMapFontProportionalWidth( std::string fontName );
	BitMapFont* CreateBitMapFontLoadFromFNTFile( std::string fontName );
	Texture2D* CreateOrGetBitMapFontTextureFromFile(const std::string& fontName_);
	Texture2D* CreateOrGetBitMapFontTextureFromImage(const Image& image_);

	Sampler* m_cachedSamplers[SAMPLE_MODE_COUNT];
	
	std::map< std::string, TextureView* > m_cachedTextureViews;
	std::map<std::string, Texture2D*> m_cachedTextures;
	// Switch to: std::map< std::string, Sampler* > m_loadedSamplers;
	std::map< std::string, Shader* > m_loadedShaders;
	std::map< std::string, Texture* > m_loadedTextures;
	std::map< std::string, BitMapFont* > m_loadedFonts;
	std::map< std::string, GPUMesh* > m_meshDatabase;
	
};

extern RenderContext* g_theRenderer;
