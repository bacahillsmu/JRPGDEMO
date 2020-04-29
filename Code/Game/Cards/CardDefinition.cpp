#include "Game/Cards/CardDefinition.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/RandomNumberGenerator.hpp"

// ------------------------------------------------------------------
std::map<CardType, CardDefinition*> CardDefinition::s_cardDefinitions;

// ------------------------------------------------------------------
CardDefinition::CardDefinition(XmlElement* cardElement_)
{
	m_type =			StringToUnitType(ParseXmlAttribute(*cardElement_, "type", ""));
	m_health =			ParseXmlAttribute(*cardElement_, "health", 0);
	m_strength =		ParseXmlAttribute(*cardElement_, "strength", 0);
	m_intellect =		ParseXmlAttribute(*cardElement_, "intellect", 0);
	m_wisdom =			ParseXmlAttribute(*cardElement_, "wisdom", 0);
	m_constitution =	ParseXmlAttribute(*cardElement_, "constitution", 0);
	m_speed =			ParseXmlAttribute(*cardElement_, "speed", 0);
	m_cardTexture =		ParseXmlAttribute(*cardElement_, "cardTexture", "");
	m_jobTexture =		ParseXmlAttribute(*cardElement_, "jobTexture", "");
}

// ------------------------------------------------------------------
CardDefinition::~CardDefinition()
{
	std::map<CardType, CardDefinition*>::iterator card_iter;
	for (card_iter = s_cardDefinitions.begin(); card_iter != s_cardDefinitions.end(); ++card_iter)
	{
		delete card_iter->second;
		card_iter->second = nullptr;
	}
	s_cardDefinitions.clear();
}

// ------------------------------------------------------------------
void CardDefinition::LoadCardsFromXML(const char* filename_)
{
	tinyxml2::XMLDocument cardXMLDoc;
	cardXMLDoc.LoadFile(filename_);

	if (cardXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", filename_);
		printf("ErrorID:      %i\n", cardXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", cardXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", cardXMLDoc.ErrorName());
	}
	else
	{
		printf("Success with XML Doc: %s\n", filename_);

		XmlElement* cardElement = cardXMLDoc.RootElement()->FirstChildElement("card");
		while (cardElement)
		{
			CardDefinition* cardDefinition = new CardDefinition(cardElement);
			s_cardDefinitions[cardDefinition->m_type] = cardDefinition;

			cardElement = cardElement->NextSiblingElement();
		}
	}
}

// ------------------------------------------------------------------
CardType CardDefinition::StringToUnitType(std::string type_)
{
	if (type_ == "Blackmage")			{ return CardType::BLACKMAGE; }
	else if (type_ == "Archer")			{ return CardType::ARCHER; }
	else if (type_ == "Dragoon")		{ return CardType::DRAGOON; }
	else if (type_ == "Knight")			{ return CardType::KNIGHT; }
	else if (type_ == "Summoner")		{ return CardType::SUMMONER; }
	else if (type_ == "Morpher")		{ return CardType::MORPHER; }
	else if (type_ == "Soldier")		{ return CardType::SOLDIER; }
	else if (type_ == "Paladin")		{ return CardType::PALADIN; }
	else if (type_ == "Fighter")		{ return CardType::FIGHTER; }
	else if (type_ == "Thief")			{ return CardType::THIEF; }
	else if (type_ == "Ninja")			{ return CardType::NINJA; }
	else if (type_ == "Whitemage")		{ return CardType::WHITEMAGE; }
	else if (type_ == "Illusionist")	{ return CardType::ILLUSIONIST; }
	else if (type_ == "Bluemage")		{ return CardType::BLUEMAGE; }
	else if (type_ == "Hunter")			{ return CardType::HUNTER; }
	else if (type_ == "Warrior")		{ return CardType::WARRIOR; }
	else if (type_ == "Defender")		{ return CardType::DEFENDER; }
	else if (type_ == "Gladiator")		{ return CardType::GLADIATOR; }
	else if (type_ == "Monk")			{ return CardType::MONK; }
	else if (type_ == "Bishop")			{ return CardType::BISHOP; }
	else if (type_ == "Templar")		{ return CardType::TEMPLAR; }
	else if (type_ == "Timemage")		{ return CardType::TIMEMAGE; }
	else if (type_ == "Alchemist")		{ return CardType::ALCHEMIST; }
	else if (type_ == "Beastmaster")	{ return CardType::BEASTMASTER; }
	else if (type_ == "Sage")			{ return CardType::SAGE; }
	else if (type_ == "Fencer")			{ return CardType::FENCER; }
	else if (type_ == "Elementalist")	{ return CardType::ELEMENTALIST; }
	else if (type_ == "Redmage")		{ return CardType::REDMAGE; }
	else if (type_ == "Assassin")		{ return CardType::ASSASSIN; }
	else if (type_ == "Sniper")			{ return CardType::SNIPER; }
	else if (type_ == "Animist")		{ return CardType::ANIMIST; }
	else if (type_ == "Gunner")			{ return CardType::GUNNER; }
	else if (type_ == "Juggler")		{ return CardType::JUGGLER; }
	else if (type_ == "Gadgeteer")		{ return CardType::GADGETEER; }

	return CardType::INVALID;
}

// ------------------------------------------------------------------
CardType CardDefinition::GetRandomCardType()
{
	int randomIndex = g_theRandomNumberGenerator->GetRandomIntInRange(0, (int)CardType::CARD_COUNT - 1);

	return (CardType)randomIndex;
}
