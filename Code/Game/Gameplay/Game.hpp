#pragma once
#include "Engine/Async/AsyncQueue.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Buffer/BufferUtilities.hpp"

#include <chrono>
#include <queue>
#include <thread>
#include <vector>

struct Camera;
class Map;
class Interface;
class TextureView;
class LobbyConsole;

enum GameStateLoading
{
	GAMESTATELOADING_INVALID = -1,

	GAMESTATELOADING_LOADING,
	GAMESTATELOADING_FINISHED,

	GAMESTATELOADING_COUNT
};

enum GameState
{
	GAMESTATE_INVALID = -1,

	GAMESTATE_STARTUP,
	GAMESTATE_MAINMENU,
	GAMESTATE_LOBBY,
	GAMESTATE_PLAY,

	GAMESTATE_COUNT
};

enum class MenuSelection
{
	NONE,
	SINGLE,
	MULTI,
	SETTINGS,
	QUIT
};



namespace RakNet
{
	struct Packet;
	struct RakNetGUID;
	struct SystemAddress;
}

struct ImageLoading
{
	ImageLoading(){}
	ImageLoading(std::string name_):imageName(name_){}

	std::string imageName;
	Image* image;
};

struct CPUMeshLoading
{
	CPUMeshLoading() {}
	CPUMeshLoading(std::string name_):meshName(name_){}

	std::string meshName;
	CPUMesh* cpuMesh;
};

class Game
{
	
public:

	// Constructor/Deconstructor
	Game();
	~Game();

	// Flow;
	void Init();
	void Startup();
	void BeginFrame();
	void AsyncLoading();
	void Update(float deltaSeconds_);
	void Render();
	void EndFrame();

	// Loading;
	bool CheckLoading(float deltaSeconds_);

	// Screens;
	void CreateLoadingScreen();
	void UpdateLoadingScreen(float deltaSeconds_);
	void RenderLoadingScreen();
	void UpdateMainMenuScreen(float deltaSeconds_);
	void RenderMainMenuScreen();

	// Cameras;
	void BeginCamera(Camera* camera);
	void EndCamera();

	// Async Loading and Assets;
	void StartLoadingAssets();
	void EnqueueWorkForTexturesAndGPUMeshes();
	void ImageAndMeshLoadThread();
	void StartLoadingTexture(std::string nameOfTexture);
	void ContinueLoading();
	bool DoneLoading();
	void EndLoadingThreads();

	// RakNet;
	void OnIncomingPacket(RakNet::Packet* packet);
	void SetupClientConnectionInformation();
	void SendLobbyMessageToServer(std::string message);
	bool JoinServerAsClient();
	void CloseConnectionToServer();
	void CheckConnectionStatus();
	void SetIsReady(bool isReady);

	// Game Config;
	void LoadGameConfigFromXML(std::string xmlPath_);

	// Testing Parse Stuff;
	void AppendTestFileBufferData(BufferWriter& bufWrite, BufferEndian endianMode);
	void ParseTestFileBufferData(BufferParser& bufParse, BufferEndian endianMode);

public:
	
	// Game Navigation and State;
	GameState m_gameState = GAMESTATE_INVALID;
	AABB2 m_singlePlayerMenuButton;
	AABB2 m_multiPlayerMenuButton;
	AABB2 m_settingsMenuButton;
	AABB2 m_quitMenuButton;
	AABB2 m_pointerPosition;
	bool m_pointer = false;
	bool m_playMenuSelect = false;
	MenuSelection m_thisFrameMenuSelection = MenuSelection::NONE;
	MenuSelection m_lastFrameMenuSelection = MenuSelection::NONE;
	
	
	
	LobbyConsole* m_lobbyConsole = nullptr;
	std::string hostIP = "";
	std::string m_username = "";


	// Game Bounds;
	Vec2 m_clientMins					= Vec2(0.0f, 0.0f);
	Vec2 m_clientMaxs					= Vec2(0.0f, 0.0f);
	
	// Cameras;
	Camera* m_gameMainCamera			= nullptr;
	Camera* m_uiCamera					= nullptr;
	Vec2 m_focalPoint					= Vec2( 0.0f, 0.0f );

	// Color Target View;
	ColorTargetView* m_colorTargetView	= nullptr;
	Rgba m_clearColor					= Rgba::BLACK;

	// Screens;
	AABB2 m_screenBackground;
	TextureView* m_screenBackgroundTexture = nullptr;

	// Async Loading and Assets;
	GameStateLoading m_gameStateLoading = GAMESTATELOADING_INVALID;
	int m_objectLoading = 0;
	int m_objectsToLoad = 100;
	std::vector<std::thread> m_threads;
	AsyncQueue<ImageLoading> imageLoadingFromDiscQueue;
	AsyncQueue<ImageLoading> imageCreatingTextureQueue;
	AsyncQueue<CPUMeshLoading> cpuLoadingFromDiscQueue;
	AsyncQueue<CPUMeshLoading> cpuCreatingGPUMeshQueue;
	float m_loadingBarWidth = 0.0f;
	float m_loadingBarProgress = 0.0f;
	float m_loadingRatio = 0.0f;
	std::string m_loadingMessage = "Loading...";

	// RakNet;
	bool m_connectionAttemptsStarted = false;
	bool m_setupConnectionInfoComplete = false;
	bool m_isConnected = false;
	bool m_serverSaysStartMultiplayerGame = false;



	int m_playerID = -1;

	// Server specific RakNet things;


	// Music;
	bool m_mainMenuMusicPlaying = false;
	size_t m_mainMenuMusicPlaybackID;


};

void CallImageAndMeshLoadThread();