#include "Engine/Renderer/Model.hpp"


#include <fstream>
#include <sstream>
#include <iostream>


Model::Model( RenderContext* renderContext, const char* meshName )
{
	m_mesh = renderContext->GetOrCreateMesh(meshName);
	m_material = renderContext->GetOrCreateMaterial(m_mesh->GetDefaultMaterialName());
	m_modelMatrix = Matrix4x4::IDENTITY;

}

void Model::SetModelMatrix( Matrix4x4 modelMatrix )
{
	m_modelMatrix = modelMatrix;
}
