///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/MusikArrays.h"
#include "../include/MusikPlayer.h"
#include "../include/MusikLibrary.h"
#include "../include/MusikFunctor.h"
#include "../include/MusikEqualizer.h"

///////////////////////////////////////////////////

static void MusikPlayerWorker( CMusikPlayer* player )
{
	TRACE0( "Player thread initialized\n" );

	ACE_Time_Value sleep;
	sleep.set( 0.5 );

	while ( !player->IsShuttingDown() )
	{
		if ( player->IsPlaying() && !player->IsPaused() )
		{

		}

		ACE_OS::sleep( sleep );
	}

	TRACE0( "Player thread terminated\n" );	
}

///////////////////////////////////////////////////

void* F_CALLBACKAPI CMusikPlayer::MusikEQCallback( void* originalbuffer, void *newbuffer, int length, int param )
{
	// two channel ( stereo ), 16 bit sound
	m_EQ->ProcessDSP( newbuffer, length, 2, 16 );
	return newbuffer;
}

///////////////////////////////////////////////////

CMusikPlayer::CMusikPlayer( CMusikFunctor* functor, CMusikLibrary* library, CMusikPlaylist* playlist )
{
	m_Functor			= functor;
	m_Library			= library;
	m_Playlist			= playlist;

	m_IsPlaying			= false;
	m_IsPaused			= false;
	m_ShutDown			= false;
	m_EQ				= NULL;
	m_ActiveStreams		= NULL;	
	m_ActiveChannels	= NULL;
	m_Mutex				= NULL;
	m_ThreadID			= NULL;
	m_ThreadHND			= NULL;
	m_MaxChannels		= -1;
	m_CurrChannel		= -1;

	InitThread();
}

///////////////////////////////////////////////////

CMusikPlayer::~CMusikPlayer()
{
	CleanThread();
	CleanSound();
}

///////////////////////////////////////////////////

void CMusikPlayer::InitThread()
{
	m_EQ = new CMusikEqualizer( m_Library );
	m_Mutex	= new ACE_Thread_Mutex();
	m_ThreadID = new ACE_thread_t();
	m_ThreadHND = new ACE_hthread_t();

	ACE_Thread::spawn( (ACE_THR_FUNC)MusikPlayerWorker,
		this,
		THR_JOINABLE | THR_NEW_LWP,
		m_ThreadID,
		m_ThreadHND );

	ACE_Thread::join( m_ThreadHND );
}

///////////////////////////////////////////////////

void CMusikPlayer::CleanThread()
{
	if ( m_EQ ) delete m_EQ;
	if ( m_Mutex ) 	delete m_Mutex;
	if ( m_ThreadID ) delete m_ThreadID;	
	if ( m_ThreadHND ) delete m_ThreadHND;
}

///////////////////////////////////////////////////

int CMusikPlayer::InitSound( int device, int driver, int rate, int channels, int mode )
{
	if ( mode == MUSIK_PLAYER_INIT_RESTART || mode == MUSIK_PLAYER_INIT_STOP )
		StopSound();

	if ( mode == MUSIK_PLAYER_INIT_START || mode == MUSIK_PLAYER_INIT_RESTART )
		return StartSound( device, driver, rate, channels );

	m_ActiveStreams = new CMusikStreamPtrArray();
	m_ActiveChannels = new CIntArray();

	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void CMusikPlayer::CleanSound()
{
	StopSound();

	if ( m_ActiveStreams ) delete m_ActiveStreams;
	if ( m_ActiveChannels ) delete m_ActiveChannels;
}

///////////////////////////////////////////////////

int CMusikPlayer::StartSound( int device, int driver, int rate, int channels )
{
	int nOSDriverID = -1;

	#ifdef WIN32
		switch( driver )
		{
		case 0:
			nOSDriverID = FSOUND_OUTPUT_DSOUND;
			break;
		case 1:
			nOSDriverID = FSOUND_OUTPUT_WINMM;
			break;
		case 2:
			nOSDriverID = FSOUND_OUTPUT_ASIO;
			break;
		}
	#else
		switch( driver )
		{
		case 0:
			nOSDriverID = FSOUND_OUTPUT_DSOUND;
			break;
		case 1:
			nOSDriverID = FSOUND_OUTPUT_WINMM;
			break;
		case 2:
			nOSDriverID = FSOUND_OUTPUT_ASIO;
			break;
		}
	#endif

	if ( nOSDriverID == -1 )
		return MUSIK_PLAYER_INIT_ERROR;

	// initialize the driver that will be used
	// for the device.
	//
	// windows supports the following:
	//   - direct sound
	//   - windows waveform
	//   - asio
	//
	// linux supports the following:
	//   - open sound system (OSS)
	//   - enlightenment sound daemon (ESD)
	//   - advanced linux sound architecture (ALSA)
	if( !FSOUND_SetOutput( nOSDriverID ) )
		return MUSIK_PLAYER_INIT_ERROR;

	// initialize the device itself.
	if ( !FSOUND_SetDriver( driver ) )
		return MUSIK_PLAYER_INIT_ERROR;

	// now initialize the whole system
	FSOUND_SetBufferSize( 100 );
	if ( !FSOUND_Init( rate, channels, 0 ) )
		return MUSIK_PLAYER_INIT_ERROR;

	// if we get here we're home free
	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void CMusikPlayer::StopSound()
{
	FSOUND_Close();
}

///////////////////////////////////////////////////

void CMusikPlayer::SetPlaylist( CMusikPlaylist* playlist )
{
	m_Playlist = playlist;
}

///////////////////////////////////////////////////

bool CMusikPlayer::Play( int index, int play_type, int start_pos )
{
	// verify song can even 
	if ( index >= (int)m_Playlist->size() || index < 0 )
	{
		TRACE0( "Playlist song out of range." );
		return false;
	}

	// get song info about the currently
	// playing song from it's ID
	m_Library->GetSongInfoFromID( m_Playlist->items()->at( index ).GetID(), &m_CurrSong );

	// setup next channel

	// call the functor. this is sort of like
	// a callback, but a bit easier.
	if ( m_Functor )
		m_Functor->Call();

	// and, we're done.
	TRACE0( "Next song started, and functor called." );
	return true;
}

///////////////////////////////////////////////////

void CMusikPlayer::PushNewChannel()
{
	if ( ( m_CurrChannel + 1 ) == m_MaxChannels || m_CurrChannel == -1 )
		m_CurrChannel = 0;
	else
		m_CurrChannel++;
}