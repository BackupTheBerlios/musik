///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/MusikPlayer.h"
#include "../include/MusikLibrary.h"
#include "../include/MusikFunctor.h"

///////////////////////////////////////////////////

CMusikPlayer::CMusikPlayer( CMusikFunctor* functor )
{
	m_IsPlaying = false;
	m_IsPaused	= false;
	m_Functor	= functor;

	m_ActiveStreams = NULL;
	m_ActiveChannels = NULL;
	m_Mutex = NULL;
	m_ThreadID = NULL;
	m_ThreadHND = NULL;

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
	m_Mutex	= new ACE_Thread_Mutex();
	m_ThreadID = new ACE_thread_t();
	m_ThreadHND = new ACE_hthread_t();

	ACE_Thread::spawn( (ACE_THR_FUNC)CMusikPlayer::PlayerThread,
		NULL,
		THR_JOINABLE,
		m_ThreadID,
		m_ThreadHND );
}

///////////////////////////////////////////////////

void CMusikPlayer::PlayerThread()
{
	MessageBox( NULL, "AHAHAHA", NULL, NULL );
}

///////////////////////////////////////////////////

void CMusikPlayer::CleanThread()
{
	if ( m_Mutex ) delete m_Mutex;
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

bool CMusikPlayer::Play( int index )
{

	m_Functor->Call();

	return true;
}

///////////////////////////////////////////////////