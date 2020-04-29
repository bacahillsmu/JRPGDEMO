#include "Game/Lobby/LobbyConsole.hpp"



// -----------------------------------------------------------------------
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Line.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Input/InputSystem.hpp"

// -----------------------------------------------------------------------
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Map.hpp"

// -----------------------------------------------------------------------
LobbyConsole::LobbyConsole(Game* game_)
	: m_game(game_)
{
	m_lobbyTextArea = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.05f, Map::HEIGHT * 0.15), Vec2(Map::WIDTH * 0.6f, Map::HEIGHT * 0.6f));
	m_lobbyTypingArea = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.05f, Map::HEIGHT * 0.05), Vec2(Map::WIDTH * 0.6f, Map::HEIGHT * 0.1f));
	m_lobbySendMessageButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.62f, Map::HEIGHT * 0.05), Vec2(Map::WIDTH * 0.7f, Map::HEIGHT * 0.1f));
	m_lobbyBackButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.72f, Map::HEIGHT * 0.05), Vec2(Map::WIDTH * 0.8f, Map::HEIGHT * 0.1f));
	m_lobbyReadyButton = AABB2::MakeFromMinsMaxs(Vec2(Map::WIDTH * 0.82f, Map::HEIGHT * 0.05), Vec2(Map::WIDTH * 0.9f, Map::HEIGHT * 0.1f));
}

// -----------------------------------------------------------------------
LobbyConsole::~LobbyConsole()
{

}

// -----------------------------------------------------------------------
// Flow;
// -----------------------------------------------------------------------
void LobbyConsole::BeginFrame()
{
	RemoveOldestText();
}

// -----------------------------------------------------------------------
void LobbyConsole::Update(float deltaSeconds_)
{
	UpdateCursorBlip(deltaSeconds_);
	HandleMouseHover();
	HandleMouseClick();
}

// -----------------------------------------------------------------------
void LobbyConsole::Render()
{
	DisplayBackgroundScreen();
	DisplayLobbyTextWindows();
	DisplayCursor();
	DisplayTextInTypingArea();
	DisplayTextInTextArea();	
	DisplayButtonText();
	DisplayPointer();
}

// -----------------------------------------------------------------------
// Key Actions;
// -----------------------------------------------------------------------
void LobbyConsole::Backspace()
{
	std::string firstChunk = m_currentTypingText.substr(0, m_cursorPosition);
	std::string secondChunk = m_currentTypingText.substr(m_cursorPosition, m_currentTypingText.size());

	firstChunk = firstChunk.substr(0, (m_cursorPosition) - 1);
	UpdateCursorPosition(-1);

	m_currentTypingText = firstChunk + secondChunk;
}

// -----------------------------------------------------------------------
void LobbyConsole::Enter()
{
	if (m_currentTypingText.size() != 0)
	{
		if(m_game->m_isConnected)
		{
			m_game->SendLobbyMessageToServer(m_currentTypingText);
		}
		else
		{
			AddStringToTextOutput(m_currentTypingText);
		}

		m_currentTypingText = "";
		m_cursorPosition = 0;
	}
}

// -----------------------------------------------------------------------
// Keys;
// -----------------------------------------------------------------------
bool LobbyConsole::HandleChar(unsigned char asKey_)
{
	bool keyHandled = false;

	if (asKey_ == VirtualKey::ENTER
	 || asKey_ == VirtualKey::ESC
	 || asKey_ == VirtualKey::BACKSPACE)
	{
		return keyHandled;
	}

	if (m_game->m_gameState == GAMESTATE_LOBBY && m_focusedOnTypingArea)
	{
		if(m_currentTypingText.size() < m_maxTypingCharacters)
		{
			std::string letter(1, asKey_);
			AddLetterToCurrentTypingText(letter);
			UpdateCursorPosition(1);
		}

		keyHandled = true;
	}

	return keyHandled;
}

// -----------------------------------------------------------------------
// Mouse;
// -----------------------------------------------------------------------
void LobbyConsole::HandleMouseHover()
{
	Vec2 mousePosition = g_theInputSystem->GetGameMousePosition(Vec2(Map::WIDTH, Map::HEIGHT));

	m_lobbyPointer = false;
	m_thisFrameLobbySelection = LobbySelection::NONE;
	if (m_lobbySendMessageButton.IsPointInside(mousePosition))
	{
		m_lobbyPointerPosition = AABB2(m_lobbySendMessageButton.center - Vec2(3.0f, 0.64f), 0.5f);
		m_lobbyPointer = true;
		m_thisFrameLobbySelection = LobbySelection::ENTER;
	}
	else if (m_lobbyReadyButton.IsPointInside(mousePosition) && m_game->m_isConnected)
	{
		m_lobbyPointerPosition = AABB2(m_lobbyReadyButton.center - Vec2(3.0f, 0.64f), 0.5f);
		m_lobbyPointer = true;
		m_thisFrameLobbySelection = LobbySelection::READY;
	}
	else if (m_lobbyBackButton.IsPointInside(mousePosition))
	{
		m_lobbyPointerPosition = AABB2(m_lobbyBackButton.center - Vec2(3.0f, 0.64f), 0.5f);
		m_lobbyPointer = true;
		m_thisFrameLobbySelection = LobbySelection::BACK;
	}

	// If we move our mouse to another button, play a sound;
	PlaySelectSoundCheck();
}

// -----------------------------------------------------------------------
void LobbyConsole::HandleMouseClick()
{
	if (g_theInputSystem->WasLeftMouseClickReleased())
	{
		m_focusedOnTypingArea = false;
		Vec2 mouseClick = g_theInputSystem->GetGameMousePosition(Vec2(Map::WIDTH, Map::HEIGHT));
		if (m_lobbySendMessageButton.IsPointInside(mouseClick))
		{
			m_game->m_username = m_currentTypingText;
			m_game->m_connectionAttemptsStarted = m_game->JoinServerAsClient();
			m_currentTypingText = "";
			m_cursorPosition = 0;
		}
		else if (m_lobbyReadyButton.IsPointInside(mouseClick) && m_game->m_isConnected)
		{
			m_readyToStart = !m_readyToStart;
			m_game->SetIsReady(m_readyToStart);
			m_currentTypingText = "";
			m_cursorPosition = 0;
		}
		else if (m_lobbyBackButton.IsPointInside(mouseClick))
		{
			m_game->m_gameState = GAMESTATE_MAINMENU;
		}
		else if (m_lobbyTypingArea.IsPointInside(mouseClick))
		{
			m_focusedOnTypingArea = true;
			m_currentTypingText = "";
			m_cursorPosition = 0;
		}
	}
}

// -----------------------------------------------------------------------
// Sound;
// -----------------------------------------------------------------------
void LobbyConsole::PlaySelectSoundCheck()
{
	if (m_lastFrameLobbySelection != m_thisFrameLobbySelection && m_thisFrameLobbySelection != LobbySelection::NONE)
	{
		m_lobbyPlaySelectSound = true;
	}
	else
	{
		m_lobbyPlaySelectSound = false;
	}
	m_lastFrameLobbySelection = m_thisFrameLobbySelection;

	if (m_lobbyPlaySelectSound)
	{
		ChannelGroupID sfxGroup = g_theAudioSystem->CreateOrGetChannelGroup("SFX");
		SoundID menuSelect = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Select.mp3");
		g_theAudioSystem->PlaySound(menuSelect, sfxGroup, false);
	}
}

// -----------------------------------------------------------------------
// Background;
// -----------------------------------------------------------------------
void LobbyConsole::DisplayBackgroundScreen()
{
	std::vector<Vertex_PCU> backgroundVerts;
	AddVertsForAABB2D(backgroundVerts, AABB2::MakeFromMinsMaxs(Vec2(0.0f, 0.0f), Vec2(Map::WIDTH, Map::HEIGHT)), Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Images/Backgrounds/Loading.png"));
	g_theRenderer->DrawVertexArray((int)backgroundVerts.size(), &backgroundVerts[0]);
}

// -----------------------------------------------------------------------
// Lobby Text Windows;
// -----------------------------------------------------------------------
void LobbyConsole::DisplayLobbyTextWindows()
{
	std::vector<Vertex_PCU> lobbyTextWindowVerts;
	Line typingAreaTop(m_lobbyTypingArea.GetTopLeft(), m_lobbyTypingArea.GetTopRight(), 0.25f);
	Line typingAreaRight(m_lobbyTypingArea.GetTopRight(), m_lobbyTypingArea.GetBottomRight(), 0.25f);
	Line typingAreaLeft(m_lobbyTypingArea.GetTopLeft(), m_lobbyTypingArea.GetBottomLeft(), 0.25f);
	Line typingAreaBottom(m_lobbyTypingArea.GetBottomLeft(), m_lobbyTypingArea.GetBottomRight(), 0.25f);
	AddVertsForAABB2D(lobbyTextWindowVerts, m_lobbyTypingArea, Rgba::FFBLUE);
	AddVertsForLine2D(lobbyTextWindowVerts, typingAreaTop, Rgba::WHITE);
	AddVertsForLine2D(lobbyTextWindowVerts, typingAreaRight, Rgba::WHITE);
	AddVertsForLine2D(lobbyTextWindowVerts, typingAreaLeft, Rgba::WHITE);
	AddVertsForLine2D(lobbyTextWindowVerts, typingAreaBottom, Rgba::WHITE);

	if (m_game->m_isConnected)
	{
		Line textAreaTop(m_lobbyTextArea.GetTopLeft(), m_lobbyTextArea.GetTopRight(), 0.25f);
		Line textAreaRight(m_lobbyTextArea.GetTopRight(), m_lobbyTextArea.GetBottomRight(), 0.25f);
		Line textAreaLeft(m_lobbyTextArea.GetTopLeft(), m_lobbyTextArea.GetBottomLeft(), 0.25f);
		Line textAreaBottom(m_lobbyTextArea.GetBottomLeft(), m_lobbyTextArea.GetBottomRight(), 0.25f);
		AddVertsForAABB2D(lobbyTextWindowVerts, m_lobbyTextArea, Rgba::FFBLUE);
		AddVertsForLine2D(lobbyTextWindowVerts, textAreaTop, Rgba::WHITE);
		AddVertsForLine2D(lobbyTextWindowVerts, textAreaRight, Rgba::WHITE);
		AddVertsForLine2D(lobbyTextWindowVerts, textAreaLeft, Rgba::WHITE);
		AddVertsForLine2D(lobbyTextWindowVerts, textAreaBottom, Rgba::WHITE);
	}

	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, nullptr);
	g_theRenderer->DrawVertexArray((int)lobbyTextWindowVerts.size(), &lobbyTextWindowVerts[0]);
}

// -----------------------------------------------------------------------
// Cursor;
// -----------------------------------------------------------------------
void LobbyConsole::UpdateCursorBlip(float deltaSeconds_)
{
	m_cursorBlipTimer += deltaSeconds_;
	m_cursorBlipTimer = Clamp(m_cursorBlipTimer, 0.0f, m_cursorBlipTime);

	if (m_cursorBlipTimer == m_cursorBlipTime)
	{
		m_cursorBlip = !m_cursorBlip;
		m_cursorBlipTimer = 0.0f;
	}
}

// -----------------------------------------------------------------------
void LobbyConsole::UpdateCursorPosition(int moveDirection)
{
	m_cursorPosition += moveDirection;
	m_cursorPosition = Clamp(m_cursorPosition, 0, m_maxTypingCharacters);
}

// -----------------------------------------------------------------------
void LobbyConsole::DisplayCursor()
{	
	if (m_game->m_gameState == GAMESTATE_LOBBY && m_focusedOnTypingArea)
	{
		std::vector<Vertex_PCU> consoleCursorQuad;

		Vec2 startPosition = m_lobbyTextArea.GetBottomLeft();
		Vec2 bottomLeft = Vec2(startPosition.x + m_cursorPosition + 0.35f, Map::HEIGHT * 0.0565f);
		Vec2 topRight = Vec2(startPosition.x + m_cursorPosition + 0.35f + Map::WIDTH * 0.001f, Map::HEIGHT * 0.092f);

		AABB2 box = AABB2::MakeFromMinsMaxs(bottomLeft, topRight);

		AddVertsForAABB2D(consoleCursorQuad, box, Rgba::WHITE);
		if (m_cursorBlip)
		{
			g_theRenderer->BindTextureViewWithSampler(0, nullptr);
			g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
			g_theRenderer->DrawVertexArray(consoleCursorQuad);
		}
	}
}

// -----------------------------------------------------------------------
// Display Text;
// -----------------------------------------------------------------------
void LobbyConsole::AddStringToTextOutput(const std::string& devConsolePrintString)
{
	m_texts.push_back(devConsolePrintString);
}

// -----------------------------------------------------------------------
void LobbyConsole::AddLetterToCurrentTypingText(const std::string& devConsolePrintString)
{
	std::string firstChunk = m_currentTypingText.substr(0, m_cursorPosition);
	std::string secondChunk = m_currentTypingText.substr(m_cursorPosition, m_currentTypingText.size());
	m_currentTypingText = firstChunk + devConsolePrintString + secondChunk;
}

// -----------------------------------------------------------------------
void LobbyConsole::DisplayTextInTextArea()
{
	BitMapFont* font = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	Vec2 textStartPosition = m_lobbyTextArea.GetBottomLeft();

	std::vector<Vertex_PCU> textVerts;
	for (int textIndex = 0; textIndex < m_texts.size(); textIndex++)
	{
		Vec2 printPosition(0.0f, (float)(m_texts.size() - textIndex));

		font->AddVertsForText2D(textVerts, (textStartPosition + printPosition), 1.0f, m_texts[textIndex], Rgba::WHITE);
	}

	if ((int)textVerts.size() > 0)
	{
		g_theRenderer->BindTextureView(0u, font->GetTextureView());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}
}

// -----------------------------------------------------------------------
void LobbyConsole::AddLineToChatText(std::string line_)
{
	m_texts.push_back(line_);
}

// -----------------------------------------------------------------------
void LobbyConsole::RemoveAllChatText()
{
	m_texts.clear();
}

// -----------------------------------------------------------------------
void LobbyConsole::DisplayTextInTypingArea() 
{
	BitMapFont* font = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");

	std::vector<Vertex_PCU> textVerts;
	Vec2 textStartPosition = m_lobbyTypingArea.GetBottomLeft() + Vec2(0.0f, 0.2f);

	if(!m_game->m_isConnected && !m_focusedOnTypingArea && m_currentTypingText == "")
	{
		font->AddVertsForText2D(textVerts, textStartPosition, 1.0f, m_informationText, Rgba::HALF_GRAY);
	}
	else
	{
		font->AddVertsForText2D(textVerts, textStartPosition, 1.0f, m_currentTypingText, Rgba::WHITE);
	}
	

	if ((int)textVerts.size() > 0)
	{
		g_theRenderer->BindTextureView(0u, font->GetTextureView());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}
}

// -----------------------------------------------------------------------
void LobbyConsole::DisplayButtonText()
{
	BitMapFont* font = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	std::vector<Vertex_PCU> textVerts;
	font->AddVertsForText2D(textVerts, m_lobbySendMessageButton.center - Vec2(2.0f, 0.8f), 0.75f, "Connect", Rgba::WHITE);
	font->AddVertsForText2D(textVerts, m_lobbyBackButton.center - Vec2(2.0f, 0.8f), 0.75f, "Back", Rgba::WHITE);
	if(m_game->m_isConnected)
	{
		font->AddVertsForText2D(textVerts, m_lobbyReadyButton.center - Vec2(2.0f, 0.8f), 0.75f, "Ready", Rgba::WHITE);
	}
	g_theRenderer->BindTextureView(0u, font->GetTextureView());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
}

// ----------------------------------------------------------------------- 
void LobbyConsole::DisplayPointer()
{
	if (m_lobbyPointer)
	{
		std::vector<Vertex_PCU> selectionVerts;
		AddVertsForAABB2D(selectionVerts, m_lobbyPointerPosition, Rgba::WHITE);
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Pointer.png"));
		g_theRenderer->DrawVertexArray((int)selectionVerts.size(), &selectionVerts[0]);
	}
}

// -----------------------------------------------------------------------
void LobbyConsole::RemoveOldestText()
{
	// Check size of DevConsole
	if (m_texts.size() > 21)
	{
		m_texts.erase(m_texts.begin());
	}
}
