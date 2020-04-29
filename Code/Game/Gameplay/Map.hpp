#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"

// Why is this pure virtual? I dont think it should be.

class Player;

class Map
{

public:

	Map();
	~Map();

	// Flow;
	virtual void Startup() = 0;
	virtual void StartTimer() = 0;
	virtual void Update(float deltaSeconds_) = 0;
	virtual void Render() = 0;

public:

	static constexpr float WIDTH = 100.0f;
	static constexpr float HEIGHT = 50.0f;

public:

	
	


};

class BattleMap : public Map
{

public:

	// Flow;
	void Startup() override;
	void StartTimer() override;
	void Update(float deltaSeconds_) override;
	void Render() override;

	// Creating the BattleMap;
	void CreatePlayingFields();
	void CreateUnitSlotsForPlayingFields(); 
	void CreateFriendlyUsernameDisplay();
	void CreateEnemyUsernameDisplay();
	void CreateBattleInformationWindows();
	void CreateBattleInformationWindowUnitSlots();

	int GetSlotForPosition(const Vec2& position_);
	Vec2 GetCenterPositionOfSlotInMyField(int slot_);
	Vec2 GetCenterPositionOfSlotInEnemyField(int slot_);

	void ResetActionTimer();

public:

	int m_unitSlotsPerField = 8;

	// Battle Phase Map Information;
	AABB2 m_friendlyField;
	AABB2 m_enemyField;
	std::vector<AABB2> m_friendlyFieldSlots;
	std::vector<AABB2> m_enemyFieldSlots;
	Vec2 m_unitSlotDimensions;

	AABB2 m_background = AABB2();
	std::string m_nameOfCurrentBattleBackground = "";
	AABB2 m_friendlyUsernameDisplay;
	AABB2 m_enemyUsernameDisplay;

	AABB2 m_friendlyBattleInformationWindow;
	AABB2 m_enemyBattleInformationWindow;
	std::vector<AABB2> m_friendlyBattleInformationUnitSlots;
	std::vector<AABB2> m_enemyBattleInformationUnitSlots;
	Vec2 m_battleWindowUnitSlotDimensions;

	//float m_timer = 0.0f;
	//float m_timeForBattlePhase = 60.0f;

	float m_actionTimer = 0.0f;
	float m_baseTimeBetweenAction = 2.0f;
	bool m_actionTurnEnding = false;
};

class PurchaseMap : public Map
{

public:

	// Flow;
	void Startup() override;
	void StartTimer() override;
	void Update(float deltaSeconds_) override;
	void Render() override;

	// Creating the various fields and slots of the Purchase Map;
	void CreateOurUnitsField();
	void CreateOurUnitsSlots();
	void CreateMarketField();
	void CreateMarketSlots();
	void CreateHandField();
	void CreateHandSlots();
	void CreateRerollButton();
	void CreateFreezeButton();
	void CreateTimerDisplay();
	void CreateGoldDisplay();
	void CreateGoldSlots();
	void CreateUsernameDisplay();
	// Not implemented;
	void CreateNextOpponentUnitLevelsArea();

	// Queries for our unit field and slots;
	int GetUnitSlotForPosition(const Vec2& position_);
	Vec2 GetCenterPositionOfSlotInMyField(int slot_);

	// Queries for our market field and slots;
	int GetCardMarketSlotForPosition(const Vec2& position_);
	Vec2 GetCenterPositionOfCardSlotInMyMarket(int slot_);

	// Queries for our hand field and slots;
	int GetCardHandSlotForPosition(const Vec2& position_);
	Vec2 GetCenterPositionOfCardSlotInMyHand(int slot_);

	// Queries for Freeze and Reroll;
	bool DidClickReroll(const Vec2& position_);
	bool DidClickLock(const Vec2& position_);

	// Mouse Click Consumption Checks;
	// Left
	bool MouseClickForMarketplace(Player* player_, const Vec2& mouseClick_);
	bool MouseClickForReroll(Player* player_, const Vec2& mouseClick_);
	bool MouseClickForLock(const Vec2& mouseClick_);
	bool MouseClickForSellingCardInHand(Player* player_, const Vec2& mouseClick_);
	bool MouseClickForPlacingUnitInSlot(Player* player_, const Vec2& mouseClick_);
	// Right
	bool MouseClickForSelectingUnitFromHand(const Vec2& mouseClick_);


public:

	int m_unitSlotsPerField = 8;
	Vec2 m_unitSlotDimensions;
	Vec2 m_marketSlotDimensions;
	Vec2 m_handSlotDimensions;
	Vec2 m_goldSlotDimensions;

	AABB2 m_ourUnitsField;
	std::vector<AABB2> m_ourUnitSlots;
	bool m_rightMouseFocus = false;
	unsigned int m_cardIDInHandToBePlaced = 0u;
	int m_cardSlotInHandToBePlaced = -1;

	AABB2 m_background = AABB2();
	AABB2 m_ourMarketField;
	std::vector<AABB2> m_ourMarketSlots;

	AABB2 m_ourHandField;
	std::vector<AABB2> m_ourHandSlots;

	AABB2 m_goldDisplay;
	std::vector<AABB2> m_ourGoldSlots;

	AABB2 m_rerollButton;
	AABB2 m_freezeButton;
	AABB2 m_timerDisplay;
	AABB2 m_usernameDisplay;
	float m_timer = 0.0f;
	float m_timeForPurchasePhase = 30.0f;

};