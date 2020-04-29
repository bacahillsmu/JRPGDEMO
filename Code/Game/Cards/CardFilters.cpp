#include "Game/Cards/CardFilters.hpp"

// ----------------------------------------------------------------------------
// MultiFilter;
// ----------------------------------------------------------------------------
CardMultiFilter::CardMultiFilter(Selector selector_, std::initializer_list<CardFilter> filters_)
	: m_selector(selector_)
	, m_filters(filters_)
{
}

// ----------------------------------------------------------------------------
bool CardMultiFilter::operator()(const Card* card_) const
{
	if (m_selector == Selector::AND)
	{
		for (auto& filter : m_filters)
		{
			if (!filter(card_))
			{
				return false;
			}
		}

		return true;
	}
	else if (m_selector == Selector::OR)
	{
		for (auto& filter : m_filters)
		{
			if (filter(card_))
			{
				return true;
			}
		}

		return false;
	}

	return false;
}

// ----------------------------------------------------------------------------
// Player Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CardBelongsToPlayerID::CardBelongsToPlayerID(int playerID_)
	: m_playerID(playerID_)
{
}

bool CardBelongsToPlayerID::operator()(const Card* card_) const
{
	return card_->m_playerID == m_playerID;
}

// ----------------------------------------------------------------------------
CardBelongsToNotPlayerID::CardBelongsToNotPlayerID(int playerID_)
	: m_playerID(playerID_)
{
}

bool CardBelongsToNotPlayerID::operator()(const Card* card_) const
{
	return card_->m_playerID != m_playerID;
}

// ----------------------------------------------------------------------------
// Market Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
bool CardInMarketplace::operator()(const Card* card_) const
{
	return card_->m_cardArea == CardArea::MARKET;
}

// ----------------------------------------------------------------------------
bool CardInDeck::operator()(const Card* card_) const
{
	return card_->m_cardArea == CardArea::DECK;
}

// ----------------------------------------------------------------------------
bool CardInHand::operator()(const Card* card_) const
{
	return card_->m_cardArea == CardArea::HAND;
}

// ----------------------------------------------------------------------------
// Simple Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
bool IsKnightCard::operator()(const Card* card_) const
{
	return card_->m_type == CardType::KNIGHT;
}

// ----------------------------------------------------------------------------
HasCardID::HasCardID(unsigned int cardID_)
	: m_cardID(cardID_)
{
}

bool HasCardID::operator()(const Card* card_) const
{
	return card_->m_cardID == m_cardID;
}

