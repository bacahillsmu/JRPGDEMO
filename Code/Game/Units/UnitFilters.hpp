#pragma once

#include "Game/Framework/Interface.hpp"
#include "Game/Units/Units.hpp"

#include <initializer_list>


// ----------------------------------------------------------------------------
// MultiFilter;
// ----------------------------------------------------------------------------
struct UnitMultiFilter
{
	enum class Selector
	{
		AND,
		OR
	};

	UnitMultiFilter(Selector selector_, std::initializer_list<UnitFilter> filters_);

	bool operator()(const Unit* unit_) const;

private:

	std::vector<UnitFilter> m_filters;
	Selector m_selector;
};

// ----------------------------------------------------------------------------
// Player Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
struct UnitBelongsToPlayerID
{
	explicit UnitBelongsToPlayerID(int playerID_);

	bool operator()(const Unit* unit_) const;

private:

	int m_playerID;

};

// ----------------------------------------------------------------------------
struct UnitBelongsToNotPlayerID
{
	explicit UnitBelongsToNotPlayerID(int playerID_);

	bool operator()(const Unit* unit_) const;

private:

	int m_playerID;

};

// ----------------------------------------------------------------------------
// Simple Filters;
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
struct IsUnitDead
{
	bool operator()(const Unit* unit_) const;
};

// ----------------------------------------------------------------------------
struct IsUnitNotDead
{
	bool operator()(const Unit* unit_) const;
};

// ----------------------------------------------------------------------------
struct IsKnightUnit
{
	bool operator()(const Unit* unit_) const;
};


