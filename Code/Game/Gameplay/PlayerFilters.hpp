#pragma once

#include "Game/Framework/Interface.hpp"
#include "Game/Gameplay/Players.hpp"

#include <initializer_list>


// ----------------------------------------------------------------------------
// MultiFilter;
// ----------------------------------------------------------------------------
struct PlayerMultiFilter
{
	enum class Selector
	{
		AND,
		OR
	};

	PlayerMultiFilter(Selector selector_, std::initializer_list<PlayerFilter> filters_);

	bool operator()(const Player* player_) const;

private:

	std::vector<PlayerFilter> m_filters;
	Selector m_selector;
};

// ----------------------------------------------------------------------------
// Simple Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
struct IsHumanPlayer
{
	bool operator()(const Player* player_) const;
};

// ----------------------------------------------------------------------------
struct IsAIPlayer
{
	bool operator()(const Player* player_) const;
};

// ----------------------------------------------------------------------------
struct IsPlayerAlive
{
	bool operator()(const Player* player_) const;
};

// ----------------------------------------------------------------------------
struct IsPlayerDead
{
	bool operator()(const Player* player_) const;
};

// ----------------------------------------------------------------------------
struct HasPlayerID
{
	explicit HasPlayerID(int playerID_);

	bool operator()(const Player* player_) const;

private:

	int m_playerID;

};

// ----------------------------------------------------------------------------
struct HasMatchID
{
	explicit HasMatchID(int matchID_);

	bool operator()(const Player* player_) const;

private:

	int m_matchID;

};

// ----------------------------------------------------------------------------
struct HasNoMatchID
{
	bool operator()(const Player* player_) const;

};
