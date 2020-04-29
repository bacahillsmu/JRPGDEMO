#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Renderer/VertextBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/BufferLayout.hpp"

#include <vector>

class BufferLayout;
class RenderContext;
class CPUMesh;
class VertexBuffer;
class IndexBuffer;
struct Vertex_PCU;
struct Vertex_Lit;

class GPUMesh
{

public:

	GPUMesh(RenderContext* renderContext);
	~GPUMesh();

	void CreateFromCPUMesh( const CPUMesh* cpuMesh, GPUMemoryUsage gpuMemoryUsage = GPU_MEMORY_USAGE_STATIC );
	void CreateMeshFromFile(const char* filename);
	void CreateObjMeshFromFile(std::vector<std::string>& objectFilenames, bool invertFaces, float scale, std::string transform);
	void ReadContentsOfObjFile(std::vector<Vec3>& verts, std::vector<Vec3>& uvcoords, std::vector<Vec3>& normals, std::vector<std::string>& faces, const char* filename);

	template <typename T>
	void CreateFromVertices( std::vector<T> vertices, std::vector<uint> indices )
	{
		m_vertexBuffer->CreateStaticFor( &vertices[0], sizeof(T), (uint)vertices.size() );
		m_indexBuffer->CreateStaticFor(&indices[0], (uint)indices.size());

		SetDrawCall( true, uint(indices.size()));
		
		m_layout = BufferLayout::For<T>();
	}

	template <typename T>
	void CreateFromVertices( std::vector<T> vertices )
	{
		m_vertexBuffer->CreateStaticFor( &vertices[0], sizeof(T), (uint)vertices.size() );

		SetDrawCall( false, uint(vertices.size()));

		m_layout = BufferLayout::For<T>();
	}

	void SetDrawCall( bool usesIndices, uint elementCount );

	inline bool UsesIndexBuffer() { return m_useIndexBuffer; }
	inline uint GetElementCount() { return m_elementCount; }
	inline const BufferLayout* GetLayout() const { return m_layout; }
	inline const std::string& GetDefaultMaterialName() const { return m_defaultMaterialName; }

public:

	const BufferLayout* m_layout;
	RenderContext* m_renderContext = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	std::string m_defaultMaterialName = "";

	// Information for drawing
	uint m_elementCount = 0;
	bool m_useIndexBuffer = false;

};