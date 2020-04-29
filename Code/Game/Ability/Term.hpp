#pragma once

#include "Engine/Renderer/SpriteAnimationDefinition.hpp"

#include "Game/Units/UnitDefinition.hpp"

class Ability;
class SpriteAnimationDefinition;

enum class TermState
{
	INVALID = -1,

	SETUP_COMPLETE,
	START,
	DO,
	END,
	FINISHED,

	TERM_STATES_COUNT
};

enum class TermType
{
	INVALID = -1,

	ANIM,
	MOVEMENT,
	EFFECT,
	AUDIO,
	DAMAGE,
	DEBUFF,
	BUFF,
	STATUS,
	ATTACKCHANGE,
	DISSPELL,

	TERM_TYPES_COUNT
};

enum class TermMovementType
{
	INVALID = -1,

	TARGET_X,
	TARGET_Y,
	ENEMY_POS,
	ORIGINAL_POS,
	OFFSCREEN_TOP,

	TERM_MOVEMENT_TYPES_COUNT
};


// -----------------------------------------------------------------------
// Base Class, Pure Virtual;
// -----------------------------------------------------------------------
class Term
{

public:

	virtual ~Term() {}

	void Run(float deltaSeconds_);
	virtual void Start(float deltaSeconds_) = 0;
	virtual void Tick(float deltaSeconds_) = 0;
	virtual void End() = 0;

	virtual Term* Clone() = 0;

	void SetSetupComplete();
	void SetStateToStart();

	// Helpers;
	float GetAtTime();
	float GetDuration();
	void SetAtTime(float atTime_);
	void AddToAtTime(float additionalTime_);
	void SetDuration(float duration_);
	void SetAbilityCurrentlySequencing(Ability* ability_);
	bool IsFinished();

	static TermType StringToTermType(const std::string& type_);
	static TermMovementType StringToMovementType(const std::string& type_);

protected:

	Ability* m_abilityCurrentlySequencing = nullptr;

	TermState m_termState = TermState::INVALID;

	float m_runningTimer = 0.0f;
	float m_atTime = 0.0f;
	float m_duration = 0.0f;
};

// -----------------------------------------------------------------------
// Anim;
// -----------------------------------------------------------------------
class AnimTerm : public Term
{

public:

	AnimTerm() = delete;
	explicit AnimTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetAnimationState(const std::string& animationName_);


private:

	std::string m_animationName = "";
	// AnimationState m_animationState = INVALID;

};

// -----------------------------------------------------------------------
// Movement;
// -----------------------------------------------------------------------
class MovementTerm : public Term
{

public:

	MovementTerm() = delete;
	explicit MovementTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetMovementType(TermMovementType movementType_);
	void SetOriginalPosition();
	void SetCurrentPosition();
	void SetTargetPosition();


private:

	TermMovementType m_movementType = TermMovementType::INVALID;
	

	// Literal values of the movement types;
	Vec2 m_currentPosition = Vec2(0.0f, 0.0f);
	Vec2 m_originalPosition = Vec2(0.0f, 0.0f);
	Vec2 m_targetPosition = Vec2(0.0f, 0.0f);

};

// -----------------------------------------------------------------------
// Effect;
// -----------------------------------------------------------------------
class EffectTerm : public Term
{

public:

	EffectTerm() = delete;
	explicit EffectTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetTexture(std::string texture_);
	void SetDimensions(Vec2 dimensions_);
	void SetSpriteAnimationDefinition(SpriteSheet* spriteSheet_, int startIndex_, int endIndex_, float duration_, std::string playback_);

private:

	std::string m_texture = "";
	Vec2 m_dimensions = Vec2(0.0f, 0.0f);
	SpriteAnimationDefinition* m_spriteAnimationDefinition = nullptr;
	float m_animationTimer = 0.0f;
};

// -----------------------------------------------------------------------
// Audio;
// -----------------------------------------------------------------------
class AudioTerm : public Term
{

public:

	AudioTerm() = delete;
	explicit AudioTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetAudio(std::string audio_);
	
private:

	std::string m_audio = "";
	
};

// -----------------------------------------------------------------------
// Damage;
// -----------------------------------------------------------------------
class DamageTerm : public Term
{

public:

	DamageTerm() = delete;
	explicit DamageTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetDamagePercent(float damagePercent_);
	void SetDamageModifier(int damageModifier_);

private:

	float m_damagePercent = 0.0f;
	int m_damageModifier = 1;
};

// -----------------------------------------------------------------------
// Debuff;
// -----------------------------------------------------------------------
class DebuffTerm : public Term
{

public:

	DebuffTerm() = delete;
	explicit DebuffTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetDebuffAbilityName(std::string debuffAbilityName_);
	void SetPercentChance(float percentChance_);
	bool RollForDebuff();

private:

	std::string m_debuffAbilityName = "";
	float m_percentChance_ = 0.0f;

};

// -----------------------------------------------------------------------
// Buff;
// -----------------------------------------------------------------------
class BuffTerm : public Term
{

public:

	BuffTerm() = delete;
	explicit BuffTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetBuffAbilityName(std::string buffAbilityName_);
	void SetPercentChance(float percentChance_);
	bool RollForBuff();

private:

	std::string m_buffAbilityName = "";
	float m_percentChance_ = 0.0f;

};

// -----------------------------------------------------------------------
// Status;
// -----------------------------------------------------------------------
class StatusTerm : public Term
{

public:

	StatusTerm() = delete;
	explicit StatusTerm(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetStatusAbilityName(std::string statusAbilityName_);
	void SetPercentChance(float percentChance_);
	bool RollForStatus();

private:

	std::string m_statusAbilityName = "";
	float m_percentChance_ = 0.0f;

};

// -----------------------------------------------------------------------
// AttackChange;
// -----------------------------------------------------------------------
class AttackChange : public Term
{

public:

	AttackChange() = delete;
	explicit AttackChange(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;

	void SetAmountChange(int amountChange_);

private:

	int m_amountChange_ = 0;

};

// -----------------------------------------------------------------------
// Disspell;
// -----------------------------------------------------------------------
class Disspell : public Term
{

public:

	Disspell() = delete;
	explicit Disspell(XmlElement* termElement_);

	virtual void Start(float deltaSeconds_) override;
	virtual void Tick(float deltaSeconds_) override;
	virtual void End() override;

	virtual Term* Clone() override;


private:


};
