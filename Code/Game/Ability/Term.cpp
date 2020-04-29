#include "Game/Ability/Term.hpp"

// -----------------------------------------------------------------------
#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Engine/Core/DevConsole.hpp"

// -----------------------------------------------------------------------
#include "Game/Units/Unit.hpp"
#include "Game/Ability/Ability.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Framework/Interface.hpp"



// -----------------------------------------------------------------------
// Static;
// -----------------------------------------------------------------------
TermType Term::StringToTermType(const std::string& type_)
{
	if		(type_ == "anim")			{ return TermType::ANIM; }
	else if (type_ == "movement")		{ return TermType::MOVEMENT; }
	else if (type_ == "effect")			{ return TermType::EFFECT; }
	else if (type_ == "audio")			{ return TermType::AUDIO; }
	else if (type_ == "damage")			{ return TermType::DAMAGE; }
	else if (type_ == "debuff")			{ return TermType::DEBUFF; }
	else if (type_ == "buff")			{ return TermType::BUFF; }
	else if (type_ == "status")			{ return TermType::STATUS; }
	else if (type_ == "attack_change")	{ return TermType::ATTACKCHANGE; }
	else if (type_ == "disspell")		{ return TermType::DISSPELL; }

	return TermType::INVALID;
}

// -----------------------------------------------------------------------
TermMovementType Term::StringToMovementType(const std::string& type_)
{
	if		(type_ == "target_x")		{ return TermMovementType::TARGET_X; }
	else if (type_ == "target_y")		{ return TermMovementType::TARGET_Y; }
	else if (type_ == "enemy_pos")		{ return TermMovementType::ENEMY_POS; }
	else if (type_ == "original_pos")	{ return TermMovementType::ORIGINAL_POS; }
	else if (type_ == "offscreen_top")	{ return TermMovementType::OFFSCREEN_TOP; }

	return TermMovementType::INVALID;
}

// -----------------------------------------------------------------------
// Base;
// -----------------------------------------------------------------------
void Term::Run(float deltaSeconds_)
{
	switch(m_termState)
	{
		case TermState::SETUP_COMPLETE:
		case TermState::START:
		{
			m_runningTimer = 0.0f;
			Start(deltaSeconds_);
			break;
		}

		case TermState::DO:
		{
			Tick(deltaSeconds_);
			break;
		}

		case TermState::END:
		{
			End();
			break;
		}

		case TermState::FINISHED:
		{
			// Do nothing;
			break;
		}

		default:
		{
			ERROR_AND_DIE("A Term in the Ability Sequence is in an invalid state!");
			break;
		}
	}

	m_abilityCurrentlySequencing = nullptr;
	return;
}

// -----------------------------------------------------------------------
void Term::SetSetupComplete()
{
	m_termState = TermState::SETUP_COMPLETE;
}

// -----------------------------------------------------------------------
void Term::SetStateToStart()
{
	m_termState = TermState::START;
}

// -----------------------------------------------------------------------
float Term::GetAtTime()
{
	return m_atTime;
}

// -----------------------------------------------------------------------
float Term::GetDuration()
{
	return m_duration;
}

// -----------------------------------------------------------------------
void Term::SetAtTime(float atTime_)
{
	m_atTime = atTime_;
}

// -----------------------------------------------------------------------
void Term::AddToAtTime(float additionalTime_)
{
	m_atTime += additionalTime_;
}

// -----------------------------------------------------------------------
void Term::SetDuration(float duration_)
{
	m_duration = duration_;
}

// -----------------------------------------------------------------------
void Term::SetAbilityCurrentlySequencing(Ability* ability_)
{
	m_abilityCurrentlySequencing = ability_;
}

// -----------------------------------------------------------------------
bool Term::IsFinished()
{
	return m_termState == TermState::FINISHED;
}

// -----------------------------------------------------------------------
// Anim;
// -----------------------------------------------------------------------
AnimTerm::AnimTerm(XmlElement* termElement_)
{
	SetAnimationState(ParseXmlAttribute(*termElement_, "animname", ""));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void AnimTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if(m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void AnimTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_abilityCurrentlySequencing->GetCaster()->m_currentAnimationName = m_animationName;
	m_abilityCurrentlySequencing->GetCaster()->m_animationTimer = 0.0f;
	m_runningTimer += deltaSeconds_;

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void AnimTerm::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* AnimTerm::Clone()
{
	return new AnimTerm(*this);
}

// -----------------------------------------------------------------------
void AnimTerm::SetAnimationState(const std::string& animationName_)
{
	m_animationName = animationName_;

	GUARANTEE_OR_DIE(m_animationName != "", "Setting Animation in an AnimTerm to nothing. This is not a valid animation!");
}

// -----------------------------------------------------------------------
// Movement;
// -----------------------------------------------------------------------
MovementTerm::MovementTerm(XmlElement* termElement_)
{
	SetMovementType(Term::StringToMovementType(ParseXmlAttribute(*termElement_, "movement_type", "")));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void MovementTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		SetOriginalPosition();
		SetCurrentPosition();
		SetTargetPosition();

		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void MovementTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	float percentAlongPath = 0.0f;
	if(m_duration > 0.0f)
	{
		percentAlongPath = m_runningTimer / m_duration;
	}

	m_currentPosition = Lerp2D(m_currentPosition, m_targetPosition, percentAlongPath);
	m_abilityCurrentlySequencing->GetCaster()->SetLocation(m_currentPosition);

	m_runningTimer += deltaSeconds_;

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void MovementTerm::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* MovementTerm::Clone()
{
	return new MovementTerm(*this);
}

// -----------------------------------------------------------------------
void MovementTerm::SetMovementType(TermMovementType movementType_)
{
	m_movementType = movementType_;
}

// -----------------------------------------------------------------------
void MovementTerm::SetOriginalPosition()
{
	m_originalPosition = m_abilityCurrentlySequencing->GetCasterOriginalLocation();
}

// -----------------------------------------------------------------------
void MovementTerm::SetCurrentPosition()
{
	m_currentPosition = m_abilityCurrentlySequencing->GetCaster()->GetLocation();
}

// -----------------------------------------------------------------------
void MovementTerm::SetTargetPosition()
{
	switch(m_movementType)
	{
		case TermMovementType::TARGET_X:
		{
			Vec2 enemyLocation = m_abilityCurrentlySequencing->m_castLocation;
			Vec2 currentLocation = m_abilityCurrentlySequencing->GetCaster()->GetLocation();

			m_targetPosition = Vec2(enemyLocation.x, currentLocation.y);

			break;
		}

		case TermMovementType::TARGET_Y:
		{
			Vec2 enemyLocation = m_abilityCurrentlySequencing->m_castLocation;
			Vec2 currentLocation = m_abilityCurrentlySequencing->GetCaster()->GetLocation();

			m_targetPosition = Vec2(currentLocation.x, enemyLocation.y);

			break;
		}

		case TermMovementType::ENEMY_POS:
		{
			m_targetPosition = m_abilityCurrentlySequencing->m_castLocation;

			break;
		}

		case TermMovementType::ORIGINAL_POS:
		{
			m_targetPosition = m_originalPosition;

			break;
		}

		case TermMovementType::OFFSCREEN_TOP:
		{
			Vec2 currentLocation = m_abilityCurrentlySequencing->GetCaster()->GetLocation();
			float offscreenTop = (Map::HEIGHT + (Map::HEIGHT * 0.1f));

			m_targetPosition = Vec2(currentLocation.x, offscreenTop);

			break;
		}

		default:
		{
			ERROR_AND_DIE("Unknown Movement Type for Movement Term!");
			break;
		}
	}
}

// -----------------------------------------------------------------------
// Effect;
// -----------------------------------------------------------------------
EffectTerm::EffectTerm(XmlElement* termElement_)
{
	SetTexture(ParseXmlAttribute(*termElement_, "texture", ""));
	SetDimensions(ParseXmlAttribute(*termElement_, "dimensions", Vec2(0.0f, 0.0f)));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	XmlElement* animationElement = termElement_->FirstChildElement("animation");
	if (animationElement)
	{
		SetSpriteAnimationDefinition
		(
			g_Interface->match().m_abilitySpriteSheets[ParseXmlAttribute(*animationElement, "ability_name", "")],
			ParseXmlAttribute(*animationElement, "start", 0),
			ParseXmlAttribute(*animationElement, "end", 0),
			ParseXmlAttribute(*animationElement, "duration", 0.0f),
			ParseXmlAttribute(*animationElement, "playback", "loop")
		);
	}

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void EffectTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		m_abilityCurrentlySequencing->SetSpriteDimensions(m_dimensions);
		m_abilityCurrentlySequencing->SetSpriteTexture(m_texture);
		m_abilityCurrentlySequencing->SetRenderAbility(true);
		m_animationTimer = 0.0f;

		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void EffectTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	m_animationTimer += deltaSeconds_;
	m_abilityCurrentlySequencing->SetCurrentSpriteDefinition(m_spriteAnimationDefinition->GetSpriteDefinitionAtTime(m_animationTimer));

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void EffectTerm::End()
{
	m_termState = TermState::END;

	m_abilityCurrentlySequencing->SetRenderAbility(false);

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* EffectTerm::Clone()
{
	return new EffectTerm(*this);
}

// -----------------------------------------------------------------------
void EffectTerm::SetTexture(std::string texture_)
{
	m_texture = texture_;
}

// -----------------------------------------------------------------------
void EffectTerm::SetDimensions(Vec2 dimensions_)
{
	m_dimensions = dimensions_;
}

// -----------------------------------------------------------------------
void EffectTerm::SetSpriteAnimationDefinition(SpriteSheet* spriteSheet_, int startIndex_, int endIndex_, float duration_, std::string playback_)
{
	SpriteAnimationPlaybackType playback = SpriteAnimationDefinition::StringToSpriteAnimationPlaybackType(playback_);
	m_spriteAnimationDefinition = new SpriteAnimationDefinition(*spriteSheet_, startIndex_, endIndex_, duration_, playback);
}

// -----------------------------------------------------------------------
// Audio;
// -----------------------------------------------------------------------
AudioTerm::AudioTerm(XmlElement* termElement_)
{
	SetAudio(ParseXmlAttribute(*termElement_, "audio", ""));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void AudioTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void AudioTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	ChannelGroupID sfxGroup = g_theAudioSystem->CreateOrGetChannelGroup("SFX");
	SoundID audio = g_theAudioSystem->CreateOrGetSound(m_audio);
	g_theAudioSystem->PlaySound(audio, sfxGroup, false);

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void AudioTerm::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* AudioTerm::Clone()
{
	return new AudioTerm(*this);
}

// -----------------------------------------------------------------------
void AudioTerm::SetAudio(std::string audio_)
{
	m_audio = audio_;
}

// -----------------------------------------------------------------------
// Damage;
// -----------------------------------------------------------------------
DamageTerm::DamageTerm(XmlElement* termElement_)
{
	SetDamagePercent(ParseXmlAttribute(*termElement_, "damage_percent", 0.0f));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));
	SetDamageModifier(ParseXmlAttribute(*termElement_, "damage_modifier", 1));
	
	SetSetupComplete();
}

// -----------------------------------------------------------------------
void DamageTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void DamageTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	m_abilityCurrentlySequencing->ApplyPercentDamage(m_damagePercent, m_damageModifier);

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void DamageTerm::End()
{
	m_termState = TermState::END;

	// Apply damage modifier, this will be 1 for most cases;
	if(m_damageModifier > 1)
	{
		m_damageModifier++;
	}

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* DamageTerm::Clone()
{
	return new DamageTerm(*this);
}

// -----------------------------------------------------------------------
void DamageTerm::SetDamagePercent(float damagePercent_)
{
	m_damagePercent = damagePercent_;
}

// -----------------------------------------------------------------------
void DamageTerm::SetDamageModifier(int damageModifier_)
{
	m_damageModifier = damageModifier_;
}

// -----------------------------------------------------------------------
// Debuff;
// -----------------------------------------------------------------------
DebuffTerm::DebuffTerm(XmlElement* termElement_)
{
	SetDebuffAbilityName(ParseXmlAttribute(*termElement_, "name", ""));
	SetPercentChance(ParseXmlAttribute(*termElement_, "chance", 0.0f));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void DebuffTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void DebuffTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	if(RollForDebuff())
	{
		m_abilityCurrentlySequencing->ApplyDebuff(m_debuffAbilityName);
	}

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void DebuffTerm::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* DebuffTerm::Clone()
{
	return new DebuffTerm(*this);
}

// -----------------------------------------------------------------------
void DebuffTerm::SetDebuffAbilityName(std::string debuffAbilityName_)
{
	m_debuffAbilityName = debuffAbilityName_;
}

// -----------------------------------------------------------------------
void DebuffTerm::SetPercentChance(float percentChance_)
{
	m_percentChance_ = percentChance_;
}

// -----------------------------------------------------------------------
bool DebuffTerm::RollForDebuff()
{
	return g_theRandomNumberGenerator->GetRandomFloatZeroToOne() <= m_percentChance_;
}

// -----------------------------------------------------------------------
// Buff;
// -----------------------------------------------------------------------
BuffTerm::BuffTerm(XmlElement* termElement_)
{
	SetBuffAbilityName(ParseXmlAttribute(*termElement_, "name", ""));
	SetPercentChance(ParseXmlAttribute(*termElement_, "chance", 0.0f));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void BuffTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void BuffTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	if (RollForBuff())
	{
		m_abilityCurrentlySequencing->ApplyBuff(m_buffAbilityName);
	}

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void BuffTerm::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* BuffTerm::Clone()
{
	return new BuffTerm(*this);
}

// -----------------------------------------------------------------------
void BuffTerm::SetBuffAbilityName(std::string buffAbilityName_)
{
	m_buffAbilityName = buffAbilityName_;
}

// -----------------------------------------------------------------------
void BuffTerm::SetPercentChance(float percentChance_)
{
	m_percentChance_ = percentChance_;
}

// -----------------------------------------------------------------------
bool BuffTerm::RollForBuff()
{
	return g_theRandomNumberGenerator->GetRandomFloatZeroToOne() <= m_percentChance_;
}

// -----------------------------------------------------------------------
// Status;
// -----------------------------------------------------------------------
StatusTerm::StatusTerm(XmlElement* termElement_)
{
	SetStatusAbilityName(ParseXmlAttribute(*termElement_, "name", ""));
	SetPercentChance(ParseXmlAttribute(*termElement_, "chance", 0.0f));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void StatusTerm::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void StatusTerm::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	if (RollForStatus())
	{
		m_abilityCurrentlySequencing->ApplyStatus(m_statusAbilityName);
	}

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void StatusTerm::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* StatusTerm::Clone()
{
	return new StatusTerm(*this);
}

// -----------------------------------------------------------------------
void StatusTerm::SetStatusAbilityName(std::string statusAbilityName_)
{
	m_statusAbilityName = statusAbilityName_;
}

// -----------------------------------------------------------------------
void StatusTerm::SetPercentChance(float percentChance_)
{
	m_percentChance_ = percentChance_;
}

// -----------------------------------------------------------------------
bool StatusTerm::RollForStatus()
{
	return g_theRandomNumberGenerator->GetRandomFloatZeroToOne() <= m_percentChance_;
}

// -----------------------------------------------------------------------
// AttackChange;
// -----------------------------------------------------------------------
AttackChange::AttackChange(XmlElement* termElement_)
{
	SetAmountChange(ParseXmlAttribute(*termElement_, "amount", 0));
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void AttackChange::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void AttackChange::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;
	m_abilityCurrentlySequencing->GetCaster()->AttackChange(m_amountChange_);

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void AttackChange::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* AttackChange::Clone()
{
	return new AttackChange(*this);
}

// -----------------------------------------------------------------------
void AttackChange::SetAmountChange(int amountChange_)
{
	m_amountChange_ = amountChange_;
}

// -----------------------------------------------------------------------
Disspell::Disspell(XmlElement* termElement_)
{
	SetAtTime(ParseXmlAttribute(*termElement_, "atTime", 0.0f));
	SetDuration(ParseXmlAttribute(*termElement_, "duration", 0.0f));

	SetSetupComplete();
}

// -----------------------------------------------------------------------
void Disspell::Start(float deltaSeconds_)
{
	m_termState = TermState::START;

	// Stay in the START state until it is time to DO this term;
	if (m_abilityCurrentlySequencing->m_lifetimeTimer >= m_atTime)
	{
		Tick(deltaSeconds_);
	}
}

// -----------------------------------------------------------------------
void Disspell::Tick(float deltaSeconds_)
{
	m_termState = TermState::DO;

	// This term will DO what needs to be done for the length of its duration;
	// Everything in this DO will run each frame;

	m_runningTimer += deltaSeconds_;

	for (Ability* status : m_abilityCurrentlySequencing->GetTarget()->m_activeStatusEffects)
	{
		std::string unitType = UnitDefinition::UnitTypeToString(m_abilityCurrentlySequencing->GetCaster()->m_unitDefinition->m_type);
		std::string printString = Stringf("%s has had %s removed.", unitType.c_str(), status->GetAbilityDefinition()->m_name.c_str());
		g_theDevConsole->Print(printString);
		status->m_abilityHasBeenDisspelled = true;
	}

	m_abilityCurrentlySequencing->m_abilityHasBeenDisspelled = true;

	// When the term has run for its duration, it will END;
	if (m_runningTimer >= m_duration)
	{
		End();
	}
}

// -----------------------------------------------------------------------
void Disspell::End()
{
	m_termState = TermState::END;

	// There is nothing to end for this term in its current iteration;

	m_termState = TermState::FINISHED;
}

// -----------------------------------------------------------------------
Term* Disspell::Clone()
{
	return new Disspell(*this);
}
