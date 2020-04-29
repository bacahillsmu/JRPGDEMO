#pragma once
#include "Engine/Core/EngineCommon.hpp"


class RenderContext;
class GPUMesh;
class Material;

class Model
{

public:

	Model(RenderContext* renderContext, const char* meshName);

	void SetModelMatrix( Matrix4x4 modelMatrix );

public:

	GPUMesh* m_mesh;
	Material* m_material;
	Matrix4x4 m_modelMatrix;



};