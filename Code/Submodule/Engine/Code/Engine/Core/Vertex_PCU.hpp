#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"


struct BufferAttribute_t;
struct VertexMaster;

struct Vertex_PCU
{

public:

	Vec3 position;
	Rgba color;
	Vec2 uvTexCoords;

public:

	// Construction/Destruction
	Vertex_PCU() {}
	~Vertex_PCU() {}
	explicit Vertex_PCU( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords )
		: position( position )
		, color( color )
		, uvTexCoords( uvTexCoords )
	{
	};

	void operator=( const Vertex_PCU& copyFrom );

	static BufferAttribute_t LAYOUT[];
	static void CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count );


};


