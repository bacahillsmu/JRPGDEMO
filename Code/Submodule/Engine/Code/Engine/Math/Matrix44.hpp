#pragma once
#include "Engine/Math/Vec4.hpp"

struct Vec2;
struct Vec3;

enum RotationOrder
{
	ROTATION_ORDER_XYZ,
	ROTATION_ORDER_ZXY,

	ROTATION_ORDER_DEFAULT = ROTATION_ORDER_ZXY
};

struct Matrix4x4
{
public:
	
	float m_values[16];	
	enum 
	{ 
		Ix, Iy, Iz, Iw,  
		Jx, Jy, Jz, Jw,  
		Kx, Ky, Kz, Kw,  
		Tx, Ty, Tz, Tw 
	};

	const static Matrix4x4 IDENTITY;

public:

	Matrix4x4();
	~Matrix4x4();

	Matrix4x4(Vec3 iBasis, Vec3 jBasis, Vec3 kBasis, Vec3 translation);
	
// 	Vec2 TransformVec2D( const Vec2& vectorQuantity ) const;   // Assumes z = 0, w = 0
// 	Vec2 TransformPosition2D( const Vec2& position ) const;    // Assumes z = 0, w = 1
// 	Vec3 TransformVec3D( const Vec3& vectorQuantity ) const;   // Assumes w = 0
 	Vec2 TransformPosition2D( const Vec2& position ) const;	   // Assumes w = 1
 	Vec3 TransformPosition3D( const Vec3& position ) const;	   // Assumes w = 1
 	Vec3 TransformVector3D( const Vec3& vector ) const;	   // Assumes w = 1
	Vec4 TransformHomogeneousPoint3D( const Vec4& position ) const; // Assumes nothing
	
	// Retrieve Columns
	Vec4 GetI4();
	Vec4 GetJ4();
	Vec4 GetK4();
	Vec4 GetT4();
	Vec3 GetI();
	Vec3 GetJ();
	Vec3 GetK();
	Vec3 GetT();
	void SetI(Vec3 iBasis);
	void SetJ(Vec3 jBasis);
	void SetK(Vec3 kBasis);
	void SetT(Vec3 tBasis);

	// Transform
	Matrix4x4 Transform(Matrix4x4 matrixB);
	void TransformPoints(int count, Vec4* outPoints);

	// Rotation, Translation, Scale
 	static const Matrix4x4 MakeXRotationDegrees( float degreesAboutX );
 	static const Matrix4x4 MakeYRotationDegrees( float degreesAboutY );
	static const Matrix4x4 MakeZRotationDegrees( float degreesAboutZ );
	static const Matrix4x4 MakeTranslation2D( const Vec2& translationXY );
	static const Matrix4x4 MakeTranslation3D( Matrix4x4& matrix, const Vec3& translationXYZ );
	static const Matrix4x4 MakeUniformScale2D( float uniformScaleXY );
	static const Matrix4x4 MakeUniformScale3D( float uniformScaleXYZ );
	
	// Projection Matrixs
	static const Matrix4x4 MakeProjectionOrthoMatrix(const Vec2& min, const Vec2& max, float nearZ, float farZ);
	static const Matrix4x4 MakeProjectionPerspectiveMatrix(float fieldOfViewDegrees, float aspect, float nearZ, float farZ);
	
	// Euler Matrixs
	static const Matrix4x4 ForEulerXYZ(Vec3 cameraEuler, Vec3 cameraPosition);
	static const Matrix4x4 ForEulerZXY(Vec3 cameraEuler, Vec3 cameraPosition);
	static const Matrix4x4 ForEuler(Vec3 cameraEuler, RotationOrder rotationOrder);

	static const Matrix4x4 LookAt(Vec3 position, Vec3 target, Vec3 upReference);

	void Transpose();
	void Invert();
	Matrix4x4 GetInverse();
	void InvertOrthonormal();

	Matrix4x4 operator*( const Matrix4x4& matrixB );
	void operator*=( const Matrix4x4 matrixB );

private:





};








