#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/NamedStrings.hpp"
//#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

DebugRender* g_theDebugRenderer = nullptr;

static bool DebugEvent(EventArgs& args)
{
	// do stuff with the word that comes before and after "="
	// example: enable=true

	g_theDebugRenderer->m_renderDebugRenderer = args.GetValue("enable", true);

	if(args.GetValue("clear", false) == true)
	{
		for(auto& debugRenderObject: g_theDebugRenderer->m_debugRenderObjects)
		{
			delete debugRenderObject.gpuMesh;
			debugRenderObject.gpuMesh = nullptr;
		}

		g_theDebugRenderer->m_debugRenderObjects.clear();
		g_theDebugRenderer->m_debugRenderMessages.clear();
	}

	return true;
}

// -----------------------------------------------------------------------
DebugRender::DebugRender()
{

}

// -----------------------------------------------------------------------
DebugRender::~DebugRender()
{
	
}

// -----------------------------------------------------------------------
void DebugRender::Startup( RenderContext* theRenderer, float virtualScreenSpace )
{
	m_virtualScreenSpace = virtualScreenSpace;

	// Store the Render Context;
	m_theRenderer = theRenderer;

	// Shader;
	m_debugRendererShader = m_theRenderer->GetOrCreateShader( "Data/Shaders/SimpleDebugRendererShader.shader" );

	// Subscribe;
	g_theEventSystem->SubscriptionEventCallbackFunction( "debug", DebugEvent );
}

// -----------------------------------------------------------------------
void DebugRender::BeginFrame()
{

}

// -----------------------------------------------------------------------
void DebugRender::Update( float deltaSeconds )
{
	for( auto& debugRenderObject: m_debugRenderObjects )
	{
		debugRenderObject.timeAlive += deltaSeconds;

		float duration;
		if(debugRenderObject.duration == -1.0f)
		{
			return;
		}
		else
		{
			duration = debugRenderObject.duration == 0.0f ? 1.0f : debugRenderObject.duration;
		}

		float percent = debugRenderObject.timeAlive / duration;

		Rgba newColor = LerpRgba( debugRenderObject.startColor, debugRenderObject.endColor, percent );
		debugRenderObject.currentColor.r = newColor.r;
		debugRenderObject.currentColor.g = newColor.g;
		debugRenderObject.currentColor.b = newColor.b;
	}

	for( auto& debugRenderMessage: m_debugRenderMessages )
	{
		debugRenderMessage.timeAlive += deltaSeconds;

		float duration;
		if(debugRenderMessage.duration == -1.0f)
		{
			return;
		}
		else
		{
			duration = debugRenderMessage.duration == 0.0f ? 1.0f : debugRenderMessage.duration;
		}

		float percent = debugRenderMessage.timeAlive / duration;

		Rgba newColor = LerpRgba( debugRenderMessage.startColor, debugRenderMessage.endColor, percent );
		debugRenderMessage.currentColor.r = newColor.r;
		debugRenderMessage.currentColor.g = newColor.g;
		debugRenderMessage.currentColor.b = newColor.b;
	}
}

// -----------------------------------------------------------------------
void DebugRender::RenderToScreen( Camera* camera)
{
	if(!m_renderDebugRenderer)
	{
		return;
	}

	m_theRenderer->BeginCamera( camera );

	// Load Shader; Using the DevConsole shader until I break something;
	Shader* shader = m_theRenderer->GetOrCreateShader( "Data/Shaders/default_unlit_devconsole.shader" );
	m_theRenderer->BindShader( shader );

	DrawDebugRenderScreenPoint();
	DrawDebugRenderScreenQuad();
	DrawDebugRenderScreenLine();
	DrawDebugRenderScreenText();
	DrawDebugRenderMessages();

	m_theRenderer->EndCamera();
}

// -----------------------------------------------------------------------
void DebugRender::RenderToCamera()
{
	if(!m_renderDebugRenderer)
	{
		return;
	}
	
	// Not setting a Camera here because we will render to the active Camera;
	// Maybe in the future, pass the camera and then change it when needed;
	// Calling Begin and End Camera;

	// Drawing Depth --------------------------------------------------------
	m_debugRendererShader->SetDepth( COMPARE_LEQUAL, true );

	// Solid. 
	m_debugRendererShader->SetRasterFill(RASTER_FILL_SOLID);	
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderPoints(DEBUG_RENDER_USE_DEPTH);
	DrawDebugRenderQuads(DEBUG_RENDER_USE_DEPTH);
	DrawDebugRenderLines(DEBUG_RENDER_USE_DEPTH);
	DrawDebugRenderText(DEBUG_RENDER_USE_DEPTH);

	// Wire.
	m_debugRendererShader->SetRasterFill(RASTER_FILL_WIRE);
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderWireShapes(DEBUG_RENDER_USE_DEPTH);

	// Drawing Always -------------------------------------------------------
	m_debugRendererShader->SetDepth( COMPARE_ALWAYS, true );

	// Solid.
	m_debugRendererShader->SetRasterFill(RASTER_FILL_SOLID);	
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderPoints(DEBUG_RENDER_ALWAYS);
	DrawDebugRenderQuads(DEBUG_RENDER_ALWAYS);
	DrawDebugRenderLines(DEBUG_RENDER_ALWAYS);
	DrawDebugRenderText(DEBUG_RENDER_ALWAYS);

	// Wire.
	m_debugRendererShader->SetRasterFill(RASTER_FILL_WIRE);
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderWireShapes(DEBUG_RENDER_ALWAYS);

	// Drawing Xray ---------------------------------------------------------
	// Draw ALWAYS with an 50% alpha
	m_debugRendererShader->SetDepth( COMPARE_GREATER, false );
	m_debugRendererShader->SetBlendMode(BLEND_MODE_ALPHA);

	// Solid.
	m_debugRendererShader->SetRasterFill(RASTER_FILL_SOLID);
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderPoints(DEBUG_RENDER_XRAY);
	DrawDebugRenderQuads(DEBUG_RENDER_XRAY);
	DrawDebugRenderLines(DEBUG_RENDER_XRAY);
	DrawDebugRenderText(DEBUG_RENDER_XRAY);

	// Wire
	m_debugRendererShader->SetRasterFill(RASTER_FILL_WIRE);	
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderWireShapes(DEBUG_RENDER_XRAY);

	// Draw with DEPTH
	m_debugRendererShader->SetDepth( COMPARE_LEQUAL, true );
	m_debugRendererShader->SetBlendMode(BLEND_MODE_OPAQUE);

	// Solid.
	m_debugRendererShader->SetRasterFill(RASTER_FILL_SOLID);	
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderPoints(DEBUG_RENDER_XRAY);
	DrawDebugRenderQuads(DEBUG_RENDER_XRAY);
	DrawDebugRenderLines(DEBUG_RENDER_XRAY);
	DrawDebugRenderText(DEBUG_RENDER_XRAY);

	// Wire
	m_debugRendererShader->SetRasterFill(RASTER_FILL_WIRE);	
	m_theRenderer->BindShader(m_debugRendererShader);

	DrawDebugRenderWireShapes(DEBUG_RENDER_XRAY);

}

// -----------------------------------------------------------------------
void DebugRender::EndFrame()
{

}

// -----------------------------------------------------------------------
void DebugRender::Shutdown()
{
	// We need to clear the vector of DebugRenderObjects
	for( auto& renderObject: m_debugRenderObjects )
	{
		delete renderObject.gpuMesh;
		renderObject.gpuMesh = nullptr;
	}
}

// -----------------------------------------------------------------------
void DebugRender::Cleanup()
{
	for ( int renderObjectIndex = 0; renderObjectIndex < m_debugRenderObjects.size(); )
	{
		if(m_debugRenderObjects[renderObjectIndex].duration == -1.0f)
		{
			delete m_debugRenderObjects[renderObjectIndex].gpuMesh;
			m_debugRenderObjects[renderObjectIndex].gpuMesh = nullptr;
			m_debugRenderObjects.erase(m_debugRenderObjects.begin() + renderObjectIndex);
		}
		else if(m_debugRenderObjects[renderObjectIndex].timeAlive >= m_debugRenderObjects[renderObjectIndex].duration 
		&& m_debugRenderObjects[renderObjectIndex].allowedToDie)
		{
			delete m_debugRenderObjects[renderObjectIndex].gpuMesh;
			m_debugRenderObjects[renderObjectIndex].gpuMesh = nullptr;
			m_debugRenderObjects.erase(m_debugRenderObjects.begin() + renderObjectIndex);
		}
		else
		{
			renderObjectIndex++;
		}
	}

	for ( int renderObjectIndex = 0; renderObjectIndex < m_debugRenderMessages.size(); )
	{
		if(m_debugRenderMessages[renderObjectIndex].duration == -1.0f)
		{
			m_debugRenderMessages.erase(m_debugRenderMessages.begin() + renderObjectIndex);
		}
		else if(m_debugRenderMessages[renderObjectIndex].timeAlive >= m_debugRenderMessages[renderObjectIndex].duration 
			&& m_debugRenderMessages[renderObjectIndex].allowedToDie)
		{
			m_debugRenderMessages.erase(m_debugRenderMessages.begin() + renderObjectIndex);
		}
		else
		{
			renderObjectIndex++;
		}
	}
}

// -----------------------------------------------------------------------
void DebugRender::CreateDebugRenderPoint( float duration, DebugRenderMode debugRenderMode, Vec3 position, Rgba startColor, Rgba endColor, float size /*= DEFAULT_POINT_SIZE*/, DebugRenderShape debugRenderShape /*= DEBUG_RENDER_SHAPE_SPHERE*/ )
{
	// Create a GPU Mesh;
	GPUMesh* gpuMesh = new GPUMesh( m_theRenderer );
	CPUMesh mesh;
	mesh.SetLayout<Vertex_PCU>();

	switch(debugRenderShape)
	{
		case DEBUG_RENDER_SHAPE_SPHERE:
		{
			CPUMeshAddUVSphere( &mesh, Vec3(0.0f, 0.0f, 0.0f), size );
			break;
		}	

		case DEBUG_RENDER_SHAPE_CUBE:
		{
			AABB3 cube = AABB3( Vec3( 0.0f ), Vec3( size ) );
			CPUMeshAddCube( &mesh, cube );
			break;
		}

		default:
		{
			ERROR_AND_DIE("Asked to create a Debug Render Point shape that is not supported.");
			break;
		}
	}
	
	gpuMesh->CreateFromCPUMesh( &mesh );

	// Store the GPU Mesh;
	DebugRenderObject debugRenderObject;

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_POINT;
	debugRenderObject.debugRenderShape = debugRenderShape;
	debugRenderObject.gpuMesh = gpuMesh;
	debugRenderObject.modelMatrix.SetT(position);
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.debugRenderMode = debugRenderMode;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;

	m_debugRenderObjects.push_back( debugRenderObject );	
}

// -----------------------------------------------------------------------
void DebugRender::DrawDebugRenderPoints(DebugRenderMode debugRenderDrawMode)
{
	for( auto &renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_POINT 
		&& renderObject.debugRenderMode == debugRenderDrawMode)
		{
			Rgba currentColor = renderObject.currentColor;

			if(renderObject.debugRenderMode == DEBUG_RENDER_XRAY
				&& m_debugRendererShader->GetDepth() == COMPARE_GREATER)
			{
				currentColor.a *= 0.5f;
			}

			// Draw the GPU Mesh;
			Image* colorImage = new Image(IntVec2(1, 1), currentColor);
			TextureView* tv = m_theRenderer->CreateTextureViewFromImage(*colorImage);

			m_theRenderer->BindTextureViewWithSampler(0, tv);
			m_theRenderer->BindModelMatrix(renderObject.modelMatrix);
			m_theRenderer->DrawMesh(renderObject.gpuMesh);

			delete colorImage;
			colorImage = nullptr;

			delete tv;
			tv = nullptr;
		}		
	}
}

// -----------------------------------------------------------------------
void DebugRender::CreateDebugRenderQuad( float duration, DebugRenderMode debugRenderMode, Vec3 position, const AABB2 quad, Rgba startColor, Rgba endColor, TextureView* textureView /*= nullptr */ )
{
	DebugRenderObject debugRenderObject;

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_QUAD;
	debugRenderObject.modelMatrix.SetT(position);
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.debugRenderMode = debugRenderMode;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;
	debugRenderObject.quad = quad;
	debugRenderObject.textureView = textureView;

	m_debugRenderObjects.push_back( debugRenderObject );
}

// -----------------------------------------------------------------------
void DebugRender::DrawDebugRenderQuads(DebugRenderMode debugRenderDrawMode)
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_QUAD
		&& renderObject.debugRenderMode == debugRenderDrawMode)
		{
			m_theRenderer->BindModelMatrix(renderObject.modelMatrix);

			Rgba renderColor;
			renderColor.r = renderObject.currentColor.r;
			renderColor.g = renderObject.currentColor.g;
			renderColor.b = renderObject.currentColor.b;
			renderColor.a = renderObject.currentColor.a;

			m_theRenderer->BindTextureViewWithSampler(0, renderObject.textureView);				

			if(renderObject.debugRenderMode == DEBUG_RENDER_XRAY
				&& m_debugRendererShader->GetDepth() == COMPARE_GREATER)
			{
				renderColor.a *= 0.5f;
			}
			
			std::vector<Vertex_PCU> quadVerts;			
			AddVertsForAABB2D(quadVerts, renderObject.quad, renderColor);
			m_theRenderer->DrawVertexArray((int)quadVerts.size(), &quadVerts[0]);
		}		
	}
}

// -----------------------------------------------------------------------
void DebugRender::CreateDebugRenderLine( float duration, DebugRenderMode debugRenderMode, Vec3 startPosition, Vec3 endPosition, Rgba startColor, Rgba endColor, float width /*= DEFAULT_LINE_WIDTH */ )
{
	DebugRenderObject debugRenderObject; 

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_LINE;
	debugRenderObject.startPosition3D = startPosition;
	debugRenderObject.endPosition3D = endPosition;
	debugRenderObject.thickness = width;
	debugRenderObject.modelMatrix.SetT(startPosition);
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.debugRenderMode = debugRenderMode;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;

	m_debugRenderObjects.push_back( debugRenderObject );
}

// -----------------------------------------------------------------------
void DebugRender::DrawDebugRenderLines(DebugRenderMode debugRenderDrawMode)
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_LINE
		&& renderObject.debugRenderMode == debugRenderDrawMode)
		{
			Rgba currentColor = renderObject.currentColor;

			if(renderObject.debugRenderMode == DEBUG_RENDER_XRAY
				&& m_debugRendererShader->GetDepth() == COMPARE_GREATER)
			{
				currentColor.a *= 0.5f;
			}

			m_theRenderer->BindModelMatrix(renderObject.modelMatrix);
			m_theRenderer->BindTextureViewWithSampler(0, nullptr);

			std::vector<Vertex_PCU> lineVerts;
			AddVertsForLine3D( lineVerts, renderObject.startPosition3D, renderObject.endPosition3D, renderObject.thickness, currentColor );
			m_theRenderer->DrawVertexArray((int)lineVerts.size(), &lineVerts[0]);
		}		
	}
}

// -----------------------------------------------------------------------
void DebugRender::CreateDebugRenderWireShape( float duration, DebugRenderMode debugRenderMode, Vec3 position, float radius, Rgba startColor, Rgba endColor, DebugRenderShape debugRenderShape /*= DEBUG_RENDER_SHAPE_SPHERE*/, uint wedges /*= 32*/, uint slices /*= 16 */ )
{
	// Create a GPU Mesh;
	GPUMesh* gpuMesh = new GPUMesh( m_theRenderer );
	CPUMesh mesh;
	mesh.SetLayout<Vertex_PCU>();

	switch(debugRenderShape)
	{
		case DEBUG_RENDER_SHAPE_SPHERE:
		{
			CPUMeshAddUVSphere( &mesh, Vec3(0.0f, 0.0f, 0.0f), radius, wedges, slices );
			break;
		}	

		case DEBUG_RENDER_SHAPE_CUBE:
		{
			AABB3 cube = AABB3( Vec3( 0.0f ), Vec3( radius ) );
			CPUMeshAddCube( &mesh, cube );
			break;
		}

		default:
		{
			ERROR_AND_DIE("Asked to create a Debug Render Shape that is not supported.");
			break;
		}
	}

	gpuMesh->CreateFromCPUMesh( &mesh );

	// Store the GPU Mesh;
	DebugRenderObject debugRenderObject;

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_WIRESHAPE;
	debugRenderObject.gpuMesh = gpuMesh;
	debugRenderObject.modelMatrix.SetT(position);
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.debugRenderMode = debugRenderMode;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;

	m_debugRenderObjects.push_back( debugRenderObject );
}

void DebugRender::DrawDebugRenderWireShapes(DebugRenderMode debugRenderDrawMode)
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_WIRESHAPE
		&& renderObject.debugRenderMode == debugRenderDrawMode)
		{
			Rgba currentColor = renderObject.currentColor;

			if(renderObject.debugRenderMode == DEBUG_RENDER_XRAY
				&& m_debugRendererShader->GetDepth() == COMPARE_GREATER)
			{
				currentColor.a *= 0.5f;
			}

			// Draw the GPU Mesh;
			Image* colorImage = new Image(IntVec2(1, 1), currentColor);
			TextureView* colorTexture = m_theRenderer->CreateTextureViewFromImage(*colorImage);

			m_theRenderer->BindTextureViewWithSampler(0, colorTexture);
			m_theRenderer->BindModelMatrix(renderObject.modelMatrix);
			m_theRenderer->DrawMesh(renderObject.gpuMesh);

			delete colorImage;
			colorImage = nullptr;

			delete colorTexture;
			colorTexture = nullptr;
		}		
	}
}

void DebugRender::DrawDebugRenderText(DebugRenderMode debugRenderDrawMode)
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_TEXT
		&& renderObject.debugRenderMode == debugRenderDrawMode)
		{
			Rgba currentColor = renderObject.currentColor;

			if(renderObject.debugRenderMode == DEBUG_RENDER_XRAY
				&& m_debugRendererShader->GetDepth() == COMPARE_GREATER)
			{
				currentColor.a *= 0.5f;
			}

			BitMapFont* bitmapFont = m_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
			TextureView* textureViewFont = bitmapFont->GetTextureView();

			std::vector<Vertex_PCU> textVerts;

			for(int textIndex = 0; textIndex < renderObject.textToDisplay.size(); textIndex++)
			{
				Vec3 printPosition((float)(renderObject.startPosition3D.x + textIndex), renderObject.startPosition3D.y, renderObject.startPosition3D.z);

				std::string text = "";
				text = renderObject.textToDisplay[textIndex];

				bitmapFont->AddVertsForText3D(textVerts, (renderObject.startPosition3D + printPosition) * renderObject.textHeight, renderObject.textHeight, text, currentColor);
			}

			if((int)textVerts.size() > 0)
			{
				g_theRenderer->BindModelMatrix(Matrix4x4::IDENTITY);
				g_theRenderer->BindTextureView( 0u, textureViewFont );
				g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
			}
		}
	}	
}

void DebugRender::CreateDebugRenderScreenPoint( float duration, Vec2 position, Rgba startColor, Rgba endColor, float size /*= DEFAULT_SCREEN_POINT_SIZE*/, DebugRenderShape debugRenderShape /*= DEBUG_RENDER_SHAPE_SPHERE */ )
{
	DebugRenderObject debugRenderObject;

	switch(debugRenderShape)
	{
		case DEBUG_RENDER_SHAPE_SPHERE:
		{
			debugRenderObject.discRadius = size;
			break;
		}	

		case DEBUG_RENDER_SHAPE_CUBE:
		{
			debugRenderObject.quad = AABB2(Vec2( position.x, position.y ), Vec2( size ));
			break;
		}

		default:
		{
			ERROR_AND_DIE("Asked to create a Debug Render Screen Point shape that is not supported.");
			break;
		}
	}

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_SCREEN_POINT;
	debugRenderObject.debugRenderShape = debugRenderShape;
	debugRenderObject.startPosition2D = position;
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;

	m_debugRenderObjects.push_back( debugRenderObject );
}

void DebugRender::DrawDebugRenderScreenPoint()
{
	for( auto const& debugRenderObject: m_debugRenderObjects )
	{
		if(debugRenderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_SCREEN_POINT)
		{
			m_theRenderer->BindTextureViewWithSampler(0, nullptr);
			std::vector<Vertex_PCU> verts;

			switch(debugRenderObject.debugRenderShape)
			{
				case DEBUG_RENDER_SHAPE_SPHERE:
				{
					AddVertsForDisc2D( verts, debugRenderObject.startPosition2D, debugRenderObject.discRadius, debugRenderObject.currentColor );
					break;
				}
				case DEBUG_RENDER_SHAPE_CUBE:
				{					
					AddVertsForAABB2D( verts, debugRenderObject.quad, debugRenderObject.currentColor );
					break;
				}
				default:
				{
					ERROR_AND_DIE("Asked to draw a Debug Render Screen Point shape that is not supported. Honestly, no idea how this could ever happen.");
					break;
				}
			}

			m_theRenderer->DrawVertexArray((int)verts.size(), &verts[0]);
		}		
	}
}

void DebugRender::CreateDebugRenderScreenQuad( float duration, const AABB2 quad, Rgba startColor, Rgba endColor, TextureView* textureView /*= nullptr */ )
{
	DebugRenderObject debugRenderObject;

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_SCREEN_QUAD;
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;
	debugRenderObject.quad = quad;
	debugRenderObject.textureView = textureView;

	m_debugRenderObjects.push_back( debugRenderObject );
}

void DebugRender::DrawDebugRenderScreenQuad()
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_SCREEN_QUAD)
		{
			Rgba renderColor;			

			if(renderObject.textureView)
			{
				m_theRenderer->BindTextureViewWithSampler(0, renderObject.textureView);
				renderColor.r = 1.0f;
				renderColor.g = 1.0f;
				renderColor.b = 1.0f;
				renderColor.a = 1.0f;
			}
			else
			{
				m_theRenderer->BindTextureViewWithSampler(0, nullptr);
				renderColor.r = renderObject.currentColor.r;
				renderColor.g = renderObject.currentColor.g;
				renderColor.b = renderObject.currentColor.b;
				renderColor.a = renderObject.currentColor.a;
			}

			std::vector<Vertex_PCU> quadVerts;			
			AddVertsForAABB2D(quadVerts, renderObject.quad, renderColor);
			m_theRenderer->DrawVertexArray((int)quadVerts.size(), &quadVerts[0]);
		}		
	}
}

void DebugRender::CreateDebugRenderScreenLine( float duration, Vec2 startPosition, Vec2 endPosition, Rgba startColor, Rgba endColor, float width /*= DEFAULT_LINE_WIDTH */ )
{
	DebugRenderObject debugRenderObject;

	debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_SCREEN_LINE;
	debugRenderObject.startPosition2D = startPosition;
	debugRenderObject.endPosition2D = endPosition;
	debugRenderObject.thickness = width;
	debugRenderObject.startColor = startColor;
	debugRenderObject.endColor = endColor;
	debugRenderObject.currentColor = startColor;
	debugRenderObject.duration = duration;
	debugRenderObject.allowedToDie = duration != 0.0f ? true : false;

	m_debugRenderObjects.push_back( debugRenderObject );
}

void DebugRender::DrawDebugRenderScreenLine()
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_SCREEN_LINE)
		{
			m_theRenderer->BindTextureViewWithSampler(0, nullptr);

			std::vector<Vertex_PCU> lineVerts;
			AddVertsForLine2D( lineVerts, renderObject.startPosition2D, renderObject.endPosition2D, renderObject.thickness, renderObject.currentColor );
			m_theRenderer->DrawVertexArray((int)lineVerts.size(), &lineVerts[0]);
		}		
	}
}

void DebugRender::DrawDebugRenderScreenText()
{
	for( auto const& renderObject: m_debugRenderObjects )
	{
		if(renderObject.debugRenderObjectType == DEBUG_RENDER_OBJECT_SCREEN_TEXT)
		{
			BitMapFont* bitmapFont = m_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
			TextureView* textureViewFont = bitmapFont->GetTextureView();

			std::vector<Vertex_PCU> textVerts;

			for(int textIndex = 0; textIndex < renderObject.textToDisplay.size(); textIndex++)
			{
				Vec2 printPosition((float)((textIndex * renderObject.textHeight)), 0.0f );

				std::string text = "";
				text = renderObject.textToDisplay[textIndex];

				bitmapFont->AddVertsForText2D(textVerts, (renderObject.startPosition2D + printPosition), renderObject.textHeight, text, renderObject.currentColor);
			}

			if((int)textVerts.size() > 0)
			{
				g_theRenderer->BindTextureView( 0u, textureViewFont );
				g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
			}
		}
	}
}

void DebugRender::DrawDebugRenderMessages()
{
	int y = 0;
	Vec2 startPosition = Vec2(0.0f, 950.0f);

	for( auto const& renderMessage: m_debugRenderMessages)
	{
		BitMapFont* bitmapFont = m_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
		TextureView* textureViewFont = bitmapFont->GetTextureView();

		std::vector<Vertex_PCU> textVerts;		

		for(int textIndex = 0; textIndex < renderMessage.textToDisplay.size(); textIndex++)
		{
			Vec2 printPosition((float)((textIndex * 15.0f)), -15.0f * y );

			std::string text = "";
			text = renderMessage.textToDisplay[textIndex];

			bitmapFont->AddVertsForText2D(textVerts, (startPosition + printPosition), 15.0f, text, renderMessage.currentColor);
		}

		if((int)textVerts.size() > 0)
		{
			g_theRenderer->BindTextureView( 0u, textureViewFont );
			g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
		}
		y++;
	}
}

// -----------------------------------------------------------------------
int DebugRender::GetCountOfCreatedDebugRenderPoints()
{
	return (int)m_debugRenderObjects.size();
}

// -----------------------------------------------------------------------
Vec3 DebugRender::GetPositionOfLastCreatedDebugRenderPoint()
{
	DebugRenderObject* lastDebugRenderObject = &m_debugRenderObjects.back();

	return lastDebugRenderObject->modelMatrix.GetT();
}
