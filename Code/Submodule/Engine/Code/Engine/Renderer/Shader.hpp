#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include <string>


#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

class BufferLayout;
class RenderContext;
struct ID3D10Blob;

// --------------------------------------------------------------------------------------
class ShaderStage
{
public:

	friend class Shader;

	ShaderStage();
	~ShaderStage();

	ID3DBlob* GetByteCode();

	bool LoadShaderFromSource( RenderContext *colorTargetView, 
		std::string const &filename, 
		void *source, 
		size_t source_len, 
		eShaderStage stage,
		const char* entryPoint=nullptr);

	inline bool IsValid() const { return m_handle != nullptr; }
	
	RenderContext* m_ownerContext = nullptr;
	eShaderStage m_shaderStage;
	union 
	{
		ID3D11Resource *m_handle; 
		ID3D11VertexShader *m_vs; 
		ID3D11PixelShader *m_ps; // sometimes called fragment shader
	};
	ID3DBlob* m_bytecode = nullptr;
};

// --------------------------------------------------------------------------------------
class Shader
{
public:
	Shader();
	~Shader();	

	static D3D11_COMPARISON_FUNC DXGetDepthCompareFunction( const CompareOp usage);
	static DXGI_FORMAT DXGetBufferFormat( const DataType dataType);
	static D3D11_FILL_MODE DXGetRasterFill( const RasterFill fill);
	static D3D11_CULL_MODE DXGetRasterCull( const RasterCull cull);
	static char const* GetEntryForStage( eShaderStage stage );
	static char const* GetShaderModelForStage( eShaderStage stage );
	bool CreateInputLayoutForVertexPCU();
	bool CreateOrUpdateInputLayout(const BufferLayout* bufferLayout);
	bool UpdateBlendStateIfDirty(RenderContext* renderContext);
	bool UpdateDepthStateIfDirty(RenderContext* renderContext);
	bool UpdateRasterStateIfDirty(RenderContext* renderContext);

	void SetDepth( CompareOp op, bool write );
	void SetBlendMode(BlendMode blendMode);
	void SetRasterFill(RasterFill fill);
	void SetRasterCull(RasterCull cull);

	CompareOp GetDepth();
	BlendMode GetBlendMode();

	void CreateFromFile( const std::string& fileName, RenderContext* renderContext );

	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage;

	const BufferLayout* m_currentLayout					= nullptr;
	ID3D11InputLayout* m_inputLayout				= nullptr;

	// Blend
	ID3D11BlendState* m_blendState					= nullptr;
	BlendMode m_blendMode							= BLEND_MODE_ALPHA;
	bool m_blendStateDirty							= true;

	// Depth
	ID3D11DepthStencilState* m_depthStencilState	= nullptr;
	CompareOp m_depthCompareOp						= COMPARE_ALWAYS;
	bool m_writeDepth								= false;
	bool m_depthStateDirty							= true;

	// Raster
	ID3D11RasterizerState* m_rasterState			= nullptr;
	RasterCull m_rasterCull							= RASTER_CULL_BACK;
	RasterFill m_rasterFill							= RASTER_FILL_SOLID;
	bool m_rasterWindCCW							= true;
	bool m_rasterStateDirty							= true;
};

