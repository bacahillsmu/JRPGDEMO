#pragma once

#include "Engine/Core/XmlUtils.hpp"
#include "Game/Ability/Term.hpp"

#include <map>

class Term;

typedef std::vector<Term*> AbilitySequence;

enum class AbilityClass
{
	INVALID = -1,

	MAGIC,
	PHYSICAL,

	COUNT_ABILITY_CLASS
};

enum class TargetChoice
{
	INVALID = -1,

	PREDETERMINED,
	RANDOM,
	RANDOMFRONTLINE,
	RANDOMBACKLINE,
	MOSTDAMAGETAKEN,
	LEASTDAMAGETAKEN,
	SELF,

	COUNT_TARGET_CHOICE
};

enum class TargetAlliance
{
	INVALID = -1,

	ENEMY,
	FRIENDLY,

	COUNT_TARGET_ALLIANCE
};

enum class ActivationPeriod
{
	INVALID = -1,

	ONSTARTTURN,
	ONTARGETTURN,

	COUNT_ACTIVATION_PREIOD
};

class AbilityDefinition
{

public:

	AbilityDefinition() = delete;
	explicit AbilityDefinition(XmlElement* abilityElement_);
	~AbilityDefinition();

	static void LoadAbilitiesFromXML(const char* filename_);
	static AbilityClass StringToAbilityClass(std::string abilityClass_);
	static TargetChoice StringToTargetChoice(std::string targetChoice_);
	static TargetAlliance StringToTargetAlliance(std::string targetAlliance_);
	static ActivationPeriod StringToActivationPeriod(std::string activationPeriod_);
	
	float GetLifetimeOfAbilitySequence();

	// Parsing XML;
	void CheckForAndLoadSequence(XmlElement* abilityElement_);
	void ParseTermInSequence(XmlElement* termElement_);

public:

	// Ability Information;
	std::string m_name = "";
	AbilityClass m_abilityClass = AbilityClass::INVALID;
	int m_baseDamage = 0;
	int m_unlockLevel = 0;
	TargetChoice m_targetChoice = TargetChoice::INVALID;
	TargetAlliance m_targetAlliance = TargetAlliance::INVALID;
	float m_sequenceLifetime = 0.0f;
	std::string m_statusIcon = "";
	std::string m_buffIcon = "";
	ActivationPeriod m_activationPeriod = ActivationPeriod::INVALID;

	// Sequence;
	AbilitySequence m_abilitySequence;

	// Static Map to hold Ability Definitions;
	static std::map<std::string, AbilityDefinition*> s_abilityDefinitions;

	
};