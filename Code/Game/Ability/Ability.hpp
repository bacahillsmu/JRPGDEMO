#pragma once

// -----------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

// -----------------------------------------------------------------------
#include "Game/Ability/AbilityDefinition.hpp"
#include "Game/Ability/Term.hpp"

class Unit;

class Ability
{

public:

	Ability() = delete;
	explicit Ability(const AbilityDefinition* abilityDefinition_);
	~Ability();

	// Flow;
	void Update(float deltaSeconds_);
	void Render();

	void AssignCasterAndCasterOriginalLocation(Unit* caster_);
	void AssignTarget(Unit* target_);
	void ApplyPercentDamage(float percentDamage_, int damageModifier_);
	void DoDamage(float percentDamage_, int damageModifier_);
	void DoHealing(float percentDamage_, int damageModifier_);
	void ApplyStatus(std::string statusName_);
	void ApplyDebuff(std::string debuffName_);
	void ApplyBuff(std::string buffName_);

	// Helpers;
	const AbilityDefinition* GetAbilityDefinition();

	// Ability Information;
	int GetBaseDamage();
	AbilityClass GetAbilityClass();
	TargetChoice GetTargetChoice();
	TargetAlliance GetTargetAlliance();
	ActivationPeriod GetActivationPeriod();
	Unit* GetCaster();
	Vec2 GetCasterOriginalLocation();
	void SetCurrentSpriteDefinition(SpriteDefinition currentSpriteDefinition_);
	void SetSpriteDimensions(Vec2 dimensions_);
	void SetSpriteTexture(std::string texture_);
	void SetRenderAbility(bool renderAbility_);
	void AddOffsetStartTimeToSequence(float offsetTime_);
	void ResetTermStateOnAllTermsInSequence();
	bool SequenceFinishedAllTerms();
	std::string GetAbilityStatusEffectIcon();
	std::string GetAbilityBuffEffectIcon();
	Unit* GetTarget();

public:

	bool m_abilityFinished = false;
	bool m_abilityHasBeenDisspelled = false;
	float m_lifetimeTimer = 0.0f;
	float m_lifetimeTotalTime = 0.0f;

	Vec2 m_castLocation = Vec2(0.0f, 0.0f);
	Vec2 m_dimensions = Vec2(0.0f, 0.0f);
	std::string m_spriteTexture = "";
	bool m_renderAbility = false;

private:

	// Only the constructor should call this, only way to create an Ability;
	void Create(const AbilityDefinition* abilityDefinition_);
	// --------------------------------------------------------------------;

	Unit* m_caster = nullptr;
	Unit* m_target = nullptr;
	Vec2 m_casterOriginalPosition = Vec2();

	// Ability Definition;
	const AbilityDefinition* m_abilityDefinition = nullptr;

	AbilitySequence m_abilitySequence;

	// Sprite Information;
	SpriteDefinition m_currentSpriteDefinition;
	Vec2 m_currentSpriteAnimationBottomLeftUV = Vec2(0.0f, 0.0f);
	Vec2 m_currentSpriteAnimationToptUV = Vec2(0.0f, 0.0f);

};