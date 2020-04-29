#include "Engine/Renderer/Prop.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/RenderContext.hpp"



Prop::Prop( RenderContext* renderContext )
{
	m_theRenderContext = renderContext;
}

Prop::Prop( const Prop& copyProp )
{
	m_theRenderContext = copyProp.m_theRenderContext;
	m_model = new Model(*copyProp.m_model);
}

Prop::~Prop()
{
	delete m_model;
	m_model = nullptr;
}

void Prop::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);



}

void Prop::Load( const char* filename )
{
	m_model = new Model(m_theRenderContext, filename);
}

void Prop::Render()
{
	g_theRenderer->BindModelMatrix( m_model->m_modelMatrix );
	g_theRenderer->BindMaterial( m_model->m_material );

	g_theRenderer->DrawMesh(m_model->m_mesh);
	//m_theRenderContext->DrawModel(m_model);
}
