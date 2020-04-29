#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"


struct VertexMaster
{
public:

	VertexMaster();

	Vec3 position;
	Rgba color;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	Vec3 bitangent;

	// skin weights;
	// skin indices;
};



