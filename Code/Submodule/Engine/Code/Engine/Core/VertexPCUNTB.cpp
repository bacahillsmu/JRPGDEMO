#include "Engine/Core/VertexPCUNTB.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Core/VertexMaster.hpp"


void VertexPCUNTB::operator=( const VertexPCUNTB& copyFrom )
{
	position = copyFrom.position;
	color = copyFrom.color;
	uvTexCoords = copyFrom.uvTexCoords;
}

STATIC BufferAttribute_t VertexPCUNTB::LAYOUT[7] =
{
	BufferAttribute_t("POSITION",	DATA_TYPE_VEC3,		offsetof(VertexPCUNTB, position)),
	BufferAttribute_t("COLOR",		DATA_TYPE_RGBA32,	offsetof(VertexPCUNTB, color)),
	BufferAttribute_t("TEXCOORD",	DATA_TYPE_VEC2,		offsetof(VertexPCUNTB, uvTexCoords)),
	BufferAttribute_t("NORMAL",		DATA_TYPE_VEC3,		offsetof(VertexPCUNTB, normal)),
	BufferAttribute_t("TANGENT",	DATA_TYPE_VEC3,		offsetof(VertexPCUNTB, tangent)),
	BufferAttribute_t("BITANGENT",	DATA_TYPE_VEC3,		offsetof(VertexPCUNTB, bitangent)),

	BufferAttribute_t()
};

STATIC void VertexPCUNTB::CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count )
{
	VertexPCUNTB* dst = (VertexPCUNTB*)buffer;

	for(unsigned int i = 0; i < count; i++)
	{
		dst[i].position = src[i].position;
		dst[i].color = src[i].color;
		dst[i].uvTexCoords = src[i].uv;
	}
}

