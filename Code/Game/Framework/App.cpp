// Defines and Includes ---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <winsock2.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

// Commons ----------------------------------------------------------------------------------------
#include "Game/Framework/GameCommon.hpp"

// Engine Includes --------------------------------------------------------------------------------
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"


// Game Includes ----------------------------------------------------------------------------------
#include "Game/Framework/App.hpp"
#include "Game/Framework/Interface.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Input/GameInput.hpp"
#include "Game/Lobby/LobbyConsole.hpp"

// Third Party Includes ----------------------------------------------------------------------------
#include "ThirdParty/RakNet/RakNetInterface.hpp"

// Global Singletons ------------------------------------------------------------------------------
App* g_theApp = nullptr;

// Constructor ------------------------------------------------------------------------------------
App::App()
{
	g_theWindowContext->HideMouse();
}

// Deconstructor ----------------------------------------------------------------------------------
App::~App()
{

}

// Init -------------------------------------------------------------------------------------------
void App::Init()
{
	// Create Systems;
	g_theInputSystem			= new InputSystem();
	g_theGameInput				= new GameInput();
	g_theAudioSystem			= new AudioSystem();	
	g_theDevConsole				= new DevConsole();
	g_theRandomNumberGenerator	= new RandomNumberGenerator((unsigned int)time(0));
	g_theEventSystem			= new EventSystem();
	m_theGame					= new Game();
	g_Interface					= new Interface(m_theGame);

	g_theRakNetInterface		= new RakNetInterface();

	// Init Systems;
	g_theRenderer->Init();
	g_Interface->Init();
	m_theGame->Init();
}

// Startup ----------------------------------------------------------------------------------------
void App::Startup()
{	
	// Start Systems;
	g_theInputSystem->Startup();
	g_theGameInput->Startup();
	g_theRenderer->Startup();
	g_theAudioSystem->Startup();
	g_theDevConsole->Startup();
	g_theEventSystem->Startup();
	g_Interface->Startup();
	m_theGame->Startup();
	g_theRakNetInterface->Startup();

	m_devConsoleFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");

	m_intialStartupDone = true;
}

// -----------------------------------------------------------------------
void App::RunFrame()
{
	BeginFrame();
	Update();
	Render();
	EndFrame();
}

// -----------------------------------------------------------------------
void App::Shutdown()
{
	g_theEventSystem->Shutdown();
	g_theDevConsole->Shutdown();
	g_theAudioSystem->Shutdown();
	g_theRenderer->Shutdown();
	g_theGameInput->Shutdown();
	g_theInputSystem->Shutdown();

	DELETE_POINTER(m_theGame);
	DELETE_POINTER(g_Interface);
	DELETE_POINTER(g_theEventSystem);
	DELETE_POINTER(g_theDevConsole);
	DELETE_POINTER(g_theRandomNumberGenerator);
	DELETE_POINTER(g_theAudioSystem);
	DELETE_POINTER(g_theRenderer);
	DELETE_POINTER(g_theGameInput);
	DELETE_POINTER(g_theInputSystem);
	DELETE_POINTER(g_theRakNetInterface);
}

// -----------------------------------------------------------------------
void App::CalculateDeltaSeconds()
{
	m_timeNow = GetCurrentTimeSeconds();
	m_deltaSeconds = (float)(m_timeNow - m_timeLastFrameBegan);
	m_timeLastFrameBegan = m_timeNow;

	//m_deltaSeconds = Clamp(m_deltaSeconds, 0.0f, m_commandFrame.m_gameMaxFps);
}

// -----------------------------------------------------------------------
void App::BeginFrame()
{
	g_theWindowContext->BeginFrame();
	g_theAudioSystem->BeginFrame();
	g_theInputSystem->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theRenderer->BeginFrame();

	m_theGame->BeginFrame();
}

// -----------------------------------------------------------------------
void App::Update()
{
	CalculateDeltaSeconds();

	if (m_isFirstFrame)
	{
		return;
	}

	g_theRakNetInterface->ProcessIncomingPackets();

	if(g_theInputSystem->IsF1Down())
	{
		m_deltaSeconds *= 10.0f;
	}
	if(g_theInputSystem->IsF2Down())
	{
		m_deltaSeconds /= 10.0f;
	}
	
	m_theGame->Update(m_deltaSeconds);
}

// -----------------------------------------------------------------------
void App::Render()
{
	m_theGame->Render();

	if (m_isFirstFrame)
	{
		return;
	}
	
	g_theDevConsole->Render(m_devConsoleFont);
}

// -----------------------------------------------------------------------
void App::EndFrame()
{
	m_theGame->EndFrame();
	g_theGameInput->EndFrame();
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theDevConsole->EndFrame();

	m_isFirstFrame = false;
}

// -----------------------------------------------------------------------
// Character Keys;
// -----------------------------------------------------------------------
bool App::HandleChar(unsigned char asKey_)
{
	return g_theGameInput->HandleChar(asKey_);
}

// -----------------------------------------------------------------------
// Virtual Keys;
// -----------------------------------------------------------------------
bool App::HandleKeyPressed(unsigned char asKey_)
{
	return g_theGameInput->HandleKeyPressed(asKey_);
}

// -----------------------------------------------------------------------
bool App::HandleKeyReleased(unsigned char asKey_)
{
	return g_theGameInput->HandleKeyReleased(asKey_);
}

