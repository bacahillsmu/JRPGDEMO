#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
//-----------------------------------------------------------------------------------------------
#include <vector>

struct AABB2;
class OBB2;
struct Rgba;
struct Vec2;
struct Vec3;
class Line;
//class ConvexPoly2D;

//-----------------------------------------------------------------------------------------------


void AddVertsForAABB2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba& color, const Vec2& uvAtMins = Vec2(0.0f, 1.0f), const Vec2& uvAtMaxs = Vec2(1.0f, 0.0f) );
void AddVertsForAABB3D( std::vector<Vertex_PCU>& vertexArray, Vec3* corners, const Rgba& color, const Vec2& uvAtMins = Vec2(0.0f, 1.0f), const Vec2& uvAtMaxs = Vec2(1.0f, 0.0f) );
void AddVertsForOBB2D( std::vector<Vertex_PCU>& vertexArray, const OBB2& box, const Rgba& color, const Vec2& uvAtMins = Vec2(0.0f, 1.0f), const Vec2& uvAtMaxs = Vec2(1.0f, 0.0f) );
void AddVertsForCapsule2D( std::vector<Vertex_PCU>& vertexArray, const OBB2& box, float radius, const Vec2& start, const Vec2& end, const Rgba& color, int numSides = 64 );
void AddVertsForDisc2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, const Rgba& color, int numSides = 64 );
void AddVertsForLine2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, const Rgba& color );
void AddVertsForLine2D( std::vector<Vertex_PCU>& vertexArray, const Line& line, const Rgba& color );
void AddVertsForLine3D( std::vector<Vertex_PCU>& vertexArray, const Vec3& start, const Vec3& end, float thickness, const Rgba& color );
void AddVertsForRay2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, float triangleSize, const Rgba& color );
void AddVertsForRay2D( std::vector<Vertex_PCU>& vertexArray, const Line& line, float triangleSize, const Rgba& color );
void AddVertsForRing2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides = 64 );
//void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& vertexArray, const ConvexPoly2D& convexPoly2D, const Rgba& color);

void TransformVertex2D(Vertex_PCU& vertex, float uniformScale, float rotationDegreesAboutZ, Vec2& translationXY);
void TransformVertexArray2D(int numVertexes, Vertex_PCU* vertexes, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);
