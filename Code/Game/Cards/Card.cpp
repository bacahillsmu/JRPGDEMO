#include "Game/Cards/Card.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Framework/GameCommon.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Engine/Core/WindowContext.hpp"

// ------------------------------------------------------------------
#include "Game/Framework/App.hpp"
#include "Game/Framework/Interface.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Cards/Cards.hpp"
#include "Game/Cards/CardDefinition.hpp"
#include "Game/Cards/CardFilters.hpp"

// ------------------------------------------------------------------
// Card;
// ------------------------------------------------------------------
Card::Card(const std::string& cardType_)
{
	CardType unitType = CardDefinition::StringToUnitType(cardType_);
	Create(unitType);
}

// ------------------------------------------------------------------
Card::Card(CardType cardType_)
{
	Create(cardType_);
}

// ------------------------------------------------------------------
Card::~Card()
{

}

// ------------------------------------------------------------------
void Card::Create(CardType cardType_)
{
	// #ToDo: Get this working;
	/*
	// Set the GameHandle;
	uint16_t slot = m_theMap->GetFreeEntityIndex();
	uint16_t cyclicId = m_theMap->GetNextCyclicId();
	GameHandle handle = GameHandle(cyclicId, slot);
	m_gameHandle = handle;
	*/

	// Get the Card Definition;
	std::map<CardType, CardDefinition*>::iterator cardIter = CardDefinition::s_cardDefinitions.find(cardType_);
	if (cardIter == CardDefinition::s_cardDefinitions.end())
	{
		ERROR_AND_DIE("Attempted to Create a Card with a card type that is not loaded.")
	}
	m_cardDefinition = cardIter->second;

	// Set Stats;
	m_type			= cardType_;
	m_health		= m_cardDefinition->m_health;
	m_strength		= m_cardDefinition->m_strength;
	m_intellect		= m_cardDefinition->m_intellect;
	m_wisdom		= m_cardDefinition->m_wisdom;
	m_constitution	= m_cardDefinition->m_constitution;
	m_speed			= m_cardDefinition->m_speed;
}

// ------------------------------------------------------------------
void Card::PurchaseUpdate(float deltaSeconds_)
{
	deltaSeconds_;
}

// ------------------------------------------------------------------
void Card::PurchaseRender()
{
	std::vector<Vertex_PCU> cardVerts;
	Vec2 dimensions;
	Vec2 centerSlot;
	switch (m_cardArea)
	{
		case CardArea::MARKET:
		{
			dimensions = g_Interface->match().GetPurchaseMap()->m_marketSlotDimensions;
			centerSlot = g_Interface->match().GetPurchaseMap()->GetCenterPositionOfCardSlotInMyMarket(m_slotID);
			break;
		}
		case CardArea::HAND:
		{
			dimensions = g_Interface->match().GetPurchaseMap()->m_handSlotDimensions;
			centerSlot = g_Interface->match().GetPurchaseMap()->GetCenterPositionOfCardSlotInMyHand(m_slotID);
			break;
		}
		case CardArea::FLOATING:
		{
			dimensions = g_Interface->match().GetPurchaseMap()->m_handSlotDimensions;
			IntVec2 mouse = g_theWindowContext->GetClientMousePosition();
			centerSlot = Vec2((float)mouse.x, (float)mouse.y);
			break;
		}
		default:
		{
			ERROR_AND_DIE("A card is trying to render, but does not have an area to render.");
			break;
		}
	}

	
	AABB2 box = AABB2(centerSlot, dimensions / 2);
	AddVertsForAABB2D(cardVerts, box, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_cardDefinition->m_cardTexture));
	g_theRenderer->DrawVertexArray((int)cardVerts.size(), &cardVerts[0]);

	std::vector<Vertex_PCU> jobVerts;
	float height = box.maxs.y - box.mins.y;
	float width = box.maxs.x - box.mins.x;
	Vec2 start = Vec2(box.mins.x + (width * 0.15f), box.mins.y + (height * 0.70f));
	Vec2 end = Vec2(box.maxs.x - (width * 0.15f), box.maxs.y - (height * 0.15f));
	AABB2 box2 = AABB2::MakeFromMinsMaxs(start, end);
	m_currentSpriteDefinition.GetUVs(m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);
	AddVertsForAABB2D(jobVerts, box2, Rgba::WHITE, m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_cardDefinition->m_jobTexture));
	g_theRenderer->DrawVertexArray((int)jobVerts.size(), &jobVerts[0]);

	if(g_Interface->client().IsMarketplaceLocked() && m_cardArea == CardArea::MARKET)
	{
		std::vector<Vertex_PCU> lockVerts;
		AABB2 box3 = AABB2(box2.center, 2.0f);
		AddVertsForAABB2D(lockVerts, box3, Rgba::WHITE);
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile("Data/Sprites/Lock.png"));
		g_theRenderer->DrawVertexArray((int)lockVerts.size(), &lockVerts[0]);
	}
}
