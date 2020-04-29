#include "Game/Ability/AbilityDefinition.hpp"


#include "Game/Framework/Interface.hpp"


// ------------------------------------------------------------------
std::map<std::string, AbilityDefinition*> AbilityDefinition::s_abilityDefinitions;

// ------------------------------------------------------------------
AbilityDefinition::AbilityDefinition(XmlElement* abilityElement_)
{
	m_name = ParseXmlAttribute(*abilityElement_, "name", "");
	m_abilityClass = StringToAbilityClass(ParseXmlAttribute(*abilityElement_, "class", ""));
	m_baseDamage = ParseXmlAttribute(*abilityElement_, "base_damage", 0);
	m_unlockLevel = ParseXmlAttribute(*abilityElement_, "unlock_level", 0);
	m_targetChoice = StringToTargetChoice(ParseXmlAttribute(*abilityElement_, "target_choice", ""));
	m_targetAlliance = StringToTargetAlliance(ParseXmlAttribute(*abilityElement_, "target_alliance", ""));
	m_statusIcon = ParseXmlAttribute(*abilityElement_, "status_icon", "");
	m_buffIcon = ParseXmlAttribute(*abilityElement_, "buff_icon", "");
	m_activationPeriod = StringToActivationPeriod(ParseXmlAttribute(*abilityElement_, "activation_period", ""));
	
	CheckForAndLoadSequence(abilityElement_);
}

// ------------------------------------------------------------------
AbilityDefinition::~AbilityDefinition()
{
	for(Term*& term : m_abilitySequence)
	{
		delete term;
		term = nullptr;
	}
	m_abilitySequence.clear();

	std::map<std::string, AbilityDefinition*>::iterator ability_iter;
	for (ability_iter = s_abilityDefinitions.begin(); ability_iter != s_abilityDefinitions.end(); ++ability_iter)
	{
		delete ability_iter->second;
		ability_iter->second = nullptr;
	}
	s_abilityDefinitions.clear();
}

// ------------------------------------------------------------------
void AbilityDefinition::LoadAbilitiesFromXML(const char* filename_)
{
	tinyxml2::XMLDocument entityXMLDoc;
	entityXMLDoc.LoadFile(filename_);

	if (entityXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", filename_);
		printf("ErrorID:      %i\n", entityXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", entityXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", entityXMLDoc.ErrorName());
		ERROR_AND_DIE("Problem reading AbilityDefinitions!");
	}
	else
	{
		printf("Success with XML Doc: %s\n", filename_);

		XmlElement* abilityElement = entityXMLDoc.RootElement()->FirstChildElement("ability");
		while (abilityElement)
		{
			AbilityDefinition* abilityDefinition = new AbilityDefinition(abilityElement);
			s_abilityDefinitions[abilityDefinition->m_name] = abilityDefinition;

			abilityElement = abilityElement->NextSiblingElement();
		}
	}
}

// ------------------------------------------------------------------
AbilityClass AbilityDefinition::StringToAbilityClass(std::string abilityClass_)
{
	if(abilityClass_ == "magic")
	{
		return AbilityClass::MAGIC;
	}
	else if(abilityClass_ == "physical")
	{
		return AbilityClass::PHYSICAL;
	}

	return AbilityClass::INVALID;
}

// ------------------------------------------------------------------
TargetChoice AbilityDefinition::StringToTargetChoice(std::string targetChoice_)
{
	if      (targetChoice_ == "predetermined")		{ return TargetChoice::PREDETERMINED; }
	else if (targetChoice_ == "random")				{ return TargetChoice::RANDOM; }
	else if (targetChoice_ == "least_damage_taken")	{ return TargetChoice::LEASTDAMAGETAKEN; }
	else if (targetChoice_ == "most_damage_taken")	{ return TargetChoice::MOSTDAMAGETAKEN; }
	else if (targetChoice_ == "self")				{ return TargetChoice::SELF; }

	return TargetChoice::INVALID;
}

// ------------------------------------------------------------------
TargetAlliance AbilityDefinition::StringToTargetAlliance(std::string targetAlliance_)
{
	if(targetAlliance_ == "enemy")
	{
		return TargetAlliance::ENEMY;
	}
	else if(targetAlliance_ == "friendly")
	{
		return TargetAlliance::FRIENDLY;
	}

	return TargetAlliance::INVALID;
}

// ------------------------------------------------------------------
ActivationPeriod AbilityDefinition::StringToActivationPeriod(std::string activationPeriod_)
{
	if		(activationPeriod_ == "on_start_turn")	{ return ActivationPeriod::ONSTARTTURN; }
	else if (activationPeriod_ == "on_target_turn")	{ return ActivationPeriod::ONTARGETTURN; }

	return ActivationPeriod::INVALID;
}

// ------------------------------------------------------------------
float AbilityDefinition::GetLifetimeOfAbilitySequence()
{
	float lifetime = 0.0f;

	if(m_abilitySequence.size() == 0)
	{
		return lifetime;
	}
	
	for(Term* term : m_abilitySequence)
	{
		float sequenceLifetime = term->GetAtTime() + term->GetDuration();
		if(sequenceLifetime > lifetime)
		{
			lifetime = sequenceLifetime;
		}
	}

	// Add a small buffer to the lifetime. This helps ensure all sequences have run entirely;
	lifetime += 0.1f;

	return lifetime;
}

// ------------------------------------------------------------------
void AbilityDefinition::CheckForAndLoadSequence(XmlElement* abilityElement_)
{
	XmlElement* sequenceElement = abilityElement_->FirstChildElement("sequence");
	if(sequenceElement)
	{
		XmlElement* termElement = sequenceElement->FirstChildElement("term");
		while (termElement)
		{
			ParseTermInSequence(termElement);
			termElement = termElement->NextSiblingElement();
		}
	}

	m_sequenceLifetime = GetLifetimeOfAbilitySequence();
}

// ------------------------------------------------------------------
void AbilityDefinition::ParseTermInSequence(XmlElement* termElement_)
{
	TermType type = Term::StringToTermType(ParseXmlAttribute(*termElement_, "type", ""));

	Term* term = nullptr;
	switch (type)
	{
	case TermType::ANIM:			{ term = new AnimTerm(termElement_); break; }
	case TermType::MOVEMENT:		{ term = new MovementTerm(termElement_); break; }
	case TermType::EFFECT:			{ term = new EffectTerm(termElement_); break; }
	case TermType::AUDIO:			{ term = new AudioTerm(termElement_); break; }
	case TermType::DAMAGE:			{ term = new DamageTerm(termElement_); break; }
	case TermType::DEBUFF:			{ term = new DebuffTerm(termElement_); break; }
	case TermType::BUFF:			{ term = new BuffTerm(termElement_); break; }
	case TermType::STATUS:			{ term = new StatusTerm(termElement_); break; }
	case TermType::ATTACKCHANGE:	{ term = new AttackChange(termElement_); break; }
	case TermType::DISSPELL:		{ term = new Disspell(termElement_); break; }

		default:
		{
			ERROR_AND_DIE("Unknown Sequence Type!");
			break;
		}
	}

	if(term)
	{
		m_abilitySequence.push_back(term);
	}
}
