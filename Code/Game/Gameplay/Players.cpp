#include "Game/Gameplay/Players.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/RandomNumberGenerator.hpp"

// ----------------------------------------------------------------------------
Players::Players(const std::vector<Player*>& players_)
	: m_players(players_)
{
}

// ----------------------------------------------------------------------------
std::vector<Player*>::iterator Players::erase(std::vector<Player*>::iterator a)
{
	return m_players.erase(a);
}

// ----------------------------------------------------------------------------
std::vector<Player*>::iterator Players::erase(std::vector<Player*>::iterator a, std::vector<Player*>::iterator b)
{
	return m_players.erase(a, b);
}

// ----------------------------------------------------------------------------
void Players::remove(const Player* player_)
{
	auto itr = std::find(m_players.begin(), m_players.end(), player_);

	if (itr != m_players.end())
	{
		m_players.erase(itr);
	}
}

// ----------------------------------------------------------------------------
bool Players::contains(const Player* player_) const
{
	return std::find(m_players.begin(), m_players.end(), player_) != m_players.end();
}

// ----------------------------------------------------------------------------
Player* Players::GetRandomPlayer()
{
	return m_players[g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)m_players.size() - 1)];
}

// ----------------------------------------------------------------------------
Player* Players::GetRandomPlayerWithNoMatchID()
{
	Players playersWithNoMatchID;

	for(Player* player : m_players)
	{
		if(player->GetMatchID() == -1)
		{
			playersWithNoMatchID.push_back(player);
		}
	}

	if(playersWithNoMatchID.size() == 0)
	{
		return nullptr;
	}

	return playersWithNoMatchID[g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)playersWithNoMatchID.size() - 1)];
}
