#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"

class RenderContext;
class UniformBuffer;
class DepthStencilTargetView;
struct AABB3;
struct Ray3;

enum FrustumFace
{
	FRUSTRUMFACE_LEFT,	
	FRUSTRUMFACE_RIGHT,
	FRUSTRUMFACE_BOTTOM,
	FRUSTRUMFACE_TOP,	
	FRUSTRUMFACE_FRONT,	
	FRUSTRUMFACE_BACK,

	FRUSTRUMFACE_SIDE_COUNT
};

struct Frustum
{

public:

	bool ContainsPoint( Vec3 position ) const;
	static Frustum MakeFrustumFromCorners(Vec3* corners);


public:

	Plane3 planes[FRUSTRUMFACE_SIDE_COUNT];

};


struct Camera
{	

public:
	
	// Constructor/Deconstructor
	Camera();
	~Camera();

	void SetColorTargetView(ColorTargetView* colorTargetView);
	void SetOrthographicProjection( Vec2 minOrtho, Vec2 maxOrtho, float minZ = -1.0f, float maxZ = 1.0f );
	void SetPerspectiveProjection( float fieldOfViewDegrees, float aspect, float minZ = 0.1f, float maxZ = 200.0f );
	DepthStencilTargetView* GetDepthStencilTargetView();

	// Transforms and Matrixs
	void SetModelMatrix(Matrix4x4 cameraModel);
	Matrix4x4 GetModelMatrix() const;
	Matrix4x4 GetViewMatrix() const;
	Matrix4x4 GetProjectionMatrix() const;

	// Get Directions
	Vec3 GetForward();
	Vec3 GetUp();
	Vec3 GetRight();

	Ray3 GetRayFromClientPosition(IntVec2 clientPosition, IntVec2 clientDimensions);

	// Is updated - must pass view and projection matrixs
	void UpdateUniformBuffer( RenderContext* colorContext ); 

	// Frustum
	Frustum GetWorldFrustum();
	Frustum GetWorldFrustumFromNDCRegion(AABB3 ndcRegion);
	Frustum GetWorldFrustumFromClientRegion( Vec2 clientRegionTL, Vec2 clientRegionBR, IntVec2 clientDimensions );
	
public:

	ColorTargetView* m_cameraColorTargetView = nullptr;
	UniformBuffer* m_cameraUniformBuffer = nullptr;

	// Orthographic Information
	Vec2 m_minOrtho; 
	Vec2 m_maxOrtho;

	// Perspective Information
	float m_fieldOfViewDegrees = 0.0f;
	float m_aspect = 0.0f;

	// Shared Information (Orthographic & Perspective)
	float m_minZ;
	float m_maxZ;

	// Cached Matrixs
	Matrix4x4 m_projection;
	Matrix4x4 m_cameraModel; 
	Matrix4x4 m_view; // is inverse of m_cameraModel

	// Depth Stencil Target View
	DepthStencilTargetView* m_depthStencilTargetView = nullptr;
};
