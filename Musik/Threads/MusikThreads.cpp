/*
 *  MusikTagThreads.cpp
 *
 *  Threads related to MusikFrame and/or global threads,
 *	such as the crossfader.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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

//--- fmod ---//
#include "fmod.h"
//---------------------------------------------------------//
//---- always running thread. it figures out when and 	---//
//---- how to que up the next song. also triggers fades	---//
//---------------------------------------------------------//
MusikFaderThread::MusikFaderThread()
 :wxThread(wxTHREAD_JOINABLE)
{
	pCrossfader = NULL;
	m_Worker = 0;
}


void MusikFaderThread::CrossfaderAbort()
{
	{
	wxCriticalSectionLocker locker( m_critCrossfader) ;
	if ( pCrossfader )
	{
		//---------------------------------------------------------//
		//--- Abort() tells fader NOT to clean up old streams	---//
		//---------------------------------------------------------//
		pCrossfader->Abort();		
	}
}
	CrossfaderStop();
}

void MusikFaderThread::CrossfaderStop()
{
	wxCriticalSectionLocker locker( m_critCrossfader);
	if ( pCrossfader )
	{
		pCrossfader->Delete();
		pCrossfader->Wait();
		delete pCrossfader;
		pCrossfader = NULL;
	}
}

void MusikFaderThread::StartNew()
{
	CrossfaderStop();
	//--------------------------------//
	//--- fire up a new crossfader ---//
	//--------------------------------//
	wxASSERT(pCrossfader == NULL);
	pCrossfader = new MusikCrossfaderThread( this );
	pCrossfader->Create();
	pCrossfader->Run();
}

void *MusikFaderThread::Entry()
{
	wxCommandEvent NextSongEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_NEXT_SONG );	

	while ( !TestDestroy() )
	{
		if ( g_Player.IsPlaying() )		
		{
			//-----------------------------------------------------//
			//--- check the player flag to see if we need		---//
			//--- to trigger the crossfade begin thread			---//
			//-----------------------------------------------------//
			if ( g_Player.BeginFade() )	
			{
				//-------------------------------------------------//
				//--- kill the existing crossfade thread (if	---//
				//--- it exists still)							---//
				//-------------------------------------------------//
				if ( IsCrossfaderActive() )
				{
					CrossfaderAbort();
					CrossfaderStop();
				}
				else
				{
					//-------------------------------------------------//
					//--- let the player know we got the message	---//
					//-------------------------------------------------//
 					g_Player.CaughtBeginFade();

					//-------------------------------------------------//
					//--- if no old crossfader is active, fire up a	---//
					//--- new one. if there was one active, once it	---//
					//--- cleans up, an event will be posted to		---//
					//--- start a new one.							---//
					//-------------------------------------------------//
					StartNew();
				}
			}

			if ( ( !g_Player.IsStartingNext() ) && ( !g_Player.IsFading() ) && ( g_Player.GetTimeLeft( FMOD_MSEC ) <= ( g_Prefs.nFadeDuration + 1000 ) ) )
			{
				//-------------------------------------------------//
				//--- if there is no fading at all going on,	---//
				//--- and there is 10 mseconds or less playback	---//
				//--- time left, go ahead and start up the next	---//
				//--- track										---//
				//-------------------------------------------------//
				if ( ( g_Prefs.nGlobalFadeEnable == 0 ) || ( ( g_Prefs.nFadeEnable == 0 ) && ( g_Player.GetTimeLeft( FMOD_MSEC ) <= 10 ) ) )
				{
					g_Player.SetStartingNext( true );
					wxPostEvent( &g_Player, NextSongEvt );
					Yield();
				}
				//-------------------------------------------------//
				//--- if currently we are not fading, but the	---//
				//--- crossfader is enabled, check to see if	---//
				//--- the duration is such that we should		---//
				//--- queue up the next song and start the fade	---//
				//-------------------------------------------------//
				else if ( ( g_Prefs.nFadeEnable == 1 ) && ( g_Player.GetTimeLeft( FMOD_MSEC ) <= g_Prefs.nFadeDuration ) )
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
	CrossfaderStop();
}

//---------------------------------------------------------//
//---- this will fade in the new channel on top of the	---//
//---- the array, while fading *all* the others out		---//
//---------------------------------------------------------//
MusikCrossfaderThread::MusikCrossfaderThread( MusikFaderThread *pParent )
:wxThread(wxTHREAD_JOINABLE)
{
	m_FadeType		= g_Player.GetCrossfadeType();
	m_Parent		= pParent;
	m_StopPlayer	= false;
	m_Aborted		= false;
	m_Parent->WorkerInc();
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
	
	wxCriticalSectionLocker locker( g_protectingStreamArrays );

	if(g_ActiveStreams.GetCount() == 0)
		return NULL;
	//-------------------------------------------------//
	//--- if the fade duration * 2 (fade in and		---//
	//--- fade out) is shorter than the stream 		---//
	//--- duration, no fading. return.				---//
	//-------------------------------------------------//
	if ( g_Player.GetDuration( FMOD_MSEC ) < ( g_Prefs.nFadeDuration * 2 ) )
	{
        FSOUND_SetVolume( g_ActiveChannels.Item( g_ActiveChannels.GetCount() - 1 ), g_Prefs.nSndVolume );
		return NULL;
	}
	
	//-------------------------------------------------//
	//--- how many and which channels will be faded	---//
	//--- in and/or out.							---//
	//-------------------------------------------------//
	int nFadeInStreamID		= -1;
	size_t nFadeOutStreams	= 0;
	
	//--- pause / stop / exit: all streams out		---//
	if ( m_FadeType == CROSSFADE_PAUSE || m_FadeType == CROSSFADE_STOP || m_FadeType == CROSSFADE_EXIT )
		nFadeOutStreams = g_ActiveStreams.GetCount();

	//--- resume, only primary stream in.			---//
	else if ( m_FadeType == CROSSFADE_RESUME )																
	{
		nFadeOutStreams = 0;
		nFadeInStreamID	= 0;
	}

	//--- regular fade, secondary out, primary in	---//
	else
	{
		nFadeOutStreams = g_ActiveStreams.GetCount() - 1;	
		nFadeInStreamID = g_ActiveStreams.GetCount() - 1;
	}

	//-------------------------------------------------//
	//--- the duration of time the fade will be 	---//
	//--- active for. this depends on the type.		---//
	//-------------------------------------------------//
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

	//-------------------------------------------------//
    //--- the total number of seconds the fade will	---//
	//--- be active for.							---//
	//-------------------------------------------------//
	float fFadeSecs = (float)nFadeDuration / 1000.0f;

	//-------------------------------------------------//
	//--- the total number of fades. there will be	---//
	//--- 10 fades every second, smoooth.			---//
	//-------------------------------------------------//
	float fFadeCount = 10.0f * fFadeSecs;
	int nFadeCount = (int) fFadeCount;
	if ( nFadeCount < 1 )
		nFadeCount = 1;

	//-------------------------------------------------//
	//--- the volume step for the primary stream.	---//
	//--- this is the channel that will fade IN		---//
	//-------------------------------------------------//
	int nFadeStep = g_Prefs.nSndVolume / nFadeCount;
	if ( nFadeStep < 1 )
		nFadeStep = 1;
	
	//-------------------------------------------------//
	//--- array of steps for the secondary streams.	---//
	//--- these channels will be faded OUT			---//
	//-------------------------------------------------//
	int nGetVol;
	wxArrayInt aSecSteps;
	for ( size_t i = 0; i < nFadeOutStreams; i++ )
	{
		nGetVol = FSOUND_GetVolume( g_ActiveChannels.Item( i ) ) / nFadeCount;
		if ( nGetVol < 1 )
			nGetVol = 1;

		aSecSteps.Add( nGetVol );
	}

	//-------------------------------------------------//
	//--- generate an array of fade out steps		---//
	//-------------------------------------------------//
	int nCurrPriVol = 0;
	int nLastSecVol	= 0;
	int nCurrSecChan = -1;

	//-------------------------------------------------//
	//--- the loop that fades.						---//
	//-------------------------------------------------//
    for ( int i = 0; i < nFadeCount; i++ )
	{
		if ( TestDestroy() || m_Aborted )
			return NULL;

		//-----------------------------------------//
		//--- fade all the secondy streams out.	---//
		//-----------------------------------------//
		for ( size_t j = 0; j < nFadeOutStreams; j++ )
		{
			nCurrSecChan	= g_ActiveChannels.Item( j );
			nLastSecVol		= FSOUND_GetVolume( nCurrSecChan ) - aSecSteps.Item( j );
			if ( nLastSecVol < 0 )
				nLastSecVol = 0;

			FSOUND_SetVolume( nCurrSecChan, nLastSecVol );
		}

		//-----------------------------------------//
		//--- fade the primary stream in.		---//
		//-----------------------------------------//
		if ( nFadeInStreamID > -1 )
		{
			nCurrPriVol += nFadeStep;
			if ( nCurrPriVol > g_Prefs.nSndVolume )
				nCurrPriVol = g_Prefs.nSndVolume;
	
			FSOUND_SetVolume( g_ActiveChannels.Item( nFadeInStreamID ), nCurrPriVol );
		}

		Sleep( 100 );
	}
	return NULL;
}

void MusikCrossfaderThread::OnExit()
{
	
	m_Parent->WorkerDec();
	//-------------------------------------------------//
	//--- if we ended naturally, that means no		---//
	//--- other crossfader spawned. tell the player	---//
	//--- no other fading is going on, so it can	---//
	//--- manage and clean up any orphan streams	---//
	//-------------------------------------------------//
	if ( !m_Aborted )
	{
		//-------------------------------------------------//
		//--- finalize whatever type of fade was going	---//
		//--- on.										---//
		//-------------------------------------------------//
		if ( m_FadeType == CROSSFADE_STOP || m_FadeType == CROSSFADE_EXIT )
			g_Player.FinalizeStop();
		else if ( m_FadeType == CROSSFADE_PAUSE )
			g_Player.FinalizePause();
		else if ( m_FadeType == CROSSFADE_RESUME )
			g_Player.FinalizeResume();

		Yield();
		if(m_FadeType != CROSSFADE_EXIT)// if we would send the FadeCompleteEvt on exit we get in trouble with the CMusikPlayer::ClearOldStreams() which tries to stop the fader thread( while it waits the ExitCompleteEvt is dispatched by wx and g_FaderThread is deleted, if CrossfaderStop returns from wait it will crash)
		{
			wxCommandEvent FadeCompleteEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_FADE_COMPLETE );	
			wxPostEvent( &g_Player, FadeCompleteEvt );
		}
		Yield();

	}
	//-------------------------------------------------//
	//--- if the fade type was an OnExit type, then	---//
	//--- post an event to close the app.			---//
	//-------------------------------------------------//
	if ( m_FadeType == CROSSFADE_EXIT )
	{
		wxCommandEvent ExitCompleteEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_EXIT_FADE_DONE );
		wxPostEvent( g_MusikFrame, ExitCompleteEvt );
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

	CMusikSongArray aDirty;
	g_Library.QuerySongsWhere( wxT("dirty = 1"), aDirty );

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
			g_Library.WriteTag( aDirty.Item( i ), m_Clear ,false); // no db update
			g_Library.UpdateItemResetDirty( aDirty.Item( i ).Filename ); // just clear dirty flag

		}

	}

	return NULL;
}

void MusikWriteDirtyThread::OnExit()
{
	wxCommandEvent WriteDirtyEndEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_END );	
	wxPostEvent( g_MusikFrame, WriteDirtyEndEvt );
}
