#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const Matrix4x4 Matrix4x4::IDENTITY;

Matrix4x4::Matrix4x4()
{
	m_values[Ix] = 1.0f;
	m_values[Iy] = 0.0f;
	m_values[Iz] = 0.0f;
	m_values[Iw] = 0.0f;

	m_values[Jx] = 0.0f;
	m_values[Jy] = 1.0f;
	m_values[Jz] = 0.0f;
	m_values[Jw] = 0.0f;

	m_values[Kx] = 0.0f;
	m_values[Ky] = 0.0f;
	m_values[Kz] = 1.0f;
	m_values[Kw] = 0.0f;

	m_values[Tx] = 0.0f;
	m_values[Ty] = 0.0f;
	m_values[Tz] = 0.0f;
	m_values[Tw] = 1.0f;
}


Matrix4x4::Matrix4x4( Vec3 iBasis, Vec3 jBasis, Vec3 kBasis, Vec3 translation )
{
	SetI(iBasis);
	SetJ(jBasis);
	SetK(kBasis);
	SetT(translation);
}

Vec2 Matrix4x4::TransformPosition2D( const Vec2& position ) const
{
	Vec2 resultVec;

	resultVec.x = (m_values[Ix] * position.x) + (m_values[Jx] * position.y) + m_values[Tx];
	resultVec.y = (m_values[Iy] * position.x) + (m_values[Jy] * position.y) + m_values[Ty];

	return resultVec;
}

Vec3 Matrix4x4::TransformPosition3D( const Vec3& position ) const
{
	Vec3 resultVec;

	resultVec.x = (m_values[Ix] * position.x) + (m_values[Jx] * position.y) + (m_values[Kx] * position.z) + m_values[Tx];
	resultVec.y = (m_values[Iy] * position.x) + (m_values[Jy] * position.y) + (m_values[Ky] * position.z) + m_values[Ty];
	resultVec.z = (m_values[Iz] * position.x) + (m_values[Jz] * position.y) + (m_values[Kz] * position.z) + m_values[Tz];

	return resultVec;
}

Vec3 Matrix4x4::TransformVector3D( const Vec3& vector ) const
{
	Vec3 resultVec;

	resultVec.x = (m_values[Ix] * vector.x) + (m_values[Jx] * vector.y) + (m_values[Kx] * vector.z);
	resultVec.y = (m_values[Iy] * vector.x) + (m_values[Jy] * vector.y) + (m_values[Ky] * vector.z);
	resultVec.z = (m_values[Iz] * vector.x) + (m_values[Jz] * vector.y) + (m_values[Kz] * vector.z);

	return resultVec;
}

Vec4 Matrix4x4::TransformHomogeneousPoint3D( const Vec4& position ) const
{
	Vec4 resultVec;

	resultVec.x = (m_values[Ix] * position.x) + (m_values[Jx] * position.y) + (m_values[Kx] * position.z) + (m_values[Tx] * position.w);
	resultVec.y = (m_values[Iy] * position.x) + (m_values[Jy] * position.y) + (m_values[Ky] * position.z) + (m_values[Ty] * position.w);
	resultVec.z = (m_values[Iz] * position.x) + (m_values[Jz] * position.y) + (m_values[Kz] * position.z) + (m_values[Tz] * position.w);
	resultVec.w = (m_values[Iw] * position.x) + (m_values[Jw] * position.y) + (m_values[Kw] * position.z) + (m_values[Tw] * position.w);

	return resultVec;
}

Matrix4x4::~Matrix4x4()
{

}

Matrix4x4 Matrix4x4::Transform( Matrix4x4 matrixB )
{
	Matrix4x4 matrixA = *this;
	Matrix4x4 maxtrixC;

	// I Basis --------------------------------------------------------------
	maxtrixC.m_values[Ix] = matrixB.m_values[Ix] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kx] + 
							matrixB.m_values[Iw] * matrixA.m_values[Tx] ) );

	maxtrixC.m_values[Iy] = matrixB.m_values[Jx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kx] + 
						    matrixB.m_values[Jw] * matrixA.m_values[Tx] ) ); 

	maxtrixC.m_values[Iz] = matrixB.m_values[Kx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kx] + 
						    matrixB.m_values[Kw] * matrixA.m_values[Tx] ) ); 

	maxtrixC.m_values[Iw] = matrixB.m_values[Tx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kx] + 
						    matrixB.m_values[Tw] * matrixA.m_values[Tx] ) );
	
	// J Basis --------------------------------------------------------------
	maxtrixC.m_values[Jx] = matrixB.m_values[Ix] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Ky] + 
						    matrixB.m_values[Iw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jy] = matrixB.m_values[Jx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Ky] + 
						    matrixB.m_values[Jw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jz] = matrixB.m_values[Kx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Ky] + 
						    matrixB.m_values[Kw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jw] = matrixB.m_values[Tx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Ky] + 
						    matrixB.m_values[Tw] * matrixA.m_values[Ty] ) );
	
	// K Basis --------------------------------------------------------------
	maxtrixC.m_values[Kx] = matrixB.m_values[Ix] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kz] + 
						    matrixB.m_values[Iw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Ky] = matrixB.m_values[Jx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kz] + 
						    matrixB.m_values[Jw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Kz] = matrixB.m_values[Kx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kz] + 
						    matrixB.m_values[Kw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Kw] = matrixB.m_values[Tx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kz] + 
						    matrixB.m_values[Tw] * matrixA.m_values[Tz] ) );
	
	// T Basis --------------------------------------------------------------
	maxtrixC.m_values[Tx] = matrixB.m_values[Ix] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kw] + 
						    matrixB.m_values[Iw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Ty] = matrixB.m_values[Jx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kw] + 
						    matrixB.m_values[Jw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Tz] = matrixB.m_values[Kx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kw] + 
						    matrixB.m_values[Kw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Tw] = matrixB.m_values[Tx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kw] + 
						    matrixB.m_values[Tw] * matrixA.m_values[Tw] ) );

	return maxtrixC;
}

void Matrix4x4::TransformPoints( int count, Vec4* outPoints )
{
	for(int i = 0; i < count; i++)
	{
		outPoints[i] = TransformHomogeneousPoint3D(outPoints[i]);
	}
}

const Matrix4x4 Matrix4x4::MakeXRotationDegrees( float degreesAboutX )
{
	float cosine = CosDegrees( degreesAboutX );
	float sine = SinDegrees( degreesAboutX );

	Matrix4x4 rotationMatrix;
	
	/*
	      I   J   K   T
	  ---------------------
	X |   1   0   0   0   |
	Y |   0   c  -s   0   |
	Z |   0   s   c   0   |
	W |   0   0   0   1   |
	  ---------------------
	*/

	rotationMatrix.m_values[ Jy ] = cosine;
	rotationMatrix.m_values[ Jz ] = sine;
	rotationMatrix.m_values[ Ky ] = -sine;
	rotationMatrix.m_values[ Kz ] = cosine;

	return rotationMatrix;
}

const Matrix4x4 Matrix4x4::MakeYRotationDegrees( float degreesAboutY )
{
	float cosine = CosDegrees( degreesAboutY );
	float sine = SinDegrees( degreesAboutY );

	Matrix4x4 rotationMatrix;

	/*
	      I   J   K   T
	  ---------------------
	X |   c   0   s   0   |
	Y |   0   1   0   0   |
	Z |  -s   0   c   0   |
	W |   0   0   0   1   |
	  ---------------------
	*/

	rotationMatrix.m_values[ Ix ] = cosine;
	rotationMatrix.m_values[ Iz ] = -sine;
	rotationMatrix.m_values[ Kx ] = sine;
	rotationMatrix.m_values[ Kz ] = cosine;

	return rotationMatrix;
}

const Matrix4x4 Matrix4x4::MakeZRotationDegrees( float degreesAboutZ )
{
	float cosine = CosDegrees( degreesAboutZ );
	float sine = SinDegrees( degreesAboutZ );

	Matrix4x4 rotationMatrix;

	/*
	      I   J   K   T
	  ---------------------
	X |   c  -s   0   0   |
	Y |   s   c   0   0   |
	Z |   0   0   1   0   |
	W |   0   0   0   1   |
	  ---------------------
	*/

	rotationMatrix.m_values[ Ix ] = cosine;
	rotationMatrix.m_values[ Iy ] = sine;
	rotationMatrix.m_values[ Jx ] = -sine;
	rotationMatrix.m_values[ Jy ] = cosine;

	return rotationMatrix;
}

const Matrix4x4 Matrix4x4::MakeTranslation2D( const Vec2& translationXY )
{
	Matrix4x4 translationMatrix;

	translationMatrix.m_values[ Tx ] = translationXY.x;
	translationMatrix.m_values[ Ty ] = translationXY.y;

	return translationMatrix;
}

const Matrix4x4 Matrix4x4::MakeTranslation3D( Matrix4x4& matrix, const Vec3& translationXYZ )
{
	matrix.m_values[Tx] = translationXYZ.x;
	matrix.m_values[Ty] = translationXYZ.y;
	matrix.m_values[Tz] = translationXYZ.z;

	return matrix;
}

const Matrix4x4 Matrix4x4::MakeUniformScale2D( float uniformScaleXY )
{
	Matrix4x4 scaleMatrix;

	scaleMatrix.m_values[ Ix ] = uniformScaleXY;
	scaleMatrix.m_values[ Jy ] = uniformScaleXY;

	return scaleMatrix;
}

const Matrix4x4 Matrix4x4::MakeUniformScale3D( float uniformScaleXYZ )
{
	Matrix4x4 scaleMatrix;

	scaleMatrix.m_values[ Ix ] = uniformScaleXYZ;
	scaleMatrix.m_values[ Jy ] = uniformScaleXYZ;
	scaleMatrix.m_values[ Kz ] = uniformScaleXYZ;

	return scaleMatrix;
}

const Matrix4x4 Matrix4x4::MakeProjectionOrthoMatrix( const Vec2& min, const Vec2& max, float nearZ, float farZ )
{
	Matrix4x4 orthoMatrix;

	orthoMatrix.m_values[ Ix ] = 2.0f * (1.0f / (max.x - min.x));
	orthoMatrix.m_values[ Jy ] = 2.0f * (1.0f / (max.y - min.y));
	orthoMatrix.m_values[ Kz ] = 1.0f * (1.0f / (farZ - nearZ));
	orthoMatrix.m_values[ Tx ] = -(max.x + min.x) / (max.x - min.x);
	orthoMatrix.m_values[ Ty ] = -(max.y + min.y) / (max.y - min.y);
	orthoMatrix.m_values[ Tz ] = -(nearZ) / (farZ - nearZ);
	orthoMatrix.m_values[ Tw ] = 1.0f;

	return orthoMatrix;
}

const Matrix4x4 Matrix4x4::MakeProjectionPerspectiveMatrix( float fieldOfViewDegrees, float aspect, float nearZ, float farZ )
{
	Matrix4x4 perspectiveMatrix;

	float radians = ConvertDegreesToRadians(fieldOfViewDegrees);
	float size = 1.0f / tanf(radians / 2.0f);

	float w = size;
	float h = size;

	if(aspect > 1.0f)
	{
		w /= aspect;
	}
	else
	{
		h *= aspect;
	}

	float q = 1.0f / (farZ - nearZ);

	perspectiveMatrix.m_values[ Ix ] = w;
	perspectiveMatrix.m_values[ Jy ] = h;
	perspectiveMatrix.m_values[ Kz ] = farZ * q;
	perspectiveMatrix.m_values[ Kw ] = 1.0f;
	perspectiveMatrix.m_values[ Tz ] = -nearZ * farZ * q;
	perspectiveMatrix.m_values[ Tw ] = 0.0f;

	return perspectiveMatrix;
}

const Matrix4x4 Matrix4x4::ForEulerXYZ( Vec3 cameraEuler, Vec3 cameraPosition )
{
	Matrix4x4 rotationMatrix;

	rotationMatrix = ForEuler(cameraEuler, ROTATION_ORDER_XYZ);
		

	return rotationMatrix;
}

const Matrix4x4 Matrix4x4::ForEulerZXY( Vec3 euler, Vec3 position )
{
	Matrix4x4 rotationMatrix;

	rotationMatrix = ForEuler(euler, ROTATION_ORDER_ZXY);
	rotationMatrix = Matrix4x4::MakeTranslation3D(rotationMatrix, position);

	return rotationMatrix;
}

const Matrix4x4 Matrix4x4::ForEuler( Vec3 cameraEuler, RotationOrder rotationOrder )
{
	Matrix4x4 rotationX = MakeXRotationDegrees(cameraEuler.x);
	Matrix4x4 rotationY = MakeYRotationDegrees(cameraEuler.y);
	Matrix4x4 rotationZ = MakeZRotationDegrees(cameraEuler.z);

	// In the order, FORWARD, RIGHT, UP
	Matrix4x4 rotationMatrix;

	if(rotationOrder == ROTATION_ORDER_ZXY)
	{
		rotationMatrix = rotationZ;
		rotationMatrix = rotationMatrix.Transform(rotationX);
		rotationMatrix = rotationMatrix.Transform(rotationY);
	}
	else
	{
		// Unhandled case
		ERROR_AND_DIE("Unhandled Rotation Order Request.");
	}

	return rotationMatrix;
}

const Matrix4x4 Matrix4x4::LookAt( Vec3 position, Vec3 target, Vec3 upReference )
{
	// FORWARD = normal of target - position
	Vec3 forward = target - position;
	forward.Normalize();

	// RIGHT = normal of cross of upReference and forward
	Vec3 right = CrossProduct(upReference, forward);
	right.Normalize();

	// UP = normal of cross of forward and right
	Vec3 up = CrossProduct(forward, right);
	up.Normalize();

	Vec3 translation = position;

	Matrix4x4 mat = Matrix4x4(right, up, forward, translation);

	return mat;
}

void Matrix4x4::Transpose()
{
	Matrix4x4 oldMatrix = *this;

	m_values[Ix] = oldMatrix.m_values[Ix];
	m_values[Iy] = oldMatrix.m_values[Jx];
	m_values[Iz] = oldMatrix.m_values[Kx];
	m_values[Iw] = oldMatrix.m_values[Tx];

	m_values[Jx] = oldMatrix.m_values[Iy];
	m_values[Jy] = oldMatrix.m_values[Jy];
	m_values[Jz] = oldMatrix.m_values[Ky];
	m_values[Jw] = oldMatrix.m_values[Ty];

	m_values[Kx] = oldMatrix.m_values[Iz];
	m_values[Ky] = oldMatrix.m_values[Jz];
	m_values[Kz] = oldMatrix.m_values[Kz];
	m_values[Kw] = oldMatrix.m_values[Tz];

	m_values[Tx] = oldMatrix.m_values[Iw];
	m_values[Ty] = oldMatrix.m_values[Jw];
	m_values[Tz] = oldMatrix.m_values[Kw];
	m_values[Tw] = oldMatrix.m_values[Tw];
}

// ----------------------------------------------------------------------
Vec4 Matrix4x4::GetI4()
{
	return Vec4(m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw]);
}

// ----------------------------------------------------------------------
Vec4 Matrix4x4::GetJ4()
{
	return Vec4(m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw]);
}

// ----------------------------------------------------------------------
Vec4 Matrix4x4::GetK4()
{
	return Vec4(m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw]);
}

// ----------------------------------------------------------------------
Vec4 Matrix4x4::GetT4()
{
	return Vec4(m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw]);
}

// ----------------------------------------------------------------------
Vec3 Matrix4x4::GetI()
{
	return Vec3(m_values[Ix], m_values[Iy], m_values[Iz]);
}

// ----------------------------------------------------------------------
Vec3 Matrix4x4::GetJ()
{
	return Vec3(m_values[Jx], m_values[Jy], m_values[Jz]);
}

// ----------------------------------------------------------------------
Vec3 Matrix4x4::GetK()
{
	return Vec3(m_values[Kx], m_values[Ky], m_values[Kz]);
}

// ----------------------------------------------------------------------
Vec3 Matrix4x4::GetT()
{
	return Vec3(m_values[Tx], m_values[Ty], m_values[Tz]);
}

// ----------------------------------------------------------------------
void Matrix4x4::SetI( Vec3 iBasis )
{
	m_values[Ix] = iBasis.x;
	m_values[Iy] = iBasis.y;
	m_values[Iz] = iBasis.z;
	m_values[Iw] = 0.0f;
}

// ----------------------------------------------------------------------
void Matrix4x4::SetJ( Vec3 jBasis )
{
	m_values[Jx] = jBasis.x;
	m_values[Jy] = jBasis.y;
	m_values[Jz] = jBasis.z;
	m_values[Jw] = 0.0f;
}

// ----------------------------------------------------------------------
void Matrix4x4::SetK( Vec3 kBasis )
{
	m_values[Kx] = kBasis.x;
	m_values[Ky] = kBasis.y;
	m_values[Kz] = kBasis.z;
	m_values[Kw] = 0.0f;
}

// ----------------------------------------------------------------------
void Matrix4x4::SetT( Vec3 tBasis )
{
	m_values[Tx] = tBasis.x;
	m_values[Ty] = tBasis.y;
	m_values[Tz] = tBasis.z;
	m_values[Tw] = 1.0f;
}

// ----------------------------------------------------------------------
void Matrix4x4::Invert()
{
	double inv[16];
	double det;
	double m[16];
	unsigned int i;

	for (i = 0; i < 16; ++i) {
		m[i] = (double) m_values[i];
	}

	inv[0] = m[5]  * m[10] * m[15] - 
			 m[5]  * m[11] * m[14] - 
			 m[9]  * m[6]  * m[15] + 
			 m[9]  * m[7]  * m[14] +
			 m[13] * m[6]  * m[11] - 
			 m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
			  m[4]  * m[11] * m[14] + 
			  m[8]  * m[6]  * m[15] - 
			  m[8]  * m[7]  * m[14] - 
			  m[12] * m[6]  * m[11] + 
			  m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
			 m[4]  * m[11] * m[13] - 
			 m[8]  * m[5] * m[15] + 
			 m[8]  * m[7] * m[13] + 
			 m[12] * m[5] * m[11] - 
			 m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
			   m[4]  * m[10] * m[13] +
			   m[8]  * m[5] * m[14] - 
			   m[8]  * m[6] * m[13] - 
			   m[12] * m[5] * m[10] + 
			   m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
			  m[1]  * m[11] * m[14] + 
			  m[9]  * m[2] * m[15] - 
			  m[9]  * m[3] * m[14] - 
			  m[13] * m[2] * m[11] + 
			  m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
			 m[0]  * m[11] * m[14] - 
			 m[8]  * m[2] * m[15] + 
			 m[8]  * m[3] * m[14] + 
			 m[12] * m[2] * m[11] - 
			 m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
			  m[0]  * m[11] * m[13] + 
			  m[8]  * m[1] * m[15] - 
			  m[8]  * m[3] * m[13] - 
			  m[12] * m[1] * m[11] + 
			  m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
			  m[0]  * m[10] * m[13] - 
			  m[8]  * m[1] * m[14] + 
			  m[8]  * m[2] * m[13] + 
			  m[12] * m[1] * m[10] - 
			  m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
			 m[1]  * m[7] * m[14] - 
			 m[5]  * m[2] * m[15] + 
			 m[5]  * m[3] * m[14] + 
			 m[13] * m[2] * m[7] - 
			 m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
			  m[0]  * m[7] * m[14] + 
			  m[4]  * m[2] * m[15] - 
			  m[4]  * m[3] * m[14] - 
			  m[12] * m[2] * m[7] + 
			  m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
			  m[0]  * m[7] * m[13] - 
			  m[4]  * m[1] * m[15] + 
			  m[4]  * m[3] * m[13] + 
			  m[12] * m[1] * m[7] - 
			  m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
			   m[0]  * m[6] * m[13] + 
			   m[4]  * m[1] * m[14] - 
			   m[4]  * m[2] * m[13] - 
			   m[12] * m[1] * m[6] + 
			   m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
			  m[1] * m[7] * m[10] + 
			  m[5] * m[2] * m[11] - 
			  m[5] * m[3] * m[10] - 
			  m[9] * m[2] * m[7] + 
			  m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
			 m[0] * m[7] * m[10] - 
			 m[4] * m[2] * m[11] + 
			 m[4] * m[3] * m[10] + 
			 m[8] * m[2] * m[7] - 
			 m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
			   m[0] * m[7] * m[9] + 
			   m[4] * m[1] * m[11] - 
			   m[4] * m[3] * m[9] - 
			   m[8] * m[1] * m[7] + 
			   m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
			  m[0] * m[6] * m[9] - 
			  m[4] * m[1] * m[10] + 
			  m[4] * m[2] * m[9] + 
			  m[8] * m[1] * m[6] - 
			  m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	for (i = 0; i < 16; i++) {
		m_values[i] = (float)(inv[i] * det);
	}
}

Matrix4x4 Matrix4x4::GetInverse()
{
	Matrix4x4 matrixA;
	double inv[16];
	double det;
	double m[16];
	unsigned int i;

	for (i = 0; i < 16; ++i) {
		m[i] = (double) m_values[i];
	}

	inv[0] = m[5]  * m[10] * m[15] - 
		m[5]  * m[11] * m[14] - 
		m[9]  * m[6]  * m[15] + 
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] - 
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
		m[4]  * m[11] * m[14] + 
		m[8]  * m[6]  * m[15] - 
		m[8]  * m[7]  * m[14] - 
		m[12] * m[6]  * m[11] + 
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
		m[4]  * m[11] * m[13] - 
		m[8]  * m[5] * m[15] + 
		m[8]  * m[7] * m[13] + 
		m[12] * m[5] * m[11] - 
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] - 
		m[8]  * m[6] * m[13] - 
		m[12] * m[5] * m[10] + 
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
		m[1]  * m[11] * m[14] + 
		m[9]  * m[2] * m[15] - 
		m[9]  * m[3] * m[14] - 
		m[13] * m[2] * m[11] + 
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
		m[0]  * m[11] * m[14] - 
		m[8]  * m[2] * m[15] + 
		m[8]  * m[3] * m[14] + 
		m[12] * m[2] * m[11] - 
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
		m[0]  * m[11] * m[13] + 
		m[8]  * m[1] * m[15] - 
		m[8]  * m[3] * m[13] - 
		m[12] * m[1] * m[11] + 
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
		m[0]  * m[10] * m[13] - 
		m[8]  * m[1] * m[14] + 
		m[8]  * m[2] * m[13] + 
		m[12] * m[1] * m[10] - 
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
		m[1]  * m[7] * m[14] - 
		m[5]  * m[2] * m[15] + 
		m[5]  * m[3] * m[14] + 
		m[13] * m[2] * m[7] - 
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
		m[0]  * m[7] * m[14] + 
		m[4]  * m[2] * m[15] - 
		m[4]  * m[3] * m[14] - 
		m[12] * m[2] * m[7] + 
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
		m[0]  * m[7] * m[13] - 
		m[4]  * m[1] * m[15] + 
		m[4]  * m[3] * m[13] + 
		m[12] * m[1] * m[7] - 
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
		m[0]  * m[6] * m[13] + 
		m[4]  * m[1] * m[14] - 
		m[4]  * m[2] * m[13] - 
		m[12] * m[1] * m[6] + 
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
		m[1] * m[7] * m[10] + 
		m[5] * m[2] * m[11] - 
		m[5] * m[3] * m[10] - 
		m[9] * m[2] * m[7] + 
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
		m[0] * m[7] * m[10] - 
		m[4] * m[2] * m[11] + 
		m[4] * m[3] * m[10] + 
		m[8] * m[2] * m[7] - 
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
		m[0] * m[7] * m[9] + 
		m[4] * m[1] * m[11] - 
		m[4] * m[3] * m[9] - 
		m[8] * m[1] * m[7] + 
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
		m[0] * m[6] * m[9] - 
		m[4] * m[1] * m[10] + 
		m[4] * m[2] * m[9] + 
		m[8] * m[1] * m[6] - 
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	for (i = 0; i < 16; i++) {
		matrixA.m_values[i] = (float)(inv[i] * det);
	}

	return matrixA;
}

void Matrix4x4::InvertOrthonormal()
{
	Vec3 translation = GetT();
	SetT( Vec3(0.0f) );  // remove translation; 
	Transpose(); // invert the rotation part;

	translation *= -1.0f;

	Vec3 newTranslation = TransformPosition3D( translation ); // transforming the inverted translation by the new rotation (since invert T comes first)
	SetT( newTranslation );
}

void Matrix4x4::operator*=( const Matrix4x4 matrixB )
{
	Matrix4x4 matrixA = *this;
	Matrix4x4 maxtrixC;

	// I Basis --------------------------------------------------------------
	maxtrixC.m_values[Ix] = matrixB.m_values[Ix] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kx] + 
							matrixB.m_values[Iw] * matrixA.m_values[Tx] ) );

	maxtrixC.m_values[Iy] = matrixB.m_values[Jx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kx] + 
							matrixB.m_values[Jw] * matrixA.m_values[Tx] ) ); 

	maxtrixC.m_values[Iz] = matrixB.m_values[Kx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kx] + 
							matrixB.m_values[Kw] * matrixA.m_values[Tx] ) ); 

	maxtrixC.m_values[Iw] = matrixB.m_values[Tx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kx] + 
							matrixB.m_values[Tw] * matrixA.m_values[Tx] ) );

	// J Basis --------------------------------------------------------------
	maxtrixC.m_values[Jx] = matrixB.m_values[Ix] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Ky] + 
							matrixB.m_values[Iw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jy] = matrixB.m_values[Jx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Ky] + 
							matrixB.m_values[Jw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jz] = matrixB.m_values[Kx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Ky] + 
							matrixB.m_values[Kw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jw] = matrixB.m_values[Tx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Ky] + 
							matrixB.m_values[Tw] * matrixA.m_values[Ty] ) );

	// K Basis --------------------------------------------------------------
	maxtrixC.m_values[Kx] = matrixB.m_values[Ix] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kz] + 
							matrixB.m_values[Iw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Ky] = matrixB.m_values[Jx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kz] + 
							matrixB.m_values[Jw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Kz] = matrixB.m_values[Kx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kz] + 
							matrixB.m_values[Kw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Kw] = matrixB.m_values[Tx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kz] + 
							matrixB.m_values[Tw] * matrixA.m_values[Tz] ) );

	// T Basis --------------------------------------------------------------
	maxtrixC.m_values[Tx] = matrixB.m_values[Ix] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kw] + 
							matrixB.m_values[Iw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Ty] = matrixB.m_values[Jx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kw] + 
							matrixB.m_values[Jw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Tz] = matrixB.m_values[Kx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kw] + 
							matrixB.m_values[Kw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Tw] = matrixB.m_values[Tx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kw] + 
							matrixB.m_values[Tw] * matrixA.m_values[Tw] ) );

	*this = maxtrixC;
}

Matrix4x4 Matrix4x4::operator*( const Matrix4x4& matrixB )
{
	Matrix4x4 matrixA = *this;
	Matrix4x4 maxtrixC;

	// I Basis --------------------------------------------------------------
	maxtrixC.m_values[Ix] = matrixB.m_values[Ix] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kx] + 
						  	matrixB.m_values[Iw] * matrixA.m_values[Tx] ) );

	maxtrixC.m_values[Iy] = matrixB.m_values[Jx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kx] + 
						  	matrixB.m_values[Jw] * matrixA.m_values[Tx] ) ); 

	maxtrixC.m_values[Iz] = matrixB.m_values[Kx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kx] + 
						  	matrixB.m_values[Kw] * matrixA.m_values[Tx] ) ); 

	maxtrixC.m_values[Iw] = matrixB.m_values[Tx] * matrixA.m_values[Ix] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jx] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kx] + 
						  	matrixB.m_values[Tw] * matrixA.m_values[Tx] ) );

	// J Basis --------------------------------------------------------------
	maxtrixC.m_values[Jx] = matrixB.m_values[Ix] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Ky] + 
						  	matrixB.m_values[Iw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jy] = matrixB.m_values[Jx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Ky] + 
						  	matrixB.m_values[Jw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jz] = matrixB.m_values[Kx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Ky] + 
						  	matrixB.m_values[Kw] * matrixA.m_values[Ty] ) ); 

	maxtrixC.m_values[Jw] = matrixB.m_values[Tx] * matrixA.m_values[Iy] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jy] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Ky] + 
						  	matrixB.m_values[Tw] * matrixA.m_values[Ty] ) );

	// K Basis --------------------------------------------------------------
	maxtrixC.m_values[Kx] = matrixB.m_values[Ix] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kz] + 
						  	matrixB.m_values[Iw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Ky] = matrixB.m_values[Jx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kz] + 
						  	matrixB.m_values[Jw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Kz] = matrixB.m_values[Kx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kz] + 
						  	matrixB.m_values[Kw] * matrixA.m_values[Tz] ) ); 

	maxtrixC.m_values[Kw] = matrixB.m_values[Tx] * matrixA.m_values[Iz] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jz] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kz] + 
						  	matrixB.m_values[Tw] * matrixA.m_values[Tz] ) );

	// T Basis --------------------------------------------------------------
	maxtrixC.m_values[Tx] = matrixB.m_values[Ix] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Iy] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Iz] * matrixA.m_values[Kw] + 
						  	matrixB.m_values[Iw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Ty] = matrixB.m_values[Jx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Jy] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Jz] * matrixA.m_values[Kw] + 
						  	matrixB.m_values[Jw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Tz] = matrixB.m_values[Kx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Ky] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Kz] * matrixA.m_values[Kw] + 
						  	matrixB.m_values[Kw] * matrixA.m_values[Tw] ) ); 

	maxtrixC.m_values[Tw] = matrixB.m_values[Tx] * matrixA.m_values[Iw] + 
						  ( matrixB.m_values[Ty] * matrixA.m_values[Jw] + 
						  ( matrixB.m_values[Tz] * matrixA.m_values[Kw] + 
						  	matrixB.m_values[Tw] * matrixA.m_values[Tw] ) );

	return maxtrixC;
}
