#include "Game/Gameplay/PlayerFilters.hpp"

// ----------------------------------------------------------------------------
// MultiFilter;
// ----------------------------------------------------------------------------
PlayerMultiFilter::PlayerMultiFilter(Selector selector_, std::initializer_list<PlayerFilter> filters_)
	: m_selector(selector_)
	, m_filters(filters_)
{
}

// ----------------------------------------------------------------------------
bool PlayerMultiFilter::operator()(const Player* player_) const
{
	if (m_selector == Selector::AND)
	{
		for (auto& filter : m_filters)
		{
			if (!filter(player_))
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
			if (filter(player_))
			{
				return true;
			}
		}

		return false;
	}

	return false;
}

// ----------------------------------------------------------------------------
// Simple Filter;
// ----------------------------------------------------------------------------
bool IsHumanPlayer::operator()(const Player* player_) const
{
	return player_->IsAIPlayer() == false;
}

// ----------------------------------------------------------------------------
bool IsAIPlayer::operator()(const Player* player_) const
{
	return player_->IsAIPlayer() == true;
}

// ----------------------------------------------------------------------------
bool IsPlayerAlive::operator()(const Player* player_) const
{
	return player_->GetPlayerHealth() > 0u;
}

// ----------------------------------------------------------------------------
bool IsPlayerDead::operator()(const Player* player_) const
{
	return player_->GetPlayerHealth() == 0u;
}

// ----------------------------------------------------------------------------
HasPlayerID::HasPlayerID(int playerID_)
	: m_playerID(playerID_)
{
}

bool HasPlayerID::operator()(const Player* player_) const
{
	return player_->GetPlayerID() == m_playerID;
}

// ----------------------------------------------------------------------------
HasMatchID::HasMatchID(int matchID_)
	: m_matchID(matchID_)
{
}

bool HasMatchID::operator()(const Player* player_) const
{
	return player_->GetMatchID() == m_matchID;
}

// ----------------------------------------------------------------------------
bool HasNoMatchID::operator()(const Player* player_) const
{
	return player_->GetMatchID() == -1;
}

