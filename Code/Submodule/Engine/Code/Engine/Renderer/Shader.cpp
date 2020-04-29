#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/BufferLayout.hpp"

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

// NEEDED FOR COMPILING
// Note:  This is not allowed for Windows Store Apps.
// Shaders must be compiled to byte-code off line.
// but for development - great if the program knows how to compile it.
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

// --------------------------------------------------------------------------------------
ShaderStage::ShaderStage()
	:m_handle(nullptr)
{

}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_handle);
	DX_SAFE_RELEASE(m_vs);
	DX_SAFE_RELEASE(m_ps);
	DX_SAFE_RELEASE(m_bytecode);
}

STATIC D3D11_COMPARISON_FUNC Shader::DXGetDepthCompareFunction( const CompareOp usage)
{
	switch (usage) 
	{
		case COMPARE_NEVER:				return D3D11_COMPARISON_NEVER;
		case COMPARE_ALWAYS:			return D3D11_COMPARISON_ALWAYS;
		case COMPARE_EQUAL:				return D3D11_COMPARISON_EQUAL;
		case COMPARE_NOTEQUAL:		    return D3D11_COMPARISON_NOT_EQUAL;
		case COMPARE_LESS:				return D3D11_COMPARISON_LESS;
		case COMPARE_LEQUAL:			return D3D11_COMPARISON_LESS_EQUAL;
		case COMPARE_GREATER:			return D3D11_COMPARISON_GREATER;
		case COMPARE_GEQUAL:			return D3D11_COMPARISON_GREATER_EQUAL;
		default:
		{
			GUARANTEE_RECOVERABLE(false, "Setting D3D11 Depth op to Always"); 
			return D3D11_COMPARISON_LESS_EQUAL;
		}
	}
}

DXGI_FORMAT Shader::DXGetBufferFormat( const DataType dataType )
{
	switch( dataType )
	{
		case DATA_TYPE_FLOAT:		return DXGI_FORMAT_R32_FLOAT;
		case DATA_TYPE_VEC2:		return DXGI_FORMAT_R32G32_FLOAT;
		case DATA_TYPE_VEC3:		return DXGI_FORMAT_R32G32B32_FLOAT;
		case DATA_TYPE_RGBA32:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
		{
			ERROR_AND_DIE("DXGetBufferFormat: Unknown DataType");
		}
	}
}

STATIC D3D11_FILL_MODE Shader::DXGetRasterFill( const RasterFill fill )
{
	switch (fill) 
	{
	case RASTER_FILL_SOLID: return D3D11_FILL_SOLID;
	case RASTER_FILL_WIRE: return D3D11_FILL_WIREFRAME;
	default:
	{
		GUARANTEE_RECOVERABLE(false, "Setting Raster Fill Mode to SOLID"); 
		return D3D11_FILL_SOLID;
	}
	}
}

STATIC D3D11_CULL_MODE Shader::DXGetRasterCull( const RasterCull cull)
{
	switch (cull) 
	{
	case RASTER_CULL_BACK: return D3D11_CULL_BACK;
	case RASTER_CULL_FRONT: return D3D11_CULL_FRONT;
	case RASTER_CULL_NONE: return D3D11_CULL_NONE;
	default:
	{
		GUARANTEE_RECOVERABLE(false, "Setting Raster Cull Mode to BACK"); 
		return D3D11_CULL_BACK;
	}
	}
}

ID3DBlob* ShaderStage::GetByteCode()
{
	return m_bytecode;
}

char const* Shader::GetEntryForStage( eShaderStage stage )
{
	switch (stage) {
	case SHADER_STAGE_VERTEX: return "VertexFunction"; 
	case SHADER_STAGE_FRAGMENT: return "FragmentFunction";
	default: GUARANTEE_OR_DIE(false, "Unknown shader stage"); 
	}
}

char const* Shader::GetShaderModelForStage( eShaderStage stage )
{
	switch (stage) {
	case SHADER_STAGE_VERTEX: return "vs_5_0"; 
	case SHADER_STAGE_FRAGMENT: return "ps_5_0";
	default: GUARANTEE_OR_DIE(false, "Unknown shader stage"); 
	}
}

bool Shader::CreateInputLayoutForVertexPCU()
{
	// Early out - we've already created it; 
	// TODO: If vertex type changes, we need to rebind; 
	if (m_inputLayout != nullptr) {
		return true; 
	}

	// This describes the input data to the shader
	// The INPUT_ELEMENT_DESC describes each element of the structure; 
	// Since we have POSITION, COLOR, UV, we need three descriptions; 
	D3D11_INPUT_ELEMENT_DESC input_description[3]; 

	// initialize memory to 0 (usually a sane default)
	memset( input_description, 0, sizeof(input_description) ); 

	// Map Position
	input_description[0].SemanticName = "POSITION";             // __semantic__ name we gave this input -> float3 pos : POSITION; 
	input_description[0].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
	input_description[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;  // Type this data is (float3/vec3 - so 3 floats)
	input_description[0].InputSlot = 0u;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
	input_description[0].AlignedByteOffset = offsetof( Vertex_PCU, position );   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
	input_description[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
	input_description[0].InstanceDataStepRate = 0u;             // If this were instance data - how often do we step it (0 for vertex data)

	// Map UV  
	input_description[1].SemanticName = "TEXCOORD";             // __semantic__ name we gave this input -> float3 pos : POSITION; 
	input_description[1].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
	input_description[1].Format = DXGI_FORMAT_R32G32_FLOAT;     // Type this data is (float3/vec3 - so 3 floats)
	input_description[1].InputSlot = 0u;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
	input_description[1].AlignedByteOffset = offsetof( Vertex_PCU, uvTexCoords );   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
	input_description[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
	input_description[1].InstanceDataStepRate = 0u;             // If this were instance data - how often do we step it (0 for vertex data)

	// Map Color
	input_description[2].SemanticName = "COLOR";             // __semantic__ name we gave this input -> float3 pos : POSITION; 
	input_description[2].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
	input_description[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;     // Type this data is (float3/vec3 - so 3 floats)
	input_description[2].InputSlot = 0u;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
	input_description[2].AlignedByteOffset = offsetof( Vertex_PCU, color );   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
	input_description[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
	input_description[2].InstanceDataStepRate = 0u;             // If this were instance data - how often do we step it (0 for vertex data)

	ID3D10Blob *vs_bytecode = m_vertexStage.GetByteCode(); 
	ID3D11Device* device = m_vertexStage.m_ownerContext->m_device;

	// Final create the layout
	HRESULT hrResult = device->CreateInputLayout( input_description, 
		ARRAYSIZE(input_description),
		vs_bytecode->GetBufferPointer(), 
		vs_bytecode->GetBufferSize(), 
		&m_inputLayout );   

	return SUCCEEDED(hrResult);
}

bool Shader::CreateOrUpdateInputLayout( const BufferLayout* bufferLayout )
{
	if(bufferLayout == m_currentLayout)
	{
		return true;
	}

	DX_SAFE_RELEASE( m_inputLayout );

	unsigned int attributeCount = bufferLayout->GetAttributeCount();

	D3D11_INPUT_ELEMENT_DESC* description = (D3D11_INPUT_ELEMENT_DESC*)malloc(attributeCount * sizeof(D3D11_INPUT_ELEMENT_DESC));
	memset( description, 0, attributeCount * sizeof(D3D11_INPUT_ELEMENT_DESC) );
	
	// Fill a new input layout
	for(unsigned int x = 0; x < attributeCount; x++)
	{
		description[x].SemanticName = bufferLayout->m_attributes[x].name.c_str();             // __semantic__ name we gave this input -> float3 pos : POSITION; 
		description[x].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
		description[x].Format = DXGetBufferFormat(bufferLayout->m_attributes[x].dataType);  // Type this data is (float3/vec3 - so 3 floats)
		description[x].InputSlot = 0u;                        // Input Pipe this comes from (ignored unless doing instanced rendering)
		description[x].AlignedByteOffset = (uint)bufferLayout->m_attributes[x].memberOffset;   // memory offset this data starts (where is position relative to the vertex, 0 in this case)
		description[x].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
		description[x].InstanceDataStepRate = 0u; 
	}

	// Create the new input layout
	ID3D10Blob *vs_bytecode = m_vertexStage.GetByteCode(); 
	ID3D11Device* device = m_vertexStage.m_ownerContext->m_device;

	// Final create the layout
	HRESULT hrResult = device->CreateInputLayout( description, 
		attributeCount,
		vs_bytecode->GetBufferPointer(), 
		vs_bytecode->GetBufferSize(), 
		&m_inputLayout );	

	free(description);

	m_currentLayout = bufferLayout;

	return SUCCEEDED(hrResult);
}

void Shader::SetBlendMode( BlendMode blendMode )
{
	m_blendMode = blendMode;
	m_blendStateDirty = true;
}

void Shader::SetRasterFill( RasterFill fill )
{
	m_rasterFill = fill;
	m_rasterStateDirty = true;
}

void Shader::SetRasterCull( RasterCull cull )
{
	m_rasterCull = cull;
	m_rasterStateDirty = true;
}

CompareOp Shader::GetDepth()
{
	return m_depthCompareOp;
}

BlendMode Shader::GetBlendMode()
{
	return m_blendMode;
}

bool Shader::UpdateBlendStateIfDirty(RenderContext* renderContext)
{
	// Blend State Dirty?
	if(m_blendStateDirty || (m_blendState == nullptr))
	{
		// Free old state
		DX_SAFE_RELEASE(m_blendState);

		// Make one
		D3D11_BLEND_DESC blendDescription;
		memset( &blendDescription, 0, sizeof(blendDescription) );

		blendDescription.AlphaToCoverageEnable = false;
		blendDescription.IndependentBlendEnable = false;
		blendDescription.RenderTarget[0].BlendEnable = true;

		if(m_blendMode == BLEND_MODE_ALPHA)
		{
			blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			blendDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		}
		else if(m_blendMode == BLEND_MODE_OPAQUE)
		{
			blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			blendDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		}
		else if(m_blendMode == BLEND_MODE_ADDITIVE)
		{
			blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			blendDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		}
		else
		{
			GUARANTEE_RECOVERABLE(true, "Unimplemented blend mode.");
		}

		blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// Create the blend state.
		ID3D11Device *device = renderContext->m_device;
		device->CreateBlendState( &blendDescription, &m_blendState );

		m_blendStateDirty = false;
		return (m_blendState != nullptr);	
	}
	else 
	{
		return false;
	}
}

bool Shader::UpdateDepthStateIfDirty( RenderContext* renderContext )
{
	if ( m_depthStateDirty || ( m_depthStencilState == nullptr ) )
	{
		// Free old state
		DX_SAFE_RELEASE( m_depthStencilState );

		// Make one
		D3D11_DEPTH_STENCIL_DESC dsDescription;
		memset( &dsDescription, 0, sizeof(dsDescription) );

		dsDescription.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and compare is always)
		dsDescription.DepthWriteMask = m_writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO; 
		dsDescription.DepthFunc = DXGetDepthCompareFunction( m_depthCompareOp ); //  

		// Stencil - just use defaults for now; 
		dsDescription.StencilEnable = false; 
		dsDescription.StencilReadMask = 0xFF; 
		dsDescription.StencilWriteMask = 0xFF; 

		D3D11_DEPTH_STENCILOP_DESC default_stencil_op = {}; 
		default_stencil_op.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
		default_stencil_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
		default_stencil_op.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
		default_stencil_op.StencilFunc = D3D11_COMPARISON_ALWAYS;      // function to test against

		// can have different rules setup for front and back face
		dsDescription.FrontFace = default_stencil_op; 
		dsDescription.BackFace = default_stencil_op; 

		ID3D11Device* device = renderContext->m_device;
		device->CreateDepthStencilState( &dsDescription, &m_depthStencilState ); 
		m_depthStateDirty = false;
		return true;
	}
	else
	{
		return false;
	}	
}

bool Shader::UpdateRasterStateIfDirty( RenderContext* renderContext )
{
	if(m_rasterStateDirty || m_rasterState == nullptr)
	{
		// Free old state
		DX_SAFE_RELEASE( m_rasterState );

		// Make one
		D3D11_RASTERIZER_DESC rasDescription;
		memset( &rasDescription, 0, sizeof(rasDescription) );

		rasDescription.FillMode = DXGetRasterFill(m_rasterFill);
		rasDescription.CullMode = DXGetRasterCull(m_rasterCull);
		rasDescription.FrontCounterClockwise = m_rasterWindCCW;

		rasDescription.DepthBias = 0;
		rasDescription.DepthBiasClamp = 0.0f;
		rasDescription.SlopeScaledDepthBias = 0.0f;

		rasDescription.DepthClipEnable = true;
		rasDescription.ScissorEnable = false;
		rasDescription.MultisampleEnable = false;
		rasDescription.AntialiasedLineEnable = false;

		ID3D11Device* device = renderContext->m_device;
		device->CreateRasterizerState( &rasDescription, &m_rasterState ); 

		m_rasterStateDirty = false;
		return true;
	}
	else
	{
		return false;
	}
}

void Shader::SetDepth( CompareOp op, bool write )
{
	if(m_depthCompareOp != op || m_writeDepth != write)
	{
		m_depthCompareOp = op;
		m_writeDepth = write;

		m_depthStateDirty = true;
	}
}

void Shader::CreateFromFile( const std::string& fileName, RenderContext* renderContext )
{
	UNUSED(renderContext);

	// Step 1 fuckme
	// Load XML
	tinyxml2::XMLDocument shaderXMLDoc;
	shaderXMLDoc.LoadFile(fileName.c_str());

	if(shaderXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", fileName.c_str());
		printf("ErrorID:      %i\n", shaderXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", shaderXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", shaderXMLDoc.ErrorName());
	}
	else
	{
		printf("Success with XML Doc: %s\n", fileName.c_str());

		XmlElement* rootElement = shaderXMLDoc.RootElement();

		XmlElement* passElement = rootElement->FirstChildElement("pass");
		std::string sourceFile = ParseXmlAttribute(*passElement, "src", "");

		XmlElement* vertElement = passElement->FirstChildElement("vert");
		std::string vertEntry = ParseXmlAttribute(*vertElement, "entry", GetEntryForStage(SHADER_STAGE_VERTEX));

		XmlElement* fragElement = passElement->FirstChildElement("frag");
		std::string fragEntry = ParseXmlAttribute(*fragElement, "entry", GetEntryForStage(SHADER_STAGE_FRAGMENT));

		char* outData = nullptr;
		unsigned long bufferSize = CreateFileBuffer(sourceFile, &outData);

		m_vertexStage.LoadShaderFromSource(renderContext, sourceFile, outData, bufferSize, SHADER_STAGE_VERTEX, vertEntry.c_str());
		m_fragmentStage.LoadShaderFromSource(renderContext, sourceFile, outData, bufferSize, SHADER_STAGE_FRAGMENT, fragEntry.c_str());

		delete[] outData;


		XmlElement* depthElement = passElement->FirstChildElement("depth");
		m_writeDepth = ParseXmlAttribute(*depthElement, "write", m_writeDepth);
		std::string depthTest = ParseXmlAttribute(*depthElement, "test", "lequal");
		if(depthTest == "never")
		{
			m_depthCompareOp = COMPARE_NEVER;
		}
		else if(depthTest == "always")
		{
			m_depthCompareOp = COMPARE_ALWAYS;
		}
		else if(depthTest == "equal")
		{
			m_depthCompareOp = COMPARE_EQUAL;
		}
		else if(depthTest == "notequal")
		{
			m_depthCompareOp = COMPARE_NOTEQUAL;
		}
		else if(depthTest == "less")
		{
			m_depthCompareOp = COMPARE_LESS;
		}
		else if(depthTest == "lequal")
		{
			m_depthCompareOp = COMPARE_LEQUAL;
		}
		else if(depthTest == "greater")
		{
			m_depthCompareOp = COMPARE_GREATER;
		}
		else if(depthTest == "gequal")
		{
			m_depthCompareOp = COMPARE_GEQUAL;
		}
		XmlElement* blendElement = passElement->FirstChildElement("blend");
		XmlElement* colorElement = blendElement->FirstChildElement("color");
		std::string blendMode = ParseXmlAttribute(*colorElement, "mode", "opaque");
		if(blendMode == "opaque")
		{
			m_blendMode = BLEND_MODE_OPAQUE;
		}
		else if(blendMode == "alpha")
		{
			m_blendMode = BLEND_MODE_ALPHA;
		}
		else if(blendMode == "additive")
		{
			m_blendMode = BLEND_MODE_ADDITIVE;
		}
		XmlElement* rasterElement = passElement->FirstChildElement("raster");
		std::string rasterCull = ParseXmlAttribute(*rasterElement, "cull", "back");
		m_rasterWindCCW = ParseXmlAttribute(*rasterElement, "windCCW", m_rasterWindCCW);
		if(rasterCull == "back")
		{
			m_rasterCull = RASTER_CULL_BACK;
		}
		else if(rasterCull == "front")
		{
			m_rasterCull = RASTER_CULL_FRONT;
		}
		else if(rasterCull == "none")
		{
			m_rasterCull = RASTER_CULL_NONE;
		}
		std::string rasterFill = ParseXmlAttribute(*rasterElement, "fill", "solid");
		if(rasterFill == "solid")
		{
			m_rasterFill = RASTER_FILL_SOLID;
		}
		else if(rasterFill == "wire")
		{
			m_rasterFill = RASTER_FILL_WIRE;
		}
	}
}

//------------------------------------------------------------------------
ID3DBlob* CompileHLSLToByteCode( char const *opt_filename,  // optional: used for error messages
	void const *source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target  )                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0u;
#define DEBUG_SHADERS
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	//compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Fastness (default is level 1)
#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = ::D3DCompile( source_code, 
		source_code_size,                   // plain text source code
		opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
		compile_flags,                      // Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors );                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) 
		{
			char *error_string = (char*) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s", 
				opt_filename, 
				error_string );
			DX_SAFE_RELEASE(errors);
		} 
		else 
		{
			DebuggerPrintf( "Failed with HRESULT: %u", hr ); 
		}
	} 

	// will be nullptr if it failed to compile
	return code;
}

// --------------------------------------------------------------------------------------
Shader::Shader()
{

}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
	DX_SAFE_RELEASE(m_blendState);
	DX_SAFE_RELEASE(m_depthStencilState);
	DX_SAFE_RELEASE( m_rasterState );
}

bool ShaderStage::LoadShaderFromSource( RenderContext *renderContext, 
	std::string const &filename, 
	void *source,
	size_t source_len,
	eShaderStage stage,
	const char* entryPoint)
{
	// The device is what is used to 
	// create resources, so lets cache it off
	// for use in this method; 
	m_ownerContext = renderContext;
	ID3D11Device *device = renderContext->m_device; 

	// save the stage; 
	m_shaderStage = stage; 

	// entry point is what function to call for this stage
	//char const* entrypoint = Shader::GetEntryForStage(stage); 

	// target is the shader model to compile against (determines feature set)
	// we'll be using shader model 5.0 in this class; 
	char const *target = Shader::GetShaderModelForStage(stage); 

	// take the source and convert it to byte code;
	// note: this byte code can be saved to disk and loaded from
	// instead of having to recompile the shader - this is 
	// usually done during shader-caching (or done at ship for D3D titles)
	ID3DBlob *bytecode = CompileHLSLToByteCode( filename.c_str(), source, source_len, entryPoint ? entryPoint : Shader::GetEntryForStage(stage), target ); 
	if (bytecode == nullptr) {
		return false; 
	}

	// based on the stage; create the appropriate D3D object; 
	switch (stage) {
	case SHADER_STAGE_VERTEX:    // Compile the byte code to the final shader (driver/hardware specific program)
	device->CreateVertexShader( bytecode->GetBufferPointer(), 
		bytecode->GetBufferSize(), 
		nullptr, 
		&m_vs );
	break;

	case SHADER_STAGE_FRAGMENT: 
	device->CreatePixelShader( bytecode->GetBufferPointer(), 
		bytecode->GetBufferSize(), 
		nullptr, 
		&m_ps );
	break; 
	}

	// we're done with the byte code.
	// later, when doing vertex buffers, we'll need
	// to keep the bytecode of the vertex shader around
	m_bytecode = bytecode;
	// DX_SAFE_RELEASE(bytecode); 
	// Save this bytecode. This is a ID3DBlob

	// if we have a handle, this succeeded; 
	return IsValid();
}
