#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Core/VertexMaster.hpp"
#include "Engine/Renderer/BufferLayout.hpp"

struct AABB2;
struct AABB3;


class CPUMesh
{

public:

	CPUMesh();
	~CPUMesh();

	// Modify the stamp
	void SetColor( const Rgba& color );
	void SetUV( const Vec2& uv );
	void SetNormal( const Vec3& normal );
	void SetTangent( const Vec3& tangent );
	void SetBiTangent( const Vec3& biTangent );



	// Stamp a vertex into the list - return the index
	uint AddVertex( const VertexMaster& vertexMaster );
	uint AddVertex( const Vec3& position);

	// Adds a single triangle
	void AddIndexedTriangle( uint index0, uint index1, uint index2 );

	// Adds two triangles
	void AddIndexedQuad( uint topLeft, uint topRight, uint bottomLeft, uint bottomRight );

	// Helpers
	uint GetVertexCount() const;
	uint GetIndexCount() const;

	// Buffer Layout;	
	const BufferLayout* GetLayout() const;
	const VertexMaster* GetVertices() const;
	void SetLayout( const BufferLayout* bufferLayout );

	template <typename T>
	void SetLayout()
	{
		SetLayout( BufferLayout::For<T>());
	}

	// Other things I havent found a place for yet
	inline bool UsesIndexBuffer() const { return GetIndexCount() > 0; }
	inline uint GetElementCount() const { return UsesIndexBuffer() ? GetIndexCount() : GetVertexCount(); }

	void Clear();

public:

	std::vector<VertexMaster> m_vertices;
	std::vector<uint> m_indices;

	const BufferLayout* m_layout;
	VertexMaster m_stamp;

	std::string m_defaultMaterialName = "";

};

void CPUMeshAddQuad( CPUMesh* out, AABB2 quad );
void CPUMeshAddCube( CPUMesh* out, AABB3 box );
void CPUMeshAddUVSphere( CPUMesh* out, Vec3 center, float radius, uint wedges = 32, uint slices = 16 );
void CPUMeshAddUVCapsule1( CPUMesh* out, Vec3 center, float radius, uint wedges = 32, uint slices = 16 );
void CPUMeshAddUVCapsule2( CPUMesh* out, Vec3 center, float radius, uint wedges = 32, uint slices = 16 );
void CPUMeshAddUVCapsule( CPUMesh* out, Vec3 start, Vec3 end, float radius, uint wedges = 32, uint slices = 16 );

void CreateMeshFromFile( const char* filename, CPUMesh* cpuMesh );
void CreateObjMeshFromFile( CPUMesh* cpuMesh, std::vector<std::string>& objectFilenames, bool invertFaces, float scalef, std::string transform );
void ReadContentsOfObjFile(std::vector<Vec3>& verts, std::vector<Vec3>& uvcoords, std::vector<Vec3>& normals, std::vector<std::string>& faces, const char* filename);
