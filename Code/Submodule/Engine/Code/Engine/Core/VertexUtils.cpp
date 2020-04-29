#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Line.hpp"

// #ToDo: Put this in Engine;
//#include "Game/Shapes/ConvexPoly.hpp"


void AddVertsForAABB2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba& color, const Vec2& uvAtMins/*=Vec2(0.0f,0.0f)*/, const Vec2& uvAtMaxs/*=Vec2(1.0f,1.0f)*/ )
{
	// The four corners of our box.
	Vec3 positionBL(box.mins.x, box.mins.y, 0.0f);
	Vec3 positionBR(box.maxs.x, box.mins.y, 0.0f);
	Vec3 positionTL(box.mins.x, box.maxs.y, 0.0f);
	Vec3 positionTR(box.maxs.x, box.maxs.y, 0.0f);

	// The four UV of our box.
	Vec2 uvBL(uvAtMins.x, uvAtMins.y);
	Vec2 uvBR(uvAtMaxs.x, uvAtMins.y);
	Vec2 uvTL(uvAtMins.x, uvAtMaxs.y);
	Vec2 uvTR(uvAtMaxs.x, uvAtMaxs.y);

	// The two sets of 3 verts for a box.
	vertexArray.push_back(Vertex_PCU(positionBL, color, uvBL));
	vertexArray.push_back(Vertex_PCU(positionBR, color, uvBR));
	vertexArray.push_back(Vertex_PCU(positionTR, color, uvTR));

	vertexArray.push_back(Vertex_PCU(positionBL, color, uvBL));
	vertexArray.push_back(Vertex_PCU(positionTR, color, uvTR));
	vertexArray.push_back(Vertex_PCU(positionTL, color, uvTL));
}

void AddVertsForAABB3D( std::vector<Vertex_PCU>& vertexArray, Vec3* corners, const Rgba& color, const Vec2& uvAtMins/*=Vec2(0.0f,0.0f)*/, const Vec2& uvAtMaxs/*=Vec2(1.0f,1.0f)*/ )
{
	Vec3 topLeft = corners[0];
	Vec3 topRight = corners[1];
	Vec3 bottomLeft = corners[2];
	Vec3 bottomRight = corners[3];

	// The four UV of our box.
	Vec2 uvBL(uvAtMins.x, uvAtMins.y);
	Vec2 uvBR(uvAtMaxs.x, uvAtMins.y);
	Vec2 uvTL(uvAtMins.x, uvAtMaxs.y);
	Vec2 uvTR(uvAtMaxs.x, uvAtMaxs.y);

	// The two sets of 3 verts for a box.
	vertexArray.push_back(Vertex_PCU(bottomLeft, color, uvBL));
	vertexArray.push_back(Vertex_PCU(bottomRight, color, uvBR));
	vertexArray.push_back(Vertex_PCU(topRight, color, uvTR));

	vertexArray.push_back(Vertex_PCU(bottomLeft, color, uvBL));
	vertexArray.push_back(Vertex_PCU(topRight, color, uvTR));
	vertexArray.push_back(Vertex_PCU(topLeft, color, uvTL));
}

void AddVertsForOBB2D( std::vector<Vertex_PCU>& vertexArray, const OBB2& box, const Rgba& color, const Vec2& uvAtMins/*=Vec2(0.0f,0.0f)*/, const Vec2& uvAtMaxs/*=Vec2(1.0f,1.0f)*/ )
{
	// The four corners of our box.
	Vec3 positionBL(box.GetBottomLeft(), 0.0f);
	Vec3 positionBR(box.GetBottomRight(), 0.0f);
	Vec3 positionTL(box.GetTopLeft(), 0.0f);
	Vec3 positionTR(box.GetTopRight(), 0.0f);

	// The four UV of our box.
	Vec2 uvBL(uvAtMins.x, uvAtMins.y);
	Vec2 uvBR(uvAtMaxs.x, uvAtMins.y);
	Vec2 uvTL(uvAtMins.x, uvAtMaxs.y);
	Vec2 uvTR(uvAtMaxs.x, uvAtMaxs.y);

	// The two sets of 3 verts for a box.
	vertexArray.push_back(Vertex_PCU(positionBL, color, uvBL));
	vertexArray.push_back(Vertex_PCU(positionBR, color, uvBR));
	vertexArray.push_back(Vertex_PCU(positionTR, color, uvTR));

	vertexArray.push_back(Vertex_PCU(positionBL, color, uvBL));
	vertexArray.push_back(Vertex_PCU(positionTR, color, uvTR));
	vertexArray.push_back(Vertex_PCU(positionTL, color, uvTL));
}

void AddVertsForCapsule2D( std::vector<Vertex_PCU>& vertexArray, const OBB2& box, float radius, const Vec2& start, const Vec2& end, const Rgba& color, int numSides /*= 64 */ )
{
	UNUSED(start);
	UNUSED(end);

	Vec2 discCenter1 = box.GetCenter() + box.m_extents.y * box.GetUp();
	Vec2 discCenter2 = box.GetCenter() + box.m_extents.y * box.GetUp() * -1.0f;

	AddVertsForDisc2D(vertexArray, discCenter1, radius, color, numSides);
	

	Vec2 displacement = discCenter2 - discCenter1;
	OBB2 updateBox = OBB2(box.GetCenter(), Vec2(radius, GetDistance(discCenter1, discCenter2) * 0.5f), displacement.GetAngleDegrees() + 90.0f); 

	AddVertsForOBB2D(vertexArray, updateBox, color);

	AddVertsForDisc2D(vertexArray, discCenter2, radius, color, numSides);
}

void AddVertsForLine2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, const Rgba& color )
{
	// Hard code a zero vector UV
	Vec2 uvTexCoords(0.0f, 0.0f);

	// Calculate dimensions
	float halfThickness = thickness / 2;
	Vec2 halfForward = end - start;
	halfForward.ClampLength(halfThickness);
	Vec2 halfLeft = halfForward.GetRotated90Degrees();

	// The four corners of our line
	Vec2 pFL = end + halfForward + halfLeft;
	Vec2 pFR = end + halfForward - halfLeft;
	Vec2 pBL = start - halfForward + halfLeft;
	Vec2 pBR = start - halfForward - halfLeft;

	// Convert to Vec3 in this silly way
	Vec3 positionFL(pFL.x, pFL.y, 0.0f);
	Vec3 positionFR(pFR.x, pFR.y, 0.0f);
	Vec3 positionBL(pBL.x, pBL.y, 0.0f);
	Vec3 positionBR(pBR.x, pBR.y, 0.0f);

	// The two sets of 3 verts for a box.
	vertexArray.push_back(Vertex_PCU(positionBL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionBR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionFR, color, uvTexCoords));

	vertexArray.push_back(Vertex_PCU(positionBL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionFR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionFL, color, uvTexCoords));
}

void AddVertsForLine2D(std::vector<Vertex_PCU>& vertexArray, const Line& line, const Rgba& color)
{
	AddVertsForLine2D(vertexArray, line.lineStart, line.lineEnd, line.lineThickness, color);
}

void AddVertsForLine3D( std::vector<Vertex_PCU>& vertexArray, const Vec3& start, const Vec3& end, float thickness, const Rgba& color )
{
	// Hard code a zero vector UV
	Vec2 uvTexCoords(0.0f, 0.0f);

	// Calculate dimensions
	float halfThickness = thickness / 2;
	Vec3 halfForward = end - start;
	halfForward.ClampLength(halfThickness);
	Vec3 halfLeft = halfForward.GetRotated90Degrees();

	// The four corners of our line
	Vec3 pFL = end + halfForward + halfLeft;
	Vec3 pFR = end + halfForward - halfLeft;
	Vec3 pBL = start - halfForward + halfLeft;
	Vec3 pBR = start - halfForward - halfLeft;

	// The two sets of 3 verts for a box.
	vertexArray.push_back(Vertex_PCU(pBL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(pBR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(pFR, color, uvTexCoords));

	vertexArray.push_back(Vertex_PCU(pBL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(pFR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(pFL, color, uvTexCoords));
}

void AddVertsForRay2D(std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, float triangleSize, const Rgba& color)
{
	// Hard code a zero vector UV
	Vec2 uvTexCoords(0.0f, 0.0f);

	// Calculate dimensions
	float halfThickness = thickness / 2;
	Vec2 halfForward = end - start;
	halfForward.ClampLength(halfThickness);
	Vec2 halfLeft = halfForward.GetRotated90Degrees();

	// The four corners of our line
	Vec2 pFL = end + halfForward + halfLeft;
	Vec2 pFR = end + halfForward - halfLeft;
	Vec2 pBL = start - halfForward + halfLeft;
	Vec2 pBR = start - halfForward - halfLeft;

	// Convert to Vec3 in this silly way
	Vec3 positionFL(pFL.x, pFL.y, 0.0f);
	Vec3 positionFR(pFR.x, pFR.y, 0.0f);
	Vec3 positionBL(pBL.x, pBL.y, 0.0f);
	Vec3 positionBR(pBR.x, pBR.y, 0.0f);

	// The two sets of 3 verts for a box.
	vertexArray.push_back(Vertex_PCU(positionBL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionBR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionFR, color, uvTexCoords));

	vertexArray.push_back(Vertex_PCU(positionBL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionFR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionFL, color, uvTexCoords));

	// Draw a triangle at the end;
	Vec2 triRight = (end - (halfForward * triangleSize)) + (halfForward * 0.5f) - (halfLeft * triangleSize);
	Vec2 triTop = end + (halfForward * triangleSize);
	Vec2 triLeft = (end - (halfForward * triangleSize)) + (halfForward * 0.5f) + (halfLeft * triangleSize);

	Vec3 positionR(triRight.x, triRight.y, 0.0f);
	Vec3 positionT(triTop.x, triTop.y, 0.0f);
	Vec3 positionL(triLeft.x, triLeft.y, 0.0f);
	Vec3 e(end.x, end.y, 0.0f);

	vertexArray.push_back(Vertex_PCU(positionR, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionT, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(e, color, uvTexCoords));

	vertexArray.push_back(Vertex_PCU(positionT, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(positionL, color, uvTexCoords));
	vertexArray.push_back(Vertex_PCU(e, color, uvTexCoords));
}

void AddVertsForRay2D(std::vector<Vertex_PCU>& vertexArray, const Line& line, float triangleSize, const Rgba& color)
{
	AddVertsForRay2D(vertexArray, line.lineStart, line.lineEnd, line.lineThickness, triangleSize, color);
}

void AddVertsForDisc2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, const Rgba& color, int numSides /*= 64*/ )
{
	// Hard code a zero vector UV
	Vec2 uvTexCoords(0.0f, 0.0f);

	// Calculate the angle of each slice
	float growingSliceAngle = 360.0f / numSides;
	float currentSliceAngle = 0.0f;

	// Draw a slice per side of the circle
	for(int i = 0; i < numSides; i++)
	{
		Vec2 vertex1 = Vec2(radius * CosDegrees(currentSliceAngle), radius * SinDegrees(currentSliceAngle));
		Vec2 vertex2 = Vec2(radius * CosDegrees(currentSliceAngle + growingSliceAngle), radius * SinDegrees(currentSliceAngle + growingSliceAngle));

		vertexArray.push_back(Vertex_PCU(Vec3(center.x+0, center.y+0, 0), color, uvTexCoords));
		vertexArray.push_back(Vertex_PCU(Vec3(center.x+vertex1.x, center.y+vertex1.y, 0), color, uvTexCoords));
		vertexArray.push_back(Vertex_PCU(Vec3(center.x+vertex2.x, center.y+vertex2.y, 0), color, uvTexCoords));

		currentSliceAngle += growingSliceAngle;
	}
}

void AddVertsForRing2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides /*= 64 */ )
{
	// Hard code a zero vector UV
	Vec2 uvTexCoords(0.0f, 0.0f);

	// Calculate the angle of each slice
	float growingSliceAngle = 360.0f / numSides;
	float currentSliceAngle = 0.0f;
	float halfThickness = thickness / 2;

	// Draw a slice per side of the circle
	for(int i = 0; i < numSides; i++)
	{
		// First Vertex of our slice
		float vertex1X = radius * CosDegrees(currentSliceAngle) + center.x;
		float vertex1Y = radius * SinDegrees(currentSliceAngle) + center.y;
		Vec2 vertex1(vertex1X, vertex1Y);

		Vec2 halfForwardV1 = vertex1 - center;
		halfForwardV1.ClampLength(halfThickness);

		Vec2 pTR = vertex1 + halfForwardV1;
		Vec2 pBR = vertex1 - halfForwardV1;

		Vec3 positionTR(pTR.x, pTR.y, 0.0f);
		Vec3 positionBR(pBR.x, pBR.y, 0.0f);

		// Second Vertex of our slice
		float vertex2X = radius * CosDegrees(currentSliceAngle + growingSliceAngle) + center.x;
		float vertex2Y = radius * SinDegrees(currentSliceAngle + growingSliceAngle) + center.y;
		Vec2 vertex2(vertex2X, vertex2Y);

		Vec2 halfForwardV2 = vertex2 - center;
		halfForwardV2.ClampLength(halfThickness);

		Vec2 pTL = vertex2 + halfForwardV2;
		Vec2 pBL = vertex2 - halfForwardV2;

		Vec3 positionTL(pTL.x, pTL.y, 0.0f);
		Vec3 positionBL(pBL.x, pBL.y, 0.0f);

		// The two sets of 3 verts for a box.
		vertexArray.push_back(Vertex_PCU(positionTL, color, uvTexCoords));
		vertexArray.push_back(Vertex_PCU(positionBL, color, uvTexCoords));
		vertexArray.push_back(Vertex_PCU(positionBR, color, uvTexCoords));

		vertexArray.push_back(Vertex_PCU(positionBR, color, uvTexCoords));
		vertexArray.push_back(Vertex_PCU(positionTR, color, uvTexCoords));
		vertexArray.push_back(Vertex_PCU(positionTL, color, uvTexCoords));

		currentSliceAngle += growingSliceAngle;
	}
}

//-----------------------------------------------------------------------------------------------
// void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& vertexArray, const ConvexPoly2D& convexPoly2D, const Rgba& color)
// {
// 	// Hard code a zero vector UV
// 	Vec2 uvTexCoords(0.0f, 0.0f);
// 
// 	Vec3 vert1;
// 	Vec3 vert2;
// 	Vec3 vert3;
// 	Vec2 start = convexPoly2D.m_verts[0];
// 	for(int i = 0; i < convexPoly2D.m_numVerts - 2; ++i)
// 	{
// 		vert1 = Vec3((start), 0.0f);
// 		vert2 = Vec3((convexPoly2D.m_verts[i+1]), 0.0f);
// 		vert3 = Vec3((convexPoly2D.m_verts[i+2]), 0.0f);
// 
// 		vertexArray.push_back(Vertex_PCU(vert1, color, uvTexCoords));
// 		vertexArray.push_back(Vertex_PCU(vert2, color, uvTexCoords));
// 		vertexArray.push_back(Vertex_PCU(vert3, color, uvTexCoords));
// 	}
// }

//-----------------------------------------------------------------------------------------------
void TransformVertex2D( Vertex_PCU& vertex, float uniformScale, float rotationDegreesAboutZ, Vec2& translationXY )
{

	Vec2 newPosition = TransformPosition(Vec2(vertex.position.x, vertex.position.y), uniformScale, rotationDegreesAboutZ, translationXY);

	// Set the new (x, y). We did not change the z.
	vertex.position.x = newPosition.x;
	vertex.position.y = newPosition.y;
}

//-----------------------------------------------------------------------------------------------
void TransformVertexArray2D( int numVertexes, Vertex_PCU* vertexes, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY )
{
	// We will manipulate each vertex in the array one at a time.
	for(int i = 0; i < numVertexes; i++)
	{
		Vec2 newPosition = TransformPosition(Vec2(vertexes[i].position.x, vertexes[i].position.y), uniformScale, rotationDegreesAboutZ, translationXY);

		// Set the new (x, y). We did not change the z.
		vertexes[i].position.x = newPosition.x;
		vertexes[i].position.y = newPosition.y;
	}

}
