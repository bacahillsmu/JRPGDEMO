#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"

struct BufferAttribute_t;
struct VertexMaster;

struct VertexPCUNTB
{

public:

	Vec3 position;
	Rgba color;
	Vec2 uvTexCoords;
	Vec3 normal;
	Vec3 tangent;
	Vec3 bitangent;

public:

	// Construction/Destruction
	VertexPCUNTB() {}
	~VertexPCUNTB() {}
	explicit VertexPCUNTB( const Vec3& position, const Rgba& color, const Vec2& uvTexCoords )
		: position( position )
		, color( color )
		, uvTexCoords( uvTexCoords )
	{
	};

	void operator=( const VertexPCUNTB& copyFrom );

	static BufferAttribute_t LAYOUT[7];
	static void CopyFromMaster( void *buffer, VertexMaster const *src, unsigned int count );



};