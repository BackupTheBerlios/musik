/*
 *  MusikTagThreads.cpp
 *
 *  Threads related to MusikFrame and/or global threads,
 *	such as the crossfader.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikThreads.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- MusikStreamArray here ---//
#include "../Classes/MusikPlayer.h"

//--- MusikFrame here ---//
#include "../Frames/MusikFrame.h"

//---------------------------------------------------------//
//---- always running thread. it figures out when and 	---//
//---- how to que up the next song. also triggers fades	---//
//---------------------------------------------------------//
MusikFaderThread::MusikFaderThread()
{
	m_CrossfaderActive = false;
	m_CrossfadersActive = 0;
}

void MusikFaderThread::SetCrossfaderActive( bool active )
{
	m_CrossfaderActive = active;
}

void MusikFaderThread::CrossfaderAbort()
{
	if ( IsCrossfaderActive() )
	{
		//---------------------------------------------------------//
		//--- Abort() tells fader NOT to clean up old streams	---//
		//---------------------------------------------------------//
		pCrossfader->Abort();		
		pCrossfader->Delete();	
	}
}

void MusikFaderThread::CrossfaderStop()
{
	if ( IsCrossfaderActive() )
		pCrossfader->Delete();
}

void *MusikFaderThread::Entry()
{
	wxCommandEvent NextSongEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_NEXT_SONG );	

	while ( !TestDestroy() )
	{
		if ( g_Player.IsPlaying() )		
		{
			//-------------------------------------------------//
			//--- check the player flag to see if we need	---//
			//--- to trigger the crossfade begin thread		---//
			//-------------------------------------------------//
			if ( g_Player.BeginFade() )	
			{
				//----------------------------------------------//
				//--- let the player know we got the message ---//
				//----------------------------------------------//
 				g_Player.CaughtBeginFade();

				//--------------------------------//
				//--- fire up a new crossfader ---//
				//--------------------------------//
				pCrossfader = new MusikCrossfaderThread( this );
				pCrossfader->Create();
				pCrossfader->Run();

				SetCrossfaderActive();
			}

			if ( ( !g_Player.IsStartingNext() ) && ( !g_Player.IsFading() ) && ( g_Player.GetTimeLeft( FMOD_MSEC ) <= ( g_Prefs.nFadeDuration + 1000 ) ) )
			{
				//-------------------------------------------------//
				//--- if currently we are not fading, but the	---//
				//--- crossfader is enabled, check to see if	---//
				//--- the duration is such that we should		---//
				//--- queue up the next song and start the fade	---//
				//-------------------------------------------------//
				if ( ( g_Prefs.nFadeEnable == 1 ) && ( g_Player.GetTimeLeft( FMOD_MSEC ) <= g_Prefs.nFadeDuration ) )
				{
					g_Player.SetStartingNext( true );
					wxPostEvent( &g_Player, NextSongEvt );
					Yield();
				}

				//-------------------------------------------------//
				//--- if there is no fading at all going on,	---//
				//--- and there is 10 mseconds or less playback	---//
				//--- time left, go ahead and start up the next	---//
				//--- track										---//
				//-------------------------------------------------//
				else if ( ( g_Prefs.nFadeEnable == 0 ) && ( g_Player.GetTimeLeft( FMOD_MSEC ) <= 10 ) )
				{
					g_Player.SetStartingNext( true );
					wxPostEvent( &g_Player, NextSongEvt );
					Yield();
				}
			}
		}	
		Sleep( 10 );
	}

	return NULL;
}

void MusikFaderThread::OnExit()
{
	if ( IsCrossfaderActive() )
		pCrossfader->Delete();
}

//---------------------------------------------------------//
//---- this will fade in the new channel on top of the	---//
//---- the array, while fading *all* the others out		---//
//---------------------------------------------------------//
MusikCrossfaderThread::MusikCrossfaderThread( MusikFaderThread *pParent )
{
	m_Parent		= pParent;
	m_StopPlayer	= false;
	m_Aborted		= false;
}

void MusikCrossfaderThread::Abort()
{
	m_Aborted = true;
}

void MusikCrossfaderThread::SetStopPlayer()
{
	m_StopPlayer = true;
}

void *MusikCrossfaderThread::Entry()
{
	//---------------------------------------------//
	//--- if the fade duration * 2				---//
	//--- (fade in + fade out) is shorter		---//
	//--- than that stream duration, no fading	---//
	//---------------------------------------------//
	if ( g_Player.GetDuration( FMOD_MSEC ) < ( g_Prefs.nFadeDuration * 2 ) )
	{
		FSOUND_SetVolume( g_ActiveChannels.Item( g_ActiveChannels.GetCount() - 1 ), g_Prefs.nSndVolume );
		return NULL;
	}
	
	//---------------------------------------------//
	//--- how many channels to fade out			---//
	//---------------------------------------------//
	m_FadeType = g_Player.GetCrossfadeType();
	int nFadeInStreamID	= -1;
	size_t nFadeOutStreams = 0;
	
	if ( m_FadeType == CROSSFADE_PAUSE || m_FadeType == CROSSFADE_STOP || m_FadeType == CROSSFADE_EXIT )	//--- if pause or stop or exit, fade all out	---//
		nFadeOutStreams = g_ActiveStreams.GetCount();
	else if ( m_FadeType == CROSSFADE_RESUME )																//--- if resume, fade none out, 0 in			---//
	{
		nFadeOutStreams = 0;
		nFadeInStreamID	= 0;
	}
	else
	{
		nFadeOutStreams = g_ActiveStreams.GetCount() - 1;													//--- else, fade ( all - 1 ) out, 	last in		---//
		nFadeInStreamID = g_ActiveStreams.GetCount() - 1;
	}

	int nFadeDuration = 0; 
	if ( m_FadeType == CROSSFADE_NORMAL )
		nFadeDuration = g_Prefs.nFadeDuration;
	else if ( m_FadeType == CROSSFADE_SEEK )
		nFadeDuration = g_Prefs.nFadeSeekDuration;
	else if ( m_FadeType == CROSSFADE_PAUSE || m_FadeType == CROSSFADE_RESUME )
		nFadeDuration = g_Prefs.nFadePauseResumeDuration;
	else if ( m_FadeType == CROSSFADE_STOP )
		nFadeDuration = g_Prefs.nFadeStopDuration;
	else if ( m_FadeType == CROSSFADE_EXIT )
		nFadeDuration = g_Prefs.nFadeExitDuration;

	float fFadeSecs 	= (float)nFadeDuration / 1000.0f;				//--- number of seconds								---//
	
	float fFadeCount;
	int nSleepTime;
	if ( fFadeSecs < 1.0f )
	{
		fFadeCount = 20.0f * fFadeSecs;									//--- 20 fades per sec if fade less than 1 sec		---//
		nSleepTime = 50;
	}
	else
	{
		fFadeCount 	= 10.0f * fFadeSecs;								//--- 10 fades per sec if fade greater than 1 sec	---//
		nSleepTime = 100;
	}
	
	int nFadeCount = (int)fFadeCount;									//--- total number of fades							---//
	
	if ( nFadeCount < 1 )
		nFadeCount = 1;

	int nFadeStep = g_Prefs.nSndVolume / nFadeCount;					//--- volume steps									----//
	
	//-----------------------------------------//
	//--- array of steps for the secondary	---//
	//--- audio streams.					---//
	//-----------------------------------------//
	wxArrayInt aSecSteps;
	int nGetVol;
	for ( size_t i = 0; i < nFadeOutStreams; i++ )
	{
		nGetVol = FSOUND_GetVolume( g_ActiveChannels.Item( i ) );
		aSecSteps.Add( nGetVol / nFadeCount );
	}

	//---------------------------------------------//
	//--- generate an array of fade out steps	---//
	//---------------------------------------------//
	int nCurrVol = 0;
	int nLastVol = 0;

	//---------------------------------//
	//--- step through the fades	---//
	//---------------------------------//
    for ( int i = 0; i < nFadeCount; i++ )
	{
		//--- check abort signal ---//
		if ( TestDestroy() )
			return NULL;

		//--- fade down ---//
		for ( size_t j = 0; j < nFadeOutStreams; j++ )
		{
			nLastVol = FSOUND_GetVolume( g_ActiveChannels.Item( j ) ) - aSecSteps.Item( j );
			FSOUND_SetVolume( g_ActiveChannels.Item( j ), nLastVol );
		}

		//--- fade up ---//
		if ( nFadeInStreamID > -1 )
		{
			nCurrVol += nFadeStep;
			if ( nCurrVol > g_Prefs.nSndVolume )
				nCurrVol = g_Prefs.nSndVolume;
	
			FSOUND_SetVolume( g_ActiveChannels.Item( nFadeInStreamID ), nCurrVol );
		}

		Sleep( nSleepTime );
	}
	return NULL;
}

void MusikCrossfaderThread::OnExit()
{
	//-------------------------------------------------//
	//--- if we ended naturally, that means no		---//
	//--- other crossfader spawned. tell the player	---//
	//--- no other fading is going on, so it can	---//
	//--- manage and clean up any orphan streams	---//
	//-------------------------------------------------//
	m_Parent->SetCrossfaderActive( false );

	if ( !m_Aborted )
	{
		wxCommandEvent FadeCompleteEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_FADE_COMPLETE );	
		wxPostEvent( &g_Player, FadeCompleteEvt );
		Yield();
	}

	if ( m_FadeType == CROSSFADE_STOP || m_FadeType == CROSSFADE_EXIT )
	{
		wxCommandEvent StopPlayerEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_STOP );	
		wxPostEvent( &g_Player, StopPlayerEvt );
		Yield();

		//-----------------------------------------------------//
		//--- and if its an exit, clean up. this probably	---//
		//--- needs work.									---//
		//-----------------------------------------------------//
		if ( m_FadeType == CROSSFADE_EXIT )
		{
			m_Parent->Delete();
			Yield();

			wxCommandEvent ExitPlayerEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_EXIT_FADE_DONE );
			wxPostEvent( g_MusikFrame, ExitPlayerEvt );
			Yield();	
		}

		return;
	}

	else if ( m_FadeType == CROSSFADE_PAUSE )
	{
		wxCommandEvent PausePlayerEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_PAUSE );	
		wxPostEvent( &g_Player, PausePlayerEvt );
		Yield();

		return;
	}

	else if ( m_FadeType == CROSSFADE_RESUME )
	{
		wxCommandEvent ResumePlayerEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_RESUME );	
		wxPostEvent( &g_Player, ResumePlayerEvt );
		Yield();

		return;
	}
}

//---------------------------------------------------------//
//--- this thread will write all dirty tags to file...	---//
//--- its placed here for lack of a better place		---//
//---------------------------------------------------------//
MusikWriteDirtyThread::MusikWriteDirtyThread( bool bClear )
{
	m_Clear = bClear;
}

void *MusikWriteDirtyThread::Entry()
{
	//--- setup ---//
	g_MusikFrame->SetProgress		( 0 );
	g_MusikFrame->SetProgressType	( MUSIK_WRITE_DIRTY_THREAD );
	g_MusikFrame->SetActiveThread	( this );

	//--- events we'll post as we go along ---//
	wxCommandEvent WriteTagStartEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_START );
	wxCommandEvent WriteTagProgEvt	( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_PROG );	

	wxPostEvent( g_MusikFrame, WriteTagStartEvt );

	CMusikSongArray aDirty = g_Library.QuerySongs( wxT("dirty = 1") );

	if ( aDirty.GetCount() > 0 )
	{
		int nLastProg = 0;
		int nCurrProg = 0;
		float fPos = 0;
		size_t nCount = aDirty.GetCount();

		for ( size_t i = 0; i < nCount; i++ )
		{
			//--- check abort signal ---//
			if ( TestDestroy() )
				break;

			//--- update progressbar ---//
			fPos = ( i * 100 ) / nCount;
			nCurrProg = (int)fPos;
			if ( nCurrProg > nLastProg )
			{
				g_MusikFrame->SetProgress( nCurrProg );
				wxPostEvent( g_MusikFrame, WriteTagProgEvt );
			}
			nLastProg = nCurrProg;

			//-----------------------------//
			//--- write the tag to file	---//
			//-----------------------------//
			if ( aDirty.Item( i ).Format == MUSIK_FORMAT_MP3 )
				g_Library.WriteMP3Tag( aDirty.Item( i ).Filename, m_Clear );
			else if ( aDirty.Item( i ).Format == MUSIK_FORMAT_OGG )
				g_Library.WriteOGGTag( aDirty.Item( i ).Filename, m_Clear );

			//-----------------------------//
			//--- flag item as clean	---//
			//-----------------------------//
			g_Library.UpdateItem( aDirty.Item( i ).Filename, aDirty.Item( i ), false );
		}

	}

	return NULL;
}

void MusikWriteDirtyThread::OnExit()
{
	wxCommandEvent WriteDirtyEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_END );	
	wxPostEvent( g_MusikFrame, WriteDirtyEndEvt );
}
