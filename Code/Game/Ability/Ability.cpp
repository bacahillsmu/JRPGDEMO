#include "Game/Ability/Ability.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Framework/GameCommon.hpp"

// ------------------------------------------------------------------
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/RandomNumberGenerator.hpp"

// ------------------------------------------------------------------
#include "Game/Units/Unit.hpp"

// ------------------------------------------------------------------
Ability::Ability(const AbilityDefinition* abilityDefinition_)
{
	Create(abilityDefinition_);
}

// ------------------------------------------------------------------
Ability::~Ability()
{
	for (Term*& term : m_abilitySequence)
	{
		delete term;
		term = nullptr;
	}
	m_abilitySequence.clear();
}

// ------------------------------------------------------------------
void Ability::Update(float deltaSeconds_)
{
	for(Term* term : m_abilitySequence)
	{
		term->SetAbilityCurrentlySequencing(this);
		term->Run(deltaSeconds_);
	}
	
	if(SequenceFinishedAllTerms())
	{
		m_abilityFinished = true;
	}

	m_lifetimeTimer += deltaSeconds_;
}

// ------------------------------------------------------------------
void Ability::Render()
{
	if(m_abilityFinished)
	{
		return;
	}

	if(!m_renderAbility)
	{
		return;
	}

	std::vector<Vertex_PCU> abilityVerts;
	
	AABB2 abilityBox = AABB2(m_castLocation, m_dimensions / 2.0f);

	m_currentSpriteDefinition.GetUVs(m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);
	AddVertsForAABB2D(abilityVerts, abilityBox, Rgba::WHITE, m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);

	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_spriteTexture));
	g_theRenderer->DrawVertexArray((int)abilityVerts.size(), &abilityVerts[0]);
}

// ------------------------------------------------------------------
void Ability::AssignCasterAndCasterOriginalLocation(Unit* caster_)
{
	m_caster = caster_;
	m_casterOriginalPosition = caster_->GetLocation();
}

// ------------------------------------------------------------------
void Ability::AssignTarget(Unit* target_)
{
	m_target = target_;
}

// ------------------------------------------------------------------
void Ability::ApplyPercentDamage(float percentDamage_, int damageModifier_)
{
	switch (GetTargetAlliance())
	{
		case TargetAlliance::ENEMY:
		{
			DoDamage(percentDamage_, damageModifier_);
			break;
		}

		case TargetAlliance::FRIENDLY:
		{
			DoHealing(percentDamage_, damageModifier_);
			break;
		}

		default:
		{
			ERROR_AND_DIE("Ability does not have a target alliance!");
			break;
		}
	}
}

// ------------------------------------------------------------------
void Ability::DoDamage(float percentDamage_, int damageModifier_)
{
	UNUSED(percentDamage_);

	switch(GetAbilityClass())
	{
		case AbilityClass::PHYSICAL:
		{
			int multiplier = m_caster->m_strength - m_target->m_constitution;
			multiplier = Clamp(multiplier, 0, m_caster->m_strength);

			int damage = GetBaseDamage() * damageModifier_ * multiplier;

			m_target->m_health -= damage;
			m_target->m_justTookDamageAmount = damage;

			break;
		}

		case AbilityClass::MAGIC:
		{
			int multiplier = m_caster->m_intellect - m_target->m_wisdom;
			multiplier = Clamp(multiplier, 0, m_caster->m_intellect);

			int damage = GetBaseDamage() * multiplier;

			m_target->m_health -= damage;
			m_target->m_justTookDamageAmount = damage;

			break;
		}

		default:
		{
			ERROR_AND_DIE("Ability has an unknown Ability Class, it needs one!");

			break;
		}
	}
}

// ------------------------------------------------------------------
void Ability::DoHealing(float percentDamage_, int damageModifier_)
{
	UNUSED(percentDamage_);

	switch (GetAbilityClass())
	{
		case AbilityClass::PHYSICAL:
		{
			ERROR_AND_DIE("We have no physical healing as of yet!");

			break;
		}

		case AbilityClass::MAGIC:
		{
			int multiplier = m_caster->m_intellect;

			int healAmount = GetBaseDamage() * damageModifier_ * multiplier;

			m_target->m_health += healAmount;
			m_target->m_justHealedAmount = healAmount;

			break;
		}

		default:
		{
			ERROR_AND_DIE("Ability has an unknown Ability Class, it needs one!");

			break;
		}
	}
}

// ------------------------------------------------------------------
void Ability::ApplyStatus(std::string statusName_)
{
	m_target->ApplyStatus(statusName_, m_caster);
}

// ------------------------------------------------------------------
void Ability::ApplyDebuff(std::string debuffName_)
{
	m_target->ApplyDebuff(debuffName_, m_caster);
}

// ------------------------------------------------------------------
void Ability::ApplyBuff(std::string buffName_)
{
	const AbilityDefinition* buffDef = AbilityDefinition::s_abilityDefinitions[buffName_];

	switch (buffDef->m_targetChoice)
	{
		case TargetChoice::SELF:
		{
			m_caster->ApplyBuff(buffName_, m_caster);
			break;
		}

		case TargetChoice::RANDOM:
		{
			m_target->ApplyBuff(buffName_, m_caster);
			break;
		}

		default:
		{
			ERROR_AND_DIE("Buff is not targeting self. Only self is supported.");
		}
	}
}

// ------------------------------------------------------------------
const AbilityDefinition* Ability::GetAbilityDefinition()
{
	return m_abilityDefinition;
}

// ------------------------------------------------------------------
int Ability::GetBaseDamage()
{
	return m_abilityDefinition->m_baseDamage;
}

// ------------------------------------------------------------------
AbilityClass Ability::GetAbilityClass()
{
	return m_abilityDefinition->m_abilityClass;
}

// ------------------------------------------------------------------
TargetChoice Ability::GetTargetChoice()
{
	return m_abilityDefinition->m_targetChoice;
}

// ------------------------------------------------------------------
TargetAlliance Ability::GetTargetAlliance()
{
	return m_abilityDefinition->m_targetAlliance;
}

// ------------------------------------------------------------------
ActivationPeriod Ability::GetActivationPeriod()
{
	return m_abilityDefinition->m_activationPeriod;
}

// ------------------------------------------------------------------
Unit* Ability::GetCaster()
{
	return m_caster;
}

// ------------------------------------------------------------------
Vec2 Ability::GetCasterOriginalLocation()
{
	return m_casterOriginalPosition;
}

// ------------------------------------------------------------------
void Ability::SetCurrentSpriteDefinition(SpriteDefinition currentSpriteDefinition_)
{
	m_currentSpriteDefinition = currentSpriteDefinition_;
}

// ------------------------------------------------------------------
void Ability::SetSpriteDimensions(Vec2 dimensions_)
{
	m_dimensions = dimensions_;
}

// ------------------------------------------------------------------
void Ability::SetSpriteTexture(std::string texture_)
{
	m_spriteTexture = texture_;
}

// ------------------------------------------------------------------
void Ability::SetRenderAbility(bool renderAbility_)
{
	m_renderAbility = renderAbility_;
}

// ------------------------------------------------------------------
void Ability::AddOffsetStartTimeToSequence(float offsetTime_)
{
	for(Term* term : m_abilitySequence)
	{
		term->AddToAtTime(offsetTime_);
	}
}

// ------------------------------------------------------------------
void Ability::ResetTermStateOnAllTermsInSequence()
{
	for(Term* term : m_abilitySequence)
	{
		term->SetStateToStart();
	}
}

// ------------------------------------------------------------------
bool Ability::SequenceFinishedAllTerms()
{
	bool allFinished = true;
	for(Term* term : m_abilitySequence)
	{
		if(!term->IsFinished())
		{
			allFinished = false;
		}
	}

	return allFinished;
}

// ------------------------------------------------------------------
std::string Ability::GetAbilityStatusEffectIcon()
{
	return m_abilityDefinition->m_statusIcon;
}

// ------------------------------------------------------------------
std::string Ability::GetAbilityBuffEffectIcon()
{
	return m_abilityDefinition->m_buffIcon;
}

// ------------------------------------------------------------------
Unit* Ability::GetTarget()
{
	return m_target;
}

// ------------------------------------------------------------------
void Ability::Create(const AbilityDefinition* abilityDefinition_)
{
	m_abilityDefinition = abilityDefinition_;

	m_lifetimeTotalTime = m_abilityDefinition->m_sequenceLifetime;

	for(Term* term : m_abilityDefinition->m_abilitySequence)
	{
		Term* clone = term->Clone();
		m_abilitySequence.push_back(clone);
	}
}
