#include "Game/Gameplay/Map.hpp"

// Commons ----------------------------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Framework/GameCommon.hpp"

// Engine Includes --------------------------------------------------------------------------------
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/Line.hpp"
#include "Engine/Core/RandomNumberGenerator.hpp"

// Game Includes ----------------------------------------------------------------------------------
#include "Game/Framework/App.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Input/GameInput.hpp"
#include "Game/Framework/Interface.hpp"
#include "Game/Gameplay/PlayerFIlters.hpp"
#include "Game/Cards/CardFilters.hpp"


// ------------------------------------------------------------------------------------------------
Map::Map()
{

}

// ------------------------------------------------------------------------------------------------
Map::~Map()
{

}

// ------------------------------------------------------------------------------------------------
// Flow;
// ------------------------------------------------------------------------------------------------
void BattleMap::Startup()
{
	CreatePlayingFields();
	CreateUnitSlotsForPlayingFields();
	CreateFriendlyUsernameDisplay();
	CreateEnemyUsernameDisplay();
	CreateBattleInformationWindows();
	CreateBattleInformationWindowUnitSlots();

	m_background = AABB2::MakeFromMinsMaxs(Vec2(0.0f, 0.0f), Vec2(WIDTH, HEIGHT));

	//m_timer = m_timeForBattlePhase;
	m_actionTimer = m_baseTimeBetweenAction;
}

void BattleMap::StartTimer()
{
	//m_timer = m_timeForBattlePhase;
	m_actionTimer = m_baseTimeBetweenAction;
}

// ------------------------------------------------------------------
void BattleMap::Update(float deltaSeconds_)
{
	m_actionTimer -= deltaSeconds_;
	m_actionTimer = Clamp(m_actionTimer, 0.0f, 999.99f);
}

// ------------------------------------------------------------------
void BattleMap::Render()
{
	// Displaying background;
	std::vector<Vertex_PCU> backgroundVerts;
	AddVertsForAABB2D(backgroundVerts, m_background, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_nameOfCurrentBattleBackground));
	g_theRenderer->DrawVertexArray((int)backgroundVerts.size(), &backgroundVerts[0]);

	// Displaying names;
	std::vector<Vertex_PCU> textVerts;
	std::string friendlyUsername = g_Interface->GetPlayer()->GetPlayerUsername();
	int currentHealth = (int)g_Interface->GetPlayer()->GetPlayerHealth();
	std::string friendlyInformation = Stringf("%s: %d(hp)", friendlyUsername.c_str(), currentHealth);
	std::string enemyUsername = g_Interface->GetEnemy()->GetPlayerUsername();
	int currentHealthEnemy = (int)g_Interface->GetEnemy()->GetPlayerHealth();
	std::string enemyInformation = Stringf("%s: %d(hp)", enemyUsername.c_str(), currentHealthEnemy);
	Vec2 friendlyUsernamePlacement = m_friendlyUsernameDisplay.center - Vec2(1.0f, 0.5f);
	Vec2 enemyUsernamePlacement = m_enemyUsernameDisplay.center - Vec2(1.0f, 0.5f);

	BitMapFont* textFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	textFont->AddVertsForText2D(textVerts, friendlyUsernamePlacement, 1.0f, friendlyInformation, Rgba::WHITE);
	textFont->AddVertsForText2D(textVerts, enemyUsernamePlacement, 1.0f, enemyInformation, Rgba::WHITE);
	g_theRenderer->BindTextureView(0u, textFont->GetTextureView());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);

	// Displaying Battle Information Windows;
	std::vector<Vertex_PCU> battleInformationWindowsVerts;
	AddVertsForAABB2D(battleInformationWindowsVerts, m_friendlyBattleInformationWindow, Rgba::THREEQUART_FFBLUE);
	AddVertsForAABB2D(battleInformationWindowsVerts, m_enemyBattleInformationWindow, Rgba::THREEQUART_FFBLUE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, nullptr);
	g_theRenderer->DrawVertexArray((int)battleInformationWindowsVerts.size(), &battleInformationWindowsVerts[0]);

	/*
	std::vector<Vertex_PCU> battleInformationUnitSlotVerts;
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[0], Rgba::RED);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[0], Rgba::RED);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[1], Rgba::GREEN);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[1], Rgba::GREEN);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[2], Rgba::YELLOW);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[2], Rgba::YELLOW);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[3], Rgba::MAGENTA);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[3], Rgba::MAGENTA);

	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[4], Rgba::BROWN);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[4], Rgba::BROWN);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[5], Rgba::BLACK);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[5], Rgba::BLACK);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[6], Rgba::WHITE);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[6], Rgba::WHITE);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_friendlyBattleInformationUnitSlots[7], Rgba::GRAY);
	AddVertsForAABB2D(battleInformationUnitSlotVerts, m_enemyBattleInformationUnitSlots[7], Rgba::GRAY);

	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, nullptr);
	g_theRenderer->DrawVertexArray((int)battleInformationUnitSlotVerts.size(), &battleInformationUnitSlotVerts[0]);
	*/
}

// ------------------------------------------------------------------
void BattleMap::CreatePlayingFields()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.82f, Map::HEIGHT * 0.26f);
	end = Vec2(Map::WIDTH * 0.88f, Map::HEIGHT * 0.64f);
	m_friendlyField = AABB2::MakeFromMinsMaxs(start, end);

	start = Vec2(Map::WIDTH * 0.12f, Map::HEIGHT * 0.26f);
	end = Vec2(Map::WIDTH * 0.18f, Map::HEIGHT * 0.64f);
	m_enemyField = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void BattleMap::CreateUnitSlotsForPlayingFields()
{
	// Calculate the size of a unit slot; The placement will be determined by the slot;
	float fieldWidth = m_friendlyField.maxs.x - m_friendlyField.mins.x;
	float fieldHeight = m_friendlyField.maxs.y - m_friendlyField.mins.y;
	m_unitSlotDimensions = Vec2(fieldWidth * 0.5f , fieldHeight / ((float)m_unitSlotsPerField / 2.0f));
	float anEigth = 1.0f / (float)m_unitSlotsPerField;

	// Calculate the placement of the AABB2s of each unit slot for both fields;
	// Making my field;
	Vec2 firstRowColumnCenter = Vec2(m_friendlyField.mins.x + (fieldWidth * 0.25f), m_friendlyField.mins.y + fieldHeight * anEigth);
	Vec2 secondColumnSlotCenter = Vec2(firstRowColumnCenter.x + m_unitSlotDimensions.x, firstRowColumnCenter.y);
	for (int i = 0; i < m_unitSlotsPerField / 2; ++i)
	{
		Vec2 currentCenter1 = Vec2(firstRowColumnCenter.x, firstRowColumnCenter.y + (i * m_unitSlotDimensions.y));
		Vec2 currentCenter2 = Vec2(secondColumnSlotCenter.x, secondColumnSlotCenter.y + (i * m_unitSlotDimensions.y));
		AABB2 slot1 = AABB2(currentCenter1, Vec2(m_unitSlotDimensions.x * 0.5f, m_unitSlotDimensions.y * 0.5f));
		AABB2 slot2 = AABB2(currentCenter2, Vec2(m_unitSlotDimensions.x * 0.5f, m_unitSlotDimensions.y * 0.5f));

		m_friendlyFieldSlots.push_back(slot1);
		m_friendlyFieldSlots.push_back(slot2);
	}

	// Making my opponents field;
	Vec2 firstColumnSlotCenter2 = Vec2(m_enemyField.mins.x + (fieldWidth * 0.25f), m_enemyField.mins.y + fieldHeight * anEigth);
	Vec2 secondColumnSlotCenter2 = Vec2(firstColumnSlotCenter2.x + m_unitSlotDimensions.x, firstColumnSlotCenter2.y);
	for (int i = 0; i < m_unitSlotsPerField / 2; ++i)
	{
		Vec2 currentCenter1 = Vec2(firstColumnSlotCenter2.x, firstColumnSlotCenter2.y + (i * m_unitSlotDimensions.y));
		Vec2 currentCenter2 = Vec2(secondColumnSlotCenter2.x, secondColumnSlotCenter2.y + (i * m_unitSlotDimensions.y));
		AABB2 slot1 = AABB2(currentCenter1, Vec2(m_unitSlotDimensions.x * 0.5f, m_unitSlotDimensions.y * 0.5f));
		AABB2 slot2 = AABB2(currentCenter2, Vec2(m_unitSlotDimensions.x * 0.5f, m_unitSlotDimensions.y * 0.5f));

		m_enemyFieldSlots.push_back(slot1);
		m_enemyFieldSlots.push_back(slot2);
	}
}

// ------------------------------------------------------------------
void BattleMap::CreateFriendlyUsernameDisplay()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.75f, Map::HEIGHT * 0.25f);
	end = Vec2(Map::WIDTH * 0.85f, Map::HEIGHT * 0.28f);
	m_friendlyUsernameDisplay = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void BattleMap::CreateEnemyUsernameDisplay()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.15f, Map::HEIGHT * 0.25f);
	end = Vec2(Map::WIDTH * 0.25f, Map::HEIGHT * 0.28f);
	m_enemyUsernameDisplay = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void BattleMap::CreateBattleInformationWindows()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.51f, Map::HEIGHT * 0.01f);
	end = Vec2(Map::WIDTH * 0.99f, Map::HEIGHT * 0.20f);
	m_friendlyBattleInformationWindow = AABB2::MakeFromMinsMaxs(start, end);

	start = Vec2(Map::WIDTH * 0.01f, Map::HEIGHT * 0.01f);
	end = Vec2(Map::WIDTH * 0.49f, Map::HEIGHT * 0.20f);
	m_enemyBattleInformationWindow = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void BattleMap::CreateBattleInformationWindowUnitSlots()
{
	// Calculate the size of a unit slot; The placement will be determined by the slot;
	float fieldWidth = m_friendlyBattleInformationWindow.maxs.x - m_friendlyBattleInformationWindow.mins.x;
	float fieldHeight = m_friendlyBattleInformationWindow.maxs.y - m_friendlyBattleInformationWindow.mins.y;
	m_battleWindowUnitSlotDimensions = Vec2(fieldWidth * 0.45f, fieldHeight / ((float)m_unitSlotsPerField / 1.8f));
	float anEigth = 1.0f / (float)m_unitSlotsPerField;
	
	// Friendly;
	Vec2 firstRowColumnCenter = Vec2(m_friendlyBattleInformationWindow.mins.x + (fieldWidth * 0.025f) + (fieldWidth * 0.25f), m_friendlyBattleInformationWindow.mins.y + fieldHeight * anEigth);
	Vec2 secondColumnSlotCenter = Vec2(firstRowColumnCenter.x + m_battleWindowUnitSlotDimensions.x, firstRowColumnCenter.y);
	for (int i = 0; i < m_unitSlotsPerField / 2; ++i)
	{
		Vec2 currentCenter1 = Vec2(firstRowColumnCenter.x, firstRowColumnCenter.y + (i * m_battleWindowUnitSlotDimensions.y));
		Vec2 currentCenter2 = Vec2(secondColumnSlotCenter.x, secondColumnSlotCenter.y + (i * m_battleWindowUnitSlotDimensions.y));
		AABB2 slot1 = AABB2(currentCenter1, Vec2(m_battleWindowUnitSlotDimensions.x * 0.5f, m_battleWindowUnitSlotDimensions.y * 0.5f));
		AABB2 slot2 = AABB2(currentCenter2, Vec2(m_battleWindowUnitSlotDimensions.x * 0.5f, m_battleWindowUnitSlotDimensions.y * 0.5f));

		m_friendlyBattleInformationUnitSlots.push_back(slot1);
		m_friendlyBattleInformationUnitSlots.push_back(slot2);
	}

	//Enemy;
	Vec2 firstColumnSlotCenter2 = Vec2(m_enemyBattleInformationWindow.mins.x + (fieldWidth * 0.025f) + (fieldWidth * 0.25f), m_enemyBattleInformationWindow.mins.y + fieldHeight * anEigth);
	Vec2 secondColumnSlotCenter2 = Vec2(firstColumnSlotCenter2.x + m_battleWindowUnitSlotDimensions.x, firstColumnSlotCenter2.y);
	for (int i = 0; i < m_unitSlotsPerField / 2; ++i)
	{
		Vec2 currentCenter1 = Vec2(firstColumnSlotCenter2.x, firstColumnSlotCenter2.y + (i * m_battleWindowUnitSlotDimensions.y));
		Vec2 currentCenter2 = Vec2(secondColumnSlotCenter2.x, secondColumnSlotCenter2.y + (i * m_battleWindowUnitSlotDimensions.y));
		AABB2 slot1 = AABB2(currentCenter1, Vec2(m_battleWindowUnitSlotDimensions.x * 0.5f, m_battleWindowUnitSlotDimensions.y * 0.5f));
		AABB2 slot2 = AABB2(currentCenter2, Vec2(m_battleWindowUnitSlotDimensions.x * 0.5f, m_battleWindowUnitSlotDimensions.y * 0.5f));

		m_enemyBattleInformationUnitSlots.push_back(slot1);
		m_enemyBattleInformationUnitSlots.push_back(slot2);
	}
}

// ------------------------------------------------------------------
int BattleMap::GetSlotForPosition(const Vec2& position_)
{
	//-----------//
	// | 6 | 7 | //
	// | 4 | 5 | //
	// | 2 | 3 | //
	// | 0 | 1 | //
	//-----------//

	for(int slotIndex = 0; slotIndex < m_friendlyFieldSlots.size(); ++slotIndex)
	{
		if(m_friendlyFieldSlots[slotIndex].IsPointInside(position_))
		{
			return slotIndex;
		}
	}

	return -1;
}

// ------------------------------------------------------------------
Vec2 BattleMap::GetCenterPositionOfSlotInMyField(int slot_)
{
	return m_friendlyFieldSlots[slot_].center;
}

// ------------------------------------------------------------------
Vec2 BattleMap::GetCenterPositionOfSlotInEnemyField(int slot_)
{
	return m_enemyFieldSlots[slot_].center;
}

// ------------------------------------------------------------------
void BattleMap::ResetActionTimer()
{
	m_actionTimer = m_baseTimeBetweenAction;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void PurchaseMap::Startup()
{
	CreateOurUnitsField();
	CreateOurUnitsSlots();
	CreateMarketField();
	CreateMarketSlots();
	CreateHandField();
	CreateHandSlots();
	CreateRerollButton();
	CreateFreezeButton();
	CreateTimerDisplay();
	CreateGoldDisplay();
	CreateGoldSlots();
	CreateUsernameDisplay();

	// This isn't done yet;
	CreateNextOpponentUnitLevelsArea();

	m_background = AABB2::MakeFromMinsMaxs(Vec2(0.0f, 0.0f), Vec2(WIDTH, HEIGHT));

	m_timer = m_timeForPurchasePhase;
}

// ------------------------------------------------------------------
void PurchaseMap::StartTimer()
{
	m_timer = m_timeForPurchasePhase;
}

// ------------------------------------------------------------------
void PurchaseMap::Update(float deltaSeconds_)
{
	m_timer -= deltaSeconds_;
	m_timer = Clamp(m_timer, 0.0f, m_timeForPurchasePhase);

	if(m_timer > 0.0f)
	{
		if (g_theInputSystem->WasLeftMouseClickReleased())
		{
			Player* player = g_Interface->GetPlayer();

			Vec2 mouseClick = g_theInputSystem->GetGameMousePosition(Vec2(Map::WIDTH, Map::HEIGHT));

			bool mouseClickConsumed = false;
			if(m_rightMouseFocus)
			{
				mouseClickConsumed = MouseClickForPlacingUnitInSlot(player, mouseClick);


			}
			else
			{
				if (!mouseClickConsumed)
				{
					mouseClickConsumed = MouseClickForMarketplace(player, mouseClick);
				}
				if (!mouseClickConsumed)
				{
					mouseClickConsumed = MouseClickForReroll(player, mouseClick);
				}
				if (!mouseClickConsumed)
				{
					mouseClickConsumed = MouseClickForLock(mouseClick);
				}
				if (!mouseClickConsumed)
				{
					mouseClickConsumed = MouseClickForSellingCardInHand(player, mouseClick);
				}
			}
		}
		else if(g_theInputSystem->WasRightMouseClickReleased())
		{
			Vec2 mouseClick = g_theInputSystem->GetGameMousePosition(Vec2(Map::WIDTH, Map::HEIGHT));

			bool mouseClickConsumed = false;
			if(!mouseClickConsumed)
			{
				mouseClickConsumed = MouseClickForSelectingUnitFromHand(mouseClick);
			}
		}
	}
	else if(m_timer == 0.0f && !g_Interface->client().IsMessageSentToServerForPurchasePhaseComplete())
	{
		g_Interface->client().SetMessageSentToServerForPurchasePhaseComplete(true);
		g_Interface->client().SendPurchasePhaseCompleteToServer();
	}
}

// ------------------------------------------------------------------
void PurchaseMap::Render()
{
	// Displaying background;
	std::vector<Vertex_PCU> backgroundVerts;
	AddVertsForAABB2D(backgroundVerts, m_background, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Images/Backgrounds/Library.png"));
	g_theRenderer->DrawVertexArray((int)backgroundVerts.size(), &backgroundVerts[0]);

	// Reroll;
	std::vector<Vertex_PCU> rerollVerts;
	AddVertsForAABB2D(rerollVerts, m_rerollButton, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Reroll.png"));
	g_theRenderer->DrawVertexArray((int)rerollVerts.size(), &rerollVerts[0]);

	// Reroll;
	std::vector<Vertex_PCU> freezeVerts;
	AddVertsForAABB2D(freezeVerts, m_freezeButton, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Freeze.png"));
	g_theRenderer->DrawVertexArray((int)freezeVerts.size(), &freezeVerts[0]);

	// Timer;
	std::vector<Vertex_PCU> timerVerts;
	AddVertsForAABB2D(timerVerts, m_timerDisplay, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/TimerDisplay.png"));
	g_theRenderer->DrawVertexArray((int)timerVerts.size(), &timerVerts[0]);

	// Timer Text;
	std::vector<Vertex_PCU> textVerts;
	Vec2 center;
	if(m_timer < 10.0f)
	{
		center = m_timerDisplay.center - Vec2(0.5f, 0.5f);
	}
	else
	{
		center = m_timerDisplay.center - Vec2(1.0f, 0.5f);
	}
	
	std::string time = Stringf("%d", (int)m_timer);
	
	std::string username = g_Interface->GetPlayer()->GetPlayerUsername();
	int currentHealth = g_Interface->GetPlayer()->GetPlayerHealth();
	std::string information = Stringf("%s: %d(hp)", username.c_str(), currentHealth);
	Vec2 usernamePlacement = m_usernameDisplay.center - Vec2(1.0f, 0.5f);


	BitMapFont* textFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	textFont->AddVertsForText2D(textVerts, center, 1.0f, time, Rgba::WHITE);
	textFont->AddVertsForText2D(textVerts, usernamePlacement, 1.0f, information, Rgba::WHITE);
	g_theRenderer->BindTextureView(0u, textFont->GetTextureView());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);

	// Gold;
	std::vector<Vertex_PCU> goldVertsBackground;
	Player* player = g_Interface->GetPlayer();
	int goldAmount = player->GetActualGold();
	if(goldAmount > 0)
	{
		for (int x = 0; x < goldAmount; ++x)
		{
			AddVertsForAABB2D(goldVertsBackground, m_ourGoldSlots[x], Rgba::WHITE);
		}
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/GoldIconBackground.png"));
		g_theRenderer->DrawVertexArray((int)goldVertsBackground.size(), &goldVertsBackground[0]);
	}	

	std::vector<Vertex_PCU> goldVerts;
	goldAmount = player->GetActualGold();
	if(goldAmount > 0)
	{
		for (int x = 0; x < goldAmount; ++x)
		{
			AddVertsForAABB2D(goldVerts, m_ourGoldSlots[x], Rgba::WHITE);
		}
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/GoldIcon.png"));
		g_theRenderer->DrawVertexArray((int)goldVerts.size(), &goldVerts[0]);
	}

	// Add "glow" around card in hand if we have right click focus;
	if(m_rightMouseFocus)
	{
		AABB2 cardSlotToHighlight = m_ourHandSlots[m_cardSlotInHandToBePlaced];
		std::vector<Vertex_PCU> highlightVerts;

		Line cardTopBorder = Line(cardSlotToHighlight.GetTopLeft(), cardSlotToHighlight.GetTopRight(), 1.0f);
		Line cardRightBorder = Line(cardSlotToHighlight.GetTopRight(), cardSlotToHighlight.GetBottomRight(), 1.0f);
		Line cardBottomBorder = Line(cardSlotToHighlight.GetBottomRight(), cardSlotToHighlight.GetBottomLeft(), 1.0f);
		Line cardLeftBorder = Line(cardSlotToHighlight.GetBottomLeft(), cardSlotToHighlight.GetTopLeft(), 1.0f);

		AddVertsForLine2D(highlightVerts, cardTopBorder, Rgba::YELLOW);
		AddVertsForLine2D(highlightVerts, cardRightBorder, Rgba::YELLOW);
		AddVertsForLine2D(highlightVerts, cardBottomBorder, Rgba::YELLOW);
		AddVertsForLine2D(highlightVerts, cardLeftBorder, Rgba::YELLOW);

		for(int i = 0; i < m_ourUnitSlots.size(); ++i)
		{
			AABB2 currentUnitSlotAABB2 = m_ourUnitSlots[i];

			Line slotTopBorder		= Line(currentUnitSlotAABB2.GetTopLeft(), currentUnitSlotAABB2.GetTopRight(), 0.25f);
			Line slotRightBorder	= Line(currentUnitSlotAABB2.GetTopRight(), currentUnitSlotAABB2.GetBottomRight(), 0.25f);
			Line slotBottomBorder	= Line(currentUnitSlotAABB2.GetBottomRight(), currentUnitSlotAABB2.GetBottomLeft(), 0.25f);
			Line slotLeftBorder		= Line(currentUnitSlotAABB2.GetBottomLeft(), currentUnitSlotAABB2.GetTopLeft(), 0.25f);

			AddVertsForLine2D(highlightVerts, slotTopBorder, Rgba::YELLOW);
			AddVertsForLine2D(highlightVerts, slotRightBorder, Rgba::YELLOW);
			AddVertsForLine2D(highlightVerts, slotBottomBorder, Rgba::YELLOW);
			AddVertsForLine2D(highlightVerts, slotLeftBorder, Rgba::YELLOW);
		}

		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, nullptr);
		g_theRenderer->DrawVertexArray((int)highlightVerts.size(), &highlightVerts[0]);
	}
}

// ------------------------------------------------------------------
void PurchaseMap::CreateOurUnitsField()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.82f, Map::HEIGHT * 0.06f);
	end = Vec2(Map::WIDTH * 0.88f, Map::HEIGHT * 0.44f);
	m_ourUnitsField = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateOurUnitsSlots()
{
	// Calculate the size of a unit slot; The placement will be determined by the slot;
	float fieldWidth = m_ourUnitsField.maxs.x - m_ourUnitsField.mins.x;
	float fieldHeight = m_ourUnitsField.maxs.y - m_ourUnitsField.mins.y;
	m_unitSlotDimensions = Vec2(fieldWidth * 0.5f, fieldHeight / ((float)m_unitSlotsPerField / 2.0f));
	float anEigth = 1.0f / (float)m_unitSlotsPerField;


	// Calculate the placement of the AABB2s of each unit slot for both fields;
	// Making my field;
	Vec2 firstRowColumnCenter = Vec2(m_ourUnitsField.mins.x + (fieldWidth * 0.25f), m_ourUnitsField.mins.y + fieldHeight * anEigth);
	Vec2 secondColumnSlotCenter = Vec2(firstRowColumnCenter.x + m_unitSlotDimensions.x, firstRowColumnCenter.y);
	for (int i = 0; i < m_unitSlotsPerField / 2; ++i)
	{
		Vec2 currentCenter1 = Vec2(firstRowColumnCenter.x, firstRowColumnCenter.y + (i * m_unitSlotDimensions.y));
		Vec2 currentCenter2 = Vec2(secondColumnSlotCenter.x, secondColumnSlotCenter.y + (i * m_unitSlotDimensions.y));
		AABB2 slot1 = AABB2(currentCenter1, Vec2(m_unitSlotDimensions.x * 0.5f, m_unitSlotDimensions.y * 0.5f));
		AABB2 slot2 = AABB2(currentCenter2, Vec2(m_unitSlotDimensions.x * 0.5f, m_unitSlotDimensions.y * 0.5f));

		m_ourUnitSlots.push_back(slot1);
		m_ourUnitSlots.push_back(slot2);
	}
}

// ------------------------------------------------------------------
void PurchaseMap::CreateMarketField()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.35f, Map::HEIGHT * 0.6f);
	end = Vec2(Map::WIDTH * 0.65f, Map::HEIGHT * 0.8f);
	m_ourMarketField = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateMarketSlots()
{
	// Calculate the size of a slot; The placement will be determined by the slot;
	float fieldWidth = m_ourMarketField.maxs.x - m_ourMarketField.mins.x;
	float fieldHeight = m_ourMarketField.maxs.y - m_ourMarketField.mins.y;
	m_marketSlotDimensions = Vec2(fieldWidth / 3.0f, fieldHeight);

	// Calculate the placement of the AABB2s of each unit slot for both fields;
	// Making my field;
	Vec2 firstRowColumnCenter = Vec2(m_ourMarketField.mins.x + (m_marketSlotDimensions.x / 2), m_ourMarketField.mins.y + (m_marketSlotDimensions.y / 2));
	for (int i = 0; i < 3; ++i)
	{
		Vec2 currentCenter = Vec2(firstRowColumnCenter.x + (i * m_marketSlotDimensions.x), firstRowColumnCenter.y);
		AABB2 slot1 = AABB2(currentCenter, Vec2(m_marketSlotDimensions.x * 0.5f, m_marketSlotDimensions.y * 0.5f));

		m_ourMarketSlots.push_back(slot1);
	}
}

// ------------------------------------------------------------------
void PurchaseMap::CreateHandField()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.35f, 0.0f);
	end = Vec2(Map::WIDTH * 0.65f, Map::HEIGHT * 0.2f);
	m_ourHandField = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateHandSlots()
{
	// Calculate the size of a slot; The placement will be determined by the slot;
	float fieldWidth = m_ourHandField.maxs.x - m_ourHandField.mins.x;
	float fieldHeight = m_ourHandField.maxs.y - m_ourHandField.mins.y;
	m_handSlotDimensions = Vec2(fieldWidth / 3.0f, fieldHeight);

	// Making my hand field, we only have 3 cards in hand;
	Vec2 firstRowColumnCenter = Vec2(m_ourHandField.mins.x + (m_handSlotDimensions.x / 2), m_ourHandField.mins.y + (m_handSlotDimensions.y / 2));
	for (int i = 0; i < 3; ++i)
	{
		Vec2 currentCenter = Vec2(firstRowColumnCenter.x + (i * m_handSlotDimensions.x), firstRowColumnCenter.y);
		AABB2 slot1 = AABB2(currentCenter, Vec2(m_handSlotDimensions.x * 0.5f, m_handSlotDimensions.y * 0.5f));

		m_ourHandSlots.push_back(slot1);
	}
}

// ------------------------------------------------------------------
void PurchaseMap::CreateRerollButton()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.68f, Map::HEIGHT * 0.65f);
	end = Vec2(Map::WIDTH * 0.73f, Map::HEIGHT * 0.75f);
	m_rerollButton = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateFreezeButton()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.74f, Map::HEIGHT * 0.65f);
	end = Vec2(Map::WIDTH * 0.79f, Map::HEIGHT * 0.75f);
	m_freezeButton = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateTimerDisplay()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.80f, Map::HEIGHT * 0.65f);
	end = Vec2(Map::WIDTH * 0.85f, Map::HEIGHT * 0.75f);
	m_timerDisplay = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateGoldDisplay()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.05f, Map::HEIGHT * 0.05f);
	end = Vec2(Map::WIDTH * 0.24f, Map::HEIGHT * 0.08f);
	m_goldDisplay = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateGoldSlots()
{
	// Calculate the size of a slot; The placement will be determined by the slot;
	float fieldWidth = m_goldDisplay.maxs.x - m_goldDisplay.mins.x;
	float fieldHeight = m_goldDisplay.maxs.y - m_goldDisplay.mins.y;
	m_goldSlotDimensions = Vec2(fieldWidth / 10.0f, fieldHeight);

	Vec2 firstRowColumnCenter = Vec2(m_goldDisplay.mins.x + (m_goldSlotDimensions.x / 2), m_goldDisplay.mins.y + (m_goldSlotDimensions.y / 2));
	for (int i = 0; i < 10; ++i)
	{
		Vec2 currentCenter = Vec2(firstRowColumnCenter.x + (i * m_goldSlotDimensions.x), firstRowColumnCenter.y);
		AABB2 slot1 = AABB2(currentCenter, Vec2(m_goldSlotDimensions.x * 0.5f, m_goldSlotDimensions.y * 0.5f));

		m_ourGoldSlots.push_back(slot1);
	}
}

// ------------------------------------------------------------------
void PurchaseMap::CreateUsernameDisplay()
{
	Vec2 start;
	Vec2 end;

	start = Vec2(Map::WIDTH * 0.75f, Map::HEIGHT * 0.05f);
	end = Vec2(Map::WIDTH * 0.85f, Map::HEIGHT * 0.08f);
	m_usernameDisplay = AABB2::MakeFromMinsMaxs(start, end);
}

// ------------------------------------------------------------------
void PurchaseMap::CreateNextOpponentUnitLevelsArea()
{

}

// ------------------------------------------------------------------
int PurchaseMap::GetUnitSlotForPosition(const Vec2& position_)
{
	//-----------//
	// | 6 | 7 | //
	// | 4 | 5 | //
	// | 2 | 3 | //
	// | 0 | 1 | //
	//-----------//

	// We need to think about what we are clicking on, my field vs. their field;
	// I don't think we will ever actually need to click their field;
	for (int slotIndex = 0; slotIndex < m_ourUnitSlots.size(); ++slotIndex)
	{
		if (m_ourUnitSlots[slotIndex].IsPointInside(position_))
		{
			return slotIndex;
		}
	}

	return -1;
}

// ------------------------------------------------------------------
Vec2 PurchaseMap::GetCenterPositionOfSlotInMyField(int slot_)
{
	return m_ourUnitSlots[slot_].center;
}

int PurchaseMap::GetCardMarketSlotForPosition(const Vec2& position_)
{
	//---------------//
	// | 0 | 1 | 2 | //
	//---------------//

	for (int slotIndex = 0; slotIndex < m_ourMarketSlots.size(); ++slotIndex)
	{
		if (m_ourMarketSlots[slotIndex].IsPointInside(position_))
		{
			return slotIndex;
		}
	}

	return -1;
}

// ------------------------------------------------------------------
Vec2 PurchaseMap::GetCenterPositionOfCardSlotInMyMarket(int slot_)
{
	return m_ourMarketSlots[slot_].center;
}

// ------------------------------------------------------------------
int PurchaseMap::GetCardHandSlotForPosition(const Vec2& position_)
{
	//---------------//
	// | 0 | 1 | 2 | //
	//---------------//

	for (int slotIndex = 0; slotIndex < m_ourHandSlots.size(); ++slotIndex)
	{
		if (m_ourHandSlots[slotIndex].IsPointInside(position_))
		{
			return slotIndex;
		}
	}

	return -1;
}

// ------------------------------------------------------------------
Vec2 PurchaseMap::GetCenterPositionOfCardSlotInMyHand(int slot_)
{
	return m_ourHandSlots[slot_].center;
}

// ------------------------------------------------------------------
bool PurchaseMap::DidClickReroll(const Vec2& position_)
{
	return m_rerollButton.IsPointInside(position_);
}

// ------------------------------------------------------------------
bool PurchaseMap::DidClickLock(const Vec2& position_)
{
	return m_freezeButton.IsPointInside(position_);
}

// ------------------------------------------------------------------
bool PurchaseMap::MouseClickForMarketplace(Player* player_, const Vec2& mouseClick_)
{
	int marketClickSlot = GetCardMarketSlotForPosition(mouseClick_);
	if (marketClickSlot > -1 && player_->GetActualGold() > 0)
	{
		Cards cardsInMarketplace = g_Interface->query().GetCards(CardInMarketplace());
		Cards cardsInHand = g_Interface->query().GetCards(CardInHand());
		int cardCount = (int)cardsInHand.size();

		for (Card* card : cardsInMarketplace)
		{
			if (card->m_slotID == marketClickSlot)
			{
				if (cardCount < player_->GetMaxHandCount())
				{
					player_->ChangeActualGoldAmount(-1);
					player_->ClampActualGoldAmount();
					g_Interface->client().SendPurchasedCardFromMarketplaceToServer(card->m_cardID, player_->GetPlayerID());
					return true;
				}
			}
		}
	}

	return false;
}

// ------------------------------------------------------------------
bool PurchaseMap::MouseClickForReroll(Player* player_, const Vec2& mouseClick_)
{
	if(DidClickReroll(mouseClick_) && player_->GetActualGold() > 0)
	{
		g_Interface->client().RollMarketplaceCards();
		player_->ChangeActualGoldAmount(-1);
		player_->ClampActualGoldAmount();
		return true;
	}

	return false;
}

// ------------------------------------------------------------------
bool PurchaseMap::MouseClickForLock(const Vec2& mouseClick_)
{
	if (DidClickLock(mouseClick_))
	{
		g_Interface->client().ToggleMarketplaceLock();
		return true;
	}

	return false;
}

// ------------------------------------------------------------------
bool PurchaseMap::MouseClickForSellingCardInHand(Player* player_, const Vec2& mouseClick_)
{
	int handClickSlot = GetCardHandSlotForPosition(mouseClick_);
	if (handClickSlot > -1)
	{
		Cards cardsInHand = g_Interface->query().GetCards(CardInHand());

		for (Card* card : cardsInHand)
		{
			if (card->m_slotID == handClickSlot)
			{
				g_Interface->client().SendSoldCardFromHandToServer(card->m_cardID, player_->GetPlayerID());
				player_->ChangeActualGoldAmount(1);
				player_->ClampActualGoldAmount();
				return true;
			}
		}
	}

	return false;
}

// ------------------------------------------------------------------
bool PurchaseMap::MouseClickForPlacingUnitInSlot(Player* player_, const Vec2& mouseClick_)
{
	int unitClickSlot = GetUnitSlotForPosition(mouseClick_);
	if (unitClickSlot > -1)
	{
		g_Interface->client().SendUnitCreatedAtFromCardInHandToServer(m_cardIDInHandToBePlaced, unitClickSlot, player_->GetPlayerID());

		m_cardIDInHandToBePlaced = 0u;
		m_cardSlotInHandToBePlaced = -1;
		m_rightMouseFocus = false;
		return true;
	}

	m_cardIDInHandToBePlaced = 0u;
	m_cardSlotInHandToBePlaced = -1;
	m_rightMouseFocus = false;

	return false;
}

// ------------------------------------------------------------------
bool PurchaseMap::MouseClickForSelectingUnitFromHand(const Vec2& mouseClick_)
{
	int handClickSlot = GetCardHandSlotForPosition(mouseClick_);
	if (handClickSlot > -1)
	{
		Cards cardsInHand = g_Interface->query().GetCards(CardInHand());

		for (Card* card : cardsInHand)
		{
			if (card->m_slotID == handClickSlot)
			{
				m_cardIDInHandToBePlaced = card->m_cardID;
				m_cardSlotInHandToBePlaced = handClickSlot;
				m_rightMouseFocus = true;
				return true;
			}
		}
	}

	m_cardIDInHandToBePlaced = 0u;
	m_cardSlotInHandToBePlaced = -1;
	m_rightMouseFocus = false;
	return false;
}
