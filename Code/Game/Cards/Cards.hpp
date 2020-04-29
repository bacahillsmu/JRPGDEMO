#pragma once

#include "Game/Cards/Card.hpp"

#include <vector>


class Cards
{

public:

	Cards() = default;
	explicit Cards(const std::vector<Card*>& cards_);

	// Easy accessor;
	std::vector<Card*> operator()()						{ return m_cards; }

	// Common functions found in std::vector implementations;
	std::vector<Card*>::iterator begin()				{ return m_cards.begin(); }
	std::vector<Card*>::iterator end()					{ return m_cards.end(); }
	std::vector<Card*>::const_iterator begin() const	{ return m_cards.begin(); }
	std::vector<Card*>::const_iterator end() const		{ return m_cards.end(); }
	Card*& operator[](std::size_t i)					{ return m_cards[i]; }
	Card* front()										{ return m_cards.front(); }
	Card* back()										{ return m_cards.back(); }
	std::size_t size() const							{ return m_cards.size(); }
	bool empty() const									{ return m_cards.empty(); }
	void reserve(size_t n)								{ return m_cards.reserve(n); }
	void push_back(Card* card_)							{ m_cards.push_back(card_); }
	void emplace_back(Card* card_)						{ m_cards.emplace_back(card_); }
	void pop_back()										{ m_cards.pop_back(); }
	void clear()										{ m_cards.clear(); }

	std::vector<Card*>::iterator erase(std::vector<Card*>::iterator a);
	std::vector<Card*>::iterator erase(std::vector<Card*>::iterator a, std::vector<Card*>::iterator b);
	void remove(const Card* card_);
	bool contains(const Card* card_) const;

	// Uses;
	Card* GetRandomCard();
	void AssignCardsToPlayerID(int playerID_);

private:

	std::vector<Card*> m_cards;
};