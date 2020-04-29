#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Line.hpp"

#include <cmath>

//-----------------------------------------------------------------------------------------------
float Clamp( float toClamp, float minRange, float maxRange )
{
	if( toClamp >= maxRange)
	{
		return maxRange;
	}
	else if(toClamp <= minRange)
	{
		return minRange;
	}

	return toClamp;
}

Vec2 Clamp( Vec2 toClamp, float minRange, float maxRange )
{
	float x = toClamp.x;
	float y = toClamp.y;

	float clampedX = Clamp(x, minRange, maxRange);
	float clampedY = Clamp(y, minRange, maxRange);

	return Vec2(clampedX, clampedY);
}

Vec2 Clamp( Vec2 toClamp, Vec2 minRange, Vec2 maxRange )
{
	float x = toClamp.x;
	float y = toClamp.y;

	float minRangeX = minRange.x;
	float minRangeY = minRange.y;

	float maxRangeX = maxRange.x;
	float maxRangeY = maxRange.y;

	float clampedX = Clamp(x, minRangeX, maxRangeX);
	float clampedY = Clamp(y, minRangeY, maxRangeY);

	return Vec2(clampedX, clampedY);
}

int Clamp( int toClamp, int minRange, int maxRange )
{
	if( toClamp >= maxRange)
	{
		return maxRange;
	}
	else if(toClamp <= minRange)
	{
		return minRange;
	}

	return toClamp;
}

double Clamp( double toClamp, double minRange, double maxRange )
{
	if( toClamp >= maxRange)
	{
		return maxRange;
	}
	else if(toClamp <= minRange)
	{
		return minRange;
	}

	return toClamp;
}

unsigned int Clamp(unsigned int toClamp, unsigned int minRange, unsigned int maxRange)
{
	if (toClamp >= maxRange)
	{
		return maxRange;
	}
	else if (toClamp <= minRange)
	{
		return minRange;
	}

	return toClamp;
}

//-----------------------------------------------------------------------------------------------
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float inRange = inEnd - inStart;
	float inDistance = inValue - inStart;
	float inFraction = inDistance / inRange;

	float outRange = outEnd - outStart;
	float outDistance = inFraction * outRange;
	float outValue = outStart + outDistance;

	return outValue;

}

//-----------------------------------------------------------------------------------------------
float RangeMap( Vec2 inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float inValueLength = inValue.GetLength();

	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	float inDistance = inValueLength - inStart;

	float inFraction = inDistance / inRange;

	float outDistance = inFraction * outRange;
	float outValue = outStart + outDistance;

	return outValue;
}

//-----------------------------------------------------------------------------------------------
float RangeMapZeroToOne( float inValue, float inStart, float inEnd )
{
	float inRange = inEnd - inStart;
	float inDistance = inValue - inStart;

	float inFraction = inDistance / inRange;

	return inFraction;

}

//-----------------------------------------------------------------------------------------------
float DotProductVec2( Vec2 vectorA, Vec2 normalVector )
{
	return (vectorA.x * normalVector.x) + (vectorA.y * normalVector.y);
}

//-----------------------------------------------------------------------------------------------
float DotProductVec3( Vec3 vectorA, Vec3 normalVector )
{
	return (vectorA.x * normalVector.x) + (vectorA.y * normalVector.y) + (vectorA.z * normalVector.z);
}

//-----------------------------------------------------------------------------------------------
float DotProductVec4( Vec4 vectorA, Vec4 normalVector )
{
	return (vectorA.x * normalVector.x) + (vectorA.y * normalVector.y) + (vectorA.z * normalVector.z) + (vectorA.w * normalVector.w);
}

bool FloatEqualAndSet( float& toCheck, float checkAgainst, float threshold )
{
	float min = checkAgainst - threshold;
	float max = checkAgainst + threshold;

	if(IsBetween(toCheck, min, max))
	{
		toCheck = checkAgainst;
		return true;
	}

	return false;
}

bool FloatEqual( float toCheck, float checkAgainst, float threshold )
{
	float min = checkAgainst - threshold;
	float max = checkAgainst + threshold;

	return IsBetween(toCheck, min, max);
}

bool IsBetween( float toCheck, float min, float max )
{
	if(toCheck > min && toCheck < max)
	{
		return true;
	}

	return false;
}

float CrossProduct( const Vec2& vectorA, const Vec2& vectorB )
{
	return vectorA.x * vectorB.y - vectorA.y * vectorB.x;
}

Vec2 CrossProduct( const Vec2& vectorA, float scalar )
{
	return Vec2(scalar * vectorA.y, -scalar * vectorA.x);
}

Vec2 CrossProduct( float scalar, const Vec2& vectorA )
{
	return Vec2( -scalar * vectorA.y, scalar * vectorA.x );
}

Vec3 CrossProduct( const Vec3& vectorA, const Vec3& vectorB )
{
	Vec3 cross;

	cross.x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
	cross.y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
	cross.z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;

	return cross;
}

//-----------------------------------------------------------------------------------------------
float Lerp( float initial, float ending, float fraction )
{
	return initial + fraction * (ending - initial);
}

//-----------------------------------------------------------------------------------------------
Vec2 Lerp2D( Vec2 initial, Vec2 ending, float fraction )
{
	return initial + fraction * (ending - initial);
}

//-----------------------------------------------------------------------------------------------
Vec3 Lerp3D( Vec3 initial, Vec3 ending, float fraction )
{
	return initial + fraction * (ending - initial);
}

Rgba LerpRgba( Rgba initial, Rgba ending, float fraction )
{
	Rgba lerpedColor = Rgba();
	
	lerpedColor.r = initial.r + fraction * (ending.r - initial.r);
	lerpedColor.g = initial.g + fraction * (ending.g - initial.g);
	lerpedColor.b = initial.b + fraction * (ending.b - initial.b);

	return lerpedColor;
}

//-----------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees( float radians )
{
	return radians * 180.0f / MATH_PI;
}

//-----------------------------------------------------------------------------------------------
float ConvertDegreesToRadians( float degrees )
{
	return degrees * MATH_PI / 180.0f;
}

//-----------------------------------------------------------------------------------------------
float CosDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians(degrees);
	float newRadians = cosf( radians );
	return newRadians;
}

//-----------------------------------------------------------------------------------------------
float SinDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians(degrees);
	float newRadians = sinf( radians );
	return newRadians;
}

//-----------------------------------------------------------------------------------------------
float ATan2Degrees( float y, float x )
{
	return atan2f( y, x ) * 180.0f / MATH_PI;
}

float GetAngularDisplacement( float startDegrees, float endDegrees )
{
	float angularDisplacement = endDegrees - startDegrees;

	while(angularDisplacement > 180.f)
	{
		angularDisplacement -= 360.0f;
	}
	while(angularDisplacement < -180.0f)
	{
		angularDisplacement += 360.0f;
	}

	return angularDisplacement;
}

float GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
	float angularDisplacement = GetAngularDisplacement(currentDegrees, goalDegrees);

	if(fabsf(angularDisplacement) < maxDeltaDegrees)
	{
		return goalDegrees;
	}
	if(angularDisplacement > 0.0f)
	{
		return currentDegrees + maxDeltaDegrees;
	}
	else
	{
		return currentDegrees - maxDeltaDegrees;
	}
}

//-----------------------------------------------------------------------------------------------
float GetDistance( const Vec2& positionA, const Vec2& positionB )
{
	float xDifference = ( positionB.x - positionA.x );
	float yDifference = ( positionB.y - positionA.y );

	return sqrtf( abs(( xDifference * xDifference ) + ( yDifference* yDifference )) );
}

//-----------------------------------------------------------------------------------------------
float GetDistance( const Vec3& positionA, const Vec3& positionB )
{
	float xDifference = ( positionB.x - positionA.x );
	float yDifference = ( positionB.y - positionA.y );
	float zDifference = ( positionB.z - positionA.z );

	return sqrtf( ( xDifference * xDifference ) + ( yDifference* yDifference ) + ( zDifference* zDifference ) );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXY( const Vec3& positionA, const Vec3& positionB )
{
	float xDifference = ( positionB.x - positionA.x );
	float yDifference = ( positionB.y - positionA.y );

	return sqrtf( ( xDifference * xDifference ) + ( yDifference * yDifference ) );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared( const Vec2& positionA, const Vec2& positionB )
{
	float xDifference = ( positionB.x - positionA.x );
	float yDifference = ( positionB.y - positionA.y );

	return ( xDifference * xDifference ) + ( yDifference* yDifference );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared( const Vec3& positionA, const Vec3& positionB )
{
	float xDifference = ( positionB.x - positionA.x );
	float yDifference = ( positionB.y - positionA.y );
	float zDifference = ( positionB.z - positionA.z );

	return ( xDifference * xDifference ) + ( yDifference* yDifference ) + ( zDifference* zDifference );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXYSquared( const Vec3& positionA, const Vec3& positionB )
{
	float xDifference = ( positionB.x - positionA.x );
	float yDifference = ( positionB.y - positionA.y );

	return ( xDifference * xDifference ) + ( yDifference* yDifference );
}

float GetAngleBetweenTwoVec2D(const Vec2& positionA, const Vec2& positionB)
{
	float length1 = sqrt((positionA.x * positionA.x) + (positionA.y * positionA.y));// calculate modulus of Vector V1 i.e. |V1|
	float length2 = sqrt((positionB.x * positionB.x) + (positionB.y * positionB.y));
	
	float dot = (positionA.x * positionB.x) + (positionA.y * positionB.y); // Dot Product

	float theta = dot / (length1 * length2); 

	if (theta >= 1.0)
		return 0.0;
	else if (theta <= -1.0)
		return MATH_PI;
	else
		return acosf(theta); // 0..PI
}

//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float distanceBetweenCenters = GetDistance( centerA, centerB );

	if(distanceBetweenCenters > (radiusA + radiusB))
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	float distanceBetweenCenters = GetDistance( centerA, centerB );

	if(distanceBetweenCenters > (radiusA + radiusB))
	{
		return false;
	}

	return true;
}

bool IsPointInDisc( const Vec2& point, const Vec2& discCenter, float radius )
{
	float distanceSquared = GetDistanceSquared(discCenter, point);
	float radiusSquared = radius * radius;

	if(distanceSquared <= radiusSquared)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsPointInSectorAngle2D( const Vec2& point, const Vec2& origin, float distance, float orientationDegrees, float apertureDegrees )
{
	if(!IsPointInDisc(point, origin, distance))
	{
		return false;
	}

	Vec2 displacementFromOriginToPoint		   = point - origin;
	float angleFromOriginToPoint			   = displacementFromOriginToPoint.GetAngleDegrees();
	float angularDisplacementFromOriginToPoint = GetAngularDisplacement(orientationDegrees, angleFromOriginToPoint);

	if(fabsf(angularDisplacementFromOriginToPoint) <= (apertureDegrees * 0.5f))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsPointInSectorVector2D( const Vec2& point, const Vec2& origin, float distance, const Vec2& sectorDirection, float apertureDegrees )
{
	float orientationDegrees = sectorDirection.GetAngleDegrees();
	return IsPointInSectorAngle2D(point, origin, distance, orientationDegrees, apertureDegrees);
}

//-----------------------------------------------------------------------------------------------
Vec2 TransformPosition( const Vec2& position, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY )
{
	// Scale
	Vec2 scaled = position * uniformScale;

	// Rotate
	Vec2 rotated = scaled.GetRotatedDegrees(rotationDegreesAboutZ);

	// Translate
	Vec2 translated = rotated + translationXY;

	// Return the finished Vec2
	return translated;
}

//-----------------------------------------------------------------------------------------------
Vec3 TransformPosition( const Vec3& position, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY )
{
	Vec2 newPosition = TransformPosition(Vec2(position.x, position.y), uniformScale, rotationDegreesAboutZ, translationXY);

	// Return the finished Vec3
	return Vec3(newPosition.x, newPosition.y, position.z);
}

Vec2 GetClosestPointOnAABB2( Vec2 point, AABB2* boundingBox )
{
	float closestX = Clamp(point.x, boundingBox->mins.x, boundingBox->maxs.x);
	float closestY = Clamp(point.y, boundingBox->mins.y, boundingBox->maxs.y);

	return Vec2(closestX, closestY);
}

Vec2 GetClosestPointOnAABB2Edge( Vec2 point, AABB2* boundingBox )
{
	Vec2 closestPoint = GetClosestPointOnAABB2(point, boundingBox);

	int edge = 0;
	float distance = 9999999999999.9f;
	if(boundingBox->maxs.y - point.y < distance)
	{
		distance = boundingBox->maxs.y - point.y;
		edge = 0;
	}

	if(boundingBox->maxs.x - point.x < distance)
	{
		distance = boundingBox->maxs.x - point.x;
		edge = 1;
	}

	if(std::abs(boundingBox->mins.y - point.y) < distance)
	{
		distance = boundingBox->mins.y - point.y;
		edge = 2;
	}

	if(std::abs(boundingBox->mins.x - point.x) < std::abs(distance))
	{
		distance = boundingBox->mins.x - point.x;
		edge = 3;
	}


	if(edge % 2 == 0)
	{
		return closestPoint + Vec2(distance, 0.0f);
	}
	else
	{
		return closestPoint + Vec2(0.0f, distance);
	}
}

bool DoesPointIntersectAABB2( Vec2 point, AABB2 boundingBox )
{
	Vec2 newPoint = GetClosestPointOnAABB2(point, &boundingBox);

	if(newPoint == point)
	{
		return true;
	}

	return false;
}

void PushDiscOutOfAABB2D( Vec2& discCenter, float radius, const AABB2& box )
{
	UNUSED(discCenter);
	UNUSED(radius);
	UNUSED(box);
// 	Vec2 closestPoint = GetClosestPointOnAABB2(discCenter, box);
// 	PushDiscOutOfPoint(discCenter, radius, closestPoint);
}

bool DoABB2sOverlap(AABB2* box1, AABB2* box2)
{
	Vec2 min = box1->mins.GetMax(box2->mins);
	Vec2 max = box1->maxs.GetMin(box2->maxs);

	if(min.x < max.x && min.y < max.y)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void PushDiscOutOfPoint( Vec2& discCenter, float radius, const Vec2& point )
{
	if(!IsPointInDisc(point, discCenter, radius))
	{
		return;
	}

	float overDistance = GetOverLapDistance(point, discCenter, radius);
	Vec2 moveDisplacement = discCenter - point;
	moveDisplacement.SetLength(overDistance);
	discCenter += moveDisplacement;
}

Vec2 ProjectedVector( const Vec2& toProject, const Vec2& ontoThis )
{
	float dotProduct = DotProductVec2(ontoThis, toProject);

	if(dotProduct == 0.0f)
	{
		return Vec2(0.0f, 0.0f);
	}

	return (ontoThis * dotProduct) / ontoThis.GetLengthSquared();
}

Vec2 GetClosestPointOnLine2D( const Vec2& referencePos, const Vec2& pointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2 dispToStartPoint = referencePos - pointOnLine;
	Vec2 dispBetweenPoints = anotherPointOnLine - pointOnLine;
	Vec2 projection = Vec2(0.0f, 0.0f);
	Vec2 closestPoint = Vec2(0.0f, 0.0f);

	projection = ProjectedVector(dispToStartPoint, dispBetweenPoints);

	closestPoint = pointOnLine + projection;

	return closestPoint;
}

Vec2 GetClosestPointOnLineSegment2D( const Vec2& referencePos, const Vec2& lineStart, const Vec2& lineEnd )
{
	Vec2 lineSegment = lineEnd - lineStart;
	Vec2 vectorToStartPoint = referencePos - lineStart;
	Vec2 vectorToEndPoint = referencePos - lineEnd;

	if(DotProductVec2(lineSegment, vectorToStartPoint) < 0)
	{
		//We are in Voronoi region 1, point is before line segment start
		return lineStart;
	}
	else if(DotProductVec2(lineSegment, vectorToEndPoint) > 0)
	{
		//We are in Voronoi region 2, point is after line segment start
		return lineEnd;
	}
	else
	{
		Vec2 closestPoint = GetClosestPointOnLine2D(referencePos, lineStart, lineEnd);
		return closestPoint;
	}
}


float SmoothStart2( float t )
{
	return t * t;
}

float SmoothStart3( float t )
{
	return t * t * t;
}

float SmoothStart4( float t )
{
	return t * t * t * t;
}

float SmoothStart5( float t )
{
	return t * t * t * t * t;
}

float SmoothStop2( float t )
{
	return 1 - ((1 - t) * (1 - t));
}

float SmoothStop3( float t )
{
	return 1 - ((1 - t) * (1 - t) * (1 - t));
}

float SmoothStop4( float t )
{
	return 1 - ((1 - t) * (1 - t) * (1 - t) * (1 - t));
}

float SmoothStop5( float t )
{
	return 1 - ((1 - t) * (1 - t) * (1 - t) * (1 - t) * (1 - t));
}

float SmoothStep3( float t )
{
	return (3 * t * t) - (2 * t * t * t);
}

float SmoothStep5( float t )
{
	return (6 * t * t * t * t * t) - (15 * t * t * t * t) + (10 * t * t * t);
}

float GetOverLapDistance( const Vec2& point, Vec2& discCenter, float radius )
{
	float distance = GetDistance(discCenter, point);
	return radius - distance;
}

Vec3 GetSphericalToCoord( float radius, float theta, float phi )
{
	Vec3 coords;

	coords.x = CosDegrees(phi) * CosDegrees(theta);
	coords.y = SinDegrees(phi);
	coords.z = CosDegrees(phi) * SinDegrees(theta);

	coords *= radius;

	return coords;
}

Vec3 GetTangentFromSphereCoords(float theta, float phi)
{
	Vec3 coords;

	coords.x = -CosDegrees(phi) * SinDegrees(theta);
	coords.y = 0.0f;
	coords.z = CosDegrees(phi) * CosDegrees(theta);

	return coords;

}

//-----------------------------------------------------------------------------------------------
bool DoLinesOverlap(const Line& line1, const Line& line2, Vec2* possibleIntersection)
{
	bool intersects = false;

	// I like to see a's and b's when doing math rather than variable names;
	Vec2 a = line1.lineStart;
	Vec2 b = line1.lineEnd;
	Vec2 c = line2.lineStart;
	Vec2 d = line2.lineEnd;

	// The vectors going from a to b and d to c;
	Vec2 r = b - a;
	Vec2 s = d - c;
	
	// Math has been reduced down to these equations;
	//float de = r.x * s.y - r.y * s.x;
	float de = CrossProduct(r, s);
	float u = CrossProduct((c - a), r) / de;
	float t = CrossProduct((c - a), s) / de;
	//float u = ((c.x - a.x) * r.y - (c.y - a.y) * r.x) / de;
	//float t = ((c.x - a.x) * s.y - (c.y - a.y) * s.x) / de;

	// Both between 0 and 1, then they share a point;
	if(0.0f <= u && u <= 1.0f && 0.0f <= t && t <= 1.0f)
	{
		*possibleIntersection = a + t * r;
		intersects = true;
	}

	return intersects;
}

uint Quadratic( float* out, float a, float b, float c )
{
	if(a == 0.0f)
	{
		if(b == 0.0f)
		{
			if(c == 0.0f)
			{
				out[0] = 0.0f;
				return 1;
			}
			else
			{
				return 0;
			}
		}

		out[0] = (-c) / b;		
		return 1;
	}

	// -b +- sqrt(b*b - 4*a*c) / 2*a

	float chunk1 = b * b;
	float chunk2 = 4.0f * a * c;
	float chunk3 = chunk1 - chunk2;

	if(chunk3 == 0.0f)
	{
		out[0] = (-b) / 2 * a;
		return 1;
	}
	if(chunk3 < 0.0f)
	{
		return 0;
	}

	float chunk4 = sqrtf(chunk3);

	float chunk5 = (-b) + chunk4;
	out[0] = chunk5 / (2.0f * a);
	float chunk6 = (-b) - chunk4;
	out[1] = chunk6 / (2.0f * a);
	return 2;
}

int GetIndexFromCoord( const IntVec2& coord, const IntVec2& dimensions )
{
	return coord.y * dimensions.x + coord.x;
}

IntVec2 GetCoordFromIndex( int index, const IntVec2& dimensions )
{
	int y = index / dimensions.x;
	int x = index % dimensions.x;

	return IntVec2(x, y);
}

bool IsTileCoordInBounds( const IntVec2& coord, const IntVec2& dimensions )
{
	if(coord.x < 0
	|| coord.x >= dimensions.x
	|| coord.y < 0
	|| coord.y >= dimensions.y)
	{
		return false;
	}

	return true;
}

float GetDiscOverlapAmount( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float distance = GetDistance(centerA, centerB);
	float radiusSum = radiusA + radiusB;
	return radiusSum - distance;
}

float GetDiscOverlapAmount( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	float distance = GetDistance(centerA, centerB);
	float radiusSum = radiusA + radiusB;
	return radiusSum - distance;
}

void PushDiscOutOfDisc( Vec2& discCenterA, float radiusA, Vec2& discCenterB, float radiusB )
{
	float overlapDistance = GetDiscOverlapAmount(discCenterA, radiusA, discCenterB, radiusB);
	Vec2 moveDirection = discCenterA - discCenterB;
	moveDirection.SetLength(overlapDistance);
	discCenterA += moveDirection;	
}

void PushDiscsOutOfDiscs( Vec2& discCenterA, float radiusA, Vec2& discCenterB, float radiusB )
{
	float overlapDistanceA = GetDiscOverlapAmount(discCenterA, radiusA, discCenterB, radiusB);
	Vec2 moveDirectionA = discCenterA - discCenterB;
	moveDirectionA.SetLength(overlapDistanceA);	

	float overlapDistanceB = GetDiscOverlapAmount(discCenterB, radiusB, discCenterA, radiusA);
	Vec2 moveDirectionB = discCenterB - discCenterA;
	moveDirectionB.SetLength(overlapDistanceB);

	discCenterA += moveDirectionA;
	discCenterB += moveDirectionB;
}

// void PushDiscsOutOfDiscs( Vec3& discCenterA, float radiusA, Vec3& discCenterB, float radiusB )
// {
// 	float overlapDistanceA = GetDiscOverlapAmount(discCenterA, radiusA, discCenterB, radiusB);
// 	Vec2 moveDirectionA = discCenterA - discCenterB;
// 	moveDirectionA.SetLength(overlapDistanceA);	
// 
// 	float overlapDistanceB = GetDiscOverlapAmount(discCenterB, radiusB, discCenterA, radiusA);
// 	Vec2 moveDirectionB = discCenterB - discCenterA;
// 	moveDirectionB.SetLength(overlapDistanceB);
// 
// 	discCenterA += moveDirectionA;
// 	discCenterB += moveDirectionB;
// }

