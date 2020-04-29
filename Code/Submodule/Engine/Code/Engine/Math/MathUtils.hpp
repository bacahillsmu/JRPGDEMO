#pragma once

#include <cmath>
#include <vector>

struct Vec2;
struct Vec3;
struct Vec4;
struct IntVec2;
struct Vertex_PCU;
struct AABB2;
struct Rgba;
class Line;

typedef unsigned int uint;
constexpr float MATH_PI = 3.14159265359f;
constexpr float fSQRT_3_OVER_3 = 0.5773502691896257645091f;

//-----------------------------------------------------------------------------------------------

// Random Math Shit
float Clamp( float toClamp, float minRange, float maxRange);
int Clamp( int toClamp, int minRange, int maxRange);
unsigned int Clamp(unsigned int toClamp, unsigned int minRange, unsigned int maxRange);
double Clamp( double toClamp, double minRange, double maxRange);
Vec2 Clamp( Vec2 toClamp, float minRange, float maxRange);
Vec2 Clamp( Vec2 toClamp, Vec2 minRange, Vec2 maxRange);
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float RangeMap( Vec2 inValue, float inStart, float inEnd, float outStart, float outEnd );
float RangeMapZeroToOne( float inValue, float inStart, float inEnd );
float DotProductVec2(Vec2 vectorA, Vec2 normalVector);
float DotProductVec3(Vec3 vectorA, Vec3 normalVector);
float DotProductVec4(Vec4 vectorA, Vec4 normalVector);

template<typename T>
T GetMin(const T& a, const T& b)
{
	return a > b ? b : a;
}

template<typename T>
T GetMax(const T& a, const T& b)
{
	return a < b ? b : a;
}

bool FloatEqualAndSet( float& toCheck, float checkAgainst, float threshold );
bool FloatEqual( float toCheck, float checkAgainst, float threshold );
bool IsBetween(float toCheck, float min, float max );

float CrossProduct(const Vec2& vectorA, const Vec2& vectorB);
Vec2 CrossProduct(const Vec2& vectorA, float scalar);
Vec2 CrossProduct(float scalar, const Vec2& vectorA);
Vec3 CrossProduct(const Vec3& vectorA, const Vec3& vectorB);

float Lerp(float initial, float ending, float fraction);
Vec2 Lerp2D(Vec2 initial, Vec2 ending, float fraction);
Vec3 Lerp3D(Vec3 initial, Vec3 ending, float fraction);
Rgba LerpRgba(Rgba initial, Rgba ending, float fraction);

// Angle Utils
float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float degrees );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float ATan2Degrees( float y, float x );
float GetAngularDisplacement( float startDegrees, float endDegrees );
float GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees );

// Geometric Queries
float GetDistance( const Vec2& positionA, const Vec2& positionB );
float GetDistance( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXY( const Vec3& positionA, const Vec3& positionB );
float GetDistanceSquared( const Vec2& positionA, const Vec2& positionB );
float GetDistanceSquared( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXYSquared( const Vec3& positionA, const Vec3& positionB );
bool DoDiscsOverlap(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
bool DoSpheresOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB);
bool IsPointInDisc(const Vec2& point, const Vec2& discCenter, float radius);
float GetOverLapDistance(const Vec2& point, Vec2& discCenter, float radius);
Vec3 GetSphericalToCoord( float radius, float theta, float phi );
Vec3 GetTangentFromSphereCoords( float theta, float phi );
bool DoLinesOverlap(const Line& line1, const Line& line2, Vec2* possibleIntersection);

// 
uint Quadratic(float* out, float a, float b, float c);

// Grid Based Utils
int GetIndexFromCoord(const IntVec2& coord, const IntVec2& dimensions);
IntVec2 GetCoordFromIndex(int index, const IntVec2& dimensions);
bool IsTileCoordInBounds(const IntVec2& coord, const IntVec2& dimensions);

bool IsPointInSectorAngle2D( const Vec2& point, const Vec2& origin, float distance, float orientationDegrees, float apertureDegrees );
bool IsPointInSectorVector2D(const Vec2& point, const Vec2& origin, float distance, const Vec2& sectorDirection, float apertureDegrees);

// Transform Utils
Vec2 TransformPosition(const Vec2& position, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);
Vec3 TransformPosition(const Vec3& position, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);

// AABB2
Vec2 GetClosestPointOnAABB2(Vec2 point, AABB2* boundingBox);
Vec2 GetClosestPointOnAABB2Edge(Vec2 point, AABB2* boundingBox);
bool DoesPointIntersectAABB2(Vec2 point, AABB2 boundingBox);
void PushDiscOutOfAABB2D( Vec2& discCenter, float radius, const AABB2& box);
bool DoABB2sOverlap(AABB2* box1, AABB2* box2);

// Disks
void PushDiscOutOfDisc(Vec2& discCenterA, float radiusA, Vec2& discCenterB, float radiusB);
void PushDiscsOutOfDiscs(Vec2& discCenterA, float radiusA, Vec2& discCenterB, float radiusB);
void PushDiscsOutOfDiscs(Vec3& discCenterA, float radiusA, Vec3& discCenterB, float radiusB);
float GetDiscOverlapAmount(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
float GetDiscOverlapAmount(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB);
void PushDiscOutOfPoint(Vec2& discCenter, float radius, const Vec2& point);

// Projection
Vec2 ProjectedVector(const Vec2& toProject, const Vec2& ontoThis);
Vec2 GetClosestPointOnLine2D( const Vec2& referencePos, const Vec2& pointOnLine, const Vec2& anotherPointOnLine );
Vec2 GetClosestPointOnLineSegment2D( const Vec2& referencePos, const Vec2& lineStart, const Vec2& lineEnd );

// Easing Functions
float SmoothStart2(float t);
float SmoothStart3(float t);
float SmoothStart4(float t);
float SmoothStart5(float t);
float SmoothStop2(float t);
float SmoothStop3(float t);
float SmoothStop4(float t);
float SmoothStop5(float t);

// Smooth Step
float SmoothStep3(float t);
float SmoothStep5(float t);

