///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
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

#include <math.h>

#include "../include/musikConfig.h"
#include "../include/musikArrays.h"
#include "../include/musikPlayer.h"
#include "../include/musikLibrary.h"
#include "../include/musikFunctor.h"
#include "../include/musikEqualizer.h"

///////////////////////////////////////////////////

#ifdef WIN32
#pragma warning (disable : 4312)		// conversion of int to * of greater size
#endif

///////////////////////////////////////////////////

#define round(x) floor( x + 0.5f )

///////////////////////////////////////////////////

static void musikPlayerWorker( CmusikThread* thread )
{
	TRACE0( "Player worker thread function started...\n" );

	CmusikPlayer* player = (CmusikPlayer*)thread->GetArgs();

	ACE_Time_Value sleep_regular, sleep_tight;
	sleep_regular.set( 0.1 );
	sleep_tight.set( 0.1 );

	bool m_Exit = false;
	int nTimeRemain;
	int nTimeElapsed;

	while ( !thread->m_Abort && !m_Exit )
	{
		// suspended? go to sleep until the flag
		// tells is its ok to resume...
		if ( thread->IsSuspended() )
		{
			thread->m_Asleep = true;

			while ( thread->IsSuspended() )
				ACE_OS::sleep( sleep_regular );

			thread->m_Asleep = false;
		}

		// check every half second if we're plaing
		// and not aborting. 
		if ( player->IsPlaying() && !thread->m_Abort && !m_Exit && !thread->IsSuspended() )
		{
			// start a tighter loop if the crossfader 
			// is inactive and there is less than
			// two seconds remaining on the song...
			if ( !player->IsCrossfaderEnabled() )
			{
				// intro play mode means we only play 
				// the first 10 seconds of every song...
				if ( player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_INTRO )
				{
					nTimeElapsed = player->GetTimeNow( MUSIK_TIME_MS );

					if ( nTimeElapsed >= 10000 )
						player->Next( true );
				}

				// not intro mode, so check to see if its
				// time to que up the next song...
				else
				{
					nTimeRemain = player->GetTimeRemain( MUSIK_TIME_MS );

					if ( nTimeRemain <= 2000 )
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
								player->Next( true );
								started = true;
							}
							else
								ACE_OS::sleep( sleep_tight );
						}
					}
				}
			}

			// we have a crossfader, and its ready
			// to begin. send the next song message...
			else
			{
				// intro play mode means we only play 
				// the first 10 seconds of every song...
				if ( player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_INTRO )
				{
					nTimeElapsed = player->GetTimeNow( MUSIK_TIME_MS );

					if ( nTimeElapsed >= 10000 )
						player->Next( true );
				}

				// see if its time to crossfade in the
				// next song
				else
				{
					nTimeRemain = player->GetTimeRemain( MUSIK_TIME_MS );

					if ( nTimeRemain <= ( player->GetCrossfader()->GetDuration( player->GetFadeType() ) * 1000 ) )
						player->Next( true );
				}
			}

			// othwerwise just monitor for the
			// crossfader flag
			if ( player->IsCrossfaderReady() )
			{
				TRACE0( "Crossfade started...\n" );

				player->UnflagCrossfade();

				bool fade_success = true;
				
				// if the song is shorter than twice the fade
				// duration ( fadein + fadeout ), don't do
				// the fade becuase something will fuck up
				if ( player->GetDuration( MUSIK_TIME_MS ) > ( player->GetCrossfader()->GetDuration( player->GetFadeType() ) * 1000 ) )
				{
					// fade duration in seconds, not milliseconds 
					int nDuration = (int)( 1000.0f * player->GetCrossfader()->GetDuration( player->GetFadeType() ) );

					// total number of steps ( 10 fades per second )
					// with a minimum of 1 step
					float fFadeSecs = (float)nDuration / 1000.0f;
					float fFadeCount = 10.0f * fFadeSecs;
					int nFadeCount = (int) fFadeCount;

					if ( nFadeCount < 1 ) nFadeCount = 1;

					// variables
					float ftemp = 0.0f;
					int ntemp = 0;

					int nFadeType = player->GetFadeType();
					int nCurrHandle	= player->GetHandle();
					int nMainStream	= -1;

					size_t nChildCount		= 0;
					CIntArray aChildSteps;

					int nFadeStep = 0;

					int nMainVol = 0;
					int nChildVol = 0;
					int nChildChan = -1;

					bool recalc_steps = true;
					bool eq_updated = false;

					// start the main loop
					for ( int i = 0; i < nFadeCount; i++ )
					{
						// if the fade type is pause, stop
						// or exit, we need ALL the streams as
						// children. children always fade out
						if ( ( nFadeType == MUSIK_CROSSFADER_PAUSE_RESUME && player->IsPlaying() && !player->IsPaused() ) || nFadeType == MUSIK_CROSSFADER_STOP || nFadeType == MUSIK_CROSSFADER_EXIT )
						{	
							if ( player->GetStreamCount() != nChildCount )
							{
								nChildCount = player->GetStreamCount();
								recalc_steps = true;
							}
						}

						// otherwise its a regular fade, so find the
						// main and child streams
						else
						{
							if ( nChildCount != player->GetStreamCount() - 1 )
							{
								nChildCount = player->GetStreamCount() - 1;	
								recalc_steps = true;
							}

							if ( nMainStream != (int)nChildCount )
							{
								nMainStream = nChildCount;
								recalc_steps = true;
							}
						}	

						// we will switch the equalizer half way
						// through the crossfade, assuming there are
						// child streams. if there are not child streams
						// just start the new EQ right away
						if ( !eq_updated )
						{
							if ( ( nChildCount > 0 && ( i == ( nFadeCount / 2 ) ) ) || ( nChildCount == 0 && i == 0 ) )
							{		
								if ( nFadeType == MUSIK_CROSSFADER_NEW_SONG || nFadeType == MUSIK_CROSSFADER_STOP )
								{
									if ( player->IsEqualizerActive() )
										player->UpdateEqualizer();
								}

								eq_updated = true;
							}
						}

						if ( recalc_steps )
						{
							// an array containing the volume steps for
							// all the secondary streams
							aChildSteps.clear();
							for ( size_t i = 0; i < nChildCount; i++ )
							{
								ftemp = (float)player->GetVolume( player->GetChannelID( i ) ) / fFadeCount;
								ntemp = (int)round( ftemp );

								if ( ntemp < 1 )	
									ntemp = 1;

								aChildSteps.push_back( ntemp );
							}

							// the amount of volume to be changed
							// in the primary stream, which ranges from
							// 0 - 255. minimum of 1 per step.
							nFadeStep = player->GetMaxVolume() / nFadeCount;

							if ( nFadeStep < 1 ) 
								nFadeStep = 1;

							recalc_steps = false;
						}

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

							// set the kill switch
							m_Exit = true;
							continue;
						}

						else if ( player->GetFadeType() == MUSIK_CROSSFADER_NEW_SONG  )
							player->FinalizeNewSong();

						TRACE0( "Crossfade finished successfully\n" );
					}					
				}
			}			
		}

		ACE_OS::sleep( sleep_regular );
	}

	TRACE0( "Player thread worker function complete...\n" );	
	thread->m_Finished = true;
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
	m_IsCrossfaderReady	= false;
	m_CrossfaderEnabled	= false;
	m_IsEQActive		= false;

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

	m_Playmode			= 0L;

	m_State				= MUSIK_PLAYER_INIT_UNINITIALIZED;

	InitThread();
}

///////////////////////////////////////////////////

CmusikPlayer::~CmusikPlayer()
{
	// flag shutdown crossfade
	Exit();

	// wait until the thread is complete
	if ( m_pThread )
	{
		if ( !IsPlaying() || !IsCrossfaderEnabled() )
			m_pThread->Abort();

		while ( !m_pThread->m_Finished )
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
	{
		m_pThread = new CmusikThread();
		m_pThread->Start( (ACE_THR_FUNC)musikPlayerWorker, (void*)this, true, MUSIK_THREAD_TYPE_PLAYER_WORKER );
	}
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanThread()
{
	if ( m_pThread )
	{
		// assure thread has abort flag, although
		// it is probably already finished
		if ( !m_pThread->m_Abort )
			m_pThread->Abort();

		// don't delete anything until it's done
		while ( !m_pThread->m_Finished )
			Sleep( 100 );

		// finish up
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
	{
		delete m_Crossfader;
		m_Crossfader = NULL;
	}
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
		GetEqualizer()->SaveCurr();

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
		m_State = StartSound( device, driver, rate, channels );

		if ( m_State != MUSIK_PLAYER_INIT_SUCCESS )
		{
			TRACE0( "FMOD failed to initialize.\n" );
			return StartSound( device, driver, rate, channels );
		}
	}

	m_ProtectingStreams.acquire();
		m_ActiveStreams = new CmusikStreamPtrArray();
		m_ActiveChannels = new CIntArray();
	m_ProtectingStreams.release();

	m_MaxChannels = channels;

	TRACE0( "FMOD initialized successfully.\n" );
	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void CmusikPlayer::CleanSound()
{
	CleanOldStreams( true );
	StopSound();

	m_ProtectingStreams.acquire();

		if ( m_ActiveStreams ) 
		{
			delete m_ActiveStreams;
			m_ActiveStreams = NULL;
		}

		if ( m_ActiveChannels ) 
		{
			delete m_ActiveChannels;
			m_ActiveChannels = NULL;
		}

	m_ProtectingStreams.release();

	m_State = MUSIK_PLAYER_INIT_UNINITIALIZED;
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
	m_State = MUSIK_PLAYER_INIT_UNINITIALIZED;
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
		TRACE0( "Playlist song out of range or finished.\n" );
		Stop();
		return false;
	}

	// check to see if we're seeking... if we
	// are then we do not need to get the song's
	// info again. otherwise, we need to get song 
	// info about the currently playing song 
	// from it's ID
	if ( fade_type != MUSIK_CROSSFADER_SEEK )
		m_Library->GetSongInfoFromID( m_Playlist->GetSongID( index ), &m_CurrSong );

	m_Functor->OnNewSong();

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

		m_IsPlaying = false;
		m_IsPaused = false;

		return false;
	}

	// increment handle
	m_Handle++;

	// setup the current fade type
	m_FadeType = fade_type;

	// inc the next channel, 
	int curr_chan = PushNewChannel();

	// add the new channel and stream
	m_ProtectingStreams.acquire();
		m_ActiveStreams->push_back( pNewStream );
		m_ActiveChannels->push_back( curr_chan );
	m_ProtectingStreams.release();

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
	if ( !IsCrossfaderEnabled() || ( m_Crossfader->GetDuration( fade_type ) <= 0 && fade_type == MUSIK_CROSSFADER_NONE ) )
	{
		// set new equalizer and post to the
		// functor... else do it when the
		// crossfade completes
		if ( IsEqualizerActive() )
			m_EQ->SetNewSong( m_Playlist->GetSongID( index ) );

		FSOUND_SetVolume( GetCurrChannel(), m_Volume );
		CleanOldStreams();

		m_Functor->OnNewSong();
	}

	// if we got a valid crossfade type, flag it
	// so the thread can pick it up when we finish
	else
		FlagCrossfade();	

	
	// once we get here the new song is "officially"
	// playing, so lock it up and set the new index
	{
		m_ProtectingStreams.acquire();

			// and, we're done.
			TRACE0( "New song started...\n" );

			// set the new index
			m_Index = index;

			// increment times played
			m_Library->IncLastPlayed( m_Playlist->GetSongID( m_Index ) );
		
		m_ProtectingStreams.release();
	}

	return true;
}

///////////////////////////////////////////////////

void CmusikPlayer::EnquePaused( int index )
{
	// should only get here if we're paused
	if ( !IsPaused() || !IsPlaying() )
		return;

	// if we're missing a playlist, or the next
	// song is out of range, we don't want anything 
	// to do with it
	if ( index == -1 )
		index = m_Index;

	if ( !m_Playlist || index > (int)m_Playlist->GetCount() - 1 || index < -1 )
		return;

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
		return;
	}

	// clean anything left over
	CleanOldStreams( true );

	// increment handle
	m_Handle++;

	// inc the next channel, 
	PushNewChannel();

	// add the new channel and stream
	{
		m_ProtectingStreams.acquire();

			m_ActiveStreams->push_back( pNewStream );
			m_ActiveChannels->push_back( GetCurrChannel() );

		m_ProtectingStreams.release();
	}

	// setup playback, then pause
	FSOUND_Stream_Play( GetCurrChannel(), pNewStream );
	FSOUND_SetVolume( GetCurrChannel(), 0 );
	FSOUND_SetPaused( FSOUND_ALL, true );

	// song has officially been resumed...
	{
		m_ProtectingStreams.acquire();

			m_Index = index;

			if ( m_Functor )
				m_Functor->OnNewSong();

		m_ProtectingStreams.release();
	}
}

///////////////////////////////////////////////////

bool CmusikPlayer::Next( bool expired )
{
	if ( !m_Playlist )
		return false;

	// if repeat single is enabled, and the
	// stream naturally expired (a "next" button
	// was not clicked, for example), we want to
	// start the song over...
	if ( expired && m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE )
	{
		// m_Index = m_index;
	}

	// if we are repeating a playlist, check to see
	// if the next song is beyond the last song. if
	// it is, start it over from the beginning
	else if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
	{
		if ( m_Index + 1 >= (int)m_Playlist->GetCount() )
			m_Index = 0;
		else
			m_Index++;
	}

	// else nothing special, we're just playing straight
	// through the playlist...
	else
	{
		if ( m_Index + 1 > (int)m_Playlist->GetCount() )
			return false;
		else
			m_Index++;
	}

	if ( IsPlaying() && !IsPaused() )
		Play( m_Index, MUSIK_CROSSFADER_NEW_SONG );
	else
		EnquePaused( -1 );
		
	return true;
}

///////////////////////////////////////////////////

bool CmusikPlayer::Prev()
{
	if ( !m_Playlist )
		return false;

	// if the song is under 2000 ms, we want to go
	// to the previous track
	if ( GetTimeNow( MUSIK_TIME_MS ) < 2000 || !IsPlaying() )
	{
		m_Index--;

		if ( m_Index < 0 )
		{
			if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
				m_Index = (int)m_Playlist->GetCount() - 1;	
			else
				m_Index = 0;
		}
	}
	//  else
	//      m_Index = m_Index;

	// song time elapsted is greater than 2000 ms
	// we want to start the song over
	// m_Index == m_Index
	if ( IsPlaying() && !IsPaused() )
		Play( m_Index, MUSIK_CROSSFADER_NEW_SONG );
	else
		EnquePaused( -1 );

	return true;
}

///////////////////////////////////////////////////

void CmusikPlayer::Stop()
{
	if ( IsCrossfaderEnabled() )
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
	if ( IsCrossfaderEnabled() && IsPlaying() )
	{
		m_FadeType = MUSIK_CROSSFADER_EXIT;
		FlagCrossfade();
		m_Handle++;
	}
	else
		FinalizeExit();
}

///////////////////////////////////////////////////

int CmusikPlayer::PushNewChannel()
{
	if ( ( m_CurrChannel + 1 ) == m_MaxChannels )
		m_CurrChannel = 0;
	else
		m_CurrChannel++;

	return m_CurrChannel;
}

///////////////////////////////////////////////////

int CmusikPlayer::GetCurrChannel()
{
	return m_CurrChannel;
}

///////////////////////////////////////////////////

FSOUND_STREAM* CmusikPlayer::GetCurrStream()
{
	FSOUND_STREAM* pStream = NULL;

	m_ProtectingStreams.acquire();

		if ( m_ActiveStreams->size() )
			pStream = m_ActiveStreams->at( m_ActiveStreams->size() - 1 );
	
	m_ProtectingStreams.release();
	
	return pStream;
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

void CmusikPlayer::EnableEqualizer( bool enable )
{
	m_IsEQActive = enable;

	if ( enable )
	{
		if ( !m_EQ_DSP )
			InitEQ_DSP();

		if ( m_EQ_DSP )
			FSOUND_DSP_SetActive( m_EQ_DSP, true );
	}
	else
	{
		CleanEQ_DSP();
		CleanEqualizer();
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
	m_ProtectingStreams.acquire();

		ASSERT( m_ActiveStreams->size() == m_ActiveChannels->size() );

		if ( !m_ActiveStreams->size() )
			return;

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

	m_ProtectingStreams.release();
}

///////////////////////////////////////////////////

bool CmusikPlayer::IsCrossfaderEnabled()
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
	if ( !IsPlaying() )
		return -1;

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

CmusikString CmusikPlayer::GetTimePerStr( int percent )
{
	return GetTimeStr( GetTimePer( percent ) );
}

///////////////////////////////////////////////////

int CmusikPlayer::GetTimeNowPer()
{
	int nCurr = GetTimeNow( MUSIK_TIME_SECONDS );
	int nTotal = GetDuration( MUSIK_TIME_SECONDS );

	if ( nTotal == 0 )
		return 0;

	return ( ( nCurr * 100 ) / nTotal );
}

///////////////////////////////////////////////////

void CmusikPlayer::SetTimeNowPer( int percent )
{
	if ( percent > 99 )
		percent = 99;

	int nPos = GetDuration( MUSIK_TIME_MS );
	float fPos = ( (float)percent / 100.0f ) * nPos;

	if ( !IsCrossfaderEnabled() )
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
	SetVolume( m_Volume, true );
	CleanOldStreams();
}

///////////////////////////////////////////////////

size_t CmusikPlayer::GetStreamCount()
{
	size_t count = 0;

	m_ProtectingStreams.acquire();
		count = m_ActiveStreams->size();
	m_ProtectingStreams.release();

	return count;
}

///////////////////////////////////////////////////

int CmusikPlayer::GetVolume( int channel_id )
{
	return FSOUND_GetVolume( channel_id );
}

///////////////////////////////////////////////////

int CmusikPlayer::GetChannelID( int n )
{
	int channel_id = 0;

	m_ProtectingStreams.acquire();
		if ( n < (int)m_ActiveChannels->size() )
			channel_id = m_ActiveChannels->at( n );
	m_ProtectingStreams.release();

	return channel_id;
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

void CmusikPlayer::EnableCrossfader( bool enable )
{
	m_CrossfaderEnabled = enable;

	if ( enable )
	{
		CleanCrossfader();
		InitCrossfader(); 
		return;
	}
	else
		CleanCrossfader();
}

///////////////////////////////////////////////////

bool CmusikPlayer::SetCrossfader( const CmusikCrossfader& fader )
{
	if ( m_CrossfaderEnabled )
	{
		if ( !m_Crossfader )
			InitCrossfader();

		*m_Crossfader = fader;

		return true;
	}

	return false;
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

CmusikString CmusikPlayer::GetTimeStr( int time_ms )
{
	CmusikString sTime;

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
		if ( IsCrossfaderEnabled() && m_Crossfader->GetDuration ( MUSIK_CROSSFADER_PAUSE_RESUME ) > 0.0f )
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
	FSOUND_SetPaused( FSOUND_ALL, true );

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
		FSOUND_SetPaused( FSOUND_ALL, false );

		if ( IsCrossfaderEnabled() && m_Crossfader->GetDuration ( MUSIK_CROSSFADER_PAUSE_RESUME ) > 0.0f )
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

void CmusikPlayer::ModifyPlaymode( unsigned long add, unsigned long remove, bool remove_first )
{
	if ( remove_first )
	{
		if ( remove )
			m_Playmode &= ~remove;

		if ( add )
			m_Playmode |= add;

		return;	
	}

	if ( add )
		m_Playmode |= add;

	if ( remove )
		m_Playmode &= ~remove;
}

///////////////////////////////////////////////////

void CmusikPlayer::UpdateEqualizer()
{
	if ( IsEqualizerActive() && IsEqualizerEnabled() && m_Playlist && m_Index > -1 )
	{	
		if ( m_Index < (int)m_Playlist->GetCount() )
			GetEqualizer()->SetNewSong( m_Playlist->GetSongID( m_Index ) );
		else
			GetEqualizer()->SetNewSong( -1 );

		m_Functor->OnNewEqualizer();
	}
}

///////////////////////////////////////////////////

bool CmusikPlayer::FindNewIndex( int songid )
{	
	bool found = false;
	m_ProtectingStreams.acquire();
		for ( size_t i = 0; i < m_Playlist->GetCount(); i++ )
		{
			if ( m_Playlist->GetSongID( i ) == songid )
			{	
				m_Index = i;
				found = true;
				break;
			}
		}
	m_ProtectingStreams.release();

	return found;
}

///////////////////////////////////////////////////
