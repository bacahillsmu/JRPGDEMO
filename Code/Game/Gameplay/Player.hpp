#pragma once

#include "Game/Units/Units.hpp"
#include "Game/Cards/Cards.hpp"

#include <vector>

// Abstract Base Class for Players;
class Player
{

public:

	virtual ~Player(){}

	virtual void Update(float deltaSeconds_) = 0;

	/*
	virtual void GetMyUnits() = 0;
	virtual void GetMyCards() = 0;
	*/
	
	virtual bool IsAIPlayer() const = 0;

	// Setters;
	void SetPlayerUsername(const std::string& username_);
	void SetPlayerID(int playerID_);
	void SetMatchID(int matchID_);
	void SetPlayerHealth(int healthValue_);
	void AddToPlayerHealth(int valueToChangeHealthBy_);
	void SubtractFromPlayerHealth(int valueToChangeHealthBy_);
	void ClampHealth();
	void SetUnits(const Units& units_);
	void SetCards(const Cards& cards_);
	void SetActualGoldAmount(int actualGold_);
	void SetGoldAmount(int goldAmount_);
	void ChangeActualGoldAmount(int valueToChangeActualGoldAmountBy_);
	void ChangeGoldAmount(int valueToChangeGoldAmountBy_);
	void ClampActualGoldAmount();
	void ClampGoldAmount();
	void SetMarketplaceLocked(bool lock_);
	void SetPurchasePhaseComplete(bool complete_);
	void SetBattlePhaseComplete(bool complete_);
	void SetEnemyPlayer(Player*& player_);

	// Resets;
	void ResetMatchID();

	// Getters;
	const std::string& GetPlayerUsername();
	const int GetPlayerID() const;
	const int GetMatchID() const;
	const int GetPlayerHealth() const;
	const Units& GetUnits();
	const Cards& GetCards();
	const int GetMaxHandCount();
	const int GetActualGold();
	const int GetGoldAmount();
	const int GetMaxGoldAmount();
	const bool GetMarketplaceLocked();
	const bool GetPurchasePhaseComplete();
	const bool BattlePhaseComplete();
	Player*& GetEnemyPlayer();

	// Cards;
	void RemoveMyMarketCards();
	void RemoveMyHandCards();
	
	void AddCardToMyCards(Card*& card_);

	// Units;
	void AddUnitToMyUnits(Unit*& unit_);
	void RemoveMyUnits();
	void SetEnemyUnits(Units& units_);
	void RemoveEnemyUnits();
	
	// Enemy Information;

	// Turn Information;
	void SetGoesFirstForBattlePhase(bool goesFirst_);
	bool GetGoesFirstForBattlePhase();
	void SetSeedToUseForRNG(unsigned int seed_);
	void SetRandomNumberGeneratorSeed();
	void SetSeedPosition(unsigned int newSeedPosition_);

	void SetJustDied(bool justDied_);
	bool GetJustDied();

protected:

	std::string m_playerUsername = "";
	int m_playerID = -1;
	int m_matchID = -1;
	int m_playerHealth = 20;
	int m_playerMaxHealth = 20;
	Units m_units;
	Cards m_cards;
	int m_maxHandCount = 3;

	Units m_enemyUnits;
	Player* m_enemyPlayer = nullptr;
	bool m_goesFirst = false;
	unsigned int m_seed = 0;
	unsigned int m_seedPosition = 0;

	int m_actualGold = 3;
	int m_goldAmount = 3;
	int m_maxGold = 10;

	// Information stored in ConnectedClients;
	bool m_marketplaceLocked = false;
	bool m_purchasePhaseComplete = false;
	bool m_battlePhaseComplete = false;

	bool m_justDied = false;

};

// Class to be used by Human Players (the player may switch to be an AI player upon death);
class HumanPlayer : public Player
{

public:

	HumanPlayer(int playerID_);
	~HumanPlayer();

	// Flow;
	void Update(float deltaSeconds_) override;

	//void GetMyUnits() override;
	//void GetMyCards() override;

	virtual bool IsAIPlayer() const override { return false; }

private:

	

};

// Class to be used by AI Players;
class AIPlayer : public Player
{
public:

	AIPlayer(int playerID_);
	~AIPlayer();

	// Flow;
	void Update(float deltaSeconds_);

	//void GetMyUnits();
	//void GetMyCards();

	virtual bool IsAIPlayer() const override { return true; }

private:

	
};