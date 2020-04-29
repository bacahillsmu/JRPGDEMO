#define WIN32_LEAN_AND_MEAN // Needed to actually be at the top of the file for RakNet;

#include "Game/Framework/Interface.hpp"

// ----------------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"

// ----------------------------------------------------------------------------
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

// ----------------------------------------------------------------------------
#include "Game/Framework/App.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/Player.hpp"
#include "Game/Units/UnitFilters.hpp"
#include "Game/Cards/CardFilters.hpp"
#include "Game/Gameplay/PlayerFilters.hpp"
#include "Game/Cards/CardDefinition.hpp"
#include "Game/Ability/Ability.hpp"

// Third Party Includes ----------------------------------------------------------------------------
#include "ThirdParty/RakNet/RakNetInterface.hpp"

#include <ctime>

// ----------------------------------------------------------------------------
Interface* g_Interface = nullptr;

// ----------------------------------------------------------------------------
// Action;
// ----------------------------------------------------------------------------
Action::Action(Units& units_, Cards& cards_)
	: m_units(units_)
	, m_cards(cards_)
{
}

// ----------------------------------------------------------------------------
void Action::AutoAttack(const Unit* attackingUnit_, const Unit* attackedUnit_)
{
	attackingUnit_;
	attackedUnit_;
}

// ----------------------------------------------------------------------------
// Match;
// ----------------------------------------------------------------------------
Match::Match(Units& units_, Cards& cards_)
	: m_units(units_)
	, m_cards(cards_)
{
}

// ----------------------------------------------------------------------------
Match::~Match()
{
	DELETE_POINTER(m_battleMap);
	DELETE_POINTER(m_purchaseMap);

	for (auto it = m_unitSpriteSheets.cbegin(); it != m_unitSpriteSheets.cend() /* not hoisted */; /* no increment */)
	{
		m_unitSpriteSheets.erase(it++);
	}
	for (auto it = m_abilitySpriteSheets.cbegin(); it != m_abilitySpriteSheets.cend() /* not hoisted */; /* no increment */)
	{
		m_abilitySpriteSheets.erase(it++);
	}
	DELETE_POINTER(m_jobIcons);
}

// ----------------------------------------------------------------------------
void Match::Init()
{
	m_battleMap = new BattleMap();
	m_purchaseMap = new PurchaseMap();
}

// ----------------------------------------------------------------------------
void Match::Startup()
{
	// Create the various fields, slots, buttons for each map type;
	m_battleMap->Startup();
	m_purchaseMap->Startup();

	// Create the sprite sheets from .png files, store these sprite sheets in a map for easy access;
	CreateSpriteSheets();
}

// ----------------------------------------------------------------------------
void Match::Update()
{

}

// ----------------------------------------------------------------------------
void Match::Render()
{

}

// ----------------------------------------------------------------------------
void Match::CreateSpriteSheets()
{
	TextureView* spriteTexture = nullptr;

	// Let the match store the job icons for the cards;
	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/JobIcons.png");
	m_jobIcons = new SpriteSheet(spriteTexture, IntVec2((int)JobType::JOB_COUNT, 1));

	// Let the match store each unit's sprite sheet;
	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Knight.png");
	m_unitSpriteSheets["Knight"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Archer.png");
	m_unitSpriteSheets["Archer"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Warrior.png");
	m_unitSpriteSheets["Warrior"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Paladin.png");
	m_unitSpriteSheets["Paladin"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Dragoon.png");
	m_unitSpriteSheets["Dragoon"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Blackmage.png");
	m_unitSpriteSheets["Blackmage"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Whitemage.png");
	m_unitSpriteSheets["Whitemage"] = new SpriteSheet(spriteTexture, IntVec2(3, 4));

	// Let the match store each ability's sprite sheet;
	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Abilities/Fire.png");
	m_abilitySpriteSheets["Fire"] = new SpriteSheet(spriteTexture, IntVec2(2, 1));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Abilities/Cure.png");
	m_abilitySpriteSheets["Cure"] = new SpriteSheet(spriteTexture, IntVec2(2, 1));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Abilities/Burn.png");
	m_abilitySpriteSheets["Burn"] = new SpriteSheet(spriteTexture, IntVec2(3, 2));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Abilities/Shimmer.png");
	m_abilitySpriteSheets["Shimmer"] = new SpriteSheet(spriteTexture, IntVec2(3, 3));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Effects/PhysicalHit.png");
	m_abilitySpriteSheets["PhysicalHit"] = new SpriteSheet(spriteTexture, IntVec2(4, 1));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/BuffEffects/EnrageBuff.png");
	m_abilitySpriteSheets["EnrageBuff"] = new SpriteSheet(spriteTexture, IntVec2(2, 1));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/BuffEffects/Enrage.png");
	m_abilitySpriteSheets["Enrage"] = new SpriteSheet(spriteTexture, IntVec2(1, 1));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Abilities/Bleed.png");
	m_abilitySpriteSheets["Bleed"] = new SpriteSheet(spriteTexture, IntVec2(3, 2));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/BuffEffects/Shield.png");
	m_abilitySpriteSheets["Shield"] = new SpriteSheet(spriteTexture, IntVec2(3, 2));

	spriteTexture = g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/BuffEffects/ShieldProc.png");
	m_abilitySpriteSheets["ShieldProc"] = new SpriteSheet(spriteTexture, IntVec2(3, 2));
}

// ----------------------------------------------------------------------------
void Match::SwitchPhases()
{
	switch(g_theRakNetInterface->m_connection)
	{
		case ConnectionType::SERVER:
		{
			g_Interface->server().SwitchPhases();
			break;
		}

		case ConnectionType::CLIENT:
		{
			g_Interface->client().SwitchPhases();
			break;
		}

		default:
		{
			ERROR_AND_DIE("Trying to switch phases with an unknown Connection Type!");
			break;
		}
	}
}

// ----------------------------------------------------------------------------
// void Match::GetPlayersUnits()
// {
// 	g_Interface->GetPlayer()->GetMyUnits();
// }

// ----------------------------------------------------------------------------
// void Match::RefreshPlayersCards()
// {
// 	g_Interface->GetPlayer()->GetMyCards();
// }

// ----------------------------------------------------------------------------
// void Match::RemoveCardSlotInHand(int cardSlot_)
// {
// 	for (auto itr = m_cards.begin(); itr != m_cards.end();)
// 	{
// 		if ((*itr)->m_cardArea == CardArea::HAND && (*itr)->m_playerID == 0 && (*itr)->m_slotID == cardSlot_)
// 		{
// 			delete *itr;
// 			itr = m_cards.erase(itr);
// 		}
// 		else
// 		{
// 			itr++;
// 		}
// 	}
// }

// ----------------------------------------------------------------------------
// Deck;
// ----------------------------------------------------------------------------
void Match::CreateCommunalDeck()
{
	Card* card = nullptr;

	CardType cardType = CardType::BLACKMAGE;
	for(int i = 0; i < blackmageCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}

	cardType = CardType::ARCHER;
	for (int i = 0; i < archerCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}

	cardType = CardType::DRAGOON;
	for (int i = 0; i < dragoonCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}

	cardType = CardType::PALADIN;
	for (int i = 0; i < paladinCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}

	cardType = CardType::WHITEMAGE;
	for (int i = 0; i < whitemageCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}

	cardType = CardType::WARRIOR;
	for (int i = 0; i < warriorCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}

	cardType = CardType::KNIGHT;
	for (int i = 0; i < knightCount; ++i)
	{
		card = new Card(cardType);
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardID = g_Interface->GetCardIDAndIncrementCounter();
		card->m_currentSpriteDefinition = m_jobIcons->GetSpriteDefinition((int)cardType);
		card->m_cardArea = CardArea::DECK;
		m_cards.push_back(card);
	}
}

// ----------------------------------------------------------------------------
BattleMap*& Match::GetBattleMap()
{
	return m_battleMap;
}

// ----------------------------------------------------------------------------
PurchaseMap*& Match::GetPurchaseMap()
{
	return m_purchaseMap;
}

// ----------------------------------------------------------------------------
// Query;
// ----------------------------------------------------------------------------
Query::Query(Units& units_, Cards& cards_, Players& players_)
	: m_units(units_)
	, m_cards(cards_)
	, m_players(players_)
{
}

// ----------------------------------------------------------------------------
int Query::GetUnitCount(const UnitFilter& filter_) const
{
	int count = 0;

	for (Unit*& unit : m_units)
	{
		if (filter_(unit))
		{
			count++;
		}
	}

	return count;
}

// ----------------------------------------------------------------------------
int Query::GetCardCount(int playerID_, const CardFilter& filter_) const
{
	int count = 0;

	for (Card*& card : m_cards)
	{
		if (filter_(card) && card->m_playerID == playerID_)
		{
			count++;
		}
	}

	return count;
}

// ----------------------------------------------------------------------------
int Query::GetCardCount(const CardFilter& filter_) const
{
	int count = 0;

	for (Card* card : m_cards)
	{
		if (filter_(card))
		{
			count++;
		}
	}

	return count;
}

// ----------------------------------------------------------------------------
Units Query::GetUnits(const UnitFilter& filter_) const
{
	Units units;

	for (Unit* unit : m_units)
	{
		if (filter_(unit))
		{
			units.push_back(unit);
		}
	}

	return units;
}

// ----------------------------------------------------------------------------
Units Query::GetUnits(Units& unitsToFilter_, const UnitFilter& filter_) 
{
	Units units;

	for (Unit* unit : unitsToFilter_)
	{
		if (filter_(unit))
		{
			units.push_back(unit);
		}
	}

	return units;
}

// ----------------------------------------------------------------------------
Units Query::GetLeastDamagedUnitsExcludingDeadUnits(Units& unitsToFilter_) const
{
	Units leastDamagedUnits;
	float healthPercentage = 0.0f;

	// Figure out what is the highest health percentage of the units;
	for(Unit* unit : unitsToFilter_)
	{
		if (unit->m_health <= 0)
		{
			continue;
		}

		float currentHealth = (float)unit->m_health;
		float maxHealth = (float)unit->m_unitDefinition->m_health;
		float currentHealthPercentage = currentHealth / maxHealth;

		if(currentHealthPercentage > healthPercentage)
		{
			healthPercentage = currentHealthPercentage;
		}
	}

	// Based on the highest health percentage, grab all units who are plus/minus of 1% of that;
	float l_healthPercentage = healthPercentage - 0.2f;
	float u_healthPercentage = healthPercentage + 0.2f;

	for(Unit* unit : unitsToFilter_)
	{
		float currentHealth = (float)unit->m_health;
		float maxHealth = (float)unit->m_unitDefinition->m_health;
		float currentHealthPercentage = currentHealth / maxHealth;

		// CurrentHealthPercentage is between the lower and upper bounds;
		if(currentHealthPercentage >= l_healthPercentage 
		&& currentHealthPercentage <= u_healthPercentage)
		{
			leastDamagedUnits.push_back(unit);
		}
	}

	return leastDamagedUnits;
}

// ----------------------------------------------------------------------------
Units Query::GetMostDamagedUnitsExcludingDeadUnits(Units& unitsToFilter_) const
{
	Units mostDamagedUnits;
	float healthPercentage = 100.0f;

	// Figure out what is the lowest health percentage of the units;
	for (Unit*& unit : unitsToFilter_)
	{
		if(unit->m_health <= 0)
		{
			continue;
		}

		float currentHealth = (float)unit->m_health;
		float maxHealth = (float)unit->m_unitDefinition->m_health;
		float currentHealthPercentage = currentHealth / maxHealth;

		if (currentHealthPercentage < healthPercentage)
		{
			healthPercentage = currentHealthPercentage;
		}
	}

	// Based on the lowest health percentage, grab all units who are plus/minus of 1% of that;
	float l_healthPercentage = healthPercentage - 0.2f;
	float u_healthPercentage = healthPercentage + 0.2f;

	for (Unit*& unit : unitsToFilter_)
	{
		float currentHealth = (float)unit->m_health;
		float maxHealth = (float)unit->m_unitDefinition->m_health;
		float currentHealthPercentage = currentHealth / maxHealth;

		// CurrentHealthPercentage is between the lower and upper bounds;
		if(currentHealthPercentage >= l_healthPercentage
		&& currentHealthPercentage <= u_healthPercentage)
		{
			mostDamagedUnits.push_back(unit);
		}
	}

	return mostDamagedUnits;
}

// ----------------------------------------------------------------------------
Cards Query::GetCards(const CardFilter& filter_) const
{
	Cards cards;

	for(Card*& card : m_cards)
	{
		if (filter_(card))
		{
			cards.push_back(card);
		}
	}

	return cards;
}

// ----------------------------------------------------------------------------
Cards Query::GetCards(Cards& cardsToFilter_, const CardFilter& filter_) const
{
	Cards cards;

	for (Card*& card : cardsToFilter_)
	{
		if (filter_(card))
		{
			cards.push_back(card);
		}
	}

	return cards;
}

// ----------------------------------------------------------------------------
Card* Query::GetCard(const CardFilter& filter_)
{
	for (int i = 0; i < (int)m_cards.size(); ++i)
	{
		if (filter_(m_cards[i]))
		{
			return m_cards[i];
		}
	}

	ERROR_AND_DIE("Looked for a player with a bad filter.");
}

// ----------------------------------------------------------------------------
Players Query::GetPlayers(const PlayerFilter& filter_) const
{
	Players players;

	for (Player*& player : m_players)
	{
		if (filter_(player))
		{
			players.push_back(player);
		}
	}

	return players;
}

// ----------------------------------------------------------------------------
Players Query::GetPlayers(Players& playersToFilter_, const PlayerFilter& filter_) const
{
	Players players;

	for (Player*& player : playersToFilter_)
	{
		if (filter_(player))
		{
			players.push_back(player);
		}
	}

	return players;
}

// ----------------------------------------------------------------------------
Player* Query::GetPlayer(const PlayerFilter& filter_) const
{
	for (int i = 0; i < (int)m_players.size(); ++i)
	{
		if (filter_(m_players[i]))
		{
			return m_players[i];
		}
	}

	ERROR_AND_DIE("Looked for a player with a bad filter.");
}

// ----------------------------------------------------------------------------
Phase Query::GetCurrentPhase()
{
	switch(g_theRakNetInterface->m_connection)
	{
		case ConnectionType::SERVER:
		{
			return g_Interface->server().GetCurrentPhase();
			break;
		}

		case ConnectionType::CLIENT:
		{
			return g_Interface->client().GetCurrentPhase();
			break;
		}

		default:
		{
			ERROR_AND_DIE("Requested the current phase for an unknown Connection Type!");
			break;
		}
	}
}

// ----------------------------------------------------------------------------
// Debug;
// ----------------------------------------------------------------------------
Debug::Debug(Units& units_, Cards& cards_)
	: m_units(units_)
	, m_cards(cards_)
{
}

// ----------------------------------------------------------------------------
// Server;
// ----------------------------------------------------------------------------
Server::Server(Units& units_, Cards& cards_, Players& players_)
	: m_units(units_)
	, m_cards(cards_)
	, m_players(players_)
{

}

Server::~Server()
{
	
}

// ----------------------------------------------------------------------------
void Server::Update(float deltaSeconds_)
{
	bool gameHasStarted = PreGameChecksAndSetups(deltaSeconds_);
	if(gameHasStarted)
	{
		// The first switch phase will make everyone go from pregame to purchase;
		// Every switch phases will then go between purchase <-> battle;
		switch(m_currentPhase)
		{
			case Phase::PREGAME:
			{
				g_Interface->match().SwitchPhases();
				g_theRakNetInterface->SendSwitchPhaseMessageToClients();
				g_theRakNetInterface->SendStartMultiplayerGameMessageToClients(); 

				break;
			}

			case Phase::PURCHASE:
			{
				bool allClientsPurchasePhaseComplete = g_theRakNetInterface->CheckAllClientsPurchasePhaseComplete();
				if(allClientsPurchasePhaseComplete)
				{
					g_Interface->match().SwitchPhases();
					g_theRakNetInterface->SendSwitchPhaseMessageToClients();
				}

				Players aiPlayers = g_Interface->query().GetPlayers(IsAIPlayer());
				for(Player*& player : aiPlayers)
				{
					player->Update(deltaSeconds_);
				}

				break;
			}

			case Phase::BATTLE:
			{
				Players aiPlayers = g_Interface->query().GetPlayers(IsAIPlayer());
				for (Player*& player : aiPlayers)
				{
					player->Update(deltaSeconds_);
				}

				bool allClientsBattlePhaseComplete = g_theRakNetInterface->CheckAllClientsBattlePhaseComplete();
				if(allClientsBattlePhaseComplete)
				{

					// If everyone has reported they are done with Battle Phase, verify we have all MatchReports;
					GUARANTEE_OR_DIE(m_matchReports.size() == (m_matchCount * 2), "The count of Match Reports does not match the count of matches assigned.");

					VerifyAndProcessEachMatchReport();
					bool itsOver = CheckForWinnerAndLoser();
					/*
					RemoveDeadPlayersFromServerPlayers();

					std::vector<Player*> alivePlayers = GetAlivePlayersFromServerPlayers();
					if(alivePlayers.size() == 1)
					{
						m_isGameOver = true;
						SendYouWinTheGameMessageToPlayerID(alivePlayers[0]->m_playerID);
					}
					*/
					if(!itsOver)
					{
						g_Interface->match().SwitchPhases();
						g_theRakNetInterface->SendSwitchPhaseMessageToClients();
					}
				}

				break;
			}

			default:
			{
				ERROR_AND_DIE("Server is in an unknown Phase!");
				break;
			}
		}
	}
}

// ----------------------------------------------------------------------------
bool Server::PreGameChecksAndSetups(float deltaSeconds_)
{
	if(m_startGameMessageSent)
	{
		return true;
	}

	bool allConnectedClientsAreReady = g_theRakNetInterface->CheckAllClientsReadyStatus();
	if(allConnectedClientsAreReady)
	{
		SendStartMessageIfHaveNotSent();
		CountdownToStartGame(deltaSeconds_);
	}

	return false;
}

// ----------------------------------------------------------------------------
void Server::SendStartMessageIfHaveNotSent()
{
	if(!m_startMessageSent)
	{
		m_startMessageSent = true;
		g_theRakNetInterface->SendGameStartingMessageToClients();
	}
}

// ----------------------------------------------------------------------------
void Server::CountdownToStartGame(float deltaSeconds_)
{
	// Have a countdown timer, I slow down the countdown timer;
	m_startGameCountdownTimer -= (deltaSeconds_ / 1.75f);

	// If the countdown timer is positive numbers, display the int numbers to clients;
	if (m_startGameCountdownTimer > 0)
	{
		int timer = (int)m_startGameCountdownTimer;
		if (timer != m_lastFrameTimer)
		{
			m_lastFrameTimer = timer;
			g_theRakNetInterface->SendGameCountdownMessageToClients(timer);
		}
	}

	// If the timer is between -1.0f and 0.0f, the game will do nothing, displaying 0s on the timer for clients;

	// Once it has waited for 1ish seconds, then we create players server-side, create the communal deck, and kick off;
	else if (m_startGameCountdownTimer < -1.0f)
	{
		CreatePlayers();
		g_Interface->match().CreateCommunalDeck();
		m_startGameMessageSent = true;
	}
}

// ----------------------------------------------------------------------------
void Server::KillConnectionWithAllClients()
{
	Players humanPlayers = g_Interface->query().GetPlayers(IsHumanPlayer());
	for(Player* player : humanPlayers)
	{
		KillConnectionWithClient(player);
	}

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		g_theRakNetInterface->m_clientList[i] = ConnectedClient();
	}
	
	g_theRakNetInterface->m_connection = ConnectionType::SERVER;
	g_theRakNetInterface->m_connectedClientCount = 0;
	m_currentPhase = Phase::PREGAME;
	m_allClientsSaidDoneWithPurchasePhase = false;
	m_allClientsSaidDoneWithBattlePhase = false;
	m_matchCount = 0;
	m_allMatchesReportedBack = false;
	m_matchReports.clear();
	m_maxMarketplaceCards = 3;
	m_startGameMessageSent = false;
	m_startGameCountdownTimer = 3.5f;

	g_Interface->ClearUnitsCardsPlayers();
}

// ----------------------------------------------------------------------------
void Server::KillConnectionWithClient(Player* player_)
{
	g_theRakNetInterface->CloseConnectionWithClient(player_->GetPlayerID());
}

// ----------------------------------------------------------------------------
void Server::CreatePlayers()
{
	// Go through each connected client;
	int playerID;
	for (playerID = 0; playerID < g_theRakNetInterface->m_connectedClientCount; ++playerID)
	{
		// Create a new player using the same index as the RakNet connectedClient index;
		// Using the same index will allow the playerID to match the index in the RakNet connectedClient info;
		HumanPlayer* player = new HumanPlayer(playerID);
		player->SetPlayerUsername(g_theRakNetInterface->m_clientList[playerID].m_username);

		// Now that the player is made server-side, the actual client needs to know which playerID they are playing with;
		g_theRakNetInterface->SendPlayerIDAndUsernameToPlayer(playerID, player->GetPlayerUsername());

		// Server holds a list of all players;
		m_players.push_back(player);
	}

	// After making the players, create AI players to fill in missing spots, up to 8;
	int playerCount = (int)m_players.size() - 1; // We subtract 1 because we started counting at 0;
	int aiPlayerCount = 8 - playerCount;
	for(playerID; playerID < aiPlayerCount; ++playerID)
	{
		AIPlayer* aiPlayer = new AIPlayer(playerID);
		std::string username = Stringf("%s%d", "AIPlayer", playerID);
		aiPlayer->SetPlayerUsername(username);

		// Server holds a list of all players;
		m_players.push_back(aiPlayer);
	}
}

// ----------------------------------------------------------------------------
void Server::ResetMatchIDsOnPlayers()
{
	for (Player* player : m_players)
	{
		player->ResetMatchID();
	}
}

// ----------------------------------------------------------------------------
void Server::AssignMatchIDsForPlayers()
{
	// Pair up two players at a time.

	// 1. Start by pairing alive human players
	// 1.a. If there is an odd number of alive human players...
	// 1.a.1 ...the odd one will pair with alive AI player
	// 1.a.2 ...the odd one will pair with dead AI player if there are no alive AI player

	// 2. Pair up alive AI players
	// 2.a. If there is an odd number of alive AI players...
	// 2.a.1 ...the odd one will play against a random dead AI player
	
	// 3. Dead AI players will not play

	// Get our options of players;
	Players aliveHumanPlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND, 
		{
			IsHumanPlayer(),
			IsPlayerAlive()
		}));
	Players aliveAIPlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND,
		{
			IsAIPlayer(),
			IsPlayerAlive()
		}));
	Players deadAIPlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND,
		{
			IsAIPlayer(),
			IsPlayerDead()
		}));

	int halfPlayerCount = (int)aliveHumanPlayers.size() / 2;

	// 1.
	for (m_matchCount = 0; m_matchCount <= halfPlayerCount; ++m_matchCount)
	{
		Player* humanPlayerWithNoMatchID = aliveHumanPlayers.GetRandomPlayerWithNoMatchID();
		humanPlayerWithNoMatchID->SetMatchID(m_matchCount);

		humanPlayerWithNoMatchID = aliveHumanPlayers.GetRandomPlayerWithNoMatchID();
		if(humanPlayerWithNoMatchID)
		{
			humanPlayerWithNoMatchID->SetMatchID(m_matchCount);
		}
		else
		{
			Player* ai = aliveAIPlayers.GetRandomPlayerWithNoMatchID();
			ai->SetMatchID(m_matchCount);
		}
		
	}

	// 1.a.
	Player* player = aliveHumanPlayers.GetRandomPlayerWithNoMatchID();
	if(player)
	{
		player->SetMatchID(m_matchCount);

		// 1.a.1
		player = aliveAIPlayers.GetRandomPlayerWithNoMatchID();
		if(player)
		{
			player->SetMatchID(m_matchCount);
		}
		else
		{
			// 1.a.2
			player = deadAIPlayers.GetRandomPlayerWithNoMatchID();
			if(player)
			{
				player->SetMatchID(m_matchCount);
			}
			else
			{
				ERROR_AND_DIE("Unable to find an AI player to play against an odd number of humans.");
			}
		}

		m_matchCount++;
	}

	// Re-get alive and dead AI players because there is a chance that an ai player was pulled to play against an odd human;
	aliveAIPlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND,
		{
			IsAIPlayer(),
			IsPlayerAlive(),
			HasNoMatchID()
		}));
	deadAIPlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND,
		{
			IsAIPlayer(),
			IsPlayerDead(),
			HasNoMatchID()
		}));
	// 2
	int halfAIPlayerCount = (int)aliveAIPlayers.size() / 2;
	for (m_matchCount; m_matchCount <= halfAIPlayerCount; ++m_matchCount)
	{
		Player* aiPlayerWithNoMatchID = aliveAIPlayers.GetRandomPlayerWithNoMatchID();
		aiPlayerWithNoMatchID->SetMatchID(m_matchCount);
		aiPlayerWithNoMatchID = aliveAIPlayers.GetRandomPlayerWithNoMatchID();
		aiPlayerWithNoMatchID->SetMatchID(m_matchCount);
	}

	// 2.a
	Player* aiPlayer = aliveAIPlayers.GetRandomPlayerWithNoMatchID();
	if(aiPlayer)
	{
		aiPlayer->SetMatchID(m_matchCount);

		// 2.a.1
		aiPlayer = deadAIPlayers.GetRandomPlayerWithNoMatchID();
		aiPlayer->SetMatchID(m_matchCount);
	}
}

// ----------------------------------------------------------------------------
void Server::SendHumanPlayerTheirEnemy(int playerID_, Player*& enemyPlayer_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECIEVEENEMYPLAYERINFOFORBATTLE);
	int enemyPlayerID = enemyPlayer_->GetPlayerID();
	unsigned int currentHealthOfEnemy = enemyPlayer_->GetPlayerHealth();
	RakNet::RakString enemyPlayerUsername = enemyPlayer_->GetPlayerUsername().c_str();
	bsOut.Write(enemyPlayerID);
	bsOut.Write(currentHealthOfEnemy);
	bsOut.Write(enemyPlayerUsername);

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendAIPlayerTheirEnemy(int playerID_, Player*& enemyPlayer_)
{
	Player* aiPlayer = g_Interface->query().GetPlayer(HasPlayerID(playerID_));

	aiPlayer->SetEnemyPlayer(enemyPlayer_);
}

// ----------------------------------------------------------------------------
void Server::SendPlayerTheirMatchID(int playerID_, int matchID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECIEVEMATCHIDFORBATTLE);
	bsOut.Write(matchID_);

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendPlayerIfTheyGoFirstForBattlePhase(int playerID_, bool goesFirst_)
{
	Player* player = g_Interface->query().GetPlayer(HasPlayerID(playerID_));

	if(player->IsAIPlayer())
	{
		player->SetGoesFirstForBattlePhase(goesFirst_);
	}
	else
	{
		RakNet::BitStream bsOut;
		bsOut.Write((unsigned char)C_RECIEVEGOESFIRSTFORBATTLE);
		bsOut.Write(goesFirst_);

		g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
	}
	
}

// ----------------------------------------------------------------------------
void Server::SendPlayerTheirSeedForRandomNumberGenrator(int playerID_, unsigned int seed_)
{
	Player* player = g_Interface->query().GetPlayer(HasPlayerID(playerID_));

	if(player->IsAIPlayer())
	{
		player->SetSeedToUseForRNG(seed_);
	}
	else
	{
		RakNet::BitStream bsOut;
		bsOut.Write((unsigned char)C_RECIEVESEEDFORBATTLE);
		bsOut.Write(seed_);

		g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
	}
}

// ----------------------------------------------------------------------------
void Server::SendPlayerTheirUpdatedHealth(int playerID_, int updatedHealth_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECIEVEUPDATEDPLAYERHEALTH);
	bsOut.Write(updatedHealth_);

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::RemoveDeadPlayersFromServerPlayers()
{
// 	std::vector<HumanPlayer*> deadPlayers;
// 
// 	for(HumanPlayer* player : m_serverPlayers)
// 	{
// 		if(player->m_playerHealth == 0u)
// 		{
// 			deadPlayers.push_back(player);
// 		}
// 	}
// 
// 	if(deadPlayers.size() > 0)
// 	{
// 
// 	}


}

// ----------------------------------------------------------------------------
void Server::SendYouWinTheGameMessageToPlayerID(int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_YOUWINTHEGAME);

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendYouLostTheGameMessageToPlayerID(int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_YOULOSETHEGAME);

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SwitchPhases()
{
	// Done when switching any phase;
	m_allClientsSaidDoneWithPurchasePhase = false;
	m_allClientsSaidDoneWithBattlePhase = false;
	m_allMatchesReportedBack = false;
	g_theRakNetInterface->UpdateAllClientsNotCompleteWithPhases();

	for (Player* player : m_players)
	{
		player->SetBattlePhaseComplete(false);
		player->SetPurchasePhaseComplete(false);
	}

	// Per Phase basis;
	if (m_currentPhase == Phase::PREGAME)
	{
		m_currentPhase = Phase::PURCHASE;
		SendAllClientsPhaseInformationForPurchasePhase();
	}
	else if (m_currentPhase == Phase::PURCHASE)
	{
		m_currentPhase = Phase::BATTLE;
		m_matchReports.clear();
		SendAllClientsPhaseInformationForBattlePhase();
	}
	else if (m_currentPhase == Phase::BATTLE)
	{
		m_currentPhase = Phase::PURCHASE;
		SendAllClientsPhaseInformationForPurchasePhase();
	}
}

// ----------------------------------------------------------------------------
Phase Server::GetCurrentPhase()
{
	return m_currentPhase;
}

// ----------------------------------------------------------------------------
void Server::SendAllClientsPhaseInformationForPurchasePhase()
{
	// All units that the server has, give them all full health;
	GiveAllEntitiesMaxHealth();

	Players alivePlayers = g_Interface->query().GetPlayers(IsPlayerAlive());

	// Human player updates;
	for (Player*& player : alivePlayers)
	{
		int playerID = player->GetPlayerID();

		if(!player->IsAIPlayer())
		{			
			RollAndSendMarketplaceCardsForClient(player);
			GetAndSendCardsInHandForPlayerID(playerID);
			GetAndSendUnitsForPlayerID(playerID);
		}
		else
		{
			RollMarketplaceCardsForAIPlayer(player);
		}
	}

	GiveAllPlayersGoldIncrease(1);
	GiveAllPlayersMaxGoldForTheTurn();

	SendAllClientsTheirGoldValue();
}

// ----------------------------------------------------------------------------
void Server::SendAllClientsPhaseInformationForBattlePhase()
{
	GiveAllEntitiesMaxHealth();

	// First reset the m_matchID that may have been set from a previous match;
	ResetMatchIDsOnPlayers();

	// Then randomly pair up players with eachother;
	AssignMatchIDsForPlayers();

	// Get the players with matching matchIDs;
	for (int i = 0; i < m_matchCount; ++i)
	{
		// This needs to account for AI Players now... Who should have match ids already;

		Players matchedPlayers = g_Interface->query().GetPlayers(HasMatchID(i));
		Player* player1 = matchedPlayers[0];
		Player* player2 = matchedPlayers[1];

		// Sending each player their enemy;
		// Human players recieve their information over the network;
		// AI players just set the information in the AI Player class;
		!player1->IsAIPlayer()
			? SendHumanPlayerTheirEnemy(player1->GetPlayerID(), player2)
			: SendAIPlayerTheirEnemy(player1->GetPlayerID(), player2);

		!player2->IsAIPlayer()
			? SendHumanPlayerTheirEnemy(player2->GetPlayerID(), player1)
			: SendAIPlayerTheirEnemy(player2->GetPlayerID(), player1);

		// Sending the player their matchID;
		// Human players need their match id sent over the network to their Client;
		// AI players can skip this step because it is already set;
		if (!player1->IsAIPlayer())
		{
			SendPlayerTheirMatchID(player1->GetPlayerID(), i);
		}
		if (!player1->IsAIPlayer())
		{
			SendPlayerTheirMatchID(player2->GetPlayerID(), i);
		}

		// Get the Units for each player as the Server sees them;
		Units player1Units = g_Interface->query().GetUnits(UnitBelongsToPlayerID(player1->GetPlayerID()));
		Units player2Units = g_Interface->query().GetUnits(UnitBelongsToPlayerID(player2->GetPlayerID()));

		// Send the Units that the player owns to the player;
		// Human, this information will get sent across the network, they will make their units on their side;
		// AI, this information will be created for the ai player server side, but made on the ai player;
		if (!player1->IsAIPlayer())
		{
			SendUnitTypesToPlayerIDForField(player1Units, player1->GetPlayerID());
		}

		if (!player2->IsAIPlayer())
		{
			SendUnitTypesToPlayerIDForField(player2Units, player2->GetPlayerID());
		}

		// Send the Units that the enemy owns to the player;
		// Human, this information will get sent across the network, they will make their enemy units on their side;
		// AI, this information will be created for the ai player server side, but made on the ai player;
		!player1->IsAIPlayer()
			? SendEnemyUnitTypesToPlayerIDForEnemyField(player2Units, player1->GetPlayerID())
			: SendEnemyUnitTypesToAIPlayerIDForEnemyField(player2Units, player1->GetPlayerID());

		!player2->IsAIPlayer()
			? SendEnemyUnitTypesToPlayerIDForEnemyField(player1Units, player2->GetPlayerID())
			: SendEnemyUnitTypesToAIPlayerIDForEnemyField(player1Units, player2->GetPlayerID());
		
		
		// Sending Random seed;
		unsigned int seed = (unsigned int)time(0);
		bool player1GoesFirst = (bool)g_theRandomNumberGenerator->GetRandomIntInRange(0, 1);
		bool player2GoesFirst = !player1GoesFirst;

		SendPlayerIfTheyGoFirstForBattlePhase(player1->GetPlayerID(), player1GoesFirst);
		SendPlayerIfTheyGoFirstForBattlePhase(player2->GetPlayerID(), player2GoesFirst);

		SendPlayerTheirSeedForRandomNumberGenrator(player1->GetPlayerID(), seed);
		SendPlayerTheirSeedForRandomNumberGenrator(player2->GetPlayerID(), seed);
	}
}

// ----------------------------------------------------------------------------
void Server::GiveAllPlayersMaxGoldForTheTurn()
{
	for (Player*& player : m_players)
	{
		player->SetActualGoldAmount(player->GetGoldAmount());
	}
}

// ----------------------------------------------------------------------------
void Server::GiveAllPlayersGoldIncrease(int goldIncreaseAmount_ /*= 0*/)
{
	for (Player*& player : m_players)
	{
		player->ChangeActualGoldAmount(goldIncreaseAmount_);
		player->ChangeGoldAmount(goldIncreaseAmount_);
		player->ClampActualGoldAmount();
		player->ClampGoldAmount();
	}
}

// ----------------------------------------------------------------------------
void Server::SendAllClientsTheirGoldValue()
{
	Players humanPlayers = g_Interface->query().GetPlayers(IsHumanPlayer());

	for (Player*& player : humanPlayers)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((unsigned char)C_RECIEVEGOLDAMOUNT);
		bsOut.Write(player->GetGoldAmount());
		bsOut.Write(player->GetActualGold());

		g_theRakNetInterface->SendBitStreamToClient(&bsOut, player->GetPlayerID());
	}
}

// ----------------------------------------------------------------------------
int Server::GetMaxMarketplaceCards()
{
	return m_maxMarketplaceCards;
}

// ----------------------------------------------------------------------------
void Server::RollAndSendMarketplaceCardsForClient(Player*& player_)
{
	// If we are rolling, more than likely we will need a fresh market;
	int amountRequested = m_maxMarketplaceCards;

	// Get the cards, if any, that are currently in the marketplace;
	Cards rolledCardsForMarketPlace;
	Cards currentMarketplaceCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
		{
			CardBelongsToPlayerID(player_->GetPlayerID()),
			CardInMarketplace()
		}));

	// If the player has locked their marketplace, then they will only need to fill the empty spaces;
	if(g_theRakNetInterface->m_clientList[player_->GetPlayerID()].m_marketplaceLocked)
	{
		amountRequested = m_maxMarketplaceCards - (int)currentMarketplaceCards.size();

		// Put our current cards into the rolledCardsForMarketPlace list that we will return;
		for(Card* oldCard : currentMarketplaceCards)
		{
			oldCard->m_slotID = -1;
			rolledCardsForMarketPlace.push_back(oldCard);
		}
	}

	// If the market is not locked, then the player will have those market cards put back in the deck;
	if(!g_theRakNetInterface->m_clientList[player_->GetPlayerID()].m_marketplaceLocked)
	{
		for(Card* mCard : currentMarketplaceCards)
		{
			mCard->m_playerID = -1;
			mCard->m_slotID = -1;
			mCard->m_cardArea = CardArea::DECK;
		}
	}

	// We will now pull out cards from the deck based on the amount needed;
	for (int i = 0; i < amountRequested; ++i)
	{
		Cards availableCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
			{
				CardBelongsToPlayerID(-1), // -1 means no one owns this card
				CardInDeck()
			}));

		Card* choosenCard = availableCards.GetRandomCard();
		choosenCard->m_playerID = player_->GetPlayerID();
		choosenCard->m_cardArea = CardArea::MARKET;
		rolledCardsForMarketPlace.push_back(choosenCard);
	}

	SendCardTypesToPlayerIDForMarketplace(rolledCardsForMarketPlace, player_->GetPlayerID());
}

// ----------------------------------------------------------------------------
void Server::ClearMarketplaceCardsForClient(int playerID_)
{
	Cards marketCardsBelongingToPlayer = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
		{
			CardBelongsToPlayerID(playerID_),
			CardInMarketplace()
		}));

	for (Card* card : marketCardsBelongingToPlayer)
	{
		card->m_playerID = -1;
		card->m_slotID = -1;
		card->m_cardArea = CardArea::DECK;
	}
}

// ----------------------------------------------------------------------------
void Server::SendCardTypesToPlayerIDForMarketplace(Cards& rolledCardsForMarketPlace_, int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECEIVECARDTYPESFORMARKETPLACE);
	bsOut.Write((int)rolledCardsForMarketPlace_.size());

	for(int i = 0; i < rolledCardsForMarketPlace_.size(); ++i)
	{
		bsOut.Write((int)rolledCardsForMarketPlace_[i]->m_type);
		bsOut.Write((unsigned int)rolledCardsForMarketPlace_[i]->m_cardID);
	}

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::UpdateMarketplaceCardsBasedOnPurchaseByPlayer(unsigned int cardID_, int playerID_)
{
	Card* card = g_Interface->query().GetCard(HasCardID(cardID_));
	if (card)
	{
		card->m_cardArea = CardArea::HAND;
		card->m_slotID = -1;
		card->m_playerID = playerID_;
	}
}

// ----------------------------------------------------------------------------
void Server::GetMarketplaceCardsForPlayerID(int playerID_)
{
	Cards marketplaceCardsForPlayer = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
		{
			CardBelongsToPlayerID(playerID_),
			CardInMarketplace()
		}));

	SendCardTypesToPlayerIDForMarketplace(marketplaceCardsForPlayer, playerID_);
}

// ----------------------------------------------------------------------------
void Server::RollMarketplaceCardsForAIPlayer(Player*& player_)
{
	// Get the cards, if any, that are currently in the marketplace;
	Cards rolledCardsForMarketPlace;
	Cards currentMarketplaceCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
		{
			CardBelongsToPlayerID(player_->GetPlayerID()),
			CardInMarketplace()
		}));

	int amountRequested = m_maxMarketplaceCards - (int)currentMarketplaceCards.size();

	// If the player has locked their marketplace, then they will only need to fill the empty spaces;
	if (player_->GetMarketplaceLocked())
	{
		// Put our current cards into the rolledCardsForMarketPlace list that we will return;
		for (Card* oldCard : currentMarketplaceCards)
		{
			oldCard->m_slotID = -1;
			rolledCardsForMarketPlace.push_back(oldCard);
		}

		amountRequested = m_maxMarketplaceCards - (int)rolledCardsForMarketPlace.size();
	}
	else // If the market is not locked, then the player will have those market cards put back in the deck;
	{
		for (Card* mCard : currentMarketplaceCards)
		{
			mCard->m_playerID = -1;
			mCard->m_slotID = -1;
			mCard->m_cardArea = CardArea::DECK;
		}
	}

	// We will now pull out cards from the deck based on the amount needed;
	for (int i = 0; i < amountRequested; ++i)
	{
		Cards availableCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
			{
				CardBelongsToPlayerID(-1), // -1 means no one owns this card
				CardInDeck()
			}));

		Card* choosenCard = availableCards.GetRandomCard();
		choosenCard->m_playerID = player_->GetPlayerID();
		choosenCard->m_cardArea = CardArea::MARKET;
		rolledCardsForMarketPlace.push_back(choosenCard);
	}
}

// ----------------------------------------------------------------------------
void Server::GetAndSendCardsInHandForPlayerID(const int playerID_)
{
	Cards handCardsBelongingToPlayer = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
		{
			CardBelongsToPlayerID(playerID_),
			CardInHand()
		}));

	SendCardTypesToPlayerIDForHand(handCardsBelongingToPlayer, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendCardTypesToPlayerIDForHand(Cards cardsInHand_, int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECEIVECARDTYPESFORHAND);
	bsOut.Write((int)cardsInHand_.size());

	for (int i = 0; i < cardsInHand_.size(); ++i)
	{
		bsOut.Write((int)cardsInHand_[i]->m_type);
		bsOut.Write((unsigned int)cardsInHand_[i]->m_cardID);
	}

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::RemoveCardFromSellingByPlayer(unsigned int cardID_)
{
	Card* card = g_Interface->query().GetCard(HasCardID(cardID_));
	if (card)
	{
		card->m_cardArea = CardArea::DECK;
		card->m_slotID = -1;
		card->m_playerID = -1;
	}
}

// ----------------------------------------------------------------------------
void Server::RemoveCardFromPlacingUnitByPlayer(unsigned int cardID_)
{
	Card* card = g_Interface->query().GetCard(HasCardID(cardID_));
	if (card)
	{
		card->m_cardArea = CardArea::UNITFIELD;
		card->m_slotID = -1;
	}
}

// ----------------------------------------------------------------------------
void Server::GetAndSendUnitsForPlayerID(const int playerID_)
{
	Units unitsBelongingToPlayer = g_Interface->query().GetUnits(UnitBelongsToPlayerID(playerID_));

	SendUnitTypesToPlayerIDForField(unitsBelongingToPlayer, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendUnitTypesToPlayerIDForField(Units units_, const int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECEIVEUNITTYPESFORFIELD);
	bsOut.Write((int)units_.size());

	for (int i = 0; i < units_.size(); ++i)
	{
		bsOut.Write((int)units_[i]->m_type);
		bsOut.Write((int)units_[i]->m_unitID);
		bsOut.Write((int)units_[i]->m_slotID);
	}

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendUnitTypesToAIPlayerIDForField(Units units_, const int playerID_)
{
	Player* aiPlayer = g_Interface->query().GetPlayer(HasPlayerID(playerID_));
	aiPlayer->SetUnits(units_);
}

// ----------------------------------------------------------------------------
void Server::SendEnemyUnitTypesToPlayerIDForEnemyField(Units& enemyUnits_, const int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_RECEIVEENEMYUNITTYPESFORENEMYFIELD);
	bsOut.Write((int)enemyUnits_.size());

	for (int i = 0; i < enemyUnits_.size(); ++i)
	{
		bsOut.Write((int)enemyUnits_[i]->m_type);
		bsOut.Write((int)enemyUnits_[i]->m_unitID);
		bsOut.Write((int)enemyUnits_[i]->m_slotID);
	}

	g_theRakNetInterface->SendBitStreamToClient(&bsOut, playerID_);
}

// ----------------------------------------------------------------------------
void Server::SendEnemyUnitTypesToAIPlayerIDForEnemyField(Units& enemyUnits_, const int playerID_)
{
	Player* aiPlayer = g_Interface->query().GetPlayer(HasPlayerID(playerID_));
	aiPlayer->SetEnemyUnits(enemyUnits_);
}

// ----------------------------------------------------------------------------
void Server::CreateNewUnitFromCardPlacedByPlayer(unsigned int cardID_, int unitSlotID_)
{
	Card* card = g_Interface->query().GetCard(HasCardID(cardID_));
	if(card)
	{
		JobType jobType = (JobType)card->m_type;
		Unit* unit = new Unit(jobType);
		unit->m_playerID = card->m_playerID;
		unit->m_slotID = unitSlotID_;
		unit->m_unitID = g_Interface->GetUnitIDAndIncrementCounter();
		//unit->m_currentSpriteDefinition = g_Interface->match().m_unitSpriteSheets[UnitDefinition::UnitTypeToString(jobType)]->GetSpriteDefinition((int)jobType);
		m_units.push_back(unit);
	}
}

// ----------------------------------------------------------------------------
void Server::GiveAllEntitiesMaxHealth()
{
	Units units;

	for (Unit* unit : m_units)
	{
		unit->m_health = unit->m_unitDefinition->m_health;
	}
}

// ----------------------------------------------------------------------------
void Server::CreateMatchReport(int winningPlayerID_, int losingPlayerID_, int damageDealtToLosingPlayer_, int matchID_, bool ignore_)
{
	MatchReport matchReport(winningPlayerID_, losingPlayerID_, damageDealtToLosingPlayer_, matchID_, ignore_);

	m_matchReports.push_back(matchReport);
}

// ----------------------------------------------------------------------------
void Server::VerifyAndProcessEachMatchReport()
{
	for(int matchID = 0; matchID < m_matchCount; ++matchID)
	{
		// Get the two match reports of each matchID; There should be 2 for now, until we deal with odd players;
		std::vector<MatchReport> matchReportsOfMatchID = GetMatchReportsOfMatchID(matchID);
		GUARANTEE_OR_DIE(matchReportsOfMatchID.size() == 2, "Verifying Match Reports and did not get a pair to verify against each other!");

		// Compare the two, they should equal each other;
		if(matchReportsOfMatchID[0] == matchReportsOfMatchID[1])
		{
			// If they equal each other, then Process Match Reports;
			ProcessMatchReports(matchReportsOfMatchID);
		}
		else
		{
			ERROR_AND_DIE("Received Match Report of a matchID that did not match each other. This means two players disagree on what happened during their battle phase!");
		}
	}
}

// ----------------------------------------------------------------------------
bool Server::CheckForWinnerAndLoser()
{	
	Players humanPlayers = g_Interface->query().GetPlayers(IsHumanPlayer());

	for (Player* player : humanPlayers)
	{
		if (player->GetPlayerHealth() == 0 && !player->GetJustDied())
		{
			player->SetJustDied(true);
			SendYouLostTheGameMessageToPlayerID(player->GetPlayerID());

			AIPlayer* aiPlayer = new AIPlayer(player->GetPlayerID());
			std::string username = Stringf("%s%s", "AI", player->GetPlayerUsername().c_str());
			aiPlayer->SetPlayerUsername(username);
			m_players.push_back(aiPlayer);
		}
	}

	Players alivePlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND,
		{
			IsPlayerAlive()
		}));

	if(alivePlayers.size() == 1)
	{
		SendYouWinTheGameMessageToPlayerID(alivePlayers[0]->GetPlayerID());
		KillConnectionWithAllClients();
		return true;
	}

	Players aliveHumanPlayers = g_Interface->query().GetPlayers(PlayerMultiFilter(PlayerMultiFilter::Selector::AND,
		{
			IsPlayerAlive(),
			IsHumanPlayer()
		}));

	if(aliveHumanPlayers.size() == 0)
	{
		KillConnectionWithAllClients();
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
void Server::ProcessMatchReports(std::vector<MatchReport>& matchReports)
{
	MatchReport& matchReport = matchReports[0];
	if(matchReport.GetIgnore())
	{
		matchReport = matchReports[1];
		if(matchReport.GetIgnore())
		{
			return;
		}
	}

	int losingPlayerID = matchReport.GetLosingPlayerID();
	int damage = matchReport.GetDamageDealtToLosingPlayer();

	Player* player = g_Interface->query().GetPlayer(HasPlayerID(losingPlayerID));
	if(player)
	{
		player->SubtractFromPlayerHealth(damage);
		player->ClampHealth();

		if(!player->IsAIPlayer())
		{
			SendPlayerTheirUpdatedHealth(losingPlayerID, player->GetPlayerHealth());
		}		
	}
	else
	{
		ERROR_AND_DIE("Requested a player with a PlayerID and received none.");
	}
}

// ----------------------------------------------------------------------------
std::vector<MatchReport> Server::GetMatchReportsOfMatchID(int matchID_)
{
	std::vector<MatchReport> matchReportsOfMatchID;

	for(MatchReport mr : m_matchReports)
	{
		if(mr.GetMatchID() == matchID_)
		{
			matchReportsOfMatchID.push_back(mr);
		}
	}

	return matchReportsOfMatchID;
}

// ----------------------------------------------------------------------------
// Client;
// ----------------------------------------------------------------------------
Client::Client(Units& units_, Cards& cards_)
	: m_units(units_)
	, m_cards(cards_)
{

}

// ----------------------------------------------------------------------------
void Client::Update(float deltaSeconds_)
{
	Player*& player = g_Interface->GetPlayer();

	switch(m_currentPhase)
	{
		case Phase::PURCHASE:
		{
			// The Purchase Map handles all click-input from the Client;
			g_Interface->match().m_purchaseMap->Update(deltaSeconds_);

			// For now, this is only unit animations during the Purchase Phase;
			for (Unit* unit : m_units)
			{
				unit->PurchaseUpdate(deltaSeconds_);
			}

			break;
		}

		case Phase::BATTLE:
		{
			g_Interface->match().m_battleMap->Update(deltaSeconds_);
			player->SetRandomNumberGeneratorSeed();

			// Start, by getting an action unit, this is the unit starting their turn;
			if(!m_actionUnit)
			{
				if (!CheckForWinnerOfBattlePhase())
				{
					if (m_isFirstPlayersTurn)
					{
						Units aliveUnitsGoingFirst = g_Interface->query().GetUnits(m_unitsGoingFirst, IsUnitNotDead());
						Units aliveUnitsGoingSecond = g_Interface->query().GetUnits(m_unitsGoingSecond, IsUnitNotDead());

						if (aliveUnitsGoingFirst.size() > 0)
						{
							RunAttackSimulationOfAttackingVsDefending(aliveUnitsGoingFirst, aliveUnitsGoingSecond);
						}
					}
					else
					{
						Units aliveUnitsGoingFirst = g_Interface->query().GetUnits(m_unitsGoingFirst, IsUnitNotDead());
						Units aliveUnitsGoingSecond = g_Interface->query().GetUnits(m_unitsGoingSecond, IsUnitNotDead());

						if (aliveUnitsGoingSecond.size() > 0)
						{
							RunAttackSimulationOfAttackingVsDefending(aliveUnitsGoingSecond, aliveUnitsGoingFirst);
						}
					}
				}
			}

			// Do, update all units. The action unit will have a main ability at this point;
			UpdateUnits(deltaSeconds_);

			// If the action unit finished using all main abilities then their turn is over;
			if(g_Interface->match().m_battleMap->m_actionTurnEnding)
			{
				if(m_actionUnit)
				{
					// This is the old actionUnit ending its action turn;
					// We should create and start, do, end for each unit?
					m_actionUnit->ResetStatusEffects();
					m_actionUnit->ResetBuffEffects();
					m_actionUnit->m_isMyTurnToDoAction = false;
					m_actionUnit->m_dontDoStatusEffects = false;
					m_actionUnit->m_dontDoBuffEffects = false;
					m_actionUnit->m_dontDoDebuffEffects = false;
					m_actionUnit = nullptr;
				}
			}

			unsigned int newSeedPosition = g_theRandomNumberGenerator->GetCurrentPosition();
			player->SetSeedPosition(newSeedPosition);

			break;
		}

		default:
		{
			ERROR_AND_DIE("Client is in an unknown Phase!");
			break;
		}
	}
}

// ----------------------------------------------------------------------------
void Client::UpdateUnits(float deltaSeconds_)
{
	Units& unitsGoingFirst = g_Interface->client().GetUnitsGoingFirst();
	for (Unit*& fUnit : unitsGoingFirst)
	{
		fUnit->BattleUpdate(deltaSeconds_);
	}

	Units& unitsGoingSecond = g_Interface->client().GetUnitsGoingSecond();
	for (Unit*& sUnit : unitsGoingSecond)
	{
		sUnit->BattleUpdate(deltaSeconds_);
	}
}

// ----------------------------------------------------------------------------
void Client::SendRequestForHandCardsToServer(int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_REQUESTHANDCARDS);
	bsOut.Write(playerID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SendRequestForMarketplaceCardsToServer(int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_REQUESTMARKETPLACECARDS);
	bsOut.Write(playerID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SendRequestForClearingMarketplaceToServer(int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLEARMARKETPLACECARDS);
	bsOut.Write(playerID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SwitchPhases()
{
	switch(m_currentPhase)
	{
		case Phase::PREGAME:
		{
			m_currentPhase = Phase::PURCHASE;
			m_marketplaceLocked = false;
			m_messageSentToServerForPurchasePhaseComplete = false;
			m_messageSentToServerForBattlePhaseComplete = false;
			SendMarketplaceLockedToServer(m_marketplaceLocked);
			g_Interface->match().m_purchaseMap->StartTimer();

			if(g_theApp->m_theGame->m_mainMenuMusicPlaying)
			{
				g_theAudioSystem->StopSound(g_theApp->m_theGame->m_mainMenuMusicPlaybackID);
				g_theApp->m_theGame->m_mainMenuMusicPlaying = false;
			}
			
			ChannelGroupID bgmGroup = g_theAudioSystem->CreateOrGetChannelGroup("BGM");
			SoundID purchaseMusic = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Prelude.mp3");
			m_purchasePhasePlaybackID = g_theAudioSystem->PlaySound(purchaseMusic, bgmGroup, true);
			m_purchasePhaseMusicPlaying = true;

			break;
		}

		case Phase::PURCHASE:
		{
			m_currentPhase = Phase::BATTLE;

			Player*& player = g_Interface->GetPlayer();
			if(player->GetGoesFirstForBattlePhase())
			{
				m_unitsGoingFirst = m_units;
				m_unitsGoingSecond = m_enemyUnits;
			}
			else
			{
				m_unitsGoingFirst = m_enemyUnits;
				m_unitsGoingSecond = m_units;
			}

			GiveAllEntitiesMaxHealth();
			m_messageSentToServerForPurchasePhaseComplete = false;
			m_messageSentToServerForBattlePhaseComplete = false;

			int roll = g_theRandomNumberGenerator->GetRandomIntInRange(0, 2);
			if (roll == 0)
			{
				g_Interface->match().m_battleMap->m_nameOfCurrentBattleBackground = "Data/Images/Backgrounds/PlainsBackground.png";
			}
			else if (roll == 1)
			{
				g_Interface->match().m_battleMap->m_nameOfCurrentBattleBackground = "Data/Images/Backgrounds/DesertBackground.png";
			}
			else if (roll == 2)
			{
				g_Interface->match().m_battleMap->m_nameOfCurrentBattleBackground = "Data/Images/Backgrounds/DungeonBackground.png";
			}

			g_Interface->match().m_battleMap->StartTimer();

			if(m_purchasePhaseMusicPlaying)
			{
				g_theAudioSystem->StopSound(m_purchasePhasePlaybackID);
				m_purchasePhaseMusicPlaying = false;
			}

			ChannelGroupID bgmGroup = g_theAudioSystem->CreateOrGetChannelGroup("BGM");
			SoundID battleMusic = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Battle.mp3");
			m_battlePhasePlaybackID = g_theAudioSystem->PlaySound(battleMusic, bgmGroup, true);
			m_battlePhaseMusicPlaying = true;

			break;
		}

		case Phase::BATTLE:
		{
			m_currentPhase = Phase::PURCHASE;
			m_marketplaceLocked = false;
			m_messageSentToServerForPurchasePhaseComplete = false;
			m_messageSentToServerForBattlePhaseComplete = false;
			SendMarketplaceLockedToServer(m_marketplaceLocked);
			g_Interface->match().m_purchaseMap->StartTimer();

			if(m_battlePhaseMusicPlaying)
			{
				g_theAudioSystem->StopSound(m_battlePhasePlaybackID);
				m_battlePhaseMusicPlaying = false;
			}

			ChannelGroupID bgmGroup = g_theAudioSystem->CreateOrGetChannelGroup("BGM");
			SoundID purchaseMusic = g_theAudioSystem->CreateOrGetSound("Data/Audio/FF/Prelude.mp3");
			m_purchasePhasePlaybackID = g_theAudioSystem->PlaySound(purchaseMusic, bgmGroup, true);
			m_purchasePhaseMusicPlaying = true;

			break;
		}

		default:
		{
			ERROR_AND_DIE("Client in an unknown Phase!");
			break;
		}
	}
}

// ----------------------------------------------------------------------------
void Client::SendBattlePhaseCompleteToServer()
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLIENTCOMPLETEBATTLEPHASE);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SendPurchasePhaseCompleteToServer()
{
	RakNet::BitStream bsOut; 
	bsOut.Write((unsigned char)S_CLIENTCOMPLETEPURCHASEPHASE);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
Phase Client::GetCurrentPhase()
{
	return m_currentPhase;
}

// ----------------------------------------------------------------------------
bool Client::IsMessageSentToServerForPurchasePhaseComplete()
{
	return m_messageSentToServerForPurchasePhaseComplete;
}

// ----------------------------------------------------------------------------
bool Client::IsMessageSentToServerForBattlePhaseComplete()
{
	return m_messageSentToServerForBattlePhaseComplete;
}

// ----------------------------------------------------------------------------
void Client::SetMessageSentToServerForPurchasePhaseComplete(bool messageSent_)
{
	m_messageSentToServerForPurchasePhaseComplete = messageSent_;
}

// ----------------------------------------------------------------------------
void Client::SetMessageSentToServerForBattlePhaseComplete(bool messageSent_)
{
	m_messageSentToServerForBattlePhaseComplete = messageSent_;
}

// ----------------------------------------------------------------------------
void Client::CleanupMarketplaceCards()
{
	// This will delete the client-side card from m_cards, NOT the server-side;
	for (auto itr = m_cards.begin(); itr != m_cards.end();)
	{
		if ((*itr)->m_cardArea == CardArea::MARKET)
		{
			delete *itr;
			itr = m_cards.erase(itr);

		}
		else
		{
			itr++;
		}
	}
}

// ----------------------------------------------------------------------------
void Client::CleanupMarketplaceCardsAndRequestNewCards()
{
	// If we have a locked marketplace, then we do not want to clear our marketplace;
	if (m_marketplaceLocked)
	{
		return;
	}

	CleanupMarketplaceCards();

	// This will let the server know that we as a client are clearing our marketplace;
	g_Interface->client().SendRequestForClearingMarketplaceToServer(g_Interface->GetPlayer()->GetPlayerID());

	
}

// ----------------------------------------------------------------------------
void Client::RollMarketplaceCards()
{
	CleanupMarketplaceCardsAndRequestNewCards();

	g_Interface->client().SendRequestForMarketplaceCardsToServer(g_Interface->GetPlayer()->GetPlayerID());
}

// ----------------------------------------------------------------------------
void Client::ToggleMarketplaceLock()
{
	m_marketplaceLocked = !m_marketplaceLocked;
	SendMarketplaceLockedToServer(m_marketplaceLocked);
}

// ----------------------------------------------------------------------------
void Client::CreateCardForMarketplace(CardType cardType_, unsigned int cardID_)
{
	Card* card = nullptr;
	card = new Card(cardType_);
	card->m_playerID = g_Interface->GetPlayer()->GetPlayerID();
	card->m_cardID = cardID_;

	Cards marketplaceCards = g_Interface->query().GetCards(CardInMarketplace());
	card->m_slotID = (int)marketplaceCards.size();
	card->m_currentSpriteDefinition = g_Interface->match().m_jobIcons->GetSpriteDefinition((int)cardType_);
	card->m_cardArea = CardArea::MARKET;
	m_cards.push_back(card);
}

// ----------------------------------------------------------------------------
bool Client::IsMarketplaceLocked()
{
	return m_marketplaceLocked;
}

// ----------------------------------------------------------------------------
void Client::SendMarketplaceLockedToServer(bool marketplaceLocked_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLIENTMARKETPLACELOCKED);
	bsOut.Write(marketplaceLocked_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SendPurchasedCardFromMarketplaceToServer(unsigned int purchasedCardID_, int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLIENTPURCHASEDMARKETCARD);
	bsOut.Write(purchasedCardID_);
	bsOut.Write(playerID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SendSoldCardFromHandToServer(unsigned int soldCardID_, int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLIENTSOLDHANDCARD);
	bsOut.Write(soldCardID_);
	bsOut.Write(playerID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::SendUnitCreatedAtFromCardInHandToServer(unsigned int placeCardID_, int unitSlotID, int playerID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLIENTPLACEDUNITFROMHANDCARD);
	bsOut.Write(placeCardID_);
	bsOut.Write(unitSlotID);
	bsOut.Write(playerID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
void Client::CreateCardForHand(CardType cardType_, unsigned int cardID_)
{
	Card* card = nullptr;
	card = new Card(cardType_);
	card->m_playerID = g_Interface->GetPlayer()->GetPlayerID();
	card->m_cardID = cardID_;

	Cards handCards = g_Interface->query().GetCards(CardInHand());
	card->m_slotID = (int)handCards.size();
	card->m_currentSpriteDefinition = g_Interface->match().m_jobIcons->GetSpriteDefinition((int)cardType_);
	card->m_cardArea = CardArea::HAND;
	m_cards.push_back(card);
}

// ----------------------------------------------------------------------------
void Client::CleanupHandCards()
{
	// This will delete the client-side card from m_cards, NOT the server-side;
	for (auto itr = m_cards.begin(); itr != m_cards.end();)
	{
		if ((*itr)->m_cardArea == CardArea::HAND)
		{
			delete *itr;
			itr = m_cards.erase(itr);

		}
		else
		{
			itr++;
		}
	}
}

// ----------------------------------------------------------------------------
void Client::CreateUnitForField(JobType jobType_, int unitID_, int slotID_)
{
	Unit* unit = nullptr;
	unit = new Unit(jobType_);
	unit->m_playerID = g_Interface->GetPlayer()->GetPlayerID();
	unit->m_unitID = unitID_;
	unit->m_slotID = slotID_;
	unit->m_location = unit->GetPosition();
	std::string jobSpriteSheet = UnitDefinition::UnitTypeToString(jobType_);
	//unit->m_currentSpriteDefinition = g_Interface->match().m_unitSpriteSheets[jobSpriteSheet]->GetSpriteDefinition((int)jobType_);
	m_units.push_back(unit);
}

// ----------------------------------------------------------------------------
void Client::CreateEnemyUnitForEnemyField(JobType jobType_, int unitID_, int slotID_)
{
	Unit* unit = nullptr;
	unit = new Unit(jobType_);
	unit->m_playerID = g_Interface->GetEnemy()->GetPlayerID();
	unit->m_unitID = unitID_;
	unit->m_slotID = slotID_;
	unit->m_location = unit->GetEnemyPosition();
	std::string jobSpriteSheet = UnitDefinition::UnitTypeToString(jobType_);
	//unit->m_currentSpriteDefinition = g_Interface->match().m_unitSpriteSheets[jobSpriteSheet]->GetSpriteDefinition((int)jobType_);
	m_enemyUnits.push_back(unit);
}

// ----------------------------------------------------------------------------
void Client::CleanupUnits()
{
	// This will delete the client-side card from m_cards, NOT the server-side;
	for (auto itr = m_units.begin(); itr != m_units.end(); ++itr)
	{
		delete *itr;
		*itr = nullptr;
	}
	m_units.clear();

	Player* player = g_Interface->GetPlayer();
	if (player->GetGoesFirstForBattlePhase())
	{
		m_unitsGoingFirst = m_units;
		m_unitsGoingSecond = m_enemyUnits;
	}
	else
	{
		m_unitsGoingFirst = m_enemyUnits;
		m_unitsGoingSecond = m_units;
	}
}

// ----------------------------------------------------------------------------
void Client::CleanupEnemyUnits()
{
	// This will delete the client-side card from m_cards, NOT the server-side;
	for (auto itr = m_enemyUnits.begin(); itr != m_enemyUnits.end(); ++itr)
	{
		delete *itr;
		*itr = nullptr;
	}
	m_enemyUnits.clear();

	Player* player = g_Interface->GetPlayer();
	if (player->GetGoesFirstForBattlePhase())
	{
		m_unitsGoingFirst = m_units;
		m_unitsGoingSecond = m_enemyUnits;
	}
	else
	{
		m_unitsGoingFirst = m_enemyUnits;
		m_unitsGoingSecond = m_units;
	}
}

// ----------------------------------------------------------------------------
void Client::GiveAllEntitiesMaxHealth()
{
	for (Unit* fUnit : m_unitsGoingFirst)
	{
		fUnit->m_health = fUnit->m_unitDefinition->m_health;
	}

	for (Unit* sUnit : m_unitsGoingSecond)
	{
		sUnit->m_health = sUnit->m_unitDefinition->m_health;
	}
}

// ----------------------------------------------------------------------------
void Client::SetMatchIDForThisBattlePhase(int matchID_)
{
	m_matchID = matchID_;
}

// ----------------------------------------------------------------------------
void Client::RunAttackSimulationOfAttackingVsDefending(Units& attackingUnits, Units& defendingUnits)
{
	GUARANTEE_OR_DIE(defendingUnits.size() > 0, "Running an attack simulation with no defenders.");

	g_Interface->match().GetBattleMap()->ResetActionTimer();

	if (m_isFirstPlayersTurn)
	{
		m_actionUnit = attackingUnits.GetAliveUnitStartingAtIndex(m_firstPlayersAttackingUnitIndex);
	}
	else
	{
		m_actionUnit = attackingUnits.GetAliveUnitStartingAtIndex(m_secondPlayersAttackingUnitIndex);
	}

	GUARANTEE_OR_DIE(m_actionUnit->m_health != 0, "Picked a dead unit to do an action.");

	// Starting the units action turn;
	g_Interface->match().m_battleMap->m_actionTurnEnding = false;
	m_actionUnit->m_isMyTurnToDoAction = true;

	// Our attacking unit will now use its main ability;
	Ability* mainAbility = new Ability(m_actionUnit->m_unitDefinition->m_mainAbilityDefinition);
	m_actionUnit->m_activeAbilities.push_back(mainAbility);
	GUARANTEE_OR_DIE(mainAbility->m_lifetimeTotalTime > 0.0f, "Using an ability with no lifetime value.");
	g_Interface->match().m_battleMap->m_actionTimer += mainAbility->m_lifetimeTotalTime;

	AssignTargetAndCasterForMainAbility(mainAbility, m_actionUnit, attackingUnits, defendingUnits);

	float additionalTime = 0.2f;

	//Status;
	for (Ability*& status : m_actionUnit->m_activeStatusEffects)
	{
		status->AddOffsetStartTimeToSequence(additionalTime);
		additionalTime += (status->m_lifetimeTotalTime + 0.1f);
		g_Interface->match().m_battleMap->m_actionTimer += additionalTime;
	}

	// Buff;
	for(Ability*& buff : m_actionUnit->m_activeBuffs)
	{
		buff->AddOffsetStartTimeToSequence(additionalTime);
		additionalTime += (buff->m_lifetimeTotalTime + 0.1f);
		g_Interface->match().m_battleMap->m_actionTimer += additionalTime;
	}

	// Debuff;
	for (Ability*& debuff : m_actionUnit->m_activeDebuffs)
	{
		debuff->AddOffsetStartTimeToSequence(additionalTime);
		additionalTime += (debuff->m_lifetimeTotalTime + 0.1f);
		g_Interface->match().m_battleMap->m_actionTimer += additionalTime;
	}

	// Main ability gets them all;
	g_Interface->match().m_battleMap->m_actionTimer += 0.2f;
	mainAbility->AddOffsetStartTimeToSequence(additionalTime);

	m_isFirstPlayersTurn = !m_isFirstPlayersTurn;
}

// ----------------------------------------------------------------------------
bool Client::CheckForWinnerOfBattlePhase()
{
	Units unitsWhoWentFirst = g_Interface->query().GetUnits(m_unitsGoingFirst, IsUnitNotDead());
	Units unitsWhoWentSeconds = g_Interface->query().GetUnits(m_unitsGoingSecond, IsUnitNotDead());

	bool thereWasAWinner = false;
	int damageDealtToLosingPlayer = 0;
	int winningPlayerID = 0;
	int losingPlayerID = 0;
	Player*& player = g_Interface->GetPlayer();

	if (unitsWhoWentFirst.size() == 0 && unitsWhoWentSeconds.size() == 0)
	{
		// Draw;
		// I don't think this can happen yet. Maybe someday;
	}
	else if(unitsWhoWentFirst.size() == 0)
	{
		// Who ever went second won, figure out who that was;
		if(player->GetGoesFirstForBattlePhase())
		{
			damageDealtToLosingPlayer = (int)unitsWhoWentSeconds.size();
			winningPlayerID = g_Interface->GetEnemy()->GetPlayerID();
			losingPlayerID = g_Interface->GetPlayer()->GetPlayerID();
			g_theDevConsole->Print(Stringf("You lost, took [%d] damage!", damageDealtToLosingPlayer));
		}
		else // !m_goesFirst
		{
			damageDealtToLosingPlayer = (int)unitsWhoWentSeconds.size();
			winningPlayerID = g_Interface->GetPlayer()->GetPlayerID();
			losingPlayerID = g_Interface->GetEnemy()->GetPlayerID();
			g_theDevConsole->Print(Stringf("You won, dealt [%d] damage!", damageDealtToLosingPlayer));
		}

		thereWasAWinner = true;
	}
	if (unitsWhoWentSeconds.size() == 0)
	{
		// Who ever went first won, figure out who that was;
		if (player->GetGoesFirstForBattlePhase())
		{
			damageDealtToLosingPlayer = (int)unitsWhoWentFirst.size();
			winningPlayerID = g_Interface->GetPlayer()->GetPlayerID();
			losingPlayerID = g_Interface->GetEnemy()->GetPlayerID();
			g_theDevConsole->Print(Stringf("You won, dealt [%d] damage!", damageDealtToLosingPlayer));
		}
		else // !m_goesFirst
		{
			damageDealtToLosingPlayer = (int)unitsWhoWentFirst.size();
			winningPlayerID = g_Interface->GetEnemy()->GetPlayerID();
			losingPlayerID = g_Interface->GetPlayer()->GetPlayerID();
			g_theDevConsole->Print(Stringf("You lost, took [%d] damage!", damageDealtToLosingPlayer));
		}

		thereWasAWinner = true;
	}

	if(thereWasAWinner && !m_messageSentToServerForBattlePhaseComplete)
	{
		SendMatchReportToServer(winningPlayerID, losingPlayerID, damageDealtToLosingPlayer, m_matchID);
		SetMessageSentToServerForBattlePhaseComplete(true);
		SendBattlePhaseCompleteToServer();
	}

	return thereWasAWinner;
}

// ----------------------------------------------------------------------------
void Client::SendMatchReportToServer(int winningPlayerID_, int losingPlayerID_, int dmageDealtToLosingPlayer_, int matchID_)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_WINNEROFMATCHBEINGREPORTED);
	bsOut.Write(winningPlayerID_);
	bsOut.Write(losingPlayerID_);
	bsOut.Write(dmageDealtToLosingPlayer_);
	bsOut.Write(matchID_);

	g_theRakNetInterface->SendBitStreamToServer(&bsOut);
}

// ----------------------------------------------------------------------------
Units& Client::GetUnitsGoingFirst()
{
	return m_unitsGoingFirst;
}

// ----------------------------------------------------------------------------
Units& Client::GetUnitsGoingSecond()
{
	return m_unitsGoingSecond;
}

// ----------------------------------------------------------------------------
void Client::AssignTargetAndCasterForMainAbility(Ability*& mainAbility_, Unit*& attackingUnit_, Units& attackingUnits_, Units& defendingUnits_)
{
	// Assigning the Caster, this is whoever is casting the mainAbility;
	mainAbility_->AssignCasterAndCasterOriginalLocation(attackingUnit_);

	// The Target depends on a couple variables, alliance and target choice;
	switch (mainAbility_->GetTargetAlliance())
	{
		// If the ability is targeting an enemy unit;
		case TargetAlliance::ENEMY:
		{
			switch (mainAbility_->GetTargetChoice())
			{
				case TargetChoice::RANDOM:
				{
					int randomTarget = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)defendingUnits_.size() - 1);
					mainAbility_->AssignTarget(defendingUnits_[randomTarget]);
					mainAbility_->m_castLocation = defendingUnits_[randomTarget]->GetLocation();

					break;
				}

				case TargetChoice::LEASTDAMAGETAKEN:
				{
					Units leastDamageTakenUnits = g_Interface->query().GetLeastDamagedUnitsExcludingDeadUnits(defendingUnits_);
					if(leastDamageTakenUnits.size() > 0)
					{
						int randomTarget = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)leastDamageTakenUnits.size() - 1);
						mainAbility_->AssignTarget(leastDamageTakenUnits[randomTarget]);
						mainAbility_->m_castLocation = leastDamageTakenUnits[randomTarget]->GetLocation();
					}

					break;
				}

				case TargetChoice::MOSTDAMAGETAKEN:
				{
					Units mostDamageTakenUnits = g_Interface->query().GetMostDamagedUnitsExcludingDeadUnits(defendingUnits_);
					if (mostDamageTakenUnits.size() > 0)
					{
						int randomTarget = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)mostDamageTakenUnits.size() - 1);
						mainAbility_->AssignTarget(mostDamageTakenUnits[randomTarget]);
						mainAbility_->m_castLocation = mostDamageTakenUnits[randomTarget]->GetLocation();
					}

					break;
				}

				default:
				{
					ERROR_AND_DIE("Main ability does not have a valid Target Choice!");
					break;
				}
			}

			break;
		}

		// If the ability is targeting a friendly unit;
		case TargetAlliance::FRIENDLY:
		{
			switch(mainAbility_->GetTargetChoice())
			{
				case TargetChoice::RANDOM:
				{
					int randomTarget = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)attackingUnits_.size() - 1);
					mainAbility_->AssignTarget(attackingUnits_[randomTarget]);
					mainAbility_->m_castLocation = attackingUnits_[randomTarget]->GetLocation();

					break;
				}

				case TargetChoice::LEASTDAMAGETAKEN:
				{
					Units leastDamageTakenUnits = g_Interface->query().GetLeastDamagedUnitsExcludingDeadUnits(attackingUnits_);
					if (leastDamageTakenUnits.size() > 0)
					{
						int randomTarget = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)leastDamageTakenUnits.size() - 1);
						mainAbility_->AssignTarget(leastDamageTakenUnits[randomTarget]);
						mainAbility_->m_castLocation = leastDamageTakenUnits[randomTarget]->GetLocation();
					}

					break;
				}

				case TargetChoice::MOSTDAMAGETAKEN:
				{
					Units mostDamageTakenUnits = g_Interface->query().GetMostDamagedUnitsExcludingDeadUnits(attackingUnits_);
					if (mostDamageTakenUnits.size() > 0)
					{
						int randomTarget = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)mostDamageTakenUnits.size() - 1);
						mainAbility_->AssignTarget(mostDamageTakenUnits[randomTarget]);
						mainAbility_->m_castLocation = mostDamageTakenUnits[randomTarget]->GetLocation();
					}

					break;
				}

				default:
				{
					ERROR_AND_DIE("Main ability does not have a valid Target Choice!");
					break;
				}
			}

			break;
		}

		default:
		{
			ERROR_AND_DIE("Ability does not know what to target!");

			break;
		}
	}

	if(!mainAbility_->GetTarget())
	{
		ERROR_AND_DIE("Main ability has no target.");
	}
}

// ----------------------------------------------------------------------------
void Client::UpdatePlayerHealth(int updatedPlayerHealth_)
{
	Player*& player = g_Interface->GetPlayer();
	player->SetPlayerHealth(updatedPlayerHealth_);
}

// ----------------------------------------------------------------------------
// Interface;
// ----------------------------------------------------------------------------
Interface::Interface(Game* game_)
	: m_game(game_)
	, m_action(m_units, m_cards)
	, m_match(m_units, m_cards)
	, m_query(m_units, m_cards, m_players)
	, m_debug(m_units, m_cards)
	, m_server(m_units, m_cards, m_players)
	, m_client(m_units, m_cards)
{
}

// ----------------------------------------------------------------------------
Interface::~Interface()
{
	for (int u = 0; u < m_units.size(); ++u)
	{
		DELETE_POINTER(m_units[u]);
	}

	for (int c = 0; c < m_cards.size(); ++c)
	{
		DELETE_POINTER(m_cards[c]);
	}

	for (int p = 0; p < m_players.size(); ++p)
	{
		DELETE_POINTER(m_players[p]);
	}

	DELETE_POINTER(m_clientPlayer);
	DELETE_POINTER(m_enemyPlayer);
}

// ----------------------------------------------------------------------------
void Interface::Init()
{
	m_match.Init();
}

// ----------------------------------------------------------------------------
void Interface::Startup()
{
	m_match.Startup();
}

// ----------------------------------------------------------------------------
void Interface::Render()
{
	// Rendering for the Battle Phase;
	if(query().GetCurrentPhase() == Phase::BATTLE)
	{
		m_match.m_battleMap->Render();
		Player*& player = g_Interface->GetPlayer();

		// We are going first;
		if(player->GetGoesFirstForBattlePhase())
		{
			// If we go first, then the first units are the friendly ones;
			Units& friendlyUnits = g_Interface->client().GetUnitsGoingFirst();
			for(Unit*& friendlyUnit : friendlyUnits)
			{
				friendlyUnit->BattleRender();
			}

			// If we go first, then the second units are the enemy ones;
			Units& enemyUnits = g_Interface->client().GetUnitsGoingSecond();
			for(Unit*& enemyUnit : enemyUnits)
			{
				enemyUnit->BattleRenderEnemy();
			}
		}
		// We are going second;
		else
		{
			// If we go second, then the first units are the enemy ones;
			Units& enemyUnits = g_Interface->client().GetUnitsGoingFirst();
			for (Unit*& enemyUnit : enemyUnits)
			{
				enemyUnit->BattleRenderEnemy();
			}

			// If we go second, then the second units are the friendly ones;
			Units& friendlyUnits = g_Interface->client().GetUnitsGoingSecond();
			for (Unit*& friendlyUnit : friendlyUnits)
			{
				friendlyUnit->BattleRender();
			}
		}

		Units& fUnits = g_Interface->client().GetUnitsGoingFirst();
		Units& sUnits = g_Interface->client().GetUnitsGoingSecond();
		for(Unit*& fUnit : fUnits)
		{
			fUnit->RenderAbilities();
		}
		for(Unit*& sUnit : sUnits)
		{
			sUnit->RenderAbilities();
		}
	}

	// Rendering for the Purchase Phase;
	else if(query().GetCurrentPhase() == Phase::PURCHASE)
	{
		m_match.m_purchaseMap->Render();

		for (Unit*& unit : m_units)
		{
			unit->PurchaseRender();
		}

		for(Card*& card : m_cards)
		{
			card->PurchaseRender();
		}
	}
}

// ----------------------------------------------------------------------------
Player* Interface::CreatePlayer(int playerID_, bool isAIPlayer_ /*= false*/)
{
	Player* player = nullptr;

	if(!isAIPlayer_)
	{
		player = new HumanPlayer(playerID_);
	}
	else
	{
		player = new AIPlayer(playerID_);
	}
	

	return player;
}

// ----------------------------------------------------------------------------
void Interface::SetClientPlayer(Player* player_)
{
	m_clientPlayer = player_;
}

// ----------------------------------------------------------------------------
Player*& Interface::CreateAIEnemyPlayer(int playerID_)
{
	if(m_enemyPlayer)
	{
		delete m_enemyPlayer;
		m_enemyPlayer = nullptr;
	}

	m_enemyPlayer = new AIPlayer(playerID_);

	return m_enemyPlayer;
}

// ----------------------------------------------------------------------------
Player*& Interface::GetPlayer()
{
	return m_clientPlayer;
}

// ----------------------------------------------------------------------------
Player*& Interface::GetEnemy()
{
	return m_enemyPlayer;
}

// ----------------------------------------------------------------------------
unsigned int Interface::GetCardIDAndIncrementCounter()
{
	m_cardIDCounter++;
	return m_cardIDCounter;
}

// ----------------------------------------------------------------------------
unsigned int Interface::GetUnitIDAndIncrementCounter()
{
	m_unitIDCounter++;
	return m_unitIDCounter;
}

// ----------------------------------------------------------------------------
void Interface::ClearUnitsCardsPlayers()
{
	for (int u = 0; u < m_units.size(); ++u)
	{
		DELETE_POINTER(m_units[u]);
	}
	m_units.clear();

	for (int c = 0; c < m_cards.size(); ++c)
	{
		DELETE_POINTER(m_cards[c]);
	}
	m_cards.clear();

	for (int p = 0; p < m_players.size(); ++p)
	{
		DELETE_POINTER(m_players[p]);
	}
	m_players.clear();

	DELETE_POINTER(m_clientPlayer);
	DELETE_POINTER(m_enemyPlayer);
}

// ----------------------------------------------------------------------------
MatchReport::MatchReport(int winningPlayerID_, int losingPlayerID_, int damageDealtToLosingPlayer_, int matchID_, bool ignore_)
{
	m_winningPlayerID = winningPlayerID_;
	m_losingPlayerID = losingPlayerID_;
	m_damageDealtToLosingPlayer = damageDealtToLosingPlayer_;
	m_matchID = matchID_;
	m_ignore = ignore_;
}

// ----------------------------------------------------------------------------
int MatchReport::GetWinningPlayerID()
{
	return m_winningPlayerID;
}

// ----------------------------------------------------------------------------
int MatchReport::GetLosingPlayerID()
{
	return m_losingPlayerID;
}

// ----------------------------------------------------------------------------
int MatchReport::GetDamageDealtToLosingPlayer()
{
	return m_damageDealtToLosingPlayer;
}

// ----------------------------------------------------------------------------
int MatchReport::GetMatchID()
{
	return m_matchID;
}

// ----------------------------------------------------------------------------
bool MatchReport::GetIgnore()
{
	return m_ignore;
}

// ----------------------------------------------------------------------------
bool MatchReport::operator==(const MatchReport& compare) const
{
	if(m_ignore)
	{
		return true;
	}

	return m_winningPlayerID				== compare.m_winningPlayerID
		&& m_losingPlayerID					== compare.m_losingPlayerID
		&& m_damageDealtToLosingPlayer		== compare.m_damageDealtToLosingPlayer
		&& m_matchID						== compare.m_matchID;
}
