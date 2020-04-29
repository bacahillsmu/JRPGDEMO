#include "Game/Units/Units.hpp"



// ----------------------------------------------------------------------------
Units::Units(const std::vector<Unit*>& units_)
	: m_units(units_)
{
}

// ----------------------------------------------------------------------------
std::vector<Unit*>::iterator Units::erase(std::vector<Unit*>::iterator a)
{
	return m_units.erase(a);
}

// ----------------------------------------------------------------------------
std::vector<Unit*>::iterator Units::erase(std::vector<Unit*>::iterator a, std::vector<Unit*>::iterator b)
{
	return m_units.erase(a, b);
}

// ----------------------------------------------------------------------------
void Units::remove(const Unit* unit_)
{
	auto itr = std::find(m_units.begin(), m_units.end(), unit_);

	if (itr != m_units.end())
	{
		m_units.erase(itr);
	}
}

// ----------------------------------------------------------------------------
bool Units::contains(const Unit* unit_) const
{
	return std::find(m_units.begin(), m_units.end(), unit_) != m_units.end();
}

// ----------------------------------------------------------------------------
Unit* Units::GetAliveUnitStartingAtIndex(int& index_)
{
	// Wrap around;
	if (index_ > m_units.size() - 1)
	{
		index_ = 0;
	}

	// Grab the unit at the requested index;
	Unit*& unit = m_units[index_];

	// We increment the index because the next time we call this function we want to be on the next unit;
	index_++;

	// Check if they are alive;
	if (unit->m_health > 0)
	{
		return unit;
	}
	else
	{
		// If the unit is dead, then we call the function again to check the next one;
		return GetAliveUnitStartingAtIndex(index_);
	}
}

