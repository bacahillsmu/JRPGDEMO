#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"


struct BufferAttribute_t;
struct VertexMaster;

struct Vertex_Lit
{

public:

	Vec3 position;
	Rgba color;
	Vec2 uvTexCoords;
	Vec3 normal;
	Vec3 tangent;
	Vec3 bitangent;

public:
	
	Vertex_Lit() {}

	explicit Vertex_Lit( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords, const Vec3 normal, const Vec3& tangent, const Vec3& bitangent )
		: position( position )
		, color( color )
		, uvTexCoords( uvTexCoords )
		, normal( normal)
		, tangent(tangent)
		, bitangent(bitangent)
	{
	};

	void operator=( const Vertex_Lit& copyFrom );

	static BufferAttribute_t LAYOUT[7];
	static void CopyFromMaster( void *buffer, VertexMaster const *src, uint count );
};


