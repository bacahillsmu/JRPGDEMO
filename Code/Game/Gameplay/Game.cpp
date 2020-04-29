// Defines and Includes ---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN	
#include <winsock2.h>

// Commons ----------------------------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Framework/GameCommon.hpp"

// Engine Includes --------------------------------------------------------------------------------
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexLit.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Renderer/Prop.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/UI/UIWidget.hpp"
#include "Engine/Input/InputSystem.hpp"

// Game Includes ----------------------------------------------------------------------------------
#include "Game/Framework/App.hpp"
#include "Game/Framework/Interface.hpp"
#include "Game/Input/GameInput.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Ability/AbilityDefinition.hpp"
#include "Game/Units/UnitDefinition.hpp"
#include "Game/Cards/CardDefinition.hpp"
#include "Game/Lobby/LobbyConsole.hpp"
#include "Game/Cards/CardFilters.hpp"
#include "Game/Gameplay/PlayerFilters.hpp"
#include "Game/Gameplay/Player.hpp"

// Third Party Includes ----------------------------------------------------------------------------
#include "ThirdParty/RakNet/RakNetInterface.hpp"

// Callbacks --------------------------------------------------------------------------------------
static bool QuitGame(EventArgs& args)
{
	UNUSED(args);

	g_theApp->HandleCloseApplication();

	return true;
}

// -----------------------------------------------------------------------
static bool CreateServer(EventArgs& args)
{
	UNUSED(args);
	g_theApp->m_theGame->m_setupConnectionInfoComplete = g_theRakNetInterface->CreateServer();
	return g_theApp->m_theGame->m_setupConnectionInfoComplete;
}

// -----------------------------------------------------------------------
static bool TestBinaryFileLoad(EventArgs& args)
{
	args;

	const char* testFilePath = "Data/Test/Test.binary";
	g_theDevConsole->Print(Stringf("Loading test binary file '%s'...\n", testFilePath));

	// Load from disk
	Buffer buff;
	bool success = BufferWriter::LoadBinaryFileToExistingBuffer(testFilePath, &buff);
	if (!success)
	{
		g_theDevConsole->Print(Stringf("FAILED to load file %s\n", testFilePath));
		return false;
	}

	// Parse and verify - note that the test data is in the file TWICE; first as little-endian, then again as big
	BufferParser bufParse(buff);
	g_theApp->m_theGame->ParseTestFileBufferData(bufParse, BufferEndian::LITTLE);
	g_theApp->m_theGame->ParseTestFileBufferData(bufParse, BufferEndian::BIG);

	g_theDevConsole->Print(Stringf("...successfully read file %s\n", testFilePath));
	
	return false;
}

// -----------------------------------------------------------------------
static bool TestBinaryFileSave(EventArgs& args)
{
	args;
	

	return true;
}


// -----------------------------------------------------------------------
static bool SetDevConsoleFontToFixedWidth16x16(EventArgs& args)
{
	args;

	g_theApp->m_devConsoleFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");

	return true;
}

static bool SetDevConsoleFontToProportionalFont(EventArgs& args)
{
	args;

	g_theApp->m_devConsoleFont = g_theRenderer->CreateOrGetBitmapFontProportionalWidth("SquirrelProportionalFont");

	return true;
}

static bool SetDevConsoleFontToFontUsingFNTFile(EventArgs& args)
{
	args;

	g_theApp->m_devConsoleFont = g_theRenderer->CreateOrGetBitmapFontLoadFromFNTFile("EccentricStd");

	return true;
}

// Constructor ------------------------------------------------------------------------------------
Game::Game() 
{
}

// Deconstructor ----------------------------------------------------------------------------------
Game::~Game()
{
	// Delete Pointers (template);
	DELETE_POINTER(m_gameMainCamera);
	DELETE_POINTER(m_uiCamera);
	DELETE_POINTER(m_lobbyConsole);
}

// -----------------------------------------------------------------------
// Flow;
// -----------------------------------------------------------------------
void Game::Init()
{
	m_gameState = GAMESTATE_STARTUP;

	// Ask the WindowContext for the Client's dimensions;
	IntVec2 clientMins = g_theWindowContext->GetClientMins();
	IntVec2 clientMaxs = g_theWindowContext->GetClientMaxs();

	// Save off the Client's dimensions, keeping in mind that Windows has 0,0 is top left, we want it bottom left;
	m_clientMins = Vec2((float)clientMins.x, (float)clientMaxs.y);
	m_clientMaxs = Vec2((float)clientMaxs.x, (float)clientMins.y);

	// Game Subscription Callbacks;
	g_theEventSystem->SubscriptionEventCallbackFunction("quit", QuitGame);
	g_theEventSystem->SubscriptionEventCallbackFunction("create_server", CreateServer);
	g_theEventSystem->SubscriptionEventCallbackFunction("test_bfl", TestBinaryFileLoad);
	g_theEventSystem->SubscriptionEventCallbackFunction("test_bfs", TestBinaryFileSave);
	g_theEventSystem->SubscriptionEventCallbackFunction("test_fixedwidth", SetDevConsoleFontToFixedWidth16x16);
	g_theEventSystem->SubscriptionEventCallbackFunction("test_proportional", SetDevConsoleFontToProportionalFont);
	g_theEventSystem->SubscriptionEventCallbackFunction("test_fntfile", SetDevConsoleFontToFontUsingFNTFile);

	m_gameMainCamera	= new Camera();
	m_uiCamera			= new Camera();
	m_lobbyConsole		= new LobbyConsole(this);

	m_gameMainCamera->SetOrthographicProjection(Vec2::ZERO, Vec2(Map::WIDTH, Map::HEIGHT));
	m_uiCamera->SetOrthographicProjection(Vec2::ZERO, Vec2(Map::WIDTH, Map::HEIGHT));

	CreateLoadingScreen();
}

// -----------------------------------------------------------------------
void Game::Startup()
{
	// Mouse Settings;
	g_theWindowContext->ShowMouse();							// During WindowContext's Init, we hid the mouse;
	g_theWindowContext->SetMouseMode(MOUSE_MODE_ABSOLUTE);		// Mouse position is where the mouse is;
	// g_theWindowContext->SetMouseMode(MOUSE_MODE_RELATIVE);	// Mouse position is locked to center;

	StartLoadingAssets();

	g_theRakNetInterface->m_gamePacketCallback = [=](RakNet::Packet* packet)
	{
		this->OnIncomingPacket(packet);
	};

	m_singlePlayerMenuButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.40f, Map::HEIGHT * 0.25), Vec2(Map::WIDTH * 0.6f, Map::HEIGHT * 0.3f));
	m_multiPlayerMenuButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.40f, Map::HEIGHT * 0.20), Vec2(Map::WIDTH * 0.6f, Map::HEIGHT * 0.25f));
	m_settingsMenuButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.40f, Map::HEIGHT * 0.15), Vec2(Map::WIDTH * 0.6f, Map::HEIGHT * 0.20f));
	m_quitMenuButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.40f, Map::HEIGHT * 0.10), Vec2(Map::WIDTH * 0.6f, Map::HEIGHT * 0.15f));
}

// -----------------------------------------------------------------------
void Game::BeginFrame()
{
	m_lobbyConsole->BeginFrame();
}

// -----------------------------------------------------------------------
void Game::AsyncLoading()
{
	if (m_gameStateLoading == GAMESTATELOADING_LOADING)
	{
		if (!DoneLoading())
		{
			ContinueLoading();
		}
		else
		{
			EndLoadingThreads();
		}
	}
}

// -----------------------------------------------------------------------
void Game::Update(float deltaSeconds_)
{
	bool loading = CheckLoading(deltaSeconds_);
	if (!loading)
	{
		if(g_theRakNetInterface->m_connection == ConnectionType::SERVER)
		{
			g_Interface->server().Update(deltaSeconds_);
		}
		else
		{
			switch (m_gameState)
			{
				// m_gameState is set to MAINMENU when pressing the space bar during loading;
				case GAMESTATE_MAINMENU:
				{
					UpdateMainMenuScreen(deltaSeconds_);
					break;
				}

				// m_gameState is set to LOBBY when client clicks Lobby button in main menu;
				case GAMESTATE_LOBBY:
				{
					m_lobbyConsole->Update(deltaSeconds_);
					CheckConnectionStatus();
					break;
				}

				// m_gameState is set to PLAY in end frame if server says start;
				case GAMESTATE_PLAY:
				{
					g_Interface->client().Update(deltaSeconds_);
					break;
				}

				default:
				{
					ERROR_AND_DIE("Game has gotten into an incorrect game state!");
					break;
				}
			}
		}
	}
}

// -----------------------------------------------------------------------
void Game::Render()
{
	BeginCamera(m_gameMainCamera);
	g_theRenderer->ClearColorTargets(m_clearColor);
	
	if (m_gameStateLoading == GAMESTATELOADING_FINISHED && m_gameState == GAMESTATE_PLAY)
	{
		g_Interface->Render();
	}
	else if (m_gameStateLoading == GAMESTATELOADING_FINISHED && m_gameState == GAMESTATE_LOBBY)
	{
		m_lobbyConsole->Render();
	}
	else if(m_gameStateLoading == GAMESTATELOADING_FINISHED && m_gameState == GAMESTATE_MAINMENU)
	{
		RenderMainMenuScreen();
	}
	else if(m_gameStateLoading == GAMESTATELOADING_LOADING && m_gameState == GAMESTATE_STARTUP)
	{
		RenderLoadingScreen();
	}

	EndCamera();

	BeginCamera(m_uiCamera);
	EndCamera();
}

// -----------------------------------------------------------------------
void Game::EndFrame()
{
	if (m_serverSaysStartMultiplayerGame)
	{
		g_theAudioSystem->StopSound(m_mainMenuMusicPlaybackID);
		m_mainMenuMusicPlaying = false;

		m_gameState = GAMESTATE_PLAY;
	}
}

// -----------------------------------------------------------------------
// Loading;
// -----------------------------------------------------------------------
bool Game::CheckLoading(float deltaSeconds_)
{
	if (m_gameStateLoading == GAMESTATELOADING_LOADING)
	{
		if (m_gameState == GAMESTATE_STARTUP)
		{
			AsyncLoading();
			UpdateLoadingScreen(deltaSeconds_);
		}

		return true;
	}

	return false;
}

// -----------------------------------------------------------------------
void Game::CreateLoadingScreen()
{
	m_screenBackground = AABB2::MakeFromMinsMaxs(Vec2((float)m_clientMins.x, (float)m_clientMins.y), Vec2((float)m_clientMaxs.x, (float)m_clientMaxs.y));
	m_screenBackgroundTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Images/SplashScreens/Loading.png");
}

// -----------------------------------------------------------------------
void Game::UpdateLoadingScreen(float deltaSeconds_)
{
	deltaSeconds_;

	if(!m_mainMenuMusicPlaying)
	{
		ChannelGroupID bgmGroup = g_theAudioSystem->CreateOrGetChannelGroup("BGM");
		SoundID mainMenuMusic = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Prelude.mp3");
		m_mainMenuMusicPlaybackID = g_theAudioSystem->PlaySound(mainMenuMusic, bgmGroup, true);

		m_mainMenuMusicPlaying = true;
	}

	m_loadingBarWidth = Map::WIDTH * 0.7f;
	m_loadingRatio = 1.0f - ((float)m_objectLoading / (float)m_objectsToLoad);
	m_loadingBarProgress = m_loadingBarWidth * m_loadingRatio;

	if(m_loadingRatio == 1.0f)
	{
		m_loadingMessage = "Press Space...";
		if(g_theInputSystem->IsSpaceDown())
		{
			m_gameState = GAMESTATE_MAINMENU;
			m_gameStateLoading = GAMESTATELOADING_FINISHED;
		}
	}
}

// -----------------------------------------------------------------------
void Game::RenderLoadingScreen()
{
	AABB2 background = AABB2::MakeFromMinsMaxs(Vec2(0.0f, 0.0f), Vec2(Map::WIDTH, Map::HEIGHT));
	std::vector<Vertex_PCU> backgroundVerts;
	AddVertsForAABB2D(backgroundVerts, background, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Images/Backgrounds/Loading.png"));
	g_theRenderer->DrawVertexArray((int)backgroundVerts.size(), &backgroundVerts[0]);

	if(m_loadingRatio != 1.0f)
	{
		std::vector<Vertex_PCU> barVerts;

		AABB2 loadingBar = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.3f, Map::HEIGHT * 0.1f), Vec2(m_loadingBarProgress, Map::HEIGHT * 0.15f));
		AddVertsForAABB2D(barVerts, loadingBar, Rgba::WHITE);

		g_theRenderer->BindShader(g_theRenderer->GetOrCreateShader("Data/Shaders/default_unlit_devconsole.shader"));
		g_theRenderer->BindTextureViewWithSampler(0u, nullptr);
		g_theRenderer->DrawVertexArray(barVerts);
	}

	std::vector<Vertex_PCU> textVerts;
	Vec2 center = Vec2(Map::WIDTH * 0.45f, Map::HEIGHT * 0.05f);

	BitMapFont* timerFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	timerFont->AddVertsForText2D(textVerts, center, 1.0f, m_loadingMessage, Rgba::WHITE);
	g_theRenderer->BindTextureView(0u, timerFont->GetTextureView());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
}

// -----------------------------------------------------------------------
void Game::UpdateMainMenuScreen(float deltaSeconds_)
{
	deltaSeconds_;
	
	Vec2 mousePosition = g_theInputSystem->GetGameMousePosition(Vec2(Map::WIDTH, Map::HEIGHT));

	// Handle the pointer being shown at current button;
	m_pointer = false;
	m_thisFrameMenuSelection = MenuSelection::NONE;
	if (m_singlePlayerMenuButton.IsPointInside(mousePosition))
	{
		m_pointerPosition = AABB2(m_singlePlayerMenuButton.center - Vec2(7.0f, 0.64f), 0.5f);
		m_pointer = true;
		m_thisFrameMenuSelection = MenuSelection::SINGLE;
	}
	else if (m_multiPlayerMenuButton.IsPointInside(mousePosition))
	{
		m_pointerPosition = AABB2(m_multiPlayerMenuButton.center - Vec2(7.0f, 0.64f), 0.5f);
		m_pointer = true;
		m_thisFrameMenuSelection = MenuSelection::MULTI;
	}
	else if (m_settingsMenuButton.IsPointInside(mousePosition))
	{
		m_pointerPosition = AABB2(m_settingsMenuButton.center - Vec2(7.0f, 0.64f), 0.5f);
		m_pointer = true;
		m_thisFrameMenuSelection = MenuSelection::SETTINGS;
	}
	else if (m_quitMenuButton.IsPointInside(mousePosition))
	{
		m_pointerPosition = AABB2(m_quitMenuButton.center - Vec2(7.0f, 0.64f), 0.5f);
		m_pointer = true;
		m_thisFrameMenuSelection = MenuSelection::QUIT;
	}

	// If we move our mouse to another button, play a sound;
	if(m_lastFrameMenuSelection != m_thisFrameMenuSelection && m_thisFrameMenuSelection != MenuSelection::NONE)
	{
		m_playMenuSelect = true;
	}
	else
	{
		m_playMenuSelect = false;
	}
	m_lastFrameMenuSelection = m_thisFrameMenuSelection;

	if(m_playMenuSelect)
	{
		ChannelGroupID sfxGroup = g_theAudioSystem->CreateOrGetChannelGroup("SFX");
		SoundID menuSelect = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Select.mp3");
		m_mainMenuMusicPlaybackID = g_theAudioSystem->PlaySound(menuSelect, sfxGroup, false);
	}

	// Handle clicking inside of our buttons;
	if(g_theInputSystem->WasLeftMouseClickReleased())
	{
		Vec2 mouseClick = g_theInputSystem->GetGameMousePosition(Vec2(Map::WIDTH, Map::HEIGHT));
		if (m_singlePlayerMenuButton.IsPointInside(mouseClick))
		{
// 			m_gameState = GAMESTATE_PLAY;
// 			g_theAudioSystem->StopSound(m_mainMenuMusicPlaybackID);
// 			m_thisFrameMenuSelection = MenuSelection::NONE;
// 			m_lastFrameMenuSelection = MenuSelection::NONE;
		}
		else if (m_multiPlayerMenuButton.IsPointInside(mouseClick))
		{
			m_gameState = GAMESTATE_LOBBY;
			m_thisFrameMenuSelection = MenuSelection::NONE;
			m_lastFrameMenuSelection = MenuSelection::NONE;
		}
		else if (m_settingsMenuButton.IsPointInside(mouseClick))
		{
			
		}
		else if (m_quitMenuButton.IsPointInside(mouseClick))
		{
			g_theApp->HandleCloseApplication();
		}
	}
}

// -----------------------------------------------------------------------
void Game::RenderMainMenuScreen()
{
	BitMapFont* font = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	std::vector<Vertex_PCU> textVerts;
	std::vector<Vertex_PCU> backgroundVerts;
	std::vector<Vertex_PCU> selectionVerts;

	AddVertsForAABB2D(backgroundVerts, AABB2::MakeFromMinsMaxs(Vec2(0.0f, 0.0f), Vec2(Map::WIDTH, Map::HEIGHT)), Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Images/Backgrounds/Loading.png"));
	g_theRenderer->DrawVertexArray((int)backgroundVerts.size(), &backgroundVerts[0]);
	
	if(m_pointer)
	{
		AddVertsForAABB2D(selectionVerts, m_pointerPosition, Rgba::WHITE);
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Pointer.png"));
		g_theRenderer->DrawVertexArray((int)selectionVerts.size(), &selectionVerts[0]);
	}

	Vec2 center = Vec2(Map::WIDTH * 0.44f, Map::HEIGHT * 0.25f);
	font->AddVertsForText2D(textVerts, center, 1.0f, "Single Player", Rgba::WHITE);
	center = Vec2(Map::WIDTH * 0.445f, Map::HEIGHT * 0.20f);
	font->AddVertsForText2D(textVerts, center, 1.0f, "Multi Player", Rgba::WHITE);
	center = Vec2(Map::WIDTH * 0.465f, Map::HEIGHT * 0.15f);
	font->AddVertsForText2D(textVerts, center, 1.0f, "Settings", Rgba::WHITE);
	center = Vec2(Map::WIDTH * 0.48f, Map::HEIGHT * 0.10f);
	font->AddVertsForText2D(textVerts, center, 1.0f, "Quit", Rgba::WHITE);
	g_theRenderer->BindTextureView(0u, font->GetTextureView());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
}

// -----------------------------------------------------------------------
// Cameras;
// -----------------------------------------------------------------------
void Game::BeginCamera(Camera* camera)
{
	m_colorTargetView = g_theRenderer->GetFrameColorTarget();
	camera->SetColorTargetView(m_colorTargetView);
	g_theRenderer->BeginCamera(camera);
}

// -----------------------------------------------------------------------
void Game::EndCamera()
{
	g_theRenderer->EndCamera();
}

// -----------------------------------------------------------------------
// Async Loading and Assets;
// -----------------------------------------------------------------------
void Game::StartLoadingAssets()
{
	LoadGameConfigFromXML("Data/XML/GameConfig.xml");
	AbilityDefinition::LoadAbilitiesFromXML("Data/XML/Abilities.xml");
	UnitDefinition::LoadUnitsFromXML("Data/XML/Units.xml");
	CardDefinition::LoadCardsFromXML("Data/XML/Cards.xml");

	EnqueueWorkForTexturesAndGPUMeshes();

	uint coreCount = std::thread::hardware_concurrency();
	for (uint i = 0; i < coreCount; ++i)
	{
		m_threads.emplace_back(CallImageAndMeshLoadThread);
	}
}

// -----------------------------------------------------------------------
void Game::EnqueueWorkForTexturesAndGPUMeshes()
{
	// Units;
	StartLoadingTexture("Data/Sprites/Knight.png");
	StartLoadingTexture("Data/Sprites/Blackmage.png");
	StartLoadingTexture("Data/Sprites/Archer.png");
	StartLoadingTexture("Data/Sprites/Warrior.png");
	StartLoadingTexture("Data/Sprites/Whitemage.png");
	StartLoadingTexture("Data/Sprites/Paladin.png");
	StartLoadingTexture("Data/Sprites/Dragoon.png");

	// Portraits;
	StartLoadingTexture("Data/Sprites/KnightPortrait.png");
	StartLoadingTexture("Data/Sprites/BlackmagePortrait.png");
	StartLoadingTexture("Data/Sprites/ArcherPortrait.png");
	StartLoadingTexture("Data/Sprites/WarriorPortrait.png");
	StartLoadingTexture("Data/Sprites/WhitemagePortrait.png");
	StartLoadingTexture("Data/Sprites/PaladinPortrait.png");
	StartLoadingTexture("Data/Sprites/DragoonPortrait.png");

	// Ability;
	StartLoadingTexture("Data/Sprites/Abilities/Fire.png");
	StartLoadingTexture("Data/Sprites/Abilities/Cure.png");
	StartLoadingTexture("Data/Sprites/Abilities/Burn.png");
	StartLoadingTexture("Data/Sprites/Abilities/Shimmer.png");
	StartLoadingTexture("Data/Sprites/Abilities/Bleed.png");

	// Effects;
	StartLoadingTexture("Data/Sprites/Effects/PhysicalHit.png");

	// Cards;
	StartLoadingTexture("Data/Sprites/CardBorder.png");
	StartLoadingTexture("Data/Sprites/JobIcons.png");

	// Purchase Phase;
	StartLoadingTexture("Data/Sprites/Reroll.png");
	StartLoadingTexture("Data/Sprites/Freeze.png");
	StartLoadingTexture("Data/Sprites/TimerDisplay.png");
	StartLoadingTexture("Data/Sprites/GoldIcon.png");
	StartLoadingTexture("Data/Sprites/GoldIconBackground.png");
	StartLoadingTexture("Data/Sprites/Lock.png");

	// Battle Phase;
	StartLoadingTexture("Data/Images/Backgrounds/DesertBackground.png");
	StartLoadingTexture("Data/Images/Backgrounds/PlainsBackground.png");
	StartLoadingTexture("Data/Images/Backgrounds/DungeonBackground.png");

	// Purchase Phase;
	StartLoadingTexture("Data/Images/Backgrounds/Library.png");

	// Misc;
	StartLoadingTexture("Data/Sprites/Pointer.png");
	StartLoadingTexture("Data/Sprites/MenuSelection.png");

	m_gameStateLoading = GAMESTATELOADING_LOADING;
	m_objectsToLoad = m_objectLoading;
}

// -----------------------------------------------------------------------
void Game::ImageAndMeshLoadThread()
{
	bool doneLoading = false;

	ImageLoading imageLoading;
	CPUMeshLoading cpuLoading;

	while (!doneLoading)
	{
		bool gotImageWork = imageLoadingFromDiscQueue.Dequeue(&imageLoading);
		if (gotImageWork)
		{
			imageLoading.image = new Image(imageLoading.imageName.c_str());
			imageCreatingTextureQueue.Enqueue(imageLoading);
		}

		bool gotCPUWork = cpuLoadingFromDiscQueue.Dequeue(&cpuLoading);
		if (gotCPUWork)
		{
			cpuLoading.cpuMesh = new CPUMesh();
			cpuLoading.cpuMesh->SetLayout<Vertex_Lit>();
			CreateMeshFromFile(cpuLoading.meshName.c_str(), cpuLoading.cpuMesh);
			cpuCreatingGPUMeshQueue.Enqueue(cpuLoading);
		}

		if (!gotImageWork && !gotCPUWork)
		{
			doneLoading = true;
		}

		Sleep(0);
	}
}

// -----------------------------------------------------------------------
void Game::StartLoadingTexture(std::string nameOfTexture)
{
	ImageLoading loading = ImageLoading(nameOfTexture);

	m_objectLoading++;
	imageLoadingFromDiscQueue.Enqueue(loading);
}

// -----------------------------------------------------------------------
void Game::ContinueLoading()
{
	ImageLoading imageLoading;
	CPUMeshLoading cpuLoading;

	bool gotImageWork = imageCreatingTextureQueue.Dequeue(&imageLoading);
	if (gotImageWork)
	{
		g_theRenderer->CreateTextureViewFromImage(imageLoading.image, imageLoading.imageName);
		m_objectLoading--;
		delete imageLoading.image;
		imageLoading.image = nullptr;
	}

	bool gotGPUWork = cpuCreatingGPUMeshQueue.Dequeue(&cpuLoading);
	if (gotGPUWork)
	{
		g_theRenderer->CreateAndRegisterGPUMesh(cpuLoading.cpuMesh, cpuLoading.meshName);
		m_objectLoading--;
		delete cpuLoading.cpuMesh;
		cpuLoading.cpuMesh = nullptr;
	}
}

// -----------------------------------------------------------------------
bool Game::DoneLoading()
{
	return m_objectLoading == 0;
}

// -----------------------------------------------------------------------
void Game::EndLoadingThreads()
{
	for (std::thread& thread : m_threads)
	{
		thread.join();
	}

	m_threads.clear();
}

// -----------------------------------------------------------------------
// RakNet;
// -----------------------------------------------------------------------
void Game::OnIncomingPacket(RakNet::Packet* packet)
{
	if(g_theRakNetInterface->m_connection == ConnectionType::NONE)
	{
		return;
	}

	// I think I would like a RakNet class for Game stuff. RakNetInterface is supposed to be Engine side;
	switch (packet->data[0])
	{
		// ----------------------------------
		case S_LOBBYMESSAGE:
		{
			if(g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->SendLobbyMessageToClients(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTUSERNAME:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->AddClientToClientList(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_READYFLAG:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateClientReadyFlag(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_REQUESTMARKETPLACECARDS:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int playerID = -1;
				bsIn.Read(playerID);

				Player* player = g_Interface->query().GetPlayer(HasPlayerID(playerID));

				g_Interface->server().RollAndSendMarketplaceCardsForClient(player);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_REQUESTHANDCARDS:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int playerID = -1;
				bsIn.Read(playerID);

				g_Interface->server().GetAndSendCardsInHandForPlayerID(playerID);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLEARMARKETPLACECARDS:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int playerID = -1;
				bsIn.Read(playerID);

				g_Interface->server().ClearMarketplaceCardsForClient(playerID);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTCOMPLETEPURCHASEPHASE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateClientCompleteWithPurchasePhase(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTCOMPLETEBATTLEPHASE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateClientCompleteWithBattlePhase(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTMARKETPLACELOCKED:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateClientMarketplaceLocked(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTPURCHASEDMARKETCARD:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateCardsBasedOnPurchaseByPlayer(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTSOLDHANDCARD:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateCardsBasedOnSellingByPlayer(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_CLIENTPLACEDUNITFROMHANDCARD:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->UpdateUnitsBasedOnPlacingCardInUnitSlotByPlayer(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case S_WINNEROFMATCHBEINGREPORTED:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::SERVER)
			{
				g_theRakNetInterface->ProcessMatchReport(packet);
			}
			else
			{
				ERROR_AND_DIE("A non-server application has received a SERVER_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_LOBBYMESSAGE:
		{
			if(g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				std::string message = rs.C_String();

				m_lobbyConsole->AddLineToChatText(message);
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}
			
			break;
		}

		// ----------------------------------
		case C_LOBBYMESSAGEGAMESTARTING:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				ChannelGroupID sfxGroup = g_theAudioSystem->CreateOrGetChannelGroup("SFX");
				SoundID ready = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Ready.mp3");
				g_theAudioSystem->PlaySound(ready, sfxGroup, false);

				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				std::string message = rs.C_String();

				m_lobbyConsole->AddLineToChatText(message);
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_STARTMULTIPLAYERGAME:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				m_lobbyConsole->AddLineToChatText("[Server]: GO!");
				m_serverSaysStartMultiplayerGame = true;
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_PLAYERID:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				RakNet::RakString username = "";
				bsIn.Read(m_playerID);
				bsIn.Read(username);
				Player* p = g_Interface->CreatePlayer(m_playerID);
				g_Interface->SetClientPlayer(p);
				Player*& player = g_Interface->GetPlayer();
				
				player->SetPlayerUsername(username.C_String());
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_SWITCHPHASE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				g_Interface->match().SwitchPhases();
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECEIVECARDTYPESFORMARKETPLACE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int receivedCardAmount = -1;
				bsIn.Read(receivedCardAmount);

				// This will delete the client-side card from m_cards, NOT the server-side;
				g_Interface->client().CleanupMarketplaceCards();

				for(int i = 0; i < receivedCardAmount; ++i)
				{
					CardType cardType;
					int iCardType = -1;
					unsigned int cardID = 0u;
					bsIn.Read(iCardType);
					bsIn.Read(cardID);
					cardType = (CardType)iCardType;
					g_Interface->client().CreateCardForMarketplace(cardType, cardID);
				}
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECEIVECARDTYPESFORHAND:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				g_Interface->client().CleanupHandCards();

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int receivedCardAmount = -1;
				bsIn.Read(receivedCardAmount);

				int handcards = g_Interface->query().GetCardCount(CardInHand());
				GUARANTEE_OR_DIE((receivedCardAmount + handcards) <= 3, "Received an Overflow of cards for players hand.");

				for (int i = 0; i < receivedCardAmount; ++i)
				{
					CardType cardType;
					int iCardType = -1;
					unsigned int cardID = 0u;
					bsIn.Read(iCardType);
					bsIn.Read(cardID);
					cardType = (CardType)iCardType;
					g_Interface->client().CreateCardForHand(cardType, cardID);
				}
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECEIVEUNITTYPESFORFIELD:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				g_Interface->client().CleanupUnits();

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int receivedUnitAmount = -1;
				bsIn.Read(receivedUnitAmount);

				for (int i = 0; i < receivedUnitAmount; ++i)
				{
					JobType jobType;
					int iJobType = -1;
					int unitID = -1;
					int slotID = -1;
					bsIn.Read(iJobType);
					bsIn.Read(unitID);
					bsIn.Read(slotID);
					jobType = (JobType)iJobType;
					g_Interface->client().CreateUnitForField(jobType, unitID, slotID);
				}
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECEIVEENEMYUNITTYPESFORENEMYFIELD:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				g_Interface->client().CleanupEnemyUnits();

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int receivedUnitAmount = -1;
				bsIn.Read(receivedUnitAmount);

				for (int i = 0; i < receivedUnitAmount; ++i)
				{
					JobType jobType;
					int iJobType = -1;
					int unitID = -1;
					int slotID = -1;
					bsIn.Read(iJobType);
					bsIn.Read(unitID);
					bsIn.Read(slotID);
					jobType = (JobType)iJobType;
					g_Interface->client().CreateEnemyUnitForEnemyField(jobType, unitID, slotID);
				}
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECIEVEGOLDAMOUNT:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				Player*& player = g_Interface->GetPlayer();
				int goldAmount = 0;
				int actualGold = 0;
				bsIn.Read(goldAmount);
				bsIn.Read(actualGold);
				player->SetGoldAmount(goldAmount);
				player->SetActualGoldAmount(actualGold);

			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECIEVEGOESFIRSTFORBATTLE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bool goesFirst = false;
				bsIn.Read(goesFirst);

				Player*& player = g_Interface->GetPlayer();
				player->SetGoesFirstForBattlePhase(goesFirst);
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECIEVESEEDFORBATTLE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int seed = 0;
				bsIn.Read(seed);

				Player*& player = g_Interface->GetPlayer();
				player->SetSeedToUseForRNG(seed);
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECIEVEENEMYPLAYERINFOFORBATTLE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int enemyPlayerID = 0;
				unsigned int currentHealthOfEnemy = 0u;
				RakNet::RakString enemyPlayerUsername;
				bsIn.Read(enemyPlayerID);
				bsIn.Read(currentHealthOfEnemy);
				bsIn.Read(enemyPlayerUsername);

				g_Interface->CreateAIEnemyPlayer(enemyPlayerID);
				Player*& enemyPlayer = g_Interface->GetEnemy();
				enemyPlayer->SetPlayerHealth(currentHealthOfEnemy);
				enemyPlayer->SetPlayerUsername(enemyPlayerUsername.C_String());
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECIEVEMATCHIDFORBATTLE:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int matchID = 0;
				bsIn.Read(matchID);

				g_Interface->client().SetMatchIDForThisBattlePhase(matchID);
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_RECIEVEUPDATEDPLAYERHEALTH:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int updatedHealth = 0;
				bsIn.Read(updatedHealth);

				g_Interface->client().UpdatePlayerHealth(updatedHealth);
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_YOUWINTHEGAME:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				g_theDevConsole->Print("YOU WIN! :)");
				m_gameState = GAMESTATE_MAINMENU;
				m_lobbyConsole->m_readyToStart = false;
				SetIsReady(false);
				m_connectionAttemptsStarted = false;
				m_setupConnectionInfoComplete = false;
				m_serverSaysStartMultiplayerGame = false;
				m_isConnected = false;
				m_lobbyConsole->RemoveAllChatText();
				CloseConnectionToServer();
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}

		// ----------------------------------
		case C_YOULOSETHEGAME:
		{
			if (g_theRakNetInterface->m_connection == ConnectionType::CLIENT)
			{
				g_theDevConsole->Print("YOU LOSE :(");
				m_gameState = GAMESTATE_MAINMENU;
				m_lobbyConsole->m_readyToStart = false;
				SetIsReady(false);
				m_connectionAttemptsStarted = false;
				m_setupConnectionInfoComplete = false;
				m_serverSaysStartMultiplayerGame = false;
				m_isConnected = false;
				m_lobbyConsole->RemoveAllChatText();
				CloseConnectionToServer();
			}
			else
			{
				ERROR_AND_DIE("A non-client application has received a CLIENT_MESSAGE.");
			}

			break;
		}
	}
}

// -----------------------------------------------------------------------
void Game::SetupClientConnectionInformation()
{
	unsigned short numberOfSystems;
	RakNet::SystemAddress systemAddress;
	g_theRakNetInterface->m_peer->GetConnectionList(&systemAddress, &numberOfSystems);

	if (numberOfSystems > 0)
	{
		g_theRakNetInterface->m_serverGUID = g_theRakNetInterface->m_peer->GetGuidFromSystemAddress(systemAddress);
		g_theRakNetInterface->m_serverAddress = systemAddress;

		m_setupConnectionInfoComplete = true;
		m_isConnected = true;
	}
}

// -----------------------------------------------------------------------
void Game::SendLobbyMessageToServer(std::string message)
{
	g_theRakNetInterface->SendLobbyMessageToServer(message);
}

// -----------------------------------------------------------------------
bool Game::JoinServerAsClient()
{
	return g_theRakNetInterface->JoinServerAsClient(hostIP);
}

void Game::CloseConnectionToServer()
{
	g_theRakNetInterface->CloseConnectionToServer();	
}

// ----------------------------------------------------------------------- 
void Game::CheckConnectionStatus()
{
	if (m_connectionAttemptsStarted && !m_setupConnectionInfoComplete)
	{
		SetupClientConnectionInformation();
		if (m_setupConnectionInfoComplete)
		{
			m_lobbyConsole->m_texts.push_back("Connected...");
			g_theRakNetInterface->SendClientUsername(m_username);
		}
	}
}

// -----------------------------------------------------------------------
void Game::SetIsReady(bool isReady)
{
	if(m_isConnected)
	{
		g_theRakNetInterface->SendIsReadyFlag(isReady);
	}
}

// -----------------------------------------------------------------------
void Game::LoadGameConfigFromXML(std::string xmlPath_)
{
	tinyxml2::XMLDocument gameConfigXMLDoc;
	gameConfigXMLDoc.LoadFile(xmlPath_.c_str());

	if (gameConfigXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", xmlPath_.c_str());
		printf("ErrorID:      %i\n", gameConfigXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", gameConfigXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", gameConfigXMLDoc.ErrorName());
	}
	else
	{
		printf("Success with XML Doc: %s\n", xmlPath_.c_str());

		XmlElement* ipElement = gameConfigXMLDoc.RootElement()->FirstChildElement("ip");
		if (ipElement)
		{
			hostIP = ParseXmlAttribute(*ipElement, "ipv4", "");
		}
	}
}

//------------------------------------------------------------------------------------------------
// Testing Parse Stuff;
//------------------------------------------------------------------------------------------------
void Game::AppendTestFileBufferData(BufferWriter& bufWrite, BufferEndian endianMode)
{
	bufWrite.SetEndianMode(endianMode);
	bufWrite.AppendChar('T');
	bufWrite.AppendChar('E');
	bufWrite.AppendChar('S');
	bufWrite.AppendChar('T');
	bufWrite.AppendByte(1); // Version 1
	bufWrite.AppendBool(bufWrite.IsEndianModeBigEndian()); // written as byte 0 or 1
	bufWrite.AppenedUInt32(0x12345678);
	bufWrite.AppendInt32(-7); // signed 32-bit int
	bufWrite.AppendFloat(1.0f); // in memory looks like hex: 00 00 80 3F (or 3F 80 00 00 in big endian)
	bufWrite.AppendDouble(3.1415926535897932384626433832795); // actually 3.1415926535897931 (best it can do)
	bufWrite.AppendStringZeroTerminated("Hello"); // written with a trailing 0 ('\0') after (6 bytes total)
	bufWrite.AppendStringAfter32BitLength("Is this thing on?"); // uint 17, then 17 chars (no zero-terminator after)
	bufWrite.AppendRgba(Rgba(0.1f, 0.2f, 0.3f, 1.0f)); // four bytes in RGBA order (endian-independent)
	bufWrite.AppendByte(8); // 0x08 == 8 (byte)
	bufWrite.AppendRgb(Rgba(0.0f, 0.1f, 0.2f, 1.0f)); // written as 3 bytes (RGB) only; ignores Alpha
	bufWrite.AppendByte(9); // 0x09 == 9 (byte)
	bufWrite.AppendIntVec2(IntVec2(1920, 1080));
	bufWrite.AppendVec2(Vec2(-0.6f, 0.8f));
	bufWrite.AppendVertexPCU(Vertex_PCU(Vec3(3.0f, 4.0f, 5.0f), Rgba(0.5f, 0.6f, 0.7f, 0.8f), Vec2(0.125f, 0.625f)));
}

//------------------------------------------------------------------------------------------------
void Game::ParseTestFileBufferData(BufferParser& bufParse, BufferEndian endianMode)
{
	// Parse known test elements
	bufParse.SetEndianMode(endianMode);
	char fourCC0_T = bufParse.ParseChar(); // 'T' == 0x54 hex == 84 decimal
	char fourCC1_E = bufParse.ParseChar(); // 'E' == 0x45 hex == 84 decimal
	char fourCC2_S = bufParse.ParseChar(); // 'S' == 0x53 hex == 69 decimal
	char fourCC3_T = bufParse.ParseChar(); // 'T' == 0x54 hex == 84 decimal
	unsigned char version = bufParse.ParseByte(); // version 1
	bool isBigEndian = bufParse.ParseBool(); // written in buffer as byte 0 or 1
	unsigned int largeUint = bufParse.ParseUInt32(); // 0x12345678
	int negativeSeven = bufParse.ParseInt32(); // -7 (as signed 32-bit int)
	float oneF = bufParse.ParseFloat(); // 1.0f
	double pi = bufParse.ParseDouble(); // 3.1415926535897932384626433832795 (or as best it can)

	std::string helloString, isThisThingOnString;
	bufParse.ParseStringZeroTerminated(helloString); // written with a trailing 0 ('\0') after (6 bytes total)
	bufParse.ParseStringAfter32BitLength(isThisThingOnString); // written as uint 17, then 17 characters (no zero-terminator after)

	Rgba color1 = bufParse.ParseRgba(); // Rgba( 0.1f, 0.2f, 0.3f, 1.0f)
	unsigned char eight = bufParse.ParseByte(); // 0x08 == 8 (byte)
	Rgba color2 = bufParse.ParseRgb(); // Rgba(0.0f, 0.1f, 0.2f, 1.0f) written as 3 bytes (RGB) only; ignores Alpha
	unsigned char nine = bufParse.ParseByte(); // 0x09 == 9 (byte)
	IntVec2 highDefRes = bufParse.ParseIntVec2(); // IntVector2( 1920, 1080 )
	Vec2 normal2D = bufParse.ParseVec2(); // Vector2( -0.6f, 0.8f )
	Vertex_PCU vertex = bufParse.ParseVertexPCU(); // VertexPCU( 3.f, 4.f, 5.f, Rgba(100,101,102,103), 0.125f, 0.625f ) );

	// Validate actual values parsed
	GUARANTEE_RECOVERABLE(fourCC0_T == 'T', "");
	GUARANTEE_RECOVERABLE(fourCC1_E == 'E', "");
	GUARANTEE_RECOVERABLE(fourCC2_S == 'S', "");
	GUARANTEE_RECOVERABLE(fourCC3_T == 'T', "");
	GUARANTEE_RECOVERABLE(version == 1, "");
	GUARANTEE_RECOVERABLE(isBigEndian == bufParse.IsCurrentlyBigEndian(), "");
	GUARANTEE_RECOVERABLE(largeUint == 0x12345678, "");
	GUARANTEE_RECOVERABLE(negativeSeven == -7, "");
	GUARANTEE_RECOVERABLE(oneF == 1.f, "");
	GUARANTEE_RECOVERABLE(pi == 3.1415926535897932384626433832795, "");
	GUARANTEE_RECOVERABLE(helloString == "Hello", "");
	GUARANTEE_RECOVERABLE(isThisThingOnString == "Is this thing on?", "");
	Rgba color1Check = Rgba(0.1f, 0.2f, 0.3f, 1.0f);
	Rgba color2Check = Rgba(0.0f, 0.1f, 0.2f, 1.0f);
	GUARANTEE_RECOVERABLE(color1 == color1Check, "");
	GUARANTEE_RECOVERABLE(eight == 8, "");
	GUARANTEE_RECOVERABLE(color2 == color2Check, "");
	GUARANTEE_RECOVERABLE(nine == 9, "");
	GUARANTEE_RECOVERABLE(highDefRes == IntVec2(1920, 1080), "");
	GUARANTEE_RECOVERABLE(normal2D == Vec2(-0.6f, 0.8f), "");
	Rgba vcolorCheck = Rgba(0.5f, 0.6f, 0.7f, 0.8f);
	GUARANTEE_RECOVERABLE(vertex.position == Vec3(3.0f, 4.0f, 5.0f), "");
	GUARANTEE_RECOVERABLE(vertex.color == vcolorCheck, "");
	GUARANTEE_RECOVERABLE(vertex.uvTexCoords == Vec2(0.125f, 0.625f), "");
}

// -----------------------------------------------------------------------
// static;
// -----------------------------------------------------------------------
void CallImageAndMeshLoadThread()
{
	g_theApp->m_theGame->ImageAndMeshLoadThread();
}