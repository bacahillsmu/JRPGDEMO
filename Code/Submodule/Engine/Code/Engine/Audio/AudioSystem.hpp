#pragma once
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
typedef size_t ChannelGroupID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs
constexpr size_t MISSING_CHANNEL_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------
class AudioSystem;
class OrbitCamera;


/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

public:
	virtual void				BeginFrame();
	virtual void				EndFrame();
	virtual void				Startup();
	virtual void				Shutdown();

	// Orbit Camera lives in Game Code in RTS. Needs to be moved to Engine Code
	//virtual void				Set3DListener(OrbitCamera* camera);

	virtual ChannelGroupID		CreateOrGetChannelGroup(const std::string& channelGroupName);

	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath );
	virtual SoundPlaybackID		PlaySound( SoundID soundID, ChannelGroupID channelGroupID, bool isLooped = false, float volume = 1.0f, float balance = 0.0f, float speed = 1.0f, bool isPaused = false );
	virtual SoundPlaybackID		Play3DSound( SoundID soundID, ChannelGroupID channelGroupID, Vec3 position, bool isLooped = false, float speed = 1.0f, bool isPaused = true );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)

	virtual void				ValidateResult( FMOD_RESULT result );

protected:

	FMOD::System*						m_fmodSystem;

	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;

	std::map< std::string, ChannelGroupID >	m_registeredChannelGroupIDs;
	std::vector< FMOD::ChannelGroup* >		m_registeredChannelGroups;
};

extern AudioSystem* g_theAudioSystem;