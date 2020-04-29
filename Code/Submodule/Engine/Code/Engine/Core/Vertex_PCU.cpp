#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Core/VertexMaster.hpp"

//-----------------------------------------------------------------------------------------------


void Vertex_PCU::operator=( const Vertex_PCU& copyFrom )
{
		position = copyFrom.position;
		color = copyFrom.color;
		uvTexCoords = copyFrom.uvTexCoords;
}

STATIC BufferAttribute_t Vertex_PCU::LAYOUT[] =
{
	BufferAttribute_t("POSITION", DATA_TYPE_VEC3, offsetof(Vertex_PCU, position)),
	BufferAttribute_t("COLOR", DATA_TYPE_RGBA32, offsetof(Vertex_PCU, color)),
	BufferAttribute_t("TEXCOORD", DATA_TYPE_VEC2, offsetof(Vertex_PCU, uvTexCoords)),

	BufferAttribute_t()
};

STATIC void Vertex_PCU::CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count )
{
	Vertex_PCU* dst = (Vertex_PCU*)buffer;

	for(unsigned int i = 0; i < count; i++)
	{
		dst[i].position = src[i].position;
		dst[i].color = src[i].color;
		dst[i].uvTexCoords = src[i].uv;
	}
}

