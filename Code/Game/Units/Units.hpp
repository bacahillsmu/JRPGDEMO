#pragma once

#include "Game/Units/Unit.hpp"


#include <vector>

class Units 
{

public:

	Units() = default;
	explicit Units(const std::vector<Unit*>& units_);

	// Easy accessor;
	std::vector<Unit*> operator()()						{ return m_units; }

	// Common functions found in std::vector implementations;
	std::vector<Unit*>::iterator begin()				{ return m_units.begin(); }
	std::vector<Unit*>::iterator end()					{ return m_units.end(); }
	std::vector<Unit*>::const_iterator begin() const	{ return m_units.begin(); }
	std::vector<Unit*>::const_iterator end() const		{ return m_units.end(); }
	Unit*& operator[](std::size_t i)					{ return m_units[i]; }
	Unit* front()										{ return m_units.front(); }
	Unit* back()										{ return m_units.back(); }
	std::size_t size() const							{ return m_units.size(); }
	bool empty() const									{ return m_units.empty(); }
	void reserve(size_t n)								{ return m_units.reserve(n); }
	void push_back(Unit* unit)							{ m_units.push_back(unit); }
	void emplace_back(Unit* unit)						{ m_units.emplace_back(unit); }
	void pop_back()										{ m_units.pop_back(); }
	void clear()										{ m_units.clear(); }

	std::vector<Unit*>::iterator erase(std::vector<Unit*>::iterator a);
	std::vector<Unit*>::iterator erase(std::vector<Unit*>::iterator a, std::vector<Unit*>::iterator b);
	void remove(const Unit* unit_);
	bool contains(const Unit* unit_) const;

	Unit* GetAliveUnitStartingAtIndex(int& index_);


private:

	std::vector<Unit*> m_units;
};