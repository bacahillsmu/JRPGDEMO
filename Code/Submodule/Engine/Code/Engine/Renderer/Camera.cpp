#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Core/DepthStencilTargetView.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vec4.hpp"


// ----------------------------------------------------------------------
Camera::Camera()
{

}

// ----------------------------------------------------------------------
Camera::~Camera()
{
	delete m_cameraUniformBuffer;
	m_cameraUniformBuffer = nullptr;

	delete m_depthStencilTargetView;
	m_depthStencilTargetView = nullptr;
}

// ----------------------------------------------------------------------
void Camera::SetColorTargetView( ColorTargetView* colorTargetView )
{
	m_cameraColorTargetView = colorTargetView;
}
 // ----------------------------------------------------------------------
void Camera::SetOrthographicProjection( Vec2 minOrtho, Vec2 maxOrtho, float minZ /*= -1.0f*/, float maxZ /*= 1.0f */ )
{
	m_minOrtho = minOrtho;
	m_maxOrtho = maxOrtho;
	m_minZ = minZ;
	m_maxZ = maxZ;

	m_projection = Matrix4x4::MakeProjectionOrthoMatrix(minOrtho, maxOrtho, minZ, maxZ);
}

// ----------------------------------------------------------------------
void Camera::SetPerspectiveProjection( float fieldOfViewDegrees, float aspect, float minZ /*= 0.1f*/, float maxZ /*= 200.0f */ )
{
	m_fieldOfViewDegrees = fieldOfViewDegrees;
	m_aspect = aspect;
	m_minZ = minZ;
	m_maxZ = maxZ;

	m_projection = Matrix4x4::MakeProjectionPerspectiveMatrix(fieldOfViewDegrees, aspect, minZ, maxZ);
}

// ----------------------------------------------------------------------
DepthStencilTargetView* Camera::GetDepthStencilTargetView()
{
	return m_depthStencilTargetView;
}

// ----------------------------------------------------------------------
void Camera::SetModelMatrix( Matrix4x4 cameraModel )
{
	m_cameraModel = cameraModel;
	m_view = m_cameraModel;

	m_view.Invert();
}

Matrix4x4 Camera::GetViewMatrix() const
{
	return m_view;
}

// ----------------------------------------------------------------------
Vec3 Camera::GetForward()
{
	Vec4 kVector = m_cameraModel.GetK4();
	Vec3 k3Vector = Vec3(kVector.x, kVector.y, kVector.z);
	k3Vector.Normalize();
	return k3Vector;
}

// ----------------------------------------------------------------------
Vec3 Camera::GetUp()
{
	Vec4 jVector = m_cameraModel.GetJ4();
	Vec3 j3Vector = Vec3(jVector.x, jVector.y, jVector.z);
	j3Vector.Normalize();
	return j3Vector;
}

// ----------------------------------------------------------------------
Vec3 Camera::GetRight()
{
	Vec3 iVector = m_cameraModel.GetI();
	Vec3 i3Vector = Vec3(iVector.x, iVector.y, iVector.z);
	i3Vector.Normalize();
	return i3Vector;
}

Ray3 Camera::GetRayFromClientPosition( IntVec2 clientPosition, IntVec2 clientDimensions )
{
	Matrix4x4 invertProjection = m_projection;
	Matrix4x4 invertView = m_view;
	invertProjection.Invert();
	invertView.Invert();
	
	float newPositionX = RangeMap((float)clientPosition.x, 0.0f, (float)clientDimensions.x, -1.0f, 1.0f);
	float newPositionY = RangeMap((float)clientPosition.y, 0.0f, (float)clientDimensions.y, 1.0f, -1.0f);

	Vec4 startPosition4 = Vec4(newPositionX, newPositionY, 0.0f, 1.0f);
	Vec4 endPosition4 = Vec4(newPositionX, newPositionY, 1.0f, 1.0f);

	startPosition4 *= invertProjection;
	endPosition4 *= invertProjection;

	startPosition4 *= invertView;
	endPosition4 *= invertView;

	Vec3 startPosition = Vec3((startPosition4.x / startPosition4.w), (startPosition4.y / startPosition4.w), (startPosition4.z / startPosition4.w));
	Vec3 endPosition = Vec3((endPosition4.x / endPosition4.w), (endPosition4.y / endPosition4.w), (endPosition4.z / endPosition4.w));

	return Ray3::FromPoints(startPosition, endPosition);
}

// ----------------------------------------------------------------------
void Camera::UpdateUniformBuffer( RenderContext* context )
{
	if(m_cameraUniformBuffer == nullptr)
	{
		m_cameraUniformBuffer = new UniformBuffer(context);
	}	

 	camera_buffer_t cpuData;
	cpuData.view = m_view;
	cpuData.projection = m_projection;
	cpuData.cameraPosition = m_cameraModel.GetT();

 	// Copy the CPU to the GPU (will create or update the buffer)
 	m_cameraUniformBuffer->CopyCPUToGPU(&cpuData, sizeof(cpuData));

}

// ----------------------------------------------------------------------
// Frustum
// ----------------------------------------------------------------------
Frustum Camera::GetWorldFrustum()
{
	// gl would be -1 to 1 GDC
	AABB3 ndc = AABB3::MakeFromMinsMaxs(Vec3(-1.0f, -1.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));

	Frustum frustum;
	frustum = GetWorldFrustumFromNDCRegion(ndc);

	return frustum;
}

// ----------------------------------------------------------------------
Frustum Camera::GetWorldFrustumFromNDCRegion(AABB3 ndcRegion)
{
	Matrix4x4 invertProjection = m_projection;
	Matrix4x4 invertView = m_view;
	invertProjection.Invert();
	invertView.Invert();


	//    6-----7
	//   /|    /|
	//  2-----3 |
	//  | 4---|-5
	//  |/    |/
	//  0-----1

	Vec3 corners[8];
	ndcRegion.GetCorners( corners );

	Vec4 corners4[8];
	for(int i = 0; i < 8; i++)
	{
		corners4[i] = Vec4(corners[i], 1.0f);
	}

	invertProjection.TransformPoints(8, corners4);
	invertView.TransformPoints(8, corners4);

	for(int i = 0; i < 8; i++)
	{
		corners[i] = Vec3((corners4[i].x / corners4[i].w), (corners4[i].y / corners4[i].w), (corners4[i].z / corners4[i].w));
	}

	return Frustum::MakeFrustumFromCorners(corners);
}

// ----------------------------------------------------------------------
Frustum Camera::GetWorldFrustumFromClientRegion( Vec2 clientRegionBottomLeft, Vec2 clientRegionTopRight, IntVec2 clientDimensions )
{
	float newPositionBLX = RangeMap((float)clientRegionBottomLeft.x, 0.0f, (float)clientDimensions.x, -1.0f, 1.0f);
	float newPositionBLY = RangeMap((float)clientRegionBottomLeft.y, 0.0f, (float)clientDimensions.y, -1.0f, 1.0f);
	float newPositionTRX = RangeMap((float)clientRegionTopRight.x, 0.0f, (float)clientDimensions.x, -1.0f, 1.0f);
	float newPositionTRY = RangeMap((float)clientRegionTopRight.y, 0.0f, (float)clientDimensions.y, -1.0f, 1.0f);
	
	AABB3 ndc = AABB3::MakeFromMinsMaxs( Vec3(newPositionBLX, newPositionBLY, 0.0f), Vec3(newPositionTRX, newPositionTRY, 1.0f) );

	Frustum frustum;
	frustum = GetWorldFrustumFromNDCRegion(ndc);

	return frustum;
}

bool Frustum::ContainsPoint( Vec3 position ) const
{
	bool testsPassed[6];
	int i = 0;
	for(Plane3 plane3: planes)
	{
		testsPassed[i] = plane3.IsInFront(position);
		i++;
	}

	return testsPassed[0] && testsPassed[1] && testsPassed[2] && testsPassed[3] && testsPassed[4] && testsPassed[5];
}

Frustum Frustum::MakeFrustumFromCorners( Vec3* corners )
{
	Frustum frustum;
	
	frustum.planes[FRUSTRUMFACE_LEFT]	= Plane3::FromTriangleLH(corners[0], corners[2], corners[6]);
	frustum.planes[FRUSTRUMFACE_RIGHT]	= Plane3::FromTriangleLH(corners[1], corners[5], corners[7]);
	frustum.planes[FRUSTRUMFACE_TOP]	= Plane3::FromTriangleLH(corners[2], corners[3], corners[7]);
	frustum.planes[FRUSTRUMFACE_BOTTOM] = Plane3::FromTriangleLH(corners[0], corners[4], corners[5]);
	frustum.planes[FRUSTRUMFACE_FRONT]	= Plane3::FromTriangleLH(corners[0], corners[1], corners[3]);
	frustum.planes[FRUSTRUMFACE_BACK]	= Plane3::FromTriangleLH(corners[4], corners[6], corners[7]);

	return frustum;
}
