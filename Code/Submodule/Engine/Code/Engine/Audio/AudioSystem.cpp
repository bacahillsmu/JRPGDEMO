#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Framework/GameCommon.hpp"
#include "Game/Framework/App.hpp"
#include "Engine/Renderer/Camera.hpp"
//#include "Game/Gameplay/Camera/OrbitCamera.hpp"


//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
// SD1 NOTE: THIS MEANS *EVERY* GAME MUST HAVE AN EngineBuildPreferences.hpp FILE IN ITS CODE/GAME FOLDER!!
#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif

AudioSystem* g_theAudioSystem = nullptr;

//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
	ValidateResult( result );

	CreateOrGetChannelGroup("SFX");
	CreateOrGetChannelGroup("BGM");	
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	ChannelGroupID sfxChannelGroupID = CreateOrGetChannelGroup("SFX");
	FMOD::ChannelGroup* sfx = m_registeredChannelGroups[ sfxChannelGroupID ];
	FMOD_RESULT result = sfx->release();
	ValidateResult( result );

	ChannelGroupID bgmChannelGroupID = CreateOrGetChannelGroup("BGM");
	FMOD::ChannelGroup* bgm = m_registeredChannelGroups[ bgmChannelGroupID ];
	result = bgm->release();
	ValidateResult( result );

	result = m_fmodSystem->release();
	ValidateResult( result );
}

// void AudioSystem::Set3DListener(OrbitCamera* camera)
// {
// 	Vec3 forward = camera->GetForward();
// 	Vec3 up = camera->GetUp();
// 	
// 	m_fmodSystem->set3DListenerAttributes(0, 
// 		(FMOD_VECTOR*)&camera->m_focalPoint,	// position
// 		(FMOD_VECTOR*)0,						// velocity
// 		(FMOD_VECTOR*)&forward,					// forward
// 		(FMOD_VECTOR*)&up);						// up
// }

//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::Startup()
{

}

//-----------------------------------------------------------------------------------------------
void AudioSystem::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
ChannelGroupID AudioSystem::CreateOrGetChannelGroup( const std::string& channelGroupName )
{
	std::map< std::string, ChannelGroupID >::iterator found = m_registeredChannelGroupIDs.find( channelGroupName );
	if( found != m_registeredChannelGroupIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::ChannelGroup* newChannelGroup = nullptr;
		m_fmodSystem->createChannelGroup(channelGroupName.c_str(), &newChannelGroup);
		if( newChannelGroup )
		{
			FMOD::ChannelGroup* master = nullptr;
			m_fmodSystem->getMasterChannelGroup(&master);
			master->addGroup(newChannelGroup);
			
			ChannelGroupID newChannelGroupID = m_registeredChannelGroupIDs.size();
			m_registeredChannelGroupIDs[ channelGroupName ] = newChannelGroupID;
			m_registeredChannelGroups.push_back( newChannelGroup );
			return newChannelGroupID;
		}
	}

	return MISSING_CHANNEL_ID;
}

//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_DEFAULT|FMOD_3D, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, ChannelGroupID channelGroupID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
	{
		return MISSING_SOUND_ID;
	}

	size_t numChannelGroups = m_registeredChannelGroups.size();
	if(channelGroupID < 0 || channelGroupID >= numChannelGroups)
	{
		return MISSING_CHANNEL_ID;
	}

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
	{
		return MISSING_SOUND_ID;
	}

	FMOD::ChannelGroup* channelGroup = m_registeredChannelGroups[ channelGroupID ];
	if( !channelGroup )
	{
		return MISSING_CHANNEL_ID;
	}

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, channelGroup, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}

//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::Play3DSound( SoundID soundID, ChannelGroupID channelGroupID, Vec3 position, bool isLooped /*= false*/, float speed /*= 1.0f*/, bool isPaused /*= true */ )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
	{
		return MISSING_SOUND_ID;
	}

	size_t numChannelGroups = m_registeredChannelGroups.size();
	if(channelGroupID < 0 || channelGroupID >= numChannelGroups)
	{
		return MISSING_CHANNEL_ID;
	}

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
	{
		return MISSING_SOUND_ID;
	}

	FMOD::ChannelGroup* channelGroup = m_registeredChannelGroups[ channelGroupID ];
	if( !channelGroup )
	{
		return MISSING_CHANNEL_ID;
	}

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, channelGroup, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		
		// now, given a channel, we can set the position of the audio
		// vec3 pos; // used for positional audio - set to world position of sound
		channelAssignedToSound->set3DAttributes( (FMOD_VECTOR*)&position, (FMOD_VECTOR*)0 );
		channelAssignedToSound->setMode( FMOD_3D_INVERSEROLLOFF );
		channelAssignedToSound->set3DMinMaxDistance( 2.0f, 100000.0f );

		//channelAssignedToSound->setVolume( 1.0f );
		//channelAssignedToSound->setPan( 0.0f ); // balance
		channelAssignedToSound->setLoopCount( loopCount );
		channelAssignedToSound->setPaused(false);
	}
	

	return (SoundPlaybackID) channelAssignedToSound;


}

//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}


#endif // !defined( ENGINE_DISABLE_AUDIO )
