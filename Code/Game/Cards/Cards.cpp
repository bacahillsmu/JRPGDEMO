#include "Game/Cards/Cards.hpp"

#include "Engine/Core/RandomNumberGenerator.hpp"

// ----------------------------------------------------------------------------
Cards::Cards(const std::vector<Card*>& cards_)
	: m_cards(cards_)
{
}

// ----------------------------------------------------------------------------
std::vector<Card*>::iterator Cards::erase(std::vector<Card*>::iterator a)
{
	return m_cards.erase(a);
}

// ----------------------------------------------------------------------------
std::vector<Card*>::iterator Cards::erase(std::vector<Card*>::iterator a, std::vector<Card*>::iterator b)
{
	return m_cards.erase(a, b);
}

// ----------------------------------------------------------------------------
void Cards::remove(const Card* card_)
{
	auto itr = std::find(m_cards.begin(), m_cards.end(), card_);

	if (itr != m_cards.end())
	{
		m_cards.erase(itr);
	}
}

// ----------------------------------------------------------------------------
bool Cards::contains(const Card* card_) const
{
	return std::find(m_cards.begin(), m_cards.end(), card_) != m_cards.end();
}

// ----------------------------------------------------------------------------
// Uses;
// ----------------------------------------------------------------------------
Card* Cards::GetRandomCard()
{
	return m_cards[g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)size() - 1)];
}

// ----------------------------------------------------------------------------
void Cards::AssignCardsToPlayerID(int playerID_)
{
	for(int i = 0; i < size(); ++i)
	{
		m_cards[i]->m_playerID = playerID_;
	}
}


