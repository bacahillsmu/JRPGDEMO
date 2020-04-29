#include "Game/Units/UnitFilters.hpp"

// ----------------------------------------------------------------------------
// MultiFilter;
// ----------------------------------------------------------------------------
UnitMultiFilter::UnitMultiFilter(Selector selector_, std::initializer_list<UnitFilter> filters_)
	: m_selector(selector_)
	, m_filters(filters_)
{
}

// ----------------------------------------------------------------------------
bool UnitMultiFilter::operator()(const Unit* unit_) const
{
	if (m_selector == Selector::AND)
	{
		for (auto& filter : m_filters)
		{
			if (!filter(unit_))
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
			if (filter(unit_))
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
UnitBelongsToPlayerID::UnitBelongsToPlayerID(int playerID_)
	: m_playerID(playerID_)
{
}

bool UnitBelongsToPlayerID::operator()(const Unit* unit_) const
{
	return unit_->m_playerID == m_playerID;
}

// ----------------------------------------------------------------------------
UnitBelongsToNotPlayerID::UnitBelongsToNotPlayerID(int playerID_)
	: m_playerID(playerID_)
{
}

bool UnitBelongsToNotPlayerID::operator()(const Unit* unit_) const
{
	return unit_->m_playerID != m_playerID;
}

// ----------------------------------------------------------------------------
// Simple Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
bool IsUnitDead::operator()(const Unit* unit_) const
{
	return unit_->m_health <= 0;
}

// ----------------------------------------------------------------------------
bool IsUnitNotDead::operator()(const Unit* unit_) const
{
	return unit_->m_health > 0;
}

// ----------------------------------------------------------------------------
bool IsKnightUnit::operator()(const Unit* unit_) const
{
	return unit_->m_type == JobType::KNIGHT;
}


