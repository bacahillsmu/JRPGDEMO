#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Matrix44.hpp"

typedef unsigned int uint;
#define MAX_LIGHTS (8)

// ENUM ------------------------------------------------------------------
// A enum to pick a sampler state to use
enum SampleMode
{
	SAMPLE_MODE_POINT = 0, 
	SAMPLE_MODE_LINEAR, 
	// SAMPLE_MODE_BILINEAR,

	SAMPLE_MODE_COUNT,
	//SAMPLE_MODE_DEFAULT = SAMPLE_MODE_LINEAR // different games may want to change this.  My projects will use Linear, SD4 will want point; 
	SAMPLE_MODE_DEFAULT = SAMPLE_MODE_POINT // different games may want to change this.  My projects will use Linear, SD4 will want point; 
}; 

enum FilterMode
{
	FILTER_MODE_POINT,
	FILTER_MODE_LINEAR
};

enum BlendMode
{
	BLEND_MODE_UNKNOWN = -1,

	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE,
	BLEND_MODE_OPAQUE,

	NUM_BLEND_MODES
};

enum CompareOp
{
	COMPARE_NEVER,       // false,      "never"
	COMPARE_ALWAYS,      // true,       "always"
	COMPARE_EQUAL,       // ==,         "equal"
	COMPARE_NOTEQUAL,    // !=,         "not"
	COMPARE_LESS,        // <           "less"
	COMPARE_LEQUAL,      // <=          "lequal"
	COMPARE_GREATER,     // >           "greater"
	COMPARE_GEQUAL,      // >=          "gequal"
};

enum eRenderBufferUsageBit : uint
{
	RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT   = BIT_FLAG(0),   // Can be bound to an vertex input stream slot
	RENDER_BUFFER_USAGE_INDEX_STREAM_BIT    = BIT_FLAG(1),   // Can be bound as an index input stream.  
	RENDER_BUFFER_USAGE_UNIFORMS_BIT        = BIT_FLAG(2)   // Can be bound to a constant buffer slot; 
};
typedef uint eRenderBufferUsageBits;

enum TextureUsageBit : uint
{
	TEXTURE_USAGE_TEXTURE_BIT				= BIT_FLAG(0),
	TEXTURE_USAGE_COLOR_TARGET_BIT			= BIT_FLAG(1),
	TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT	= BIT_FLAG(2)
};
typedef uint TextureUsageBits;

enum GPUMemoryUsage 
{
	GPU_MEMORY_USAGE_GPU,     // Can be written/read from GPU only (Color Targets are a good example)
	GPU_MEMORY_USAGE_STATIC,  // Created, and are read only after that (ex: textures from images, sprite atlas)
	GPU_MEMORY_USAGE_DYNAMIC, // Update often from CPU, used by the GPU (CPU -> GPU updates, used by shaders.  ex: Uniform Buffers)
	GPU_MEMORY_USAGE_STAGING // For getting memory from GPU to CPU (can be copied into, but not directly bound as an output.  ex: Screen shots system)
};

enum CoreUniformSlot 
{
	UNIFORM_SLOT_FRAME         = 1,
	UNIFORM_SLOT_CAMERA        = 2, 
	UNIFORM_SLOT_MODEL		   = 3,
	UNIFORM_SLOT_LIGHT			= 4,
	UNIFORM_SLOT_MATERIAL		= 8,
}; 

enum eShaderStage
{
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_FRAGMENT,

	SHADER_STAGE_COUNT
};

enum RasterCull
{
	RASTER_CULL_BACK,
	RASTER_CULL_FRONT,
	RASTER_CULL_NONE
};

enum RasterFill
{
	RASTER_FILL_SOLID,
	RASTER_FILL_WIRE
};

enum DataType
{
	DATA_TYPE_UNKNOWN = -1,

	DATA_TYPE_NULL,
	DATA_TYPE_FLOAT,
	DATA_TYPE_VEC2,
	DATA_TYPE_VEC3,
	DATA_TYPE_RGBA32,

	DATA_TYPE_COUNT
};

// STRUCT ----------------------------------------------------------------
// Uniform Buffers;

// Slot 1;
struct frame_buffer_t
{
	float time; 
	float halfTime; 
	float cosTime; 
	float sinTime; 
};

//------------------------------------------------------------------------
// Slot 2; 
struct camera_buffer_t 
{
	Matrix4x4 view;
	Matrix4x4 projection;
	Vec3 cameraPosition;
	float padcamera;
};

//------------------------------------------------------------------------
// Slot 3;
struct model_buffer_t
{
	Matrix4x4 model;
};

//------------------------------------------------------------------------
// Used by light buffer; Something created by Game.
struct light_t
{
	Rgba color;					// Alpha will be used as intensity;

	Vec3 position;				// Where is the light;
	float pad00;				// Keep a 16 byte alignment for D3D11;

	Vec3 direction;				// Which way is the light facing;
	float isDirection = 0.0f;	// 0 means use the light as a point;

	Vec3 diffuseAttenuation = Vec3( 0.0f, 1.0f, 0.0f ); // No attenuation
	float pad01;

	Vec3 specularAttenuation = Vec3( 0.0f, 1.0f, 0.0f ); // No attenuation
	float pad02;
};

//------------------------------------------------------------------------
// Slot 4;
// m_cpuLightBuffer in RenderContext (Unified Light Structure)
struct light_buffer_t
{
	Vec4 ambient;
	float specFactor = 1.0f;
	float specPower = 64.0f;
	float emissiveFactor = 1.0f;
	float pad;

	light_t lights[MAX_LIGHTS];
};

//------------------------------------------------------------------------
// Slot 8;
// 
struct tonemap_buffer_t
{
	float intensity;
	float pad00;
	float pad01;
	float pad02;
};
