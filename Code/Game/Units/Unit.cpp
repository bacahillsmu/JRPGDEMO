#include "Game/Units/Unit.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Framework/GameCommon.hpp"

// ------------------------------------------------------------------
#include "Engine/Core/RandomNumberGenerator.hpp"
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"
#include "Engine/Renderer/BitMapFont.hpp"

// ------------------------------------------------------------------
#include "Game/Framework/App.hpp"
#include "Game/Framework/Interface.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Units/Units.hpp"
#include "Game/Units/UnitDefinition.hpp"
#include "Game/Units/UnitFilters.hpp"
#include "Game/Ability/Ability.hpp"

// ------------------------------------------------------------------
// Unit;
// ------------------------------------------------------------------
Unit::Unit(const std::string& unitType_)
{
	JobType unitType = UnitDefinition::StringToUnitType(unitType_);
	Create(unitType);
}

// ------------------------------------------------------------------
Unit::Unit(JobType unitType_)
{
	Create(unitType_);
}

// ------------------------------------------------------------------
Unit::~Unit()
{
	for(Ability*& ability : m_activeAbilities)
	{
		DELETE_POINTER(ability);
	}
	m_activeAbilities.clear();

	for (Ability*& debuff : m_activeDebuffs)
	{
		DELETE_POINTER(debuff);
	}
	m_activeDebuffs.clear();
}

// ------------------------------------------------------------------
void Unit::Create(JobType unitType_)
{
	/*
	// Set the GameHandle;
	uint16_t slot = m_theMap->GetFreeEntityIndex();
	uint16_t cyclicId = m_theMap->GetNextCyclicId();
	GameHandle handle = GameHandle(cyclicId, slot);
	m_gameHandle = handle;
	*/

	// Get the Entity Definition;
	std::map<JobType, UnitDefinition*>::iterator unitIter = UnitDefinition::s_unitDefinitions.find(unitType_);
	if (unitIter == UnitDefinition::s_unitDefinitions.end())
	{
		ERROR_AND_DIE("Attempted to Create a Unit with a unit type that is not loaded.")
	}
	m_unitDefinition = unitIter->second;

	// Set Stats;
	m_type			= unitType_;
	m_health		= m_unitDefinition->m_health;
	m_mana			= m_unitDefinition->m_mana;
	m_strength		= m_unitDefinition->m_strength;
	m_intellect		= m_unitDefinition->m_intellect;
	m_wisdom		= m_unitDefinition->m_wisdom;
	m_constitution	= m_unitDefinition->m_constitution;
	m_speed			= m_unitDefinition->m_speed;

	m_currentAnimationName = "Idle";
	m_animationTimer = 0.0f;
}

// ------------------------------------------------------------------
void Unit::BattleUpdate(float deltaSeconds_)
{
	m_health = Clamp(m_health, 0, m_unitDefinition->m_health);

	// Sprites and Animations;
	UpdateSpriteDefinition(deltaSeconds_);

	// Trackers;
	UpdateDamageTracker(deltaSeconds_);
	UpdateHealTracker(deltaSeconds_);

	// Status Checks (only apply when it is their turn to act);
	// Order must retain: Status, Buff, Debuff, Main;
	if(m_isMyTurnToDoAction && m_health > 0)
	{
		if(!AllStatusEffectsCompleteForThisTurn() && !m_dontDoStatusEffects)
		{
			// Update Status;
			UpdateStatusEffects(deltaSeconds_);
		}
		else if(!AllBuffEffectsCompleteForThisTurn() && !m_dontDoBuffEffects)
		{
			m_dontDoStatusEffects = true;

			// Update Buffs;
			UpdateBuffEffects(deltaSeconds_);
		}
// 		else if(!AllDebuffEffectsCompleteForThisTurn() && !m_dontDoDebuffEffects)
// 		{
// 			m_dontDoBuffEffects = true;
// 			m_dontDoBuffEffects = true;
// 
// 			// Update Debuffs;
// 			UpdateDebuffEffects(deltaSeconds_);
// 		}
		else
		{
			m_dontDoStatusEffects = true;
			m_dontDoBuffEffects = true;
			m_dontDoDebuffEffects = true;

			// Update Main;
			UpdateAbilities(deltaSeconds_);
		}
	}

	// Cleanup abilities that have finished;
	CheckForCleanupOfAbilities();

	// Logic for displaying a status icon above the head;
	UpdateStatusIcon(deltaSeconds_);
	UpdateBuffIcon(deltaSeconds_);

	// This is how the turn for myself will end and we will pass it off to the next person;
	if (m_isMyTurnToDoAction && (m_activeAbilities.size() == 0 || m_health <= 0))
	{
		g_Interface->match().m_battleMap->m_actionTurnEnding = true;
	}
}

// ------------------------------------------------------------------
void Unit::PurchaseUpdate(float deltaSeconds_)
{
	// Sprites and Animations;
	m_animationTimer += deltaSeconds_;
	SpriteAnimationDefinition* spriteAnimationDefinition = m_unitDefinition->m_animationSet["Idle"];
	m_currentSpriteDefinition = spriteAnimationDefinition->GetSpriteDefinitionAtTime(m_animationTimer);
}

// ------------------------------------------------------------------
void Unit::BattleRender()
{
	std::vector<Vertex_PCU> boxVerts;

	Vec2 dimensions = g_Interface->match().GetBattleMap()->m_unitSlotDimensions;
	Vec2 centerSlot = m_location;

	AABB2 box = AABB2(centerSlot, dimensions / 2);

	m_currentSpriteDefinition.GetUVs(m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);
	AddVertsForAABB2D(boxVerts, box, Rgba::WHITE, m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);

	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_unitDefinition->m_texture));
	g_theRenderer->DrawVertexArray((int)boxVerts.size(), &boxVerts[0]);

	// Portrait;
	std::vector<Vertex_PCU> portraitVerts;
	Vec2 portraitDimensions = g_Interface->match().GetBattleMap()->m_battleWindowUnitSlotDimensions;
	portraitDimensions = portraitDimensions * Vec2(0.25f, 1.0f);
	Vec2 portraitSlot = g_Interface->match().GetBattleMap()->m_friendlyBattleInformationUnitSlots[m_slotID].mins;
	portraitSlot.x += portraitDimensions.x * 0.5f;
	portraitSlot.y += portraitDimensions.y * 0.5f;
	AABB2 portrait = AABB2(portraitSlot, portraitDimensions / 2);

	AddVertsForAABB2D(portraitVerts, portrait, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_unitDefinition->m_portrait));
	g_theRenderer->DrawVertexArray((int)portraitVerts.size(), &portraitVerts[0]);

	// Stats;
	std::vector<Vertex_PCU> statsVerts;
	float offset = (portrait.maxs.y - portrait.mins.y) * 0.2f;
	float xOffset = (portrait.maxs.x - portrait.mins.x) * 1.1f;
	Vec2 healthPoint = portrait.center;
	healthPoint.x += xOffset;
	healthPoint.y += offset;
	Vec2 manaPoint = portrait.center;
	manaPoint.x += xOffset;
	manaPoint.y -= offset;

	std::string health = Stringf("%d / %d", m_health, m_unitDefinition->m_health);
	std::string mana = Stringf("%d / %d", m_mana, m_unitDefinition->m_mana);
	BitMapFont* statsFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	statsFont->AddVertsForText2D(statsVerts, healthPoint, 0.5f, health, Rgba::WHITE);
	statsFont->AddVertsForText2D(statsVerts, manaPoint, 0.5f, mana, Rgba::WHITE);
	g_theRenderer->BindTextureView(0u, statsFont->GetTextureView());
	g_theRenderer->DrawVertexArray((int)statsVerts.size(), &statsVerts[0]);

	if(m_justTookDamageAmount >= 0)
	{
		// Figure the spot to show the damage amount; Somewhere close to the unit;
		Vec2 position = GetPosition();
		Vec2 start = position - Vec2(2.0f, 2.0f);
		Vec2 end = position + Vec2(2.0f, 2.0f);
		AABB2 box2 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> textVerts;
		std::string amount = Stringf("%d", m_justTookDamageAmount);
		BitMapFont* timerFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
		if(m_justTookDamageAmount > 0)
		{
			timerFont->AddVertsForText2D(textVerts, box2.center, 1.0f, amount, Rgba::RED);
		}
		else
		{
			timerFont->AddVertsForText2D(textVerts, box2.center, 1.0f, "Miss", Rgba::TEAL);
		}
		
		g_theRenderer->BindTextureView(0u, timerFont->GetTextureView());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}
	else if(m_justHealedAmount >= 0)
	{
		// Figure the spot to show the heal amount; Somewhere close to the unit;
		Vec2 position = GetPosition();
		Vec2 start = position - Vec2(2.0f, 2.0f);
		Vec2 end = position + Vec2(2.0f, 2.0f);
		AABB2 box2 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> textVerts;
		std::string amount = Stringf("%d", m_justHealedAmount);
		BitMapFont* timerFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
		timerFont->AddVertsForText2D(textVerts, box2.center, 1.0f, amount, Rgba::GREEN);

		g_theRenderer->BindTextureView(0u, timerFont->GetTextureView());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}

	if(m_activeStatusEffects.size() > 0)
	{
		Vec2 position = GetPosition();
		Vec2 start = position + Vec2(-3.0f, 1.0f);
		Vec2 end = position + Vec2(-1.0f, 2.0f);
		AABB2 box3 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> statusIconVerts;
		AddVertsForAABB2D(statusIconVerts, box3, Rgba::WHITE);
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_activeStatusEffects[m_currentStatusEffectIconIndex]->GetAbilityStatusEffectIcon().c_str()));
		g_theRenderer->DrawVertexArray((int)statusIconVerts.size(), &statusIconVerts[0]);
	}

	if (m_activeBuffs.size() > 0)
	{
		Vec2 position = GetPosition();
		Vec2 start = position + Vec2(2.0f, 1.0f);
		Vec2 end = position + Vec2(4.0f, 2.0f);
		AABB2 box4 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> buffIconVerts;
		AddVertsForAABB2D(buffIconVerts, box4, Rgba::WHITE);
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_activeBuffs[m_currentBuffEffectIconIndex]->GetAbilityBuffEffectIcon().c_str()));
		g_theRenderer->DrawVertexArray((int)buffIconVerts.size(), &buffIconVerts[0]);
	}
}

// ------------------------------------------------------------------
void Unit::BattleRenderEnemy()
{
	std::vector<Vertex_PCU> boxVerts;

	Vec2 dimensions = g_Interface->match().GetBattleMap()->m_unitSlotDimensions;
	Vec2 centerSlot = m_location;

	AABB2 box = AABB2(centerSlot, dimensions / 2);

	m_currentSpriteDefinition.GetUVs(m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);

	// flip
	float tempx = m_currentSpriteAnimationToptUV.x;
	m_currentSpriteAnimationToptUV.x = m_currentSpriteAnimationBottomLeftUV.x;
	m_currentSpriteAnimationBottomLeftUV.x = tempx;

	AddVertsForAABB2D(boxVerts, box, Rgba::WHITE, m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);

	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_unitDefinition->m_texture));
	g_theRenderer->DrawVertexArray((int)boxVerts.size(), &boxVerts[0]);

	// Portrait;
	std::vector<Vertex_PCU> portraitVerts;
	Vec2 portraitDimensions = g_Interface->match().GetBattleMap()->m_battleWindowUnitSlotDimensions;
	portraitDimensions = portraitDimensions * Vec2(0.25f, 1.0f);
	Vec2 portraitSlot = g_Interface->match().GetBattleMap()->m_enemyBattleInformationUnitSlots[m_slotID].mins;
	portraitSlot.x += portraitDimensions.x * 0.5f;
	portraitSlot.y += portraitDimensions.y * 0.5f;
	AABB2 portrait = AABB2(portraitSlot, portraitDimensions / 2);

	AddVertsForAABB2D(portraitVerts, portrait, Rgba::WHITE);
	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_unitDefinition->m_portrait));
	g_theRenderer->DrawVertexArray((int)portraitVerts.size(), &portraitVerts[0]);

	// Stats;
	std::vector<Vertex_PCU> statsVerts;
	float offset = (portrait.maxs.y - portrait.mins.y) * 0.2f;
	float xOffset = (portrait.maxs.x - portrait.mins.x) * 1.1f;
	Vec2 healthPoint = portrait.center;
	healthPoint.x += xOffset;
	healthPoint.y += offset;
	Vec2 manaPoint = portrait.center;
	manaPoint.x += xOffset;
	manaPoint.y -= offset;

	std::string health = Stringf("%d / %d", m_health, m_unitDefinition->m_health);
	std::string mana = Stringf("%d / %d", m_mana, m_unitDefinition->m_mana);
	BitMapFont* statsFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
	statsFont->AddVertsForText2D(statsVerts, healthPoint, 0.5f, health, Rgba::WHITE);
	statsFont->AddVertsForText2D(statsVerts, manaPoint, 0.5f, mana, Rgba::WHITE);
	g_theRenderer->BindTextureView(0u, statsFont->GetTextureView());
	g_theRenderer->DrawVertexArray((int)statsVerts.size(), &statsVerts[0]);

	if (m_justTookDamageAmount >= 0)
	{
		// Figure the spot to show the damage amount; Somewhere close to the unit;
		Vec2 position = GetEnemyPosition();
		Vec2 start = position - Vec2(2.0f, 2.0f);
		Vec2 end = position + Vec2(2.0f, 2.0f);
		AABB2 box2 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> textVerts;
		std::string amount = Stringf("%d", m_justTookDamageAmount);
		BitMapFont* timerFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
		if (m_justTookDamageAmount > 0)
		{
			timerFont->AddVertsForText2D(textVerts, box2.center, 1.0f, amount, Rgba::RED);
		}
		else
		{
			timerFont->AddVertsForText2D(textVerts, box2.center, 1.0f, "Miss", Rgba::TEAL);
		}

		g_theRenderer->BindTextureView(0u, timerFont->GetTextureView());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}
	else if (m_justHealedAmount >= 0)
	{
		// Figure the spot to show the heal amount; Somewhere close to the unit;
		Vec2 position = GetEnemyPosition();
		Vec2 start = position - Vec2(2.0f, 2.0f);
		Vec2 end = position + Vec2(2.0f, 2.0f);
		AABB2 box2 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> textVerts;
		std::string amount = Stringf("%d", m_justHealedAmount);
		BitMapFont* timerFont = g_theRenderer->CreateOrGetBitmapFontFixedWidth16x16("SquirrelFixedFont");
		timerFont->AddVertsForText2D(textVerts, box2.center, 1.0f, amount, Rgba::GREEN);

		g_theRenderer->BindTextureView(0u, timerFont->GetTextureView());
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}

	if (m_statusIconEffects > 0)
	{
		Vec2 position = GetEnemyPosition();
		Vec2 start = position + Vec2(2.0f, 1.0f);
		Vec2 end = position + Vec2(4.0f, 2.0f);
		AABB2 box3 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> statusIconVerts;
		AddVertsForAABB2D(statusIconVerts, box3, Rgba::WHITE);
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_activeStatusEffects[m_currentStatusEffectIconIndex]->GetAbilityStatusEffectIcon().c_str()));
		g_theRenderer->DrawVertexArray((int)statusIconVerts.size(), &statusIconVerts[0]);
	}

	if (m_buffIconEffects > 0)
	{
		Vec2 position = GetEnemyPosition();
		Vec2 start = position + Vec2(-3.0f, 1.0f);
		Vec2 end = position + Vec2(-1.0f, 2.0f);
		AABB2 box4 = AABB2::MakeFromMinsMaxs(start, end);

		// Show the damage amount;
		std::vector<Vertex_PCU> buffIconVerts;
		AddVertsForAABB2D(buffIconVerts, box4, Rgba::WHITE);
		g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
		g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_activeBuffs[m_currentBuffEffectIconIndex]->GetAbilityBuffEffectIcon().c_str()));
		g_theRenderer->DrawVertexArray((int)buffIconVerts.size(), &buffIconVerts[0]);
	}
}

// ------------------------------------------------------------------
void Unit::PurchaseRender()
{
	// We are rendering our opponents cards. We need to only render our cards.

	std::vector<Vertex_PCU> boxVerts;

	Vec2 dimensions = g_Interface->match().GetPurchaseMap()->m_unitSlotDimensions;
	Vec2 centerSlot = Vec2(-10.0f, -10.0f);
	if (m_slotID < 8)
	{
		centerSlot = g_Interface->match().GetPurchaseMap()->GetCenterPositionOfSlotInMyField(m_slotID);
	}

	AABB2 box = AABB2(centerSlot, dimensions / 2);

	m_currentSpriteDefinition.GetUVs(m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);
	//AddVertsForAABB2D(boxVerts, box, Rgba::WHITE);
	AddVertsForAABB2D(boxVerts, box, Rgba::WHITE, m_currentSpriteAnimationBottomLeftUV, m_currentSpriteAnimationToptUV);

	g_theRenderer->BindShader("Data/Shaders/default_unlit_devconsole.shader");
	//g_theRenderer->BindTextureViewWithSampler(0, nullptr);
	g_theRenderer->BindTextureViewWithSampler(0, g_theRenderer->CreateOrGetTextureViewFromFile(m_unitDefinition->m_texture));
	g_theRenderer->DrawVertexArray((int)boxVerts.size(), &boxVerts[0]);
}

// ------------------------------------------------------------------
void Unit::RenderAbilities()
{
	// Status;
	for (Ability* status : m_activeStatusEffects)
	{
		status->Render();
	}

	// Buffs;
	for (Ability* buff : m_activeBuffs)
	{
		buff->Render();
	}

	// Main;
	for (Ability* ability : m_activeAbilities)
	{
		ability->Render();
	}
}

// ------------------------------------------------------------------
void Unit::UpdateSpriteDefinition(float deltaSeconds_)
{
	m_animationTimer += deltaSeconds_;

	SpriteAnimationDefinition* spriteAnimationDefinition;
	if (m_health <= 0)
	{
		m_animationTimer = 0.0f;
		spriteAnimationDefinition = m_unitDefinition->m_animationSet["Dead"];
	}
	else
	{
		spriteAnimationDefinition = m_unitDefinition->m_animationSet[m_currentAnimationName];
	}

	m_currentSpriteDefinition = spriteAnimationDefinition->GetSpriteDefinitionAtTime(m_animationTimer);
}

// ------------------------------------------------------------------
void Unit::UpdateDamageTracker(float deltaSeconds_)
{
	// Just Took Damage Tracker;
	if (m_justTookDamageAmount >= 0)
	{
		m_justTookDamageTimer += deltaSeconds_;
		m_justTookDamageTimer = Clamp(m_justTookDamageTimer, 0.0f, m_justTookDamageTimeAmount);
		if (m_justTookDamageTimer == m_justTookDamageTimeAmount)
		{
			m_justTookDamageAmount = -1;
			m_justHealedAmount = -1;
			m_justTookDamageTimer = 0.0f;
		}
	}
}

// ------------------------------------------------------------------
void Unit::UpdateHealTracker(float deltaSeconds_)
{
	if (m_justHealedAmount >= 0)
	{
		m_justTookDamageTimer += deltaSeconds_;
		m_justTookDamageTimer = Clamp(m_justTookDamageTimer, 0.0f, m_justTookDamageTimeAmount);
		if (m_justTookDamageTimer == m_justTookDamageTimeAmount)
		{
			m_justTookDamageAmount = -1;
			m_justHealedAmount = -1;
			m_justTookDamageTimer = 0.0f;
		}
	}
}

// ------------------------------------------------------------------
Vec2 Unit::GetPosition()
{
	return g_Interface->match().GetBattleMap()->GetCenterPositionOfSlotInMyField(m_slotID);
}

// ------------------------------------------------------------------
Vec2 Unit::GetEnemyPosition()
{
	return g_Interface->match().GetBattleMap()->GetCenterPositionOfSlotInEnemyField(m_slotID);
}

// ------------------------------------------------------------------
Vec2 Unit::GetLocation()
{
	return m_location;
}

// ------------------------------------------------------------------
void Unit::SetLocation(Vec2 location_)
{
	m_location = location_;
}

// ------------------------------------------------------------------
void Unit::ApplyStatus(const std::string& status_, Unit* caster_)
{
	const AbilityDefinition* statusDef = AbilityDefinition::s_abilityDefinitions[status_];

	// Check to apply an abilityDef once for a debuff;
	bool hasStatusAlready = CheckIfStatusAlreadyIsApplied(statusDef);

	if (!hasStatusAlready)
	{
		Ability* status = new Ability(statusDef);
		status->AssignTarget(this);
		status->m_castLocation = GetLocation();
		status->AssignCasterAndCasterOriginalLocation(caster_);

		m_activeStatusEffects.push_back(status);
	}
}

// ------------------------------------------------------------------
void Unit::ApplyDebuff(const std::string& debuff_, Unit* caster_)
{
	const AbilityDefinition* debuffDef = AbilityDefinition::s_abilityDefinitions[debuff_];

	// Check to apply an abilityDef once for a debuff;
	bool hasDebuffAlready = CheckIfDebuffAlreadyIsApplied(debuffDef);

	if(!hasDebuffAlready)
	{
		Ability* debuff = new Ability(debuffDef);
		debuff->AssignTarget(this);
		debuff->m_castLocation = GetLocation();
		debuff->AssignCasterAndCasterOriginalLocation(caster_);

		m_activeDebuffs.push_back(debuff);
	}
}

// ------------------------------------------------------------------
void Unit::ApplyBuff(const std::string& buff_, Unit* caster_)
{
	const AbilityDefinition* buffDef = AbilityDefinition::s_abilityDefinitions[buff_];

	// Check to apply an abilityDef once for a buff;
	bool hasbuffAlready = CheckIfBuffAlreadyIsApplied(buffDef);

	if (!hasbuffAlready)
	{
		Ability* buff = new Ability(buffDef);

		switch(buff->GetTargetChoice())
		{
			case TargetChoice::SELF:
			{
				buff->AssignTarget(caster_);
				buff->m_castLocation = caster_->GetLocation();
				break;
			}

			case TargetChoice::RANDOM:
			{
				buff->AssignTarget(this);
				buff->m_castLocation = GetLocation();
				break;
			}

			default:
			{
				ERROR_AND_DIE("Unknown buff target choice.");
			}
		}

		buff->AssignCasterAndCasterOriginalLocation(this);

		m_activeBuffs.push_back(buff);
	}
}

// ------------------------------------------------------------------
void Unit::AttackChange(int amountChange_)
{
	m_strength += amountChange_;
}

// ------------------------------------------------------------------
bool Unit::CheckIfStatusAlreadyIsApplied(const AbilityDefinition* newStatus_)
{
	bool hasStatusAlready = false;
	if (m_activeStatusEffects.size() > 0)
	{
		for (Ability* status : m_activeStatusEffects)
		{
			if (status->GetAbilityDefinition()->m_name == newStatus_->m_name)
			{
				hasStatusAlready = true;
				break;
			}
		}
	}

	return hasStatusAlready;
}

// ------------------------------------------------------------------
bool Unit::CheckIfDebuffAlreadyIsApplied(const AbilityDefinition* newDebuff_)
{
	bool hasDebuffAlready = false;
	for (Ability* debuff : m_activeDebuffs)
	{
		if (debuff->GetAbilityDefinition()->m_name == newDebuff_->m_name)
		{
			hasDebuffAlready = true;
			break;
		}
	}

	return hasDebuffAlready;
}

// ------------------------------------------------------------------
bool Unit::CheckIfBuffAlreadyIsApplied(const AbilityDefinition* newBuff_)
{
	bool hasBuffAlready = false;
	for (Ability* buff : m_activeBuffs)
	{
		if (buff->GetAbilityDefinition()->m_name == newBuff_->m_name)
		{
			hasBuffAlready = true;
			break;
		}
	}

	return hasBuffAlready;
}

// ------------------------------------------------------------------
bool Unit::AllStatusEffectsCompleteForThisTurn()
{
	// Start as true, we are then checking if ANY are not done;
	bool allStatusEffectsComplete = true;

	// Go through each status effects on this unit;
	for(Ability* status : m_activeStatusEffects)
	{
		// If ANY are not finished, then set to false;
		if(!status->m_abilityFinished)
		{
			allStatusEffectsComplete = false;
		}
	}

	return allStatusEffectsComplete;
}

// ------------------------------------------------------------------
bool Unit::AllBuffEffectsCompleteForThisTurn()
{
	// Start as true, we are then checking if ANY are not done;
	bool allbuffEffectsComplete = true;

	// Go through each status effects on this unit;
	for (Ability* buff : m_activeBuffs)
	{
		// If ANY are not finished, then set to false;
		if (!buff->m_abilityFinished)
		{
			allbuffEffectsComplete = false;
		}
	}

	return allbuffEffectsComplete;
}

// ------------------------------------------------------------------
void Unit::ResetStatusEffects()
{
	for (Ability* status : m_activeStatusEffects)
	{
		status->m_abilityFinished = false;
		status->m_lifetimeTimer = 0.0f;
		status->ResetTermStateOnAllTermsInSequence();
	}
}

void Unit::ResetBuffEffects()
{
	for (Ability* buff : m_activeBuffs)
	{
		buff->m_abilityFinished = false;
		buff->m_lifetimeTimer = 0.0f;
		buff->ResetTermStateOnAllTermsInSequence();
	}
}

// ------------------------------------------------------------------
void Unit::UpdateStatusEffects(float deltaSeconds_)
{
	for (Ability* status : m_activeStatusEffects)
	{
		status->Update(deltaSeconds_);
	}
}

// ------------------------------------------------------------------
void Unit::UpdateBuffEffects(float deltaSeconds_)
{
	for(Ability* buff : m_activeBuffs)
	{
		buff->Update(deltaSeconds_);
	}
}

// ------------------------------------------------------------------
void Unit::UpdateAbilities(float deltaSeconds_)
{
	UpdateMainAbilities(deltaSeconds_);
}

// ------------------------------------------------------------------
bool Unit::UpdateBuffAbilities(float deltaSeconds_)
{
	// Buff Ability Update;
	for (Ability* buff : m_activeBuffs)
	{
		buff->Update(deltaSeconds_);
	}

	for (Ability* buff : m_activeBuffs)
	{
		if (!buff->m_abilityFinished)
		{
			return false;
		}
	}

	return true;
}

// ------------------------------------------------------------------
bool Unit::UpdateDebuffAbilities(float deltaSeconds_)
{
	// Debuff Ability Update;
	for(Ability* debuff : m_activeDebuffs)
	{
		debuff->Update(deltaSeconds_);
	}

	for(Ability* debuff : m_activeDebuffs)
	{
		if(!debuff->m_abilityFinished)
		{
			return false;
		}
	}

	return true;
}

// ------------------------------------------------------------------
void Unit::UpdateMainAbilities(float deltaSeconds_)
{
	// Main Ability Updates;
	for (Ability* ability : m_activeAbilities)
	{
		ability->Update(deltaSeconds_);
	}
}

// ------------------------------------------------------------------
void Unit::CheckForCleanupOfAbilities()
{
	// Status Effect Cleanups;
	for (auto statusIter = m_activeStatusEffects.begin(); statusIter != m_activeStatusEffects.end();)
	{
		if ((*statusIter)->m_abilityHasBeenDisspelled || m_health == 0)
		{
			delete (*statusIter);
			(*statusIter) = nullptr;
			statusIter = m_activeStatusEffects.erase(statusIter);
			ResetStatusIcons();
		}
		else
		{
			++statusIter;
		}
	}

	// Buff Cleanups;
	for (auto buffIter = m_activeBuffs.begin(); buffIter != m_activeBuffs.end();)
	{
		if ((*buffIter)->m_abilityHasBeenDisspelled || m_health == 0)
		{
			delete (*buffIter);
			(*buffIter) = nullptr;
			buffIter = m_activeBuffs.erase(buffIter);
			ResetBuffIcon();
		}
		else
		{
			++buffIter;
		}
	}

	// Main Ability Cleanups;
	for (auto abilityIter = m_activeAbilities.begin(); abilityIter != m_activeAbilities.end();)
	{
		if ((*abilityIter)->m_abilityFinished || m_health == 0)
		{
			delete (*abilityIter);
			(*abilityIter) = nullptr;
			abilityIter = m_activeAbilities.erase(abilityIter);
		}
		else
		{
			++abilityIter;
		}
	}
}

// ------------------------------------------------------------------
void Unit::UpdateStatusIcon(float deltaSeconds_)
{
	// Cycles through each status icon currently applied;
	if (m_activeStatusEffects.size() > 0)
	{
		m_statusIconEffects = (int)m_activeStatusEffects.size();
		m_displayStatusTimer += deltaSeconds_;

		if (m_displayStatusTimer >= m_displayStatusTimeLength)
		{
			m_displayStatusTimer = 0.0f;
			m_currentStatusEffectIconIndex++;

			if (m_currentStatusEffectIconIndex > m_statusIconEffects - 1)
			{
				m_currentStatusEffectIconIndex = 0;
			}
		}
	}
	else
	{
		m_currentStatusEffectIconIndex = 0;
		m_statusIconEffects = 0;
	}
}

// ------------------------------------------------------------------
void Unit::UpdateBuffIcon(float deltaSeconds_)
{
	// Cycles through each buff icon currently applied;
	if (m_activeBuffs.size() > 0)
	{
		m_buffIconEffects = (int)m_activeBuffs.size();
		m_displayBuffTimer += deltaSeconds_;

		if (m_displayBuffTimer >= m_displayBuffTimeLength)
		{
			m_displayBuffTimer = 0.0f;
			m_currentBuffEffectIconIndex++;

			if (m_currentBuffEffectIconIndex > m_buffIconEffects - 1)
			{
				m_currentBuffEffectIconIndex = 0;
			}
		}
	}
	else
	{
		m_currentBuffEffectIconIndex = 0;
		m_buffIconEffects = 0;
	}
}

// ------------------------------------------------------------------
void Unit::ResetStatusIcons()
{
	m_statusIconEffects = (int)m_activeStatusEffects.size();

	m_displayStatusTimer = 0.0f;
	m_currentStatusEffectIconIndex = 0;
}

// ------------------------------------------------------------------
void Unit::ResetBuffIcon()
{
	m_buffIconEffects = (int)m_activeBuffs.size();

	m_displayBuffTimer = 0.0f;
	m_currentBuffEffectIconIndex = 0;
}

