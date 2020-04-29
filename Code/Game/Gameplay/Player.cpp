#include "Game/Gameplay/Player.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Framework/GameCommon.hpp"

#include "Engine/Core/RandomNumberGenerator.hpp"

// ------------------------------------------------------------------
#include "Game/Gameplay/Game.hpp"
#include "Game/Units/UnitFilters.hpp"
#include "Game/Cards/CardFilters.hpp"

// ------------------------------------------------------------------
// Setters;
// ------------------------------------------------------------------
void Player::SetPlayerUsername(const std::string& username_)
{
	m_playerUsername = username_;
}

// ------------------------------------------------------------------
void Player::SetPlayerID(int playerID_)
{
	m_playerID = playerID_;
}

// ------------------------------------------------------------------
void Player::SetMatchID(int matchID_)
{
	m_matchID = matchID_;
}

// ------------------------------------------------------------------
void Player::SetPlayerHealth(int healthValue_)
{
	m_playerHealth = healthValue_;
}

// ------------------------------------------------------------------
void Player::AddToPlayerHealth(int valueToChangeHealthBy_)
{
	m_playerHealth += valueToChangeHealthBy_;
}

// ------------------------------------------------------------------
void Player::SubtractFromPlayerHealth(int valueToChangeHealthBy_)
{
	m_playerHealth -= valueToChangeHealthBy_;
}

// ------------------------------------------------------------------
void Player::ClampHealth()
{
	m_playerHealth = Clamp(m_playerHealth, 0, m_playerMaxHealth);
}

// ------------------------------------------------------------------
void Player::SetUnits(const Units& units_)
{
	m_units = units_;
}

// ------------------------------------------------------------------
void Player::SetCards(const Cards& cards_)
{
	m_cards = cards_;
}

// ------------------------------------------------------------------
void Player::SetActualGoldAmount(int actualGold_)
{
	m_actualGold = actualGold_;
}

// ------------------------------------------------------------------
void Player::SetGoldAmount(int goldAmount_)
{
	m_goldAmount = goldAmount_;
}

// ------------------------------------------------------------------
void Player::ChangeActualGoldAmount(int valueToChangeActualGoldAmountBy_)
{
	m_actualGold += valueToChangeActualGoldAmountBy_;
}

// ------------------------------------------------------------------
void Player::ChangeGoldAmount(int valueToChangeGoldAmountBy_)
{
	m_goldAmount += valueToChangeGoldAmountBy_;
}

// ------------------------------------------------------------------
void Player::ClampActualGoldAmount()
{
	m_actualGold = Clamp(m_actualGold, 0, m_maxGold);
}

// ------------------------------------------------------------------
void Player::ClampGoldAmount()
{
	m_goldAmount = Clamp(m_goldAmount, 0, m_maxGold);
}

// ------------------------------------------------------------------
void Player::SetMarketplaceLocked(bool lock_)
{
	m_marketplaceLocked = lock_;
}

// ------------------------------------------------------------------
void Player::SetPurchasePhaseComplete(bool complete_)
{
	m_purchasePhaseComplete = complete_;
}

// ------------------------------------------------------------------
void Player::SetBattlePhaseComplete(bool complete_)
{
	m_battlePhaseComplete = complete_;
}

// ------------------------------------------------------------------
void Player::SetEnemyPlayer(Player*& player_)
{
	m_enemyPlayer = player_;
}

// ------------------------------------------------------------------
// Resets;
// ------------------------------------------------------------------
void Player::ResetMatchID()
{
	m_matchID = -1;
}

// ------------------------------------------------------------------
// Getters;
// ------------------------------------------------------------------
const std::string& Player::GetPlayerUsername()
{
	return m_playerUsername;
}

// ------------------------------------------------------------------
const int Player::GetPlayerID() const
{
	return m_playerID;
}

// ------------------------------------------------------------------
const int Player::GetMatchID() const
{
	return m_matchID;
}

// ------------------------------------------------------------------
const int Player::GetPlayerHealth() const
{
	return m_playerHealth;
}

// ------------------------------------------------------------------
const Units& Player::GetUnits()
{
	return m_units;
}

// ------------------------------------------------------------------
const Cards& Player::GetCards()
{
	return m_cards;
}

// ------------------------------------------------------------------
const int Player::GetMaxHandCount()
{
	return m_maxHandCount;
}

// ------------------------------------------------------------------
const int Player::GetActualGold()
{
	return m_actualGold;
}

// ------------------------------------------------------------------
const int Player::GetGoldAmount()
{
	return m_goldAmount;
}

// ------------------------------------------------------------------
const int Player::GetMaxGoldAmount()
{
	return m_maxGold;
}

// ------------------------------------------------------------------
const bool Player::GetMarketplaceLocked()
{
	return m_marketplaceLocked;
}

// ------------------------------------------------------------------
const bool Player::GetPurchasePhaseComplete()
{
	return m_purchasePhaseComplete;
}

// ------------------------------------------------------------------
const bool Player::BattlePhaseComplete()
{
	return m_battlePhaseComplete;
}

// ------------------------------------------------------------------
Player*& Player::GetEnemyPlayer()
{
	return m_enemyPlayer;
}

// ------------------------------------------------------------------
// Cards;
// ------------------------------------------------------------------
void Player::RemoveMyMarketCards()
{
	if (IsAIPlayer())
	{
		ERROR_AND_DIE("AI player tried to delete Market Cards, AI players should not do this because they do not own cards.");
	}

	for (auto itr = m_cards.begin(); itr != m_cards.end();)
	{
		if ((*itr)->m_cardArea == CardArea::MARKET)
		{
			delete *itr;
			itr = m_cards.erase(itr);

		}
		else
		{
			itr++;
		}
	}
}

// ------------------------------------------------------------------
void Player::RemoveMyHandCards()
{
	if (IsAIPlayer())
	{
		ERROR_AND_DIE("AI player tried to delete Hand Cards, AI players should not do this because they do not own cards.");
	}

	for (auto itr = m_cards.begin(); itr != m_cards.end();)
	{
		if ((*itr)->m_cardArea == CardArea::HAND)
		{
			delete *itr;
			itr = m_cards.erase(itr);

		}
		else
		{
			itr++;
		}
	}
}

// ------------------------------------------------------------------
void Player::AddCardToMyCards(Card*& card_)
{
	m_cards.push_back(card_);
}

// ------------------------------------------------------------------
void Player::AddUnitToMyUnits(Unit*& unit_)
{
	m_units.push_back(unit_);
}

// ------------------------------------------------------------------
void Player::RemoveMyUnits()
{
	if(IsAIPlayer())
	{
		ERROR_AND_DIE("AI player tried to delete Units, AI players should not do this because they do not own units.");
	}

	for (auto itr = m_units.begin(); itr != m_units.end();)
	{
		delete *itr;
		itr = m_units.erase(itr);
	}
}

// ------------------------------------------------------------------
void Player::SetEnemyUnits(Units& units_)
{
	m_enemyUnits = units_;
}

// ------------------------------------------------------------------
void Player::RemoveEnemyUnits()
{
	if (IsAIPlayer())
	{
		ERROR_AND_DIE("AI player tried to delete Enemy Units, AI players should not do this because they do not own units.");
	}

	for (auto itr = m_enemyUnits.begin(); itr != m_enemyUnits.end();)
	{
		delete *itr;
		itr = m_enemyUnits.erase(itr);
	}
}

// ------------------------------------------------------------------
// Turn Information;
// ------------------------------------------------------------------
void Player::SetGoesFirstForBattlePhase(bool goesFirst_)
{
	m_goesFirst = goesFirst_;
}

// ------------------------------------------------------------------
bool Player::GetGoesFirstForBattlePhase()
{
	return m_goesFirst;
}

// ------------------------------------------------------------------
void Player::SetSeedToUseForRNG(unsigned int seed_)
{
	m_seed = seed_;
	m_seedPosition = 0;
}

// ------------------------------------------------------------------
void Player::SetRandomNumberGeneratorSeed()
{
	g_theRandomNumberGenerator->NewSeed(m_seed);
	g_theRandomNumberGenerator->JumpToPosition(m_seedPosition);
}

void Player::SetSeedPosition(unsigned int newSeedPosition_)
{
	m_seedPosition = newSeedPosition_;
}

// ------------------------------------------------------------------
void Player::SetJustDied(bool justDied_)
{
	m_justDied = justDied_;
}

// ------------------------------------------------------------------
bool Player::GetJustDied()
{
	return m_justDied;
}

// ------------------------------------------------------------------
// Human Player;
// ------------------------------------------------------------------
HumanPlayer::HumanPlayer(int playerID_)
{
	m_playerID = playerID_;
}

// ------------------------------------------------------------------
HumanPlayer::~HumanPlayer()
{

}

// ------------------------------------------------------------------
void HumanPlayer::Update(float deltaSeconds_)
{
	deltaSeconds_;
}

/*
// ------------------------------------------------------------------
void HumanPlayer::GetMyUnits()
{
	m_units = g_Interface->query().GetUnits(PlayerFilter::UnitBelongsToPlayerID(m_playerID));
}

// ------------------------------------------------------------------
void HumanPlayer::GetMyCards()
{
	m_cards = g_Interface->query().GetCards(PlayerFilter::CardBelongsToPlayerID(m_playerID));

	for (int i = 0; i < m_cards.size(); ++i)
	{
		m_cards[i]->m_slotID = i;
	}
}
*/

// ------------------------------------------------------------------
// AI Player;
// ------------------------------------------------------------------
AIPlayer::AIPlayer(int playerID_)
{
	m_playerID = playerID_;
}

// ------------------------------------------------------------------
AIPlayer::~AIPlayer()
{

}

// ------------------------------------------------------------------
void AIPlayer::Update(float deltaSeconds_)
{
	deltaSeconds_;

	switch (g_Interface->server().GetCurrentPhase())
	{
		case Phase::PREGAME:
		{
			

			break;
		}

		case Phase::PURCHASE:
		{
			// Get cards in hand, we need to make sure we have room in our hand to buy a card;
			Cards handCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND, 
				{
					CardBelongsToPlayerID(m_playerID),
					CardInHand()
				}));

			int handSize = (int)handCards.size();
			if(handSize < m_maxHandCount)
			{
				// Check to see if we have the gold to buy a card;
				if(m_actualGold > 0)
				{
					// Get the cards in our market, transfer one card from market to hand and lose 1 gold;
					Cards marketCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
						{
							CardBelongsToPlayerID(m_playerID),
							CardInMarketplace()
						}));
					if(marketCards.size() > 0)
					{
						Card* card = marketCards.GetRandomCard();
						card->m_cardArea = CardArea::HAND;
						m_actualGold--;
					}
				}
			}

			// Get our units, we need to see if we have an open slot to play a card;
			Units inPlayUnits = g_Interface->query().GetUnits(UnitBelongsToPlayerID(m_playerID));
			int inPlaySize = (int)inPlayUnits.size();
			if(inPlaySize < 8)
			{
				// Get our cards in our hand again;
				handCards = g_Interface->query().GetCards(CardMultiFilter(CardMultiFilter::Selector::AND,
					{
						CardBelongsToPlayerID(m_playerID),
						CardInHand()
					}));
				if(handCards.size() > 0)
				{
					Card* cardToPlace = handCards.GetRandomCard();

					g_Interface->server().RemoveCardFromPlacingUnitByPlayer(cardToPlace->m_cardID);
					g_Interface->server().CreateNewUnitFromCardPlacedByPlayer(cardToPlace->m_cardID, (int)inPlayUnits.size());
					Units units = g_Interface->query().GetUnits(UnitBelongsToPlayerID(m_playerID));
					m_units = units;
				}
			}

			break;
		}

		case Phase::BATTLE:
		{
			if(!BattlePhaseComplete())
			{
				g_Interface->server().CreateMatchReport(0, 0, 0, m_matchID, true);
				SetBattlePhaseComplete(true);
			}

			break;
		}

		default:
		{
			ERROR_AND_DIE("AI Player is in an unknown Phase!");
			break;
		}
	}
}

/*
// ------------------------------------------------------------------
void AIPlayer::GetMyUnits()
{
	m_units = g_Interface->query().GetUnits(PlayerFilter::UnitBelongsToPlayerID(m_playerID));
}

// ------------------------------------------------------------------
void AIPlayer::GetMyCards()
{
	m_cards = g_Interface->query().GetCards(PlayerFilter::CardBelongsToPlayerID(m_playerID));

	for (int i = 0; i < m_cards.size(); ++i)
	{
		m_cards[i]->m_slotID = i;
	}
}
*/