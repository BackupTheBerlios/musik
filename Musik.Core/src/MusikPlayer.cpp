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
		// check every half second
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
	// the param value is really the address
	// to the CMusikEqualizer that is gonna
	// be used to process the sample
	CMusikEqualizer* ptrEQ = (CMusikEqualizer*)param;

	// two channel ( stereo ), 16 bit sound
	ptrEQ->ProcessDSP( newbuffer, length, 2, 16 );
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
	m_Crossfader		= NULL;
	m_ActiveStreams		= NULL;	
	m_ActiveChannels	= NULL;
	m_Mutex				= NULL;
	m_ThreadID			= NULL;
	m_ThreadHND			= NULL;
	m_DSP				= NULL;

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
	m_Crossfader = new CMusikCrossfader();

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
	if ( m_Crossfader ) delete m_Crossfader;
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
	{
		if ( StartSound( device, driver, rate, channels ) != MUSIK_PLAYER_INIT_SUCCESS )
		{
			TRACE0( "FMOD failed to initialize.\n" );
			return StartSound( device, driver, rate, channels );
		}
	}

	m_ActiveStreams = new CMusikStreamPtrArray();
	m_ActiveChannels = new CIntArray();

	m_MaxChannels = channels;

	TRACE0( "FMOD successfully initialized.\n" );
	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void CMusikPlayer::CleanSound()
{
	CleanOldStreams( true );
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
	if ( !FSOUND_SetDriver( device ) )
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
	if ( index >= (int)m_Playlist->GetCount() || index < 0 )
	{
		TRACE0( "Playlist song out of range.\n" );
		return false;
	}

	// get song info about the currently
	// playing song from it's ID
	m_Library->GetSongInfoFromID( m_Playlist->GetSongID( index ), &m_CurrSong );

	// setup next stream
	FSOUND_STREAM* pNewStream = FSOUND_Stream_Open( 
		m_CurrSong.GetFilename().c_str(), 
		FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE, 
		0,
		0 );

	// assure the stream was loaded
	if ( pNewStream == NULL )
	{
		TRACE0( "Song file failed to load. Invalid filename?\n" );
		return false;
	}

	// inc the next channel, 
	PushNewChannel();

	// add the new channel and stream
	m_ActiveStreams->push_back( pNewStream );
	m_ActiveChannels->push_back( GetCurrChannel() );

	// play it: set volume and time
	FSOUND_Stream_Play( GetCurrChannel(), pNewStream );
	FSOUND_SetVolume( GetCurrChannel(), 128 );
	FSOUND_Stream_SetTime( pNewStream, start_pos * 1000 );

	// if the current type of crossfade is either
	// disabled or at 0.0 seconds, just cut
	// out any old streams before we start up the next
	CleanOldStreams();

	// toggle the flag
	m_IsPlaying = true;
	m_IsPaused = false;

	// call the functor. this is sort of like
	// a callback, but a bit easier.
	if ( m_Functor )
		m_Functor->Call();

	// and, we're done.
	TRACE0( "Next song started, and functor called.\n" );
	return true;
}

///////////////////////////////////////////////////

void CMusikPlayer::PushNewChannel()
{
	if ( ( m_CurrChannel + 1 ) == m_MaxChannels )
		m_CurrChannel = 0;
	else
		m_CurrChannel++;
}

///////////////////////////////////////////////////

int CMusikPlayer::GetCurrChannel()
{
	return m_CurrChannel;
}

///////////////////////////////////////////////////

FSOUND_STREAM* CMusikPlayer::GetCurrStream()
{
	return m_ActiveStreams->at( m_ActiveStreams->size() );
}

///////////////////////////////////////////////////

void CMusikPlayer::InitDSP()
{
	if ( !m_DSP )
	{
		void* ptrEQ = m_EQ;
		m_DSP = FSOUND_DSP_Create( &MusikEQCallback, FSOUND_DSP_DEFAULTPRIORITY_USER, (int)ptrEQ );		
	}
}

///////////////////////////////////////////////////

void CMusikPlayer::ToggleDSP()
{
	if ( m_DSP )
		FSOUND_DSP_SetActive( m_DSP, m_IsEQActive );
}

///////////////////////////////////////////////////

void CMusikPlayer::CleanDSP()
{
	if ( m_DSP )
	{
		FSOUND_DSP_Free( m_DSP );
		m_DSP = NULL;
	}
}

///////////////////////////////////////////////////

void CMusikPlayer::SetEqualizerActive( bool active )
{
	m_IsEQActive = active;
	if ( active )
	{
		InitDSP();
		ToggleDSP();
	}
	else
	{
		ToggleDSP();
		CleanDSP();		
	}
}

///////////////////////////////////////////////////

void CMusikPlayer::CleanOldStreams( bool kill_primary )
{
	ASSERT( m_ActiveStreams->size() == m_ActiveChannels->size() );

	size_t nStreamCount = m_ActiveStreams->size();

	if ( nStreamCount <= 0 )
		return;
	else if ( !kill_primary )
		nStreamCount--;

	for ( size_t i = 0; i < nStreamCount; i++ )
	{
		FSOUND_Stream_Stop	( m_ActiveStreams->at( 0 ) );
		FSOUND_Stream_Close	( m_ActiveStreams->at( 0 ) );
		
		m_ActiveStreams->erase( m_ActiveStreams->begin() );
		m_ActiveChannels->erase( m_ActiveChannels->begin() );
	}	
}

///////////////////////////////////////////////////

bool CMusikPlayer::IsCrossfaderActive()
{
	if ( m_Crossfader )
		return true;
	
	return false;
}

///////////////////////////////////////////////////

int CMusikPlayer::GetDuration( int mode )
{
	int nTime = FSOUND_Stream_GetLengthMs( GetCurrStream() );

	if ( mode == MUSIK_TIME_SECONDS )
		nTime /= 1000;

	return nTime;
}

///////////////////////////////////////////////////

int CMusikPlayer::GetTimeNow( int mode )
{
	int nCurr = FSOUND_Stream_GetTime( GetCurrStream() );

	if ( mode == MUSIK_TIME_SECONDS )
		nCurr /= 1000;

	return nCurr;
}

///////////////////////////////////////////////////

int CMusikPlayer::GetTimeRemain( int mode )
{
	int nLeft = GetDuration( mode ) - GetTimeNow( mode );

	if ( mode == MUSIK_TIME_SECONDS )
		nLeft /= 1000;

	return nLeft;
}


///////////////////////////////////////////////////

