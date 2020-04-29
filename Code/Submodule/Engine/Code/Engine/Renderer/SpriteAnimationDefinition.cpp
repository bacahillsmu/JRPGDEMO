
// Defines and Includes ---------------------------------------------------------------------------
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"

SpriteAnimationDefinition::SpriteAnimationDefinition( SpriteSheet& spriteSheet, int startIndex, int endIndex, float durationSeconds /*= 1.0f*/, SpriteAnimationPlaybackType playbackType /*= SPRITE_ANIMATION_PLAYBACK_PINGPONG*/ )
	: m_spriteSheet(spriteSheet)
	, m_startIndex(startIndex)
	, m_endIndex(endIndex)
	, m_durationSeconds(durationSeconds)
	, m_spriteAnimationPlaybackType(playbackType)
{
}

const SpriteDefinition& SpriteAnimationDefinition::GetSpriteDefinitionAtTime( float seconds ) const
{
	int numBaseFrames = (m_endIndex - m_startIndex + 1);

	float timeIntoCycle = fmodf(seconds, m_durationSeconds);
	float timePerFrame = (m_durationSeconds / numBaseFrames);

	int animationFrameNumber = (int)floorf(timeIntoCycle / timePerFrame);

	if(m_spriteAnimationPlaybackType == SPRITE_ANIMATION_PLAYBACK_ONCE)
	{
		if(seconds < m_durationSeconds)
		{
			return m_spriteSheet.GetSpriteDefinition(animationFrameNumber + m_startIndex);
		}
		else
		{
			return m_spriteSheet.GetSpriteDefinition(m_endIndex);
		}
		
	}
	else if(m_spriteAnimationPlaybackType == SPRITE_ANIMATION_PLAYBACK_LOOP)
	{
		return m_spriteSheet.GetSpriteDefinition(animationFrameNumber + m_startIndex);
	}
	else if(m_spriteAnimationPlaybackType == SPRITE_ANIMATION_PLAYBACK_PINGPONG)
	{
		int numFramesInCycle = (m_endIndex - m_startIndex) * 2;
		timePerFrame = (m_durationSeconds / numFramesInCycle);
		animationFrameNumber = (int)floorf(timeIntoCycle / timePerFrame);

		if(animationFrameNumber < numBaseFrames)
		{
			return m_spriteSheet.GetSpriteDefinition(animationFrameNumber + m_startIndex);
		}
		else
		{
			int reverseIndexFromEnd = (numFramesInCycle) - animationFrameNumber;

			return m_spriteSheet.GetSpriteDefinition(reverseIndexFromEnd + m_startIndex);
		}

		
	}

	return m_spriteSheet.GetSpriteDefinition(m_startIndex);
}

int SpriteAnimationDefinition::GetAnimationFrameAtTime( float seconds )
{
	int numBaseFrames = (m_endIndex - m_startIndex + 1);

	float timeIntoCycle = fmodf(seconds, m_durationSeconds);
	float timePerFrame = (m_durationSeconds / numBaseFrames);

	int animationFrameNumber = (int)floorf(timeIntoCycle / timePerFrame);

	if(m_spriteAnimationPlaybackType == SPRITE_ANIMATION_PLAYBACK_ONCE)
	{
		if(seconds < m_durationSeconds)
		{
			return animationFrameNumber;
		}
		else
		{
			return 0;
		}

	}
	else if(m_spriteAnimationPlaybackType == SPRITE_ANIMATION_PLAYBACK_LOOP)
	{
		return animationFrameNumber;
	}
	else if(m_spriteAnimationPlaybackType == SPRITE_ANIMATION_PLAYBACK_PINGPONG)
	{
		int numFramesInCycle = (m_endIndex - m_startIndex) * 2;
		timePerFrame = (m_durationSeconds / numFramesInCycle);
		animationFrameNumber = (int)floorf(timeIntoCycle / timePerFrame);

		if(animationFrameNumber < numBaseFrames)
		{
			return animationFrameNumber;
		}
		else
		{
			int reverseIndexFromEnd = (numFramesInCycle) - animationFrameNumber;

			return reverseIndexFromEnd;
		}


	}

	return 0;
}

SpriteAnimationPlaybackType SpriteAnimationDefinition::StringToSpriteAnimationPlaybackType(const std::string& playbackType_)
{
	if (playbackType_ == "once")
	{
		return SPRITE_ANIMATION_PLAYBACK_ONCE;
	}
	else if (playbackType_ == "loop")
	{
		return SPRITE_ANIMATION_PLAYBACK_LOOP;
	}
	else if (playbackType_ == "pingpong")
	{
		return SPRITE_ANIMATION_PLAYBACK_PINGPONG;
	}

	return SPRITE_ANIMATION_PLAYBACK_UNKNOWN;
}

