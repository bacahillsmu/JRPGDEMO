#pragma once

// -----------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

// -----------------------------------------------------------------------
#include "Game/Cards/CardDefinition.hpp"

enum class CardArea
{
	INVALID = -1,

	MARKET,
	HAND,
	DECK,
	UNITFIELD,
	FLOATING,

	CARDAREA_COUNT
};

class Card
{

public:

	Card() = delete;
	Card(const std::string& cardType_);
	Card(CardType cardType_);
	~Card();

	void Create(CardType cardType_);
	void PurchaseUpdate(float deltaSeconds_);
	void PurchaseRender();

	int m_playerID = -1;
	int m_slotID = -1;
	unsigned int m_cardID = 0;

	CardArea m_cardArea = CardArea::INVALID;

	CardDefinition* m_cardDefinition = nullptr;
	CardType m_type = CardType::INVALID;
	int m_health = 0;
	int m_strength = 0;
	int m_intellect = 0;
	int m_wisdom = 0;
	int m_constitution = 0;
	int m_speed = 0;



	// Sprite Information;
	SpriteDefinition m_currentSpriteDefinition;
	Vec2 m_currentSpriteAnimationBottomLeftUV = Vec2(0.0f, 0.0f);
	Vec2 m_currentSpriteAnimationToptUV = Vec2(0.0f, 0.0f);



};