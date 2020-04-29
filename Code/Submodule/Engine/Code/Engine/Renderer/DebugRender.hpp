#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/Shader.hpp"


#define DEFAULT_POINT_SIZE (0.05f)
#define DEFAULT_LINE_WIDTH (0.05f)

#define DEFAULT_SCREEN_POINT_SIZE (15.0f)
#define DEFAULT_SCREEN_LINE_SIZE (15.0f)

struct Camera;

enum DebugRenderObjectType
{
	DEBUG_RENDER_OBJECT_POINT,
	DEBUG_RENDER_OBJECT_QUAD,
	DEBUG_RENDER_OBJECT_LINE,
	DEBUG_RENDER_OBJECT_WIRESHAPE,
	DEBUG_RENDER_OBJECT_TEXT,
	DEBUG_RENDER_OBJECT_SCREEN_POINT,
	DEBUG_RENDER_OBJECT_SCREEN_QUAD,
	DEBUG_RENDER_OBJECT_SCREEN_LINE,
	DEBUG_RENDER_OBJECT_SCREEN_TEXT
};

enum DebugRenderShape
{
	DEBUG_RENDER_SHAPE_CUBE,
	DEBUG_RENDER_SHAPE_SPHERE
};

enum DebugRenderMode
{
	DEBUG_RENDER_USE_DEPTH,
	DEBUG_RENDER_ALWAYS,
	DEBUG_RENDER_XRAY
};

struct DebugRenderObject
{
	DebugRenderObjectType debugRenderObjectType;
	DebugRenderMode debugRenderMode;
	DebugRenderShape debugRenderShape;

	// Common
	Matrix4x4 modelMatrix		= Matrix4x4::IDENTITY;
	Rgba startColor				= Rgba::WHITE;
	Rgba endColor				= Rgba::WHITE;
	Rgba currentColor			= Rgba::WHITE;
	float duration				= 0.0f;
	float timeAlive				= 0.0f;
	bool allowedToDie			= false;

	// Debug Render Object Point
	// Debug Render Object Wireframe
	GPUMesh* gpuMesh			= nullptr;
	
	// Debug Render Object Quad
	// Debug Render Object Screen Quad
	AABB2 quad					= AABB2(0.0f, 0.0f);
	TextureView* textureView	= nullptr;

	// Debug Render Object Line
	Vec3 startPosition3D		= Vec3( 0.0f );
	Vec3 endPosition3D			= Vec3( 0.0f );
	float thickness				= 0.0f;

	// Debug Render Object Text
	std::string textToDisplay	= "";
	float textHeight			= 0.0f;
	// Vec3 startPosition3D		= Vec3(0.0f, 0.0f, 0.0f);

	// Debug Render Object Screen Point
	Vec2 startPosition2D		= Vec2( 0.0f );
	float discRadius			= 0.0f;
	// AABB2 quad				= AABB2(0.0f, 0.0f);

	// Debug Render Object Screen Line
	// Vec2 startPosition2D		= Vec2( 0.0f );
	Vec2 endPosition2D			= Vec2( 0.0f );
	// float thickness			= 0.0f;

	// Debug Render Object Screen Text
	// Vec2 startPosition2D		= Vec2( 0.0f );
	// float textHeight			= 0.0f;
	// std::string textToDisplay	= "";

};

struct DebugRenderMessage
{
	float duration				= 0.0f;
	float timeAlive				= 0.0f;
	bool allowedToDie			= false;
	Rgba startColor				= Rgba::WHITE;
	Rgba currentColor			= Rgba::WHITE;
	Rgba endColor				= Rgba::WHITE;
	std::string textToDisplay	= "";
};

class DebugRender
{

public:

	// Constructor/Deconstructor
	DebugRender();
	~DebugRender();

	// Game Flow
	void Startup( RenderContext* theRenderer, float virtualScreenSpace );
	void BeginFrame();
	void Update( float deltaSeconds );
	void RenderToScreen(Camera* camera);
	void RenderToCamera();
	void EndFrame();
	void Shutdown();
	void Cleanup();

	// 3D - Camera Space;
	void CreateDebugRenderPoint( float duration, DebugRenderMode debugRenderMode, Vec3 position, Rgba startColor, Rgba endColor, float size = DEFAULT_POINT_SIZE, DebugRenderShape debugRenderShape = DEBUG_RENDER_SHAPE_SPHERE );
	void DrawDebugRenderPoints(DebugRenderMode debugRenderDrawMode);

	void CreateDebugRenderQuad( float duration, DebugRenderMode debugRenderMode, Vec3 position, const AABB2 quad, Rgba startColor, Rgba endColor, TextureView* textureView = nullptr );
	void DrawDebugRenderQuads(DebugRenderMode debugRenderDrawMode);

	void CreateDebugRenderLine( float duration, DebugRenderMode debugRenderMode, Vec3 startPosition, Vec3 endPosition, Rgba startColor, Rgba endColor, float width = DEFAULT_LINE_WIDTH );
	void DrawDebugRenderLines(DebugRenderMode debugRenderDrawMode);

	void CreateDebugRenderWireShape( float duration, DebugRenderMode debugRenderMode, Vec3 position, float radius, Rgba startColor, Rgba endColor, DebugRenderShape debugRenderShape = DEBUG_RENDER_SHAPE_SPHERE, uint wedges = 32, uint slices = 16 ); 
	void DrawDebugRenderWireShapes(DebugRenderMode debugRenderDrawMode);

	template<typename ...Types>
	void CreateDebugRenderText(float duration, DebugRenderMode debugRenderMode, Vec3 position, float height, Rgba startColor, Rgba endColor, std::string textToDisplay, Types... args)
	{
		DebugRenderObject debugRenderObject;

		debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_TEXT;
		debugRenderObject.modelMatrix.SetT(position);
		debugRenderObject.startPosition3D = position;
		debugRenderObject.startColor = startColor;
		debugRenderObject.endColor = endColor;
		debugRenderObject.currentColor = startColor;
		debugRenderObject.duration = duration;
		debugRenderObject.debugRenderMode = debugRenderMode;
		debugRenderObject.allowedToDie = duration != 0.0f ? true : false;
		debugRenderObject.textHeight = height;

		debugRenderObject.textToDisplay = Stringf(textToDisplay.c_str(), args...);

		m_debugRenderObjects.push_back( debugRenderObject );
	}
	void DrawDebugRenderText(DebugRenderMode debugRenderDrawMode);

	// 2D - Screen Space;
	void CreateDebugRenderScreenPoint( float duration, Vec2 position, Rgba startColor, Rgba endColor, float size = DEFAULT_SCREEN_POINT_SIZE, DebugRenderShape debugRenderShape = DEBUG_RENDER_SHAPE_SPHERE ); 
	void DrawDebugRenderScreenPoint();

	void CreateDebugRenderScreenQuad( float duration, const AABB2 quad, Rgba startColor, Rgba endColor, TextureView* textureView = nullptr );
	void DrawDebugRenderScreenQuad();

	void CreateDebugRenderScreenLine( float duration, Vec2 startPosition, Vec2 endPosition, Rgba startColor, Rgba endColor, float width = DEFAULT_SCREEN_LINE_SIZE );
	void DrawDebugRenderScreenLine();

	template<typename ...Types>
	void CreateDebugRenderScreenText( float duration, Vec2 position, float height, Rgba startColor, Rgba endColor, std::string textToDisplay, Types... args )
	{
		DebugRenderObject debugRenderObject;

		debugRenderObject.debugRenderObjectType = DEBUG_RENDER_OBJECT_SCREEN_TEXT;
		debugRenderObject.startPosition2D = position;
		debugRenderObject.startColor = startColor;
		debugRenderObject.endColor = endColor;
		debugRenderObject.currentColor = startColor;
		debugRenderObject.duration = duration;
		debugRenderObject.allowedToDie = duration != 0.0f ? true : false;
		debugRenderObject.textHeight = height;

		debugRenderObject.textToDisplay = Stringf(textToDisplay.c_str(), args...);

		m_debugRenderObjects.push_back( debugRenderObject );
	}
	void DrawDebugRenderScreenText();

	template<typename ...Types>
	void CreateDebugRenderMessage( float duration, Rgba startColor, Rgba endColor, std::string textToDisplay, Types... args )
	{
		DebugRenderMessage debugRenderMessage;

		debugRenderMessage.duration = duration;
		debugRenderMessage.startColor = startColor;
		debugRenderMessage.currentColor = startColor;
		debugRenderMessage.endColor = endColor;
		debugRenderMessage.allowedToDie = duration != 0.0f ? true : false;

		debugRenderMessage.textToDisplay = Stringf(textToDisplay.c_str(), args...);

		m_debugRenderMessages.push_back( debugRenderMessage );
	}
	void DrawDebugRenderMessages();

	// Get
	int GetCountOfCreatedDebugRenderPoints();
	Vec3 GetPositionOfLastCreatedDebugRenderPoint();

	//
	bool m_renderDebugRenderer		= true;

	float m_virtualScreenSpace		= 0.0f;

	RenderContext* m_theRenderer	= nullptr;
	Shader* m_debugRendererShader	= nullptr;


	std::vector<DebugRenderObject> m_debugRenderObjects;
	std::vector<DebugRenderMessage> m_debugRenderMessages;



};

extern DebugRender* g_theDebugRenderer;

