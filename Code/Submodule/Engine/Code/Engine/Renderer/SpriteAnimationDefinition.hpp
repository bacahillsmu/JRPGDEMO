#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

enum SpriteAnimationPlaybackType
{
	SPRITE_ANIMATION_PLAYBACK_UNKNOWN = -1,

	SPRITE_ANIMATION_PLAYBACK_ONCE,
	SPRITE_ANIMATION_PLAYBACK_LOOP,
	SPRITE_ANIMATION_PLAYBACK_PINGPONG,

	NUM_SPRITE_ANIMATION_PLAYBACK
};


class SpriteAnimationDefinition
{

public:

	SpriteAnimationDefinition(SpriteSheet& spriteSheet, int startIndex, int endIndex, float durationSeconds = 1.0f, SpriteAnimationPlaybackType playbackType = SPRITE_ANIMATION_PLAYBACK_PINGPONG);
	~SpriteAnimationDefinition(){}

	const SpriteDefinition& GetSpriteDefinitionAtTime(float seconds) const;
	int GetAnimationFrameAtTime(float seconds);
	static SpriteAnimationPlaybackType StringToSpriteAnimationPlaybackType(const std::string& playbackType_);

private:

	const SpriteSheet& m_spriteSheet;
	int m_startIndex = -1;
	int m_endIndex = -1;
	float m_durationSeconds = 1.0f;
	SpriteAnimationPlaybackType m_spriteAnimationPlaybackType = SPRITE_ANIMATION_PLAYBACK_ONCE;
	


};