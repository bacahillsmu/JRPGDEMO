#include "Game/Units/UnitDefinition.hpp"


// ------------------------------------------------------------------
#include "Game/Framework/Interface.hpp"
#include "Game/Ability/AbilityDefinition.hpp"

// ------------------------------------------------------------------
std::map<JobType, UnitDefinition*> UnitDefinition::s_unitDefinitions;

// ------------------------------------------------------------------
UnitDefinition::UnitDefinition(XmlElement* unitElement_)
{
	m_type =			StringToUnitType(ParseXmlAttribute(*unitElement_, "type", ""));
	m_health =			ParseXmlAttribute(*unitElement_, "health", 0);
	m_mana =			ParseXmlAttribute(*unitElement_, "mana", 0);
	m_strength =		ParseXmlAttribute(*unitElement_, "strength", 0);
	m_intellect =		ParseXmlAttribute(*unitElement_, "intellect", 0);
	m_wisdom =			ParseXmlAttribute(*unitElement_, "wisdom", 0);
	m_constitution =	ParseXmlAttribute(*unitElement_, "constitution", 0);
	m_speed =			ParseXmlAttribute(*unitElement_, "speed", 0);
	m_portrait =		ParseXmlAttribute(*unitElement_, "portrait", "");
	m_texture =			ParseXmlAttribute(*unitElement_, "texture", "");

	CheckForAndLoadAnimations(unitElement_);
	CheckForAndLoadAbilities(unitElement_);
}

// ------------------------------------------------------------------
UnitDefinition::~UnitDefinition()
{
	std::map<JobType, UnitDefinition*>::iterator unit_iter;
	for (unit_iter = s_unitDefinitions.begin(); unit_iter != s_unitDefinitions.end(); ++unit_iter)
	{
		delete unit_iter->second;
		unit_iter->second = nullptr;
	}
	s_unitDefinitions.clear();

	std::map<std::string, SpriteAnimationDefinition*>::iterator spriteAnimDef_iter;
	for(spriteAnimDef_iter = m_animationSet.begin(); spriteAnimDef_iter != m_animationSet.end(); ++spriteAnimDef_iter)
	{
		delete spriteAnimDef_iter->second;
		spriteAnimDef_iter->second = nullptr;
	}
	m_animationSet.clear();
}

// ------------------------------------------------------------------
void UnitDefinition::LoadUnitsFromXML(const char* filename_)
{
	tinyxml2::XMLDocument entityXMLDoc;
	entityXMLDoc.LoadFile(filename_);

	if (entityXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", filename_);
		printf("ErrorID:      %i\n", entityXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", entityXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", entityXMLDoc.ErrorName());
	}
	else
	{
		printf("Success with XML Doc: %s\n", filename_);

		XmlElement* unitElement = entityXMLDoc.RootElement()->FirstChildElement("unit");
		while(unitElement)
		{
			UnitDefinition* unitDefinition = new UnitDefinition(unitElement);						
			
			s_unitDefinitions[unitDefinition->m_type] = unitDefinition;
			unitElement = unitElement->NextSiblingElement();
		}
	}
}

// ------------------------------------------------------------------
JobType UnitDefinition::StringToUnitType(const std::string& type_)
{
	if (type_ == "Blackmage")			{ return JobType::BLACKMAGE; }
	else if (type_ == "Archer")			{ return JobType::ARCHER; }
	else if (type_ == "Dragoon")		{ return JobType::DRAGOON; }
	else if(type_ == "Knight")			{ return JobType::KNIGHT; }
	else if (type_ == "Summoner")		{ return JobType::SUMMONER; }
	else if (type_ == "Morpher")		{ return JobType::MORPHER; }
	else if (type_ == "Soldier")		{ return JobType::SOLDIER; }
	else if (type_ == "Paladin")		{ return JobType::PALADIN; }
	else if (type_ == "Fighter")		{ return JobType::FIGHTER; }
	else if (type_ == "Thief")			{ return JobType::THIEF; }
	else if (type_ == "Ninja")			{ return JobType::NINJA; }
	else if (type_ == "Whitemage")		{ return JobType::WHITEMAGE; }
	else if (type_ == "Illusionist")	{ return JobType::ILLUSIONIST; }
	else if (type_ == "Bluemage")		{ return JobType::BLUEMAGE; }
	else if (type_ == "Hunter")			{ return JobType::HUNTER; }
	else if (type_ == "Warrior")		{ return JobType::WARRIOR; }
	else if (type_ == "Defender")		{ return JobType::DEFENDER; }
	else if (type_ == "Gladiator")		{ return JobType::GLADIATOR; }
	else if (type_ == "Monk")			{ return JobType::MONK; }
	else if (type_ == "Bishop")			{ return JobType::BISHOP; }
	else if (type_ == "Templar")		{ return JobType::TEMPLAR; }
	else if (type_ == "Timemage")		{ return JobType::TIMEMAGE; }
	else if (type_ == "Alchemist")		{ return JobType::ALCHEMIST; }
	else if (type_ == "Beastmaster")	{ return JobType::BEASTMASTER; }
	else if (type_ == "Sage")			{ return JobType::SAGE; }
	else if (type_ == "Fencer")			{ return JobType::FENCER; }
	else if (type_ == "Elementalist")	{ return JobType::ELEMENTALIST; }
	else if (type_ == "Redmage")		{ return JobType::REDMAGE; }
	else if (type_ == "Assassin")		{ return JobType::ASSASSIN; }
	else if (type_ == "Sniper")			{ return JobType::SNIPER; }
	else if (type_ == "Animist")		{ return JobType::ANIMIST; }
	else if (type_ == "Gunner")			{ return JobType::GUNNER; }
	else if (type_ == "Juggler")		{ return JobType::JUGGLER; }
	else if (type_ == "Gadgeteer")		{ return JobType::GADGETEER; }

	return JobType::INVALID;
}

// ------------------------------------------------------------------
std::string UnitDefinition::UnitTypeToString(const JobType jobType_)
{
	switch(jobType_)
	{
		case JobType::ARCHER:			{ return "Archer"; break; }
		case JobType::BLACKMAGE:		{ return "Blackmage"; break; }
		case JobType::DRAGOON:			{ return "Dragoon"; break; }
		case JobType::MORPHER:			{ return "Morpher"; break; }
		case JobType::SUMMONER:			{ return "Summoner"; break; }
		case JobType::KNIGHT:			{ return "Knight"; break; }
		case JobType::SOLDIER:			{ return "Soldier"; break; }
		case JobType::PALADIN:			{ return "Paladin"; break; }
		case JobType::FIGHTER:			{ return "Fighter"; break; }
		case JobType::THIEF:			{ return "Thief"; break; }
		case JobType::NINJA:			{ return "Ninja"; break; }
		case JobType::WHITEMAGE:		{ return "Whitemage"; break; }
		case JobType::ILLUSIONIST:		{ return "Illusionist"; break; }
		case JobType::BLUEMAGE:			{ return "Bluemage"; break; }
		case JobType::HUNTER:			{ return "Hunter"; break; }
		case JobType::WARRIOR:			{ return "Warrior"; break; }
		case JobType::DEFENDER:			{ return "Defender"; break; }
		case JobType::GLADIATOR:		{ return "Gladiator"; break; }
		case JobType::MONK:				{ return "Monk"; break; }
		case JobType::BISHOP:			{ return "Bishop"; break; }
		case JobType::TEMPLAR:			{ return "Templar"; break; }
		case JobType::TIMEMAGE:			{ return "Timemage"; break; }
		case JobType::ALCHEMIST:		{ return "Alchemist"; break; }
		case JobType::BEASTMASTER:		{ return "Beastmaster"; break; }
		case JobType::SAGE:				{ return "Sage"; break; }
		case JobType::FENCER:			{ return "Fencer"; break; }
		case JobType::ELEMENTALIST:		{ return "Elementalist"; break; }
		case JobType::REDMAGE:			{ return "Redmage"; break; }
		case JobType::ASSASSIN:			{ return "Assassin"; break; }
		case JobType::SNIPER:			{ return "Sniper"; break; }
		case JobType::ANIMIST:			{ return "Animist"; break; }
		case JobType::GUNNER:			{ return "Gunner"; break; }
		case JobType::JUGGLER:			{ return "Juggler"; break; }
		case JobType::GADGETEER:		{ return "Gadgeteer"; break; }

		default:
		{
			ERROR_AND_DIE("Asked for an Invalid Job Type String!");
		}
	}
}

// ------------------------------------------------------------------
SpriteAnimationPlaybackType UnitDefinition::StringToSpriteAnimationPlaybackType(const std::string& playbackType_)
{
	if(playbackType_ == "once")
	{
		return SPRITE_ANIMATION_PLAYBACK_ONCE;
	}
	else if(playbackType_ == "loop")
	{
		return SPRITE_ANIMATION_PLAYBACK_LOOP;
	}
	else if(playbackType_ == "pingpong")
	{
		return SPRITE_ANIMATION_PLAYBACK_PINGPONG;
	}

	return SPRITE_ANIMATION_PLAYBACK_UNKNOWN;
}

// ------------------------------------------------------------------
void UnitDefinition::CheckForAndLoadAnimations(XmlElement* unitElement_)
{
	XmlElement* animsetElement = unitElement_->FirstChildElement("animset");
	if (animsetElement)
	{
		XmlElement* animationElement = animsetElement->FirstChildElement("animation");
		while (animationElement)
		{
			std::string unitType = ParseXmlAttribute(*unitElement_, "type", "");
			std::string animationName = ParseXmlAttribute(*animationElement, "name", "error");
			int start = ParseXmlAttribute(*animationElement, "start", 0);
			int end = ParseXmlAttribute(*animationElement, "end", 0);
			float duration = ParseXmlAttribute(*animationElement, "duration", 1.0f);
			SpriteAnimationPlaybackType playback = StringToSpriteAnimationPlaybackType((*animationElement, "playback", "loop"));

			m_animationSet[animationName] = new SpriteAnimationDefinition(*g_Interface->match().m_unitSpriteSheets[unitType], start, end, duration, playback);

			animationElement = animationElement->NextSiblingElement();
		}
	}
}

// ------------------------------------------------------------------
void UnitDefinition::CheckForAndLoadAbilities(XmlElement* unitElement_)
{
	// Read in the abilities of this Unit. It could have main, reaction, or passive. Or none;
	XmlElement* abilitiesElement = unitElement_->FirstChildElement("abilities");
	if (abilitiesElement)
	{
		CheckForAndLoadMainAbilities(abilitiesElement);
		CheckForAndLoadReactionAbilties(abilitiesElement);
		CheckForAndLoadPassiveAbilities(abilitiesElement);
	}
}

// ------------------------------------------------------------------
void UnitDefinition::CheckForAndLoadMainAbilities(XmlElement* abilitiesElement_)
{
	// Main abilities section;
	XmlElement* mainElement = abilitiesElement_->FirstChildElement("main");
	if (mainElement)
	{
		// Actual main ability;
		XmlElement* mainAbilityElement = mainElement->FirstChildElement("main_ability");
		while (mainAbilityElement)
		{
			std::string mainAbilityName = ParseXmlAttribute(*mainAbilityElement, "ability_name", "");
			m_mainAbilityDefinition = AbilityDefinition::s_abilityDefinitions[mainAbilityName];

			mainAbilityElement = mainAbilityElement->NextSiblingElement();
		}
	}
}

// ------------------------------------------------------------------
void UnitDefinition::CheckForAndLoadReactionAbilties(XmlElement* abilitiesElement_)
{
	// Reaction abilities section;
	XmlElement* reactionElement = abilitiesElement_->FirstChildElement("reaction");
	if (reactionElement)
	{
		// Actual reaction ability;
		XmlElement* reactionAbilityElement = reactionElement->FirstChildElement("reaction_ability");
		while (reactionAbilityElement)
		{
			std::string reactionAbilityName = ParseXmlAttribute(*reactionAbilityElement, "ability_name", "");
			m_reactionAbilityDefinition = AbilityDefinition::s_abilityDefinitions[reactionAbilityName];

			reactionAbilityElement = reactionAbilityElement->NextSiblingElement();
		}
	}
}

// ------------------------------------------------------------------
void UnitDefinition::CheckForAndLoadPassiveAbilities(XmlElement* abilitiesElement_)
{
	// Passive abilities section;
	XmlElement* passiveElement = abilitiesElement_->FirstChildElement("passive");
	if (passiveElement)
	{
		// Actual reaction ability;
		XmlElement* passiveAbilityElement = passiveElement->FirstChildElement("passive_ability");
		while (passiveAbilityElement)
		{
			std::string passiveAbilityName = ParseXmlAttribute(*passiveAbilityElement, "ability_name", "");
			m_passiveAbilityDefinition = AbilityDefinition::s_abilityDefinitions[passiveAbilityName];

			passiveAbilityElement = passiveAbilityElement->NextSiblingElement();
		}
	}
}