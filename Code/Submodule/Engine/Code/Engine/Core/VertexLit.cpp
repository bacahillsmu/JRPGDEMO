#include "Engine/Core/VertexLit.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Core/VertexMaster.hpp"

void Vertex_Lit::operator=( const Vertex_Lit& copyFrom )
{
	position = copyFrom.position;
	color = copyFrom.color;
	uvTexCoords = copyFrom.uvTexCoords;
	normal = copyFrom.normal;
	tangent = copyFrom.tangent;
	bitangent = copyFrom.bitangent;
}

STATIC BufferAttribute_t Vertex_Lit::LAYOUT[7] =
{
	BufferAttribute_t("POSITION",	DATA_TYPE_VEC3,		offsetof(Vertex_Lit, position)),
	BufferAttribute_t("COLOR",		DATA_TYPE_RGBA32,	offsetof(Vertex_Lit, color)),
	BufferAttribute_t("TEXCOORD",	DATA_TYPE_VEC2,		offsetof(Vertex_Lit, uvTexCoords)),
	BufferAttribute_t("NORMAL",		DATA_TYPE_VEC3,		offsetof(Vertex_Lit, normal)),
	BufferAttribute_t("TANGENT",	DATA_TYPE_VEC3,		offsetof(Vertex_Lit, tangent)),
	BufferAttribute_t("BITANGENT",	DATA_TYPE_VEC3,		offsetof(Vertex_Lit, bitangent)),

	BufferAttribute_t()
};

STATIC void Vertex_Lit::CopyFromMaster( void *buffer, VertexMaster const *src, uint count )
{
	Vertex_Lit* dst = (Vertex_Lit*)buffer;

	for(uint i = 0; i < count; i++)
	{
		dst[i].position = src[i].position;
		dst[i].color = src[i].color;
		dst[i].uvTexCoords = src[i].uv;
		dst[i].normal = src[i].normal;
		dst[i].tangent = src[i].tangent;
		dst[i].bitangent = src[i].bitangent;
	}
}

