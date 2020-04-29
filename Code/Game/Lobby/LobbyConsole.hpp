#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <string.h>
#include <vector>

class Game;

enum class LobbySelection
{
	NONE,
	ENTER,
	READY,
	BACK
};

class LobbyConsole
{

public:

	LobbyConsole(Game* game_);
	~LobbyConsole();

	// Flow;
	void BeginFrame();
	void Update(float deltaSeconds_);
	void Render();

	// Key Actions;
	void Backspace();
	void Enter();

	// Keys;
	bool HandleChar(unsigned char asKey_);

	// Mouse;
	void HandleMouseHover();
	void HandleMouseClick();

	// Sound;
	void PlaySelectSoundCheck();

	// Background;
	void DisplayBackgroundScreen();

	// Lobby Text Windows;
	void DisplayLobbyTextWindows();

	// Cursor
	void UpdateCursorBlip(float deltaSeconds_);
	void UpdateCursorPosition(int moveDirection);
	void DisplayCursor();

	// Display Text;
	void AddStringToTextOutput(const std::string& devConsolePrintString);
	void AddLetterToCurrentTypingText(const std::string& devConsolePrintString);
	void DisplayTextInTextArea();
	void AddLineToChatText(std::string line_);
	void RemoveAllChatText();
	void DisplayTextInTypingArea();
	void DisplayButtonText();
	void DisplayPointer();

	// Helpers;
	void RemoveOldestText();

public:

	Game* m_game = nullptr;

	// Typing Area and Text Area;
	bool m_focusedOnTypingArea = false;
	int m_maxTypingCharacters = 54;
	std::string m_informationText = "Enter username and press CONNECT!";
	std::string m_currentTypingText = "";
	std::vector<std::string> m_texts;

	// Cursor;
	bool m_cursorBlip = true;
	int m_cursorPosition = 0;
	float m_cursorBlipTime = 0.5f;
	float m_cursorBlipTimer = 0.0f;

	// Lobby Text Windows and Buttons;
	bool m_lobbyPointer = false;
	bool m_lobbyPlaySelectSound = false;
	LobbySelection m_thisFrameLobbySelection = LobbySelection::NONE;
	LobbySelection m_lastFrameLobbySelection = LobbySelection::NONE;
	AABB2 m_lobbyPointerPosition;
	AABB2 m_lobbyTypingArea;
	AABB2 m_lobbyTextArea;
	AABB2 m_lobbySendMessageButton;
	AABB2 m_lobbyBackButton;
	AABB2 m_lobbyReadyButton;

	bool m_readyToStart = false;
};