///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(s): 
//
//   CmusikPlayer
//
// Filename(s): 
//
//   musikPlayer.h, musikPlayer.cpp
//
// Information:
//
//   CmusikPlayer is a front end to FMOD for playing
//   files from a CmusikPlaylist, querying information
//   from it's CmusikLibrary
// 
// Example: 
//
//   A UI can use this class to control song playback easily
//
// Usage: 
//
//   Create the class and using the basic playback controls,
//   such as CmusikPlayer::Play(), CmusikPlayer::Next(), etc.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/musikConfig.h"
#include "../include/musikArrays.h"
#include "../include/musikPlayer.h"
#include "../include/musikLibrary.h"
#include "../include/musikFunctor.h"
#include "../include/musikEqualizer.h"

///////////////////////////////////////////////////

static void musikPlayerWorker( CmusikPlayer* player )
{
	TRACE0( "Player thread initialized\n" );

	ACE_Thread_Mutex m_ProtectingStreams;

	ACE_Time_Value sleep_regular, sleep_tight;
	sleep_regular.set( 0.1 );
	sleep_tight.set( 0.1 );

	while ( !player->IsShuttingDown() )
	{
		// check every half second
		if ( player->IsPlaying() )
		{
			// start a tighter loop if the crossfader 
			// is inactive and there is less than
			// two seconds remaining on the song...
			if ( !player->IsCrossfaderActive() )
			{
				if ( player->GetTimeRemain( MUSIK_TIME_MS ) <= 2000 )
				{
					bool started = false;
					while ( !started )
					{
						// when its time for the next song
						// to fade in, send the player the signal
						// and exit the tighter loop. go along our
						// mary way until the it sends the begin 
						// crossfade flag back.
						if ( player->GetTimeRemain( MUSIK_TIME_MS ) <= 10 )
						{
							player->Next();
							started = true;
						}
						else
							ACE_OS::sleep( sleep_tight );
					}
				}
			}

			// we have a crossfader, see if it should
			// be queued up... we'll again need to enter a tighter
			// loop to be more accurate, so if its around 2
			// seconds until the fade, start the tighter loop
			else if ( player->GetTimeRemain( MUSIK_TIME_MS ) <= ( player->GetCrossfader()->GetDuration( player->GetFadeType() ) * 1000 ) )
				player->Next();

			// othwerwise just monitor for the
			// crossfader flag
			if ( player->IsCrossfaderReady() )
			{
				TRACE0( "Crossfade started... " );

				player->UnflagCrossfade();

				bool fade_success = true;
				
				// if the song is shorter than twice the fade
				// duration ( fadein + fadeout ), don't do
				// the fade becuase something will fuck up
				if ( player->GetDuration( MUSIK_TIME_MS ) > ( player->GetCrossfader()->GetDuration( player->GetFadeType() ) * 1000 ) )
				{
					m_ProtectingStreams.acquire();

					int temp				= 0;
					int nFadeType			= player->GetFadeType();
					int nCurrHandle			= player->GetHandle();
					int nMainStream			= -1;
					size_t nChildCount		= 0;

					// if the fade type is pause, stop
					// or exit, we need ALL the streams as
					// children. children always fade out
					if ( ( nFadeType == MUSIK_CROSSFADER_PAUSE_RESUME && player->IsPlaying() && !player->IsPaused() ) || nFadeType == MUSIK_CROSSFADER_STOP || nFadeType == MUSIK_CROSSFADER_EXIT )
						nChildCount = player->GetStreamCount();
    
					// otherwise its a regular fade, so find the
					// main and child streams
					else
					{
						nChildCount = player->GetStreamCount() - 1;	
						nMainStream = nChildCount;
					}	

					// fade duration in seconds, not milliseconds 
					int nDuration = (int)( 1000.0f * player->GetCrossfader()->GetDuration( player->GetFadeType() ) );

					// total number of steps ( 10 fades per second )
					// with a minimum of 1 step
					float fFadeSecs = (float)nDuration / 1000.0f;
					float fFadeCount = 10.0f * fFadeSecs;
					int nFadeCount = (int) fFadeCount;

					if ( nFadeCount < 1 ) nFadeCount = 1;

					// the amount of volume to be changed
					// in the primary stream, which ranges from
					// 0 - 255. minimum of 1 per step.
					int nFadeStep = player->GetMaxVolume() / nFadeCount;

					if ( nFadeStep < 1 ) 
						nFadeStep = 1;

					// an array containing the volume steps for
					// all the secondary streams
					CIntArray aChildSteps;
					for ( size_t i = 0; i < nChildCount; i++ )
					{
						temp = player->GetVolume( player->GetChannelID( i ) ) / nFadeCount;
						if ( temp < 1 )	temp = 1;
						aChildSteps.push_back( temp );
					}

					// current volume trackers
					int nMainVol	= 0;
					int nChildVol	= 0;
					int nChildChan	= -1;

					// start the main loop
					for ( int i = 0; i < nFadeCount; i++ )
					{
						// see if we should abort. 
						if ( player->GetHandle() != nCurrHandle )
						{
							TRACE0( "Crossfade aborted\n" );
							fade_success = false;
							break;
						}

						// modify all the child streams
						for ( size_t j = 0; j < nChildCount; j++ )
						{
							nChildChan	= player->GetChannelID( j );
							nChildVol	= player->GetVolume( nChildChan );
							nChildVol   -= aChildSteps.at( j );

							if ( nChildVol < 0 ) nChildVol = 0;

							player->SetVolume( nChildChan, nChildVol );
						}

						// modify primary stream, if one exists
						if ( nMainStream > -1 )
						{
							nMainVol += nFadeStep;
							if ( nMainVol > player->GetMaxVolume() )
								nMainVol = player->GetMaxVolume();

							player->SetVolume( player->GetChannelID( nMainStream ), nMainVol );
						}

						// sleep 10 ms
						ACE_OS::sleep( sleep_tight );
					}

					m_ProtectingStreams.release();

					// if there was a successful fade, then some
					// clean up is in order...
					if ( fade_success )
					{
						if ( player->GetFadeType() == MUSIK_CROSSFADER_PAUSE_RESUME )
						{
							if ( !player->IsPaused() )
								player->FinalizePause();
							else
								player->FinalizeResume();
						}

						else if ( player->GetFadeType() == MUSIK_CROSSFADER_STOP )
							player->FinalizeStop();

						else if ( player->GetFadeType() == MUSIK_CROSSFADER_EXIT )
						{
							player->FinalizeExit();
							player->SetSafeShutdown();
							return;
						}

						else if ( player->GetFadeType() != MUSIK_CROSSFADER_NEW_SONG  )
							player->FinalizeNewSong();

						TRACE0( "Crossfade finished successfully\n" );
					}					
				}
			}			
		}

		else if ( !player->IsPlaying() && player->IsShuttingDown() )
			player->SetSafeShutdown();

		ACE_OS::sleep( sleep_regular );
	}

	TRACE0( "Player thread terminated\n" );	
}

///////////////////////////////////////////////////

void* F_CALLBACKAPI CmusikPlayer::musikEQCallback( void* originalbuffer, void *newbuffer, int length, int param )
{
	// the param value is really the address
	// to the CmusikEqualizer that is gonna
	// be used to process the sample
	if ( param )
	{	
		CmusikEqualizer* ptrEQ = (CmusikEqualizer*)param;

		// two channel ( stereo ), 16 bit sound
		ptrEQ->ProcessDSP( newbuffer, length, 2, 16 );
	}
	return newbuffer;
}

///////////////////////////////////////////////////

CmusikPlayer::CmusikPlayer( CmusikFunctor* functor, CmusikLibrary* library )
{
	m_Functor			= functor;
	m_Library			= library;

	m_Playlist			= new CmusikPlaylist();

	m_IsPlaying			= false;
	m_IsPaused			= false;
	m_ShutDown			= false;
	m_IsCrossfaderReady	= false;

	m_EQ				= NULL;
	m_Crossfader		= NULL;
	m_ActiveStreams		= NULL;	
	m_ActiveChannels	= NULL;
	m_EQ_DSP			= NULL;

	m_pThread			= NULL;

	m_Handle			= 0;

	m_MaxChannels		= -1;
	m_CurrChannel		= -1;
	m_Index				= -1;
	m_FadeType			= -1;

	m_Volume			= 128;

	m_PlayMode			= MUSIK_PLAYER_PLAYMODE_NORMAL;

	InitThread();
}

///////////////////////////////////////////////////

CmusikPlayer::~CmusikPlayer()
{
	Exit();

	// thread will trigger this back
	// once it has exited
	if ( IsPlaying() && IsCrossfaderActive() )
	{
		while ( !m_ShutDown )
			Sleep( 100 );
	}


	FSOUND_Close();
	
	CleanThread();
	CleanEQ_DSP();
	CleanEqualizer();
	CleanCrossfader();
	CleanSound();
	CleanPlaylist();
}

///////////////////////////////////////////////////

void CmusikPlayer::InitThread()
{
	if ( !m_pThread )
		m_pThread = new CmusikThread( (ACE_THR_FUNC)musikPlayerWorker, this );
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanThread()
{
	if ( m_pThread )
	{
		m_pThread->Kill();
		delete m_pThread;
		m_pThread = NULL;
	}
}

///////////////////////////////////////////////////

void CmusikPlayer::InitCrossfader()
{
	if ( m_Crossfader ) 
		delete m_Crossfader;

	m_Crossfader = new CmusikCrossfader();
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanCrossfader()
{
	if ( m_Crossfader ) 
		delete m_Crossfader;
}

///////////////////////////////////////////////////

void CmusikPlayer::InitEqualizer()
{
	CleanEqualizer();

	m_EQ = new CmusikEqualizer( m_Library );
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanEqualizer()
{
	if ( m_EQ )
	{
		delete m_EQ;
		m_EQ = NULL;
	}	
}

///////////////////////////////////////////////////

int CmusikPlayer::InitSound( int device, int driver, int rate, int channels, int mode )
{
	TRACE0( "Attempting to initialize FMOD...\n" );

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

	m_ActiveStreams = new CmusikStreamPtrArray();
	m_ActiveChannels = new CIntArray();

	m_MaxChannels = channels;

	TRACE0( "FMOD initialized successfully.\n" );
	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanSound()
{
	CleanOldStreams( true );
	StopSound();

	if ( m_ActiveStreams ) delete m_ActiveStreams;
	if ( m_ActiveChannels ) delete m_ActiveChannels;
}

///////////////////////////////////////////////////

int CmusikPlayer::StartSound( int device, int driver, int rate, int channels )
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

void CmusikPlayer::StopSound()
{
	FSOUND_Close();
}

///////////////////////////////////////////////////

void CmusikPlayer::SetPlaylist( CmusikPlaylist* playlist )
{
	CleanPlaylist();
	m_Playlist = playlist;
}

///////////////////////////////////////////////////

bool CmusikPlayer::Play( int index, int fade_type, int start_pos )
{
	// verify playlist even exists
	if ( !m_Playlist )
	{
		TRACE0( "Playlist does not exist.\n" );
		return false;
	}

	// verify song can is within the playlist's range 
	if ( ( index >= (int)m_Playlist->GetCount() || index < 0 ) && fade_type != MUSIK_CROSSFADER_SEEK )
	{
		TRACE0( "Playlist song out of range.\n" );
		return false;
	}

	// check to see if we're seeking... if we
	// are then we do not need to get the song's
	// info again. otherwise, we need to get song 
	// info about the currently playing song 
	// from it's ID
	if ( fade_type != MUSIK_CROSSFADER_SEEK )
		m_Library->GetSongInfoFromID( m_Playlist->GetSongID( index ), &m_CurrSong );

	// setup next stream
	FSOUND_STREAM* pNewStream = FSOUND_Stream_Open( 
		m_CurrSong.GetFilename().c_str(), 
		FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE, 
		0,
		0 );

	// assure the stream was loaded
	if ( !pNewStream )
	{
		TRACE0( "Song file failed to load. Invalid filename?\n" );
		return false;
	}

	// increment handle
	m_Handle++;

	// setup the current fade type
	m_FadeType = fade_type;

	// inc the next channel, 
	PushNewChannel();

	// add the new channel and stream
	m_ActiveStreams->push_back( pNewStream );
	m_ActiveChannels->push_back( GetCurrChannel() );

	// play it: set volume
	FSOUND_Stream_Play( GetCurrChannel(), pNewStream );
	FSOUND_SetVolume( GetCurrChannel(), 0 );

	// set time
	if ( start_pos )
		FSOUND_Stream_SetTime( pNewStream, start_pos );

	// toggle the flag
	m_IsPlaying = true;
	m_IsPaused = false;

	// if the current type of crossfade is either
	// disabled or at 0.0 seconds, just cut
	// out any old streams before we start up the next
	if ( !IsCrossfaderActive() || ( m_Crossfader->GetDuration( fade_type ) <= 0 && fade_type == MUSIK_CROSSFADER_NONE ) )
	{
		// set new equalizer...
		m_EQ->GetSongEq( m_Playlist->GetSongID( index ) );
		FSOUND_SetVolume( GetCurrChannel(), m_Volume );
		CleanOldStreams();
	}

	// if we got a valid crossfade type, flag it
	// so the thread can pick it up when we finish
	else
		FlagCrossfade();	

	// call the functor. this is sort of like
	// a callback, but a bit easier.
	if ( m_Functor && fade_type != MUSIK_CROSSFADER_SEEK )
		m_Functor->OnNewSong();

	// set the new index
	m_Index = index;

	// and, we're done.
	TRACE0( "Next song started, and functor called: " );
	TRACE0( m_CurrSong.GetFilename().c_str() );
	TRACE0( "\n" );
	return true;
}

///////////////////////////////////////////////////

bool CmusikPlayer::Next()
{
	if ( !m_Playlist )
		return false;

	if ( GetPlaymode() == MUSIK_PLAYER_PLAYMODE_LOOP || GetPlaymode() == MUSIK_PLAYER_PLAYMODE_NORMAL )
	{
		if ( m_Index + 1 == (int)m_Playlist->GetCount() )
		{
			if ( GetPlaymode() == MUSIK_PLAYER_PLAYMODE_LOOP )	
				m_Index = 0;
			else
				return false;
		}
	}

	if ( !IsCrossfaderActive() )
		Play( m_Index + 1 );
	else
		Play( m_Index + 1, MUSIK_CROSSFADER_NEW_SONG );

	return true;
}

///////////////////////////////////////////////////

bool CmusikPlayer::Prev()
{
	if ( !m_Playlist )
		return false;

	// 2 second grace period... if the time right now
	// is before 2000 ms (2 seconds), then start the
	// previous song...
	if ( GetTimeNow( MUSIK_TIME_MS ) < 2000 )
	{
		if ( GetPlaymode() == MUSIK_PLAYER_PLAYMODE_NORMAL )
		{
			if ( m_Index - 1 < 0 )
				m_Index = 0;
			else
				m_Index--;
		}
		else if ( GetPlaymode() == MUSIK_PLAYER_PLAYMODE_LOOP )
		{
			if ( m_Index -1 < 0 )
				m_Index = (int)m_Playlist->GetCount() - 1;				
		}
	}

	if ( !IsCrossfaderActive() )
		Play( m_Index );
	else
		Play( m_Index, MUSIK_CROSSFADER_NEW_SONG );

	return true;
}

///////////////////////////////////////////////////

void CmusikPlayer::Stop()
{
	if ( IsCrossfaderActive() )
	{
		m_Handle++;
		m_FadeType = MUSIK_CROSSFADER_STOP;
		FlagCrossfade();
	}
	else
		FinalizeStop();
}

///////////////////////////////////////////////////

void CmusikPlayer::FinalizeStop()
{
	CleanOldStreams( true );

	m_IsPlaying = false;
	m_IsPaused = false;

	if ( m_Functor )
		m_Functor->OnStop();
}

///////////////////////////////////////////////////

void CmusikPlayer::FinalizeExit()
{
	CleanOldStreams( true );

	m_IsPlaying = false;
	m_IsPaused = false;
}

///////////////////////////////////////////////////

void CmusikPlayer::Exit()
{
	if ( IsCrossfaderActive() && IsPlaying() )
	{
		m_Handle++;
		m_FadeType = MUSIK_CROSSFADER_EXIT;
		FlagCrossfade();
	}
	else
	{
		m_ShutDown = true;
		CleanOldStreams( true );
	}
}

///////////////////////////////////////////////////

void CmusikPlayer::PushNewChannel()
{
	if ( ( m_CurrChannel + 1 ) == m_MaxChannels )
		m_CurrChannel = 0;
	else
		m_CurrChannel++;
}

///////////////////////////////////////////////////

int CmusikPlayer::GetCurrChannel()
{
	return m_CurrChannel;
}

///////////////////////////////////////////////////

FSOUND_STREAM* CmusikPlayer::GetCurrStream()
{
	return m_ActiveStreams->at( m_ActiveStreams->size() - 1 );
}

///////////////////////////////////////////////////

void CmusikPlayer::InitEQ_DSP()
{
	CleanEQ_DSP();

	if ( !m_EQ_DSP )
	{
		if ( !m_EQ )
			InitEqualizer();

		void* ptrEQ = m_EQ;
		m_EQ_DSP = FSOUND_DSP_Create( &musikEQCallback, FSOUND_DSP_DEFAULTPRIORITY_USER, (int)ptrEQ );
		return;
	}

	TRACE0( "Equalizer failed to initialize becuase the DSP callback is already active.\n" );
}

///////////////////////////////////////////////////

void CmusikPlayer::EnableEQ( bool enable )
{
	if ( enable )
	{
		if ( !m_EQ_DSP )
			InitEQ_DSP();

		if ( m_EQ_DSP )
			FSOUND_DSP_SetActive( m_EQ_DSP, m_IsEQActive );
	}
	else
	{
		CleanEqualizer();
		CleanEQ_DSP();
	}
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanEQ_DSP()
{
	if ( m_EQ_DSP )
	{
		FSOUND_DSP_Free( m_EQ_DSP );
		m_EQ_DSP = NULL;
	}
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanOldStreams( bool kill_primary )
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

bool CmusikPlayer::IsCrossfaderActive()
{
	if ( m_Crossfader )
		return true;
	
	return false;
}

///////////////////////////////////////////////////

int CmusikPlayer::GetDuration( int mode )
{
	int nTime = FSOUND_Stream_GetLengthMs( GetCurrStream() );

	if ( mode == MUSIK_TIME_SECONDS )
		nTime /= 1000;

	return nTime;
}

///////////////////////////////////////////////////

int CmusikPlayer::GetTimeNow( int mode )
{
	int nCurr = FSOUND_Stream_GetTime( GetCurrStream() );

	if ( mode == MUSIK_TIME_SECONDS )
		nCurr /= 1000;

	return nCurr;
}

///////////////////////////////////////////////////

int CmusikPlayer::GetTimePer ( int percent )
{
	if ( percent == 0 )
		percent = 1;
	else if ( percent > 99 )
		percent = 100;
	
	int nDuration = GetDuration( MUSIK_TIME_MS );
	float fFinal = (float)nDuration * ( (float)percent / 100.0f );

	return (int)fFinal;
}

///////////////////////////////////////////////////

CStdString CmusikPlayer::GetTimePerStr( int percent )
{
	return GetTimeStr( GetTimePer( percent ) );
}

///////////////////////////////////////////////////

int CmusikPlayer::GetTimeNowPer()
{
	int nCurr = GetTimeNow( MUSIK_TIME_SECONDS );
	int nTotal = GetDuration( MUSIK_TIME_SECONDS );

	return ( ( nCurr * 100 ) / nTotal );
}

///////////////////////////////////////////////////

void CmusikPlayer::SetTimeNowPer( int percent )
{
	if ( percent > 99 )
		percent = 99;

	int nPos = GetDuration( MUSIK_TIME_MS );
	float fPos = ( (float)percent / 100.0f ) * nPos;

	if ( !IsCrossfaderActive() )
		FSOUND_Stream_SetTime( GetCurrStream(), (int)fPos );
	else
		Play( m_Index, MUSIK_CROSSFADER_SEEK, (int)fPos );
}

///////////////////////////////////////////////////

int CmusikPlayer::GetTimeRemain( int mode )
{
	int nLeft = GetDuration( mode ) - GetTimeNow( mode );

	if ( mode == MUSIK_TIME_SECONDS )
		nLeft /= 1000;

	return nLeft;
}

///////////////////////////////////////////////////

void CmusikPlayer::FinalizeNewSong()
{
	if ( IsEqualizerActive() && m_EQ )
		m_EQ->GetSongEq( m_Playlist->GetSongID( m_Index ) );
	
	SetVolume( m_Volume, true );
	CleanOldStreams();
}

///////////////////////////////////////////////////

size_t CmusikPlayer::GetStreamCount()
{
	return m_ActiveStreams->size();
}

///////////////////////////////////////////////////

int CmusikPlayer::GetVolume( int channel_id )
{
	return FSOUND_GetVolume( channel_id );
}

///////////////////////////////////////////////////

int CmusikPlayer::GetChannelID( int n )
{
	return m_ActiveChannels->at( n );
}

///////////////////////////////////////////////////

void CmusikPlayer::SetVolume( int channel_id, int n )
{
	FSOUND_SetVolume( channel_id, n );
}

///////////////////////////////////////////////////

void CmusikPlayer::SetMaxVolume( int n, bool set_now )
{
	m_Volume = n;

	if ( set_now )
		FSOUND_SetVolume( GetCurrChannel(), n );
}

///////////////////////////////////////////////////

void CmusikPlayer::SetCrossfader( CmusikCrossfader fader )
{
	CleanCrossfader();

	if ( !IsCrossfaderActive() )
		InitCrossfader();

	*m_Crossfader = fader;
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanPlaylist()
{
	if ( m_Playlist )
	{
		delete m_Playlist;
		m_Playlist = NULL;
	}
}

///////////////////////////////////////////////////

CStdString CmusikPlayer::GetTimeStr( int time_ms )
{
	CStdString sTime;

	int ms = time_ms;
	int hours = ms / 1000 / 60 / 60;
	ms -= hours * 1000 * 60 * 60;
	int minutes = ms / 1000 / 60;
	ms -= minutes * 1000 * 60;
	int seconds = ms / 1000;

	if ( hours > 0 )
		sTime.Format( "%d:%02d:%02d", hours, minutes, seconds );
	else
		sTime.Format( "%d:%02d", minutes, seconds );
	
	return sTime;
}

///////////////////////////////////////////////////

bool CmusikPlayer::Pause()
{
	if ( IsPlaying() && !IsPaused() )
	{
		if ( IsCrossfaderActive() && m_Crossfader->GetDuration ( MUSIK_CROSSFADER_PAUSE_RESUME ) > 0.0f )
		{
			m_FadeType = MUSIK_CROSSFADER_PAUSE_RESUME;
			FlagCrossfade();
		}
		else
			FinalizePause();
	}

	return true;
}

///////////////////////////////////////////////////

void CmusikPlayer::FinalizePause()
{
	FSOUND_SetPaused( FSOUND_ALL, TRUE );

	CleanOldStreams();
	m_IsPaused = true;
	m_IsPlaying = true;

	if ( m_Functor )
		m_Functor->OnPause();
}

///////////////////////////////////////////////////

bool CmusikPlayer::Resume()
{
	if ( IsPlaying() && IsPaused() )
	{
		CleanOldStreams();
		FSOUND_SetPaused( FSOUND_ALL, FALSE );

		if ( IsCrossfaderActive() && m_Crossfader->GetDuration ( MUSIK_CROSSFADER_PAUSE_RESUME ) > 0.0f )
		{
			m_FadeType = MUSIK_CROSSFADER_PAUSE_RESUME;
			FlagCrossfade();
		}
		else
			FinalizeResume();
	}

	return true;
}

///////////////////////////////////////////////////

void CmusikPlayer::FinalizeResume()
{
	FSOUND_SetVolume( GetCurrChannel(), m_Volume );
	CleanOldStreams();

	m_IsPaused = false;
	m_IsPlaying = true;

	if ( m_Functor )
		m_Functor->OnResume();
}

///////////////////////////////////////////////////