#pragma once

#include "Game/Units/Units.hpp"
#include "Game/Units/UnitDefinition.hpp"
#include "Game/Cards/Cards.hpp"
#include "Game/Cards/CardDefinition.hpp"
#include "Game/Gameplay/Players.hpp"

#include <vector>
#include <map>
#include <functional>

class Game;
class Unit;
class Card;
class Player;
class HumanPlayer;
class AIPlayer;
class BattleMap;
class PurchaseMap;
class SpriteSheet;
class Ability;

typedef std::function<bool(const Unit* unit_)> UnitFilter;
typedef std::function<bool(const Card* card_)> CardFilter;
typedef std::function<bool(const Player* player_)> PlayerFilter;

enum class Phase
{
	INVALID = -1,

	PREGAME,
	BATTLE,
	PURCHASE,

	PHASE_COUNT
};

struct MatchReport
{

public:

	MatchReport(int winningPlayerID_, int losingPlayerID_, int damageDealtToLosingPlayer_, int matchID_, bool ignore_ = false);

	int GetWinningPlayerID();
	int GetLosingPlayerID();
	int GetDamageDealtToLosingPlayer();
	int GetMatchID();
	bool GetIgnore();

	bool operator==(const MatchReport& compare) const;

private:

	int m_winningPlayerID = -1;
	int m_losingPlayerID = -1;
	int m_damageDealtToLosingPlayer = -1;
	int m_matchID = -1;
	bool m_ignore = false;

};

// ----------------------------------------------------------------------------
// Action; Allows actions to be done by units; Controlled by the Interface;
// ----------------------------------------------------------------------------
struct Action
{

public:

	Action(Units& units_, Cards& cards_);

	void AutoAttack(const Unit* attackingUnit_, const Unit* attackedUnit_);

	

private:

	Units& m_units;
	Cards& m_cards;
};

// ----------------------------------------------------------------------------
// Control; Allows interaction with the game; Controlled by the Interface;
// ----------------------------------------------------------------------------
struct Match
{

public:

	Match(Units& units_, Cards& cards_);
	~Match();

	void Init();
	void Startup();
	void Update();
	void Render();

	// Sprites;
	void CreateSpriteSheets();
	void SwitchPhases();

	

	// Hand;
	//void RemoveCardSlotInHand(int cardSlot_);

	// Deck;
	void CreateCommunalDeck();

	// Maps;
	BattleMap*& GetBattleMap();
	PurchaseMap*& GetPurchaseMap();

	// Why isn't jobIcons inside m_spriteSheets?
	std::map<std::string, SpriteSheet*> m_unitSpriteSheets;
	std::map<std::string, SpriteSheet*> m_abilitySpriteSheets;
	SpriteSheet* m_jobIcons;

	BattleMap* m_battleMap = nullptr;
	PurchaseMap* m_purchaseMap = nullptr;
	
	// Deck Counts;
	int blackmageCount		= 16;
	int archerCount			= 16;
	int dragoonCount		= 16;
	int paladinCount		= 16;
	int warriorCount		= 16;
	int knightCount			= 16;
	int whitemageCount		= 16;

private:

	Units& m_units;
	Cards& m_cards;
};

// ----------------------------------------------------------------------------
// Query; Allows queries about the game and game state; Controlled by the Interface;
// ----------------------------------------------------------------------------
struct Query
{

public:

	Query(Units& units_, Cards& cards_, Players& players_);

	// Filters;
	int GetUnitCount(const UnitFilter& filter_) const;
	int GetCardCount(int playerID_, const CardFilter& filter_) const;
	int GetCardCount(const CardFilter& filter_) const;
	Units GetUnits(const UnitFilter& filter_) const;
	Units GetUnits(Units& unitsToFilter_, const UnitFilter& filter_);
	Units GetLeastDamagedUnitsExcludingDeadUnits(Units& unitsToFilter_) const;
	Units GetMostDamagedUnitsExcludingDeadUnits(Units& unitsToFilter_) const;
	Cards GetCards(const CardFilter& filter_) const;
	Cards GetCards(Cards& cardsToFilter_, const CardFilter& filter_) const;
	Card* GetCard(const CardFilter& filter_);
	Players GetPlayers(const PlayerFilter& filter_) const;
	Players GetPlayers(Players& playersToFilter_, const PlayerFilter& filter_) const;
	Player* GetPlayer(const PlayerFilter& filter_) const;


	// Game Queries;
	Phase GetCurrentPhase();

private:

	Units& m_units;
	Cards& m_cards;
	Players& m_players;
};

// ----------------------------------------------------------------------------
// Debug;
// ----------------------------------------------------------------------------
struct Debug
{

public:

	Debug(Units& units_, Cards& cards_);

	

private:

	Units& m_units;
	Cards& m_cards;
};

// ----------------------------------------------------------------------------
// Server; Handling direct communication between game and the server;
// This and Client are only areas where RakNet types will be used;
// ----------------------------------------------------------------------------
struct Server
{

public:

	Server(Units& units_, Cards& cards_, Players& players_);
	~Server();

	// Flow;
	void Update(float deltaSeconds_);

	// Starting the Game;
	bool PreGameChecksAndSetups(float deltaSeconds_);
	void SendStartMessageIfHaveNotSent();
	void CountdownToStartGame(float deltaSeconds_);
	
	// Ending the Game;
	void KillConnectionWithAllClients();
	void KillConnectionWithClient(Player* player_);

	// Players;
	void CreatePlayers();
	void ResetMatchIDsOnPlayers();
	void AssignMatchIDsForPlayers();
	void SendHumanPlayerTheirEnemy(int playerID_, Player*& enemyPlayer_);
	void SendAIPlayerTheirEnemy(int playerID_, Player*& enemyPlayer_);
	void SendPlayerTheirMatchID(int playerID_, int matchID_);
	void SendPlayerIfTheyGoFirstForBattlePhase(int playerID_, bool goesFirst_);
	void SendPlayerTheirSeedForRandomNumberGenrator(int playerID_, unsigned int seed_);
	void SendPlayerTheirUpdatedHealth(int playerID_, int updatedHealth_);
	void RemoveDeadPlayersFromServerPlayers();
	void SendYouWinTheGameMessageToPlayerID(int playerID_);
	void SendYouLostTheGameMessageToPlayerID(int playerID_);

	// Phases;
	void SwitchPhases();
	Phase GetCurrentPhase();
	void SendAllClientsPhaseInformationForPurchasePhase();
	void SendAllClientsPhaseInformationForBattlePhase();

	// Upkeep;
	void GiveAllPlayersMaxGoldForTheTurn();
	void GiveAllPlayersGoldIncrease(int goldIncreaseAmount_ = 1);
	void SendAllClientsTheirGoldValue();
	
	// Market;
	int GetMaxMarketplaceCards();
	void RollAndSendMarketplaceCardsForClient(Player*& player_);
	void ClearMarketplaceCardsForClient(int playerID_);
	void SendCardTypesToPlayerIDForMarketplace(Cards& rolledCardsForMarketPlace_, int playerID_);
	void UpdateMarketplaceCardsBasedOnPurchaseByPlayer(unsigned int cardID_, int playerID_);
	void GetMarketplaceCardsForPlayerID(int playerID_);
	void RollMarketplaceCardsForAIPlayer(Player*& player_);
	
	// Hand;
	void GetAndSendCardsInHandForPlayerID(const int playerID_);
	void SendCardTypesToPlayerIDForHand(Cards cardsInHand_, int playerID_);
	void RemoveCardFromSellingByPlayer(unsigned int cardID_);
	void RemoveCardFromPlacingUnitByPlayer(unsigned int cardID_);

	// Units;
	void GetAndSendUnitsForPlayerID(const int playerID_);
	void SendUnitTypesToPlayerIDForField(Units units_, const int playerID_);
	void SendUnitTypesToAIPlayerIDForField(Units units_, const int playerID_);
	void SendEnemyUnitTypesToPlayerIDForEnemyField(Units& enemyUnits_, const int playerID_);
	void SendEnemyUnitTypesToAIPlayerIDForEnemyField(Units& enemyUnits_, const int playerID_);
	void CreateNewUnitFromCardPlacedByPlayer(unsigned int cardID_, int unitSlotID_);
	void GiveAllEntitiesMaxHealth();

	// Match Report;
	void CreateMatchReport(int winningPlayerID_, int losingPlayerID_, int damageDealtToLosingPlayer_, int matchID_, bool ignore_ = false);
	void VerifyAndProcessEachMatchReport();
	bool CheckForWinnerAndLoser();
	void ProcessMatchReports(std::vector<MatchReport>& matchReports);
	std::vector<MatchReport> GetMatchReportsOfMatchID(int matchID_);

private:

	// Players;
	// std::vector<Player*> m_serverPlayers;

	// Starting Game Timer;
	bool m_startMessageSent = false;
	bool m_startGameMessageSent = false;
	float m_startGameCountdownTimer = 3.5f;
	int m_lastFrameTimer = 0;

	// Phase;
	Phase m_currentPhase = Phase::PREGAME;

	// Purchase Phase;
	bool m_allClientsSaidDoneWithPurchasePhase = false;

	// Battle Phase;
	bool m_allClientsSaidDoneWithBattlePhase = false;
	int m_matchCount = 0;
	bool m_allMatchesReportedBack = false;
	std::vector<MatchReport> m_matchReports;

	// Shared Units and Cards;
	int m_maxMarketplaceCards = 3;
	Units& m_units;
	Cards& m_cards;
	Players& m_players;

	bool m_isGameOver = false;
};

// ----------------------------------------------------------------------------
// Server; Handling direct communication between game and the server;
// This and Server are only areas where RakNet types will be used;
// ----------------------------------------------------------------------------
struct Client
{

public:

	Client(Units& units_, Cards& cards_);

	// Flow;
	void Update(float deltaSeconds_);
	void UpdateUnits(float deltaSeconds_);

	void SendRequestForHandCardsToServer(int playerID_);
	void SendRequestForMarketplaceCardsToServer(int playerID_);
	void SendRequestForClearingMarketplaceToServer(int playerID_);

	// Phase;
	void SwitchPhases();
	void SendBattlePhaseCompleteToServer();
	void SendPurchasePhaseCompleteToServer();
	Phase GetCurrentPhase();
	bool IsMessageSentToServerForPurchasePhaseComplete();
	bool IsMessageSentToServerForBattlePhaseComplete();
	void SetMessageSentToServerForPurchasePhaseComplete(bool messageSent_);
	void SetMessageSentToServerForBattlePhaseComplete(bool messageSent_);
	
	// Market;
	void CleanupMarketplaceCards();
	void CleanupMarketplaceCardsAndRequestNewCards();
	void RollMarketplaceCards();
	void ToggleMarketplaceLock();
	void CreateCardForMarketplace(CardType cardType_, unsigned int cardID_);
	bool IsMarketplaceLocked();
	void SendMarketplaceLockedToServer(bool marketplaceLocked_);
	void SendPurchasedCardFromMarketplaceToServer(unsigned int purchasedCardID_, int playerID_);
	void SendSoldCardFromHandToServer(unsigned int soldCardID_, int playerID_);
	void SendUnitCreatedAtFromCardInHandToServer(unsigned int placeCardID_, int unitSlotID, int playerID_);

	// Hand;
	void CreateCardForHand(CardType cardType_, unsigned int cardID_);
	void CleanupHandCards();

	// Unit;
	void CreateUnitForField(JobType jobType_, int unitID_, int slotID_);
	void CreateEnemyUnitForEnemyField(JobType jobType_, int unitID_, int slotID_);
	void CleanupUnits();
	void CleanupEnemyUnits();
	void GiveAllEntitiesMaxHealth();

	// Running Battle Simulation;
	void SetMatchIDForThisBattlePhase(int matchID_);
	void RunAttackSimulationOfAttackingVsDefending(Units& attackingUnits, Units& defendingUnits);
	bool CheckForWinnerOfBattlePhase();
	void SendMatchReportToServer(int winningPlayerID_, int losingPlayerID_, int dmageDealtToLosingPlayer_, int matchID_);
	Units& GetUnitsGoingFirst();
	Units& GetUnitsGoingSecond();
	void AssignTargetAndCasterForMainAbility(Ability*& mainAbility_, Unit*& attackingUnit_, Units& attackingUnits_, Units& defendingUnits_);

	// Player;
	void UpdatePlayerHealth(int updatedPlayerHealth_);

private:

	// Phase;
	Phase m_currentPhase = Phase::PREGAME;
	bool m_messageSentToServerForPurchasePhaseComplete = false;
	bool m_messageSentToServerForBattlePhaseComplete = false;

	// Market;
	bool m_marketplaceLocked = false;
	
	// Battle Phase Information To Temp Hold Onto;
	Unit* m_actionUnit = nullptr;
	Units m_enemyUnits;
	int m_matchID = -1;
	
	Units m_unitsGoingFirst;
	Units m_unitsGoingSecond;
	bool m_isFirstPlayersTurn = true;
	int m_firstPlayersAttackingUnitIndex = 0;
	int m_secondPlayersAttackingUnitIndex = 0;
	
	// Sound;
	bool m_purchasePhaseMusicPlaying = false;
	bool m_battlePhaseMusicPlaying = false;
	SoundPlaybackID m_purchasePhasePlaybackID;
	SoundPlaybackID m_battlePhasePlaybackID;

	// Shared Units and Cards;
	Units& m_units;
	Cards& m_cards;
};

// ----------------------------------------------------------------------------
// Interface; The highest level which updates submodules; Holds and allows interaction with the submodules;
// ----------------------------------------------------------------------------
class Interface
{

public:

	Interface(Game* game_);
	~Interface();

	Action&		action()	{ return m_action; }
	Match&		match()		{ return m_match; }
	Query&		query()		{ return m_query; }
	Debug&		debug()		{ return m_debug; }
	Server&		server()	{ return m_server; }
	Client&		client()	{ return m_client; }

	void Init();
	void Startup();
	void Render();

	Player* CreatePlayer(int playerID_, bool isAIPlayer_ = false);
	void SetClientPlayer(Player* player_);
	Player*& CreateAIEnemyPlayer(int playerID_);
	Player*& GetPlayer();
	Player*& GetEnemy();

	unsigned int GetCardIDAndIncrementCounter();
	unsigned int GetUnitIDAndIncrementCounter();

	void ClearUnitsCardsPlayers();
	
	

private:

	Action m_action;
	Match m_match;
	Query m_query;
	Debug m_debug;
	Server m_server;
	Client m_client;

	Units m_units;
	Cards m_cards;
	Players m_players;

	unsigned int m_cardIDCounter = 0u;
	unsigned int m_unitIDCounter = 0u;
	
	Player* m_clientPlayer = nullptr;
	Player* m_enemyPlayer = nullptr;

	

	int m_playersTurnToAttack = 0;
	int m_player0sMinionsTurnToAttack = 0;
	int m_player1sMinionsTurnToAttack = 0;

	Game* m_game = nullptr;
};

extern Interface* g_Interface;



