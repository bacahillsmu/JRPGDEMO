#pragma once
#include "Engine/Core/XmlUtils.hpp"

#include <vector>

class RenderContext;
class Shader;
class Sampler;
class TextureView;
class UniformBuffer;

class Material
{

public:

	Material( RenderContext* renderContext );
	~Material();

	void SetShader( Shader* shader );
	void SetShader( const char* shaderName );

	void SetTextureView( unsigned int slot, TextureView* textureView );
	void SetTextureView( unsigned int slot, const char* name );

	void SetSampler( unsigned int slot, Sampler* sampler );

	TextureView* GetTextureView( unsigned int slot) const;
	Sampler* GetSampler( unsigned int slot ) const;

	// Load from file;
	void CreateMaterialFromFile(const char* filename);

	// Convenience Methods
	void SetDiffuseMap( TextureView* view );	// default to WHITE on nullptr;
	void SetNormalMap( TextureView* view );		// default to FLAT on nullptr;
	void SetSpecularMap( TextureView* view );	// default to WHITE on nullptr;
	void SetEmmisiveMap( TextureView* view );	// default to BLACK on nullptr;

	void SetUniforms( const void* data, const size_t size);
	void SetUniforms( UniformBuffer* uniformBuffer );
	UniformBuffer* GetUniformBuffer() const;


public:

	RenderContext* m_renderContext = nullptr;
	UniformBuffer* m_materialBuffer = nullptr;
	Shader* m_shader = nullptr;
	std::vector<TextureView*> m_textures;
	std::vector<Sampler*> m_samplers;


};