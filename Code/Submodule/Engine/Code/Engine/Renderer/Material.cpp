#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/EngineCommon.hpp"


// -----------------------------------------------------------------------
Material::Material( RenderContext* renderContext )
{
	m_renderContext = renderContext;
}

// -----------------------------------------------------------------------
Material::~Material()
{
	delete m_materialBuffer;
	m_materialBuffer = nullptr;

	/* ??
	delete m_shader;
	m_shader = nullptr;
	*/
}

// -----------------------------------------------------------------------
void Material::SetShader( Shader* shader )
{
	m_shader = shader;
}

// -----------------------------------------------------------------------
void Material::SetShader( const char* shaderName )
{
	if(strcmp(shaderName, "") == 0)
	{
		ERROR_AND_DIE("Tried to create Material with no Shader Path.");
		return;
	}

	m_shader = m_renderContext->CreateShaderFromFile(shaderName);
}

// -----------------------------------------------------------------------
void Material::SetTextureView( unsigned int slot, TextureView* textureView )
{
	int size = (int)m_textures.size();

	if(size <= (int)slot)
	{
		m_textures.resize(slot + 1);
	}

	m_textures[slot] = textureView;
}

// -----------------------------------------------------------------------
void Material::SetTextureView( unsigned int slot, const char* name )
{
	TextureView* textureView = m_renderContext->CreateOrGetTextureViewFromFile(name);
	SetTextureView(slot, textureView);
}

// -----------------------------------------------------------------------
void Material::SetSampler( unsigned int slot, Sampler* sampler )
{
	int size = (int)m_samplers.size();

	if(size <= (int)slot)
	{
		m_samplers.resize(slot + 1);
	}

	m_samplers[slot] = sampler;
}

// -----------------------------------------------------------------------
TextureView* Material::GetTextureView( unsigned int slot ) const
{
	return m_textures[slot];
}

// -----------------------------------------------------------------------
Sampler* Material::GetSampler( unsigned int slot ) const
{
	return m_samplers[slot];
}

void Material::CreateMaterialFromFile( const char* filename )
{

	tinyxml2::XMLDocument materialXMLDoc;
	materialXMLDoc.LoadFile(filename);

	if(materialXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", filename);
		printf("ErrorID:      %i\n", materialXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", materialXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", materialXMLDoc.ErrorName());
	}
	else
	{
		printf("Success with XML Doc: %s\n", filename);

		XmlElement* rootElement = materialXMLDoc.RootElement();

		// Shader
		XmlElement* shaderElement = rootElement->FirstChildElement("shader");
		std::string shaderSourceFile = ParseXmlAttribute(*shaderElement, "src", "");
		SetShader(m_renderContext->GetOrCreateShader(shaderSourceFile.c_str()));

		// Diffuse
		XmlElement* diffuseElement = rootElement->FirstChildElement("diffuse");
		std::string diffuseSourceFile = ParseXmlAttribute(*diffuseElement, "src", "");
		if(diffuseSourceFile == "")
		{
			SetDiffuseMap(m_renderContext->m_blankWhiteTexture);
		}
		else
		{
			SetDiffuseMap(m_renderContext->CreateOrGetTextureViewFromFile(diffuseSourceFile.c_str()));
		}

		// Normal
		XmlElement* normalElement = rootElement->FirstChildElement("normal");
		std::string normalSourceFile = ParseXmlAttribute(*normalElement, "src", "");
		if(normalSourceFile == "")
		{
			SetNormalMap(m_renderContext->m_blankFlatTexture);
		}
		else
		{
			SetNormalMap(m_renderContext->CreateOrGetTextureViewFromFile(normalSourceFile.c_str()));
		}
		

		// Specular
		XmlElement* specularElement = rootElement->FirstChildElement("spec");
		std::string specularSourceFile = ParseXmlAttribute(*specularElement, "src", "");
		if(specularSourceFile == "")
		{
			SetSpecularMap(m_renderContext->m_blankWhiteTexture);
		}
		else
		{
			SetSpecularMap(m_renderContext->CreateOrGetTextureViewFromFile(specularSourceFile.c_str()));
		}

		// Emissive
		XmlElement* emissiveElement = rootElement->FirstChildElement("emissive");
		std::string emissiveSourceFile = ParseXmlAttribute(*emissiveElement, "src", "");
		if(emissiveSourceFile == "")
		{
			SetEmmisiveMap(m_renderContext->m_blankBlackTexture);
		}
		else
		{
			SetEmmisiveMap(m_renderContext->CreateOrGetTextureViewFromFile(emissiveSourceFile.c_str()));
		}

		

		// Sampler
		XmlElement* samplerElement = rootElement->FirstChildElement("sampler");
		int samplerIdx = ParseXmlAttribute(*samplerElement, "idx", -1);
		std::string samplerType = ParseXmlAttribute(*samplerElement, "type", "");
		
		SampleMode sampleMode = SAMPLE_MODE_LINEAR;
		if(samplerType == "linear")
		{
			sampleMode = SAMPLE_MODE_LINEAR;
		}
		else if(samplerType == "point")
		{
			sampleMode = SAMPLE_MODE_POINT;
		}

		Sampler* sampler = m_renderContext->GetCachedSampler(sampleMode);
		SetSampler(samplerIdx, sampler);
	}
}

// -----------------------------------------------------------------------
void Material::SetDiffuseMap( TextureView* view )
{
	if(view == nullptr)
	{
		SetTextureView(0u, g_theRenderer->m_blankWhiteTexture);
	}
	SetTextureView(0u, view);
}

// -----------------------------------------------------------------------
void Material::SetNormalMap( TextureView* view )
{
	if(view == nullptr)
	{
		SetTextureView(1u, g_theRenderer->m_blankFlatTexture);
	}

	SetTextureView(1u, view);
}

// -----------------------------------------------------------------------
void Material::SetSpecularMap( TextureView* view )
{
	if(view == nullptr)
	{
		SetTextureView(2u, g_theRenderer->m_blankWhiteTexture);
	}

	SetTextureView(2u, view);
}

// -----------------------------------------------------------------------
void Material::SetEmmisiveMap( TextureView* view )
{
	if(view == nullptr)
	{
		SetTextureView(3u, g_theRenderer->m_blankBlackTexture);
	}

	SetTextureView(3u, view);
}

// -----------------------------------------------------------------------
void Material::SetUniforms( const void* data, const size_t size )
{
	if(!m_materialBuffer)
	{
		m_materialBuffer = new UniformBuffer(m_renderContext);
	}

	bool result = m_materialBuffer->CopyCPUToGPU(data, size);

	if(!result)
	{
		ERROR_AND_DIE("Uniform Buffer was not set for material.");
	}
}

// -----------------------------------------------------------------------
void Material::SetUniforms( UniformBuffer* uniformBuffer )
{
	m_materialBuffer = uniformBuffer;
}

// -----------------------------------------------------------------------
UniformBuffer* Material::GetUniformBuffer() const
{
	return m_materialBuffer;
}
