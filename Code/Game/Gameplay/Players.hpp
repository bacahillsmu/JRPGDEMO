#pragma once


#include "Game/Gameplay/Player.hpp"

#include <vector>


class Players
{

public:

	Players() = default;
	explicit Players(const std::vector<Player*>& players_);

	// Easy accessor;
	std::vector<Player*> operator()()					{ return m_players; }

	// Common functions found in std::vector implementations;
	std::vector<Player*>::iterator begin()				{ return m_players.begin(); }
	std::vector<Player*>::iterator end()				{ return m_players.end(); }
	std::vector<Player*>::const_iterator begin() const	{ return m_players.begin(); }
	std::vector<Player*>::const_iterator end() const	{ return m_players.end(); }
	Player*& operator[](std::size_t i)					{ return m_players[i]; }
	Player* front()									{ return m_players.front(); }
	Player* back()										{ return m_players.back(); }
	std::size_t size() const							{ return m_players.size(); }
	bool empty() const									{ return m_players.empty(); }
	void reserve(size_t n)								{ return m_players.reserve(n); }
	void push_back(Player* player_)						{ m_players.push_back(player_); }
	void emplace_back(Player* player_)					{ m_players.emplace_back(player_); }
	void pop_back()										{ m_players.pop_back(); }
	void clear()										{ m_players.clear(); }

	std::vector<Player*>::iterator erase(std::vector<Player*>::iterator a);
	std::vector<Player*>::iterator erase(std::vector<Player*>::iterator a, std::vector<Player*>::iterator b);
	void remove(const Player* player_);
	bool contains(const Player* player_) const;

	// Uses;
	Player* GetRandomPlayer();
	Player* GetRandomPlayerWithNoMatchID();

private:

	std::vector<Player*> m_players;
};