// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "MusikThreads.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- MusikStreamArray here ---//
#include "../Classes/MusikPlayer.h"

//----------------------------//
//--- MusikCrossfadeThread ---//
//----------------------------//
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
		pCrossfader->Abort();		//--- will tell the fader not to clean up old streams ---//
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

MusikCrossfaderThread::MusikCrossfaderThread( MusikFaderThread *pParent )
{
	m_Parent = pParent;
	m_Aborted = false;
}

void MusikCrossfaderThread::Abort()
{
	m_Aborted = true;
}

void *MusikCrossfaderThread::Entry()
{
	int nFadeDuration = 0; 
	if ( g_Player.GetCrossfadeType() == CROSSFADE_NORMAL )
		nFadeDuration = g_Prefs.nFadeDuration;
	else if ( g_Player.GetCrossfadeType() == CROSSFADE_SEEK )
		nFadeDuration = g_Prefs.nFadeSeekDuration;

	int nFadePerSec = 20;
	float fFadeCount = (float)nFadePerSec * (float)( (float)g_Prefs.nFadeDuration / 1000.0f );
	int nFadeCount = (int)fFadeCount;
	if ( nFadeCount <= 0 )
		nFadeCount = 1;

	int nFadeStep = g_Prefs.nSndVolume / nFadeCount;
	if ( nFadeStep <= 0 )
		nFadeStep = 1;

	//-----------------------------------------//
	//--- how long the crossfade will sleep	---//
	//-----------------------------------------//
	float fSleepTime	= (float)nFadeDuration / nFadeCount; 
	int nSleepTime		= MusikRound( fSleepTime );

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
		{
			return NULL;
		}

		//--- fade down ---//
		for ( size_t j = 0; j < g_ActiveStreams.GetCount() - 1; j++ )
		{
			nLastVol =  FSOUND_GetVolume( g_ActiveChannels.Item( j ) ) - nFadeStep;
			FSOUND_SetVolume( g_ActiveChannels.Item( j ), nLastVol );
		}

		//--- fade up ---//
		nCurrVol += nFadeStep;
		if ( nCurrVol > g_Prefs.nSndVolume )
			nCurrVol = g_Prefs.nSndVolume;

		FSOUND_SetVolume( g_ActiveChannels.Item( g_ActiveChannels.GetCount() - 1 ), nCurrVol );

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
	if ( !m_Aborted )
	{
		wxCommandEvent FadeCompleteEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_FADE_COMPLETE );	
		wxPostEvent( &g_Player, FadeCompleteEvt );
		Yield();
	}

	m_Parent->SetCrossfaderActive( false );
}
