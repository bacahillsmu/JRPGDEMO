#pragma once

// -----------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

// -----------------------------------------------------------------------
#include "Game/Units/UnitDefinition.hpp"


class Ability;


// -----------------------------------------------------------------------
class Unit
{

public:

	Unit() = delete;
	Unit(const std::string& unitType_);
	Unit(JobType unitType_);
	~Unit();

	// Flow;
	void Create(JobType unitType_);
	void BattleUpdate(float deltaSeconds_);
	void PurchaseUpdate(float deltaSeconds_);
	void BattleRender();
	void BattleRenderEnemy();
	void PurchaseRender();
	void RenderAbilities();
	void UpdateSpriteDefinition(float deltaSeconds_);
	void UpdateDamageTracker(float deltaSeconds_);
	void UpdateHealTracker(float deltaSeconds_);

	// Helpers;
	Vec2 GetPosition();
	Vec2 GetEnemyPosition();
	Vec2 GetLocation();
	void SetLocation(Vec2 location_);
	void ApplyStatus(const std::string& status_, Unit* caster_);
	void ApplyDebuff(const std::string& debuff_, Unit* caster_);
	void ApplyBuff(const std::string& buff_, Unit* caster_);
	void AttackChange(int amountChange_);
	bool CheckIfStatusAlreadyIsApplied(const AbilityDefinition* newStatus_);
	bool CheckIfDebuffAlreadyIsApplied(const AbilityDefinition* newDebuff_);
	bool CheckIfBuffAlreadyIsApplied(const AbilityDefinition* newBuff_);

	// Abilities;
	bool AllStatusEffectsCompleteForThisTurn();
	bool AllBuffEffectsCompleteForThisTurn();
	void ResetStatusEffects();
	void ResetBuffEffects();
	void UpdateStatusEffects(float deltaSeconds_);
	void UpdateBuffEffects(float deltaSeconds_);
	void UpdateAbilities(float deltaSeconds_);
	bool UpdateBuffAbilities(float deltaSeconds_);
	bool UpdateDebuffAbilities(float deltaSeconds_);
	void UpdateMainAbilities(float deltaSeconds_);
	void CheckForCleanupOfAbilities();
	void UpdateStatusIcon(float deltaSeconds_);
	void UpdateBuffIcon(float deltaSeconds_);
	void ResetStatusIcons();
	void ResetBuffIcon();

	int m_playerID = -1;
	int m_slotID = -1;
	unsigned int m_unitID = 0u;
	Vec2 m_location = Vec2(0.0f, 0.0f);
	bool m_isMyTurnToDoAction = false;
	
	UnitDefinition* m_unitDefinition = nullptr;
	JobType m_type = JobType::INVALID;
	int m_health = 0;
	int m_mana = 0;
	int m_strength = 0;
	int m_intellect = 0;
	int m_wisdom = 0;
	int m_constitution = 0;
	int m_speed = 0;
	std::string m_currentAnimationName = "";

	int m_justTookDamageAmount = -1;
	int m_justHealedAmount = -1;
	float m_justTookDamageTimer = 0.0f;
	float m_justTookDamageTimeAmount = 1.0f;
	

	// Sprite Information;
	SpriteDefinition m_currentSpriteDefinition;
	Vec2 m_currentSpriteAnimationBottomLeftUV = Vec2(0.0f, 0.0f);
	Vec2 m_currentSpriteAnimationToptUV = Vec2(0.0f, 0.0f);
	float m_animationTimer = 0.0f;

	// Ability;
	std::vector<Ability*> m_activeAbilities;

	// Debuffs;
	std::vector<Ability*> m_activeDebuffs;
	std::vector<Ability*> m_activeBuffs;
	std::vector<Ability*> m_activeStatusEffects;
	float m_displayStatusTimer = 0.0f;
	float m_displayStatusTimeLength = 2.0f;
	float m_displayBuffTimer = 0.0f;
	float m_displayBuffTimeLength = 2.0f;
	int m_currentStatusEffectIconIndex = 0;
	int m_currentBuffEffectIconIndex = 0;
	int m_statusIconEffects = 0;
	int m_buffIconEffects = 0;
	bool m_dontDoStatusEffects = false;
	bool m_dontDoBuffEffects = false;
	bool m_dontDoDebuffEffects = false;
};
