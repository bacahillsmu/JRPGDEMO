#pragma once

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"

#include <map>

class AbilityDefinition;

enum class JobType
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

	JOB_COUNT
};

class UnitDefinition
{

public:

	UnitDefinition() = delete;
	explicit UnitDefinition(XmlElement* unitElement_);
	~UnitDefinition();

	static void LoadUnitsFromXML(const char* filename_);
	static JobType StringToUnitType(const std::string& type_);
	static std::string UnitTypeToString(const JobType jobType_);
	static SpriteAnimationPlaybackType StringToSpriteAnimationPlaybackType(const std::string& playbackType_);

	// Helpers for loading parts of a UnitDefinition;
	void CheckForAndLoadAnimations(XmlElement* unitElement_);
	void CheckForAndLoadAbilities(XmlElement* unitElement_);
	void CheckForAndLoadMainAbilities(XmlElement* abilitiesElement_);
	void CheckForAndLoadReactionAbilties(XmlElement* abilitiesElement_);
	void CheckForAndLoadPassiveAbilities(XmlElement* abilitiesElement_);


public:

	// Type Information;
	JobType m_type = JobType::INVALID;
	int m_health = 0;
	int m_mana = 0;
	int m_strength = 0;
	int m_intellect = 0;
	int m_wisdom = 0;
	int m_constitution = 0;
	int m_speed = 0;
	std::string m_portrait = "";
	std::string m_texture = "";

	// Animations;
	std::map<std::string, SpriteAnimationDefinition*> m_animationSet;
	
	// Static Map to hold Unit Definitions;
	static std::map<JobType, UnitDefinition*> s_unitDefinitions;

	// Map to hold Unit Ability Information for this Unit Definition;
	AbilityDefinition* m_mainAbilityDefinition = nullptr;
	AbilityDefinition* m_reactionAbilityDefinition = nullptr;
	AbilityDefinition* m_passiveAbilityDefinition = nullptr;
	
	//std::map<std::string, AbilityDefinition*> m_mainAbilities;
	//std::map<std::string, AbilityDefinition*> m_reactionAbilities;
	//std::map<std::string, AbilityDefinition*> m_passiveAbilities;

};