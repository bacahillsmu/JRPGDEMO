#pragma once

#include "Game/Framework/Interface.hpp"
#include "Game/Cards/Cards.hpp"

#include <initializer_list>


// ----------------------------------------------------------------------------
// MultiFilter;
// ----------------------------------------------------------------------------
struct CardMultiFilter
{
	enum class Selector
	{
		AND,
		OR
	};

	CardMultiFilter(Selector selector_, std::initializer_list<CardFilter> filters_);

	bool operator()(const Card* card_) const;

private:

	std::vector<CardFilter> m_filters;
	Selector m_selector;
};

// ----------------------------------------------------------------------------
// Player Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
struct CardBelongsToPlayerID
{
	explicit CardBelongsToPlayerID(int playerID_);

	bool operator()(const Card* card_) const;

private:

	int m_playerID;

};

// ----------------------------------------------------------------------------
struct CardBelongsToNotPlayerID
{
	explicit CardBelongsToNotPlayerID(int playerID_);

	bool operator()(const Card* card_) const;

private:

	int m_playerID;

};

// ----------------------------------------------------------------------------
// Market Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
struct CardInMarketplace
{
	bool operator()(const Card* card_) const;
};

// ----------------------------------------------------------------------------
struct CardInDeck
{
	bool operator()(const Card* card_) const;
};

// ----------------------------------------------------------------------------
struct CardInHand
{
	bool operator()(const Card* card_) const;
};

// ----------------------------------------------------------------------------
// Simple Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
struct IsKnightCard
{
	bool operator()(const Card* card_) const;
};

// ----------------------------------------------------------------------------
struct HasCardID
{
	explicit HasCardID(unsigned int cardID_);

	bool operator()(const Card* card_) const;

private:

	unsigned int m_cardID = 0u;
};