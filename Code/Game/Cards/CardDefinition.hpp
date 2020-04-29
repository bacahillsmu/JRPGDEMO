#pragma once

#include "Engine/Core/XmlUtils.hpp"

#include <map>
#include <vector>


// Do we need this and the UnitDefinition one? Or just one?
enum class CardType
{
	INVALID = -1,

	SOLDIER,
	PALADIN,
	FIGHTER,
	THIEF,
	NINJA,
	WHITEMAGE,
	BLACKMAGE,
	ILLUSIONIST,
	BLUEMAGE,
	ARCHER,
	HUNTER,
	WARRIOR,
	DRAGOON,
	DEFENDER,
	GLADIATOR,
	MONK,
	BISHOP,
	TEMPLAR,
	TIMEMAGE,
	ALCHEMIST,
	BEASTMASTER,
	MORPHER,
	SAGE,
	FENCER,
	ELEMENTALIST,
	REDMAGE,
	SUMMONER,
	ASSASSIN,
	SNIPER,
	ANIMIST,
	KNIGHT,
	GUNNER,
	JUGGLER,
	GADGETEER,

	CARD_COUNT
};



class CardDefinition
{

public:

	CardDefinition() = delete;
	explicit CardDefinition(XmlElement* cardElement_);
	~CardDefinition();

	static void LoadCardsFromXML(const char* filename_);
	static CardType StringToUnitType(std::string type_);

	static CardType GetRandomCardType();

public:

	// Type Information;
	CardType m_type = CardType::INVALID;
	int m_health = 0;
	int m_strength = 0;
	int m_intellect = 0;
	int m_wisdom = 0;
	int m_constitution = 0;
	int m_speed = 0;
	std::string m_cardTexture = "";
	std::string m_jobTexture = "";

	// Static Map to hold Card Definitions;
	static std::map<CardType, CardDefinition*> s_cardDefinitions;

};