/*
 *  MusikPlayer.cpp
 *
 *  Object which controls audio playback.
 *
 *  Uses FMOD sound API.
 *  Information about FMOD is available at http://www.fmod.org
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "MusikPlayer.h"

//--- so we can use a pointer to the main frame ---//
#include "../Frames/MusikFrame.h"

//--- globals: library / player / prefs ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- CMusikStreamArray ---//
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( CMusikStreamArray );

BEGIN_EVENT_TABLE( CMusikPlayer, wxEvtHandler )
	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU			( MUSIK_PLAYER_NEXT_SONG, CMusikPlayer::OnNextSongEvt )
    EVT_MENU			( MUSIK_PLAYER_FADE_COMPLETE,	CMusikPlayer::OnFadeCompleteEvt )
END_EVENT_TABLE()

CMusikPlayer::CMusikPlayer()
	: wxEvtHandler()
{
	m_Playing		= false;
	m_Paused		= false;
	m_Fading		= false;
	m_BeginFade		= false;
	m_StartingNext	= false;
	m_SongIndex		= 0;
	m_LastSong		= 0;
	m_Channels		= -1;
	m_Playmode		= MUSIK_PLAYMODE_NORMAL;
	m_CrossfadeType	= 0;

	//--- initialize random playback ---//
	m_RandomSeed = wxGetLocalTime();
	m_RandomIndex = 0;
}

CMusikPlayer::~CMusikPlayer()
{
	//---------------------------------------------------------//
	//--- this will only get called when the program exits	---//
	//--- so nothing needs to be done. FMOD will always		---//
	//--- clean up after itself								---//
	//---------------------------------------------------------//
}

void CMusikPlayer::Shutdown()
{
	FSOUND_Close();
}

int CMusikPlayer::InitializeFMOD( int nFunction, int nSndOutput, int nSndDevice, int nSndRate )
{
	if ( ( nFunction == FMOD_INIT_RESTART ) || ( nFunction == FMOD_INIT_STOP ) )
		FSOUND_Close();

	// set driver
	if ( ( nFunction == FMOD_INIT_START ) || ( nFunction == FMOD_INIT_RESTART ) )
	{
#if defined(__WXMSW__)
		// direct sound
		if ( nSndOutput == 0 )
			if( FSOUND_SetOutput( FSOUND_OUTPUT_DSOUND ) == FALSE )
				return FMOD_INIT_ERROR_DSOUND;
		// windows waveform
		else if ( nSndOutput == 1 )
			if ( FSOUND_SetOutput( FSOUND_OUTPUT_WINMM ) == FALSE )
				return FMOD_INIT_ERROR_WINDOWS_WAVEFORM;
		// asio?
		else if ( nSndOutput == 2 )
			if ( FSOUND_SetOutput( FSOUND_OUTPUT_ASIO ) == FALSE )
				return FMOD_INIT_ERROR_ASIO;
#elif defined (__WXMAC__)
		// mac stuff
#elif defined (__WXGTK__)
		// oss
		if ( nSndOutput == 0 )
			if( FSOUND_SetOutput( FSOUND_OUTPUT_OSS ) == FALSE )
				return FMOD_INIT_ERROR_OSS;
		// esd
		else if ( nSndOutput == 1 )
			if ( FSOUND_SetOutput( FSOUND_OUTPUT_ESD ) == FALSE )
				return FMOD_INIT_ERROR_ESD;
		// alsa
		else if ( nSndOutput == 2 )
			if ( FSOUND_SetOutput( FSOUND_OUTPUT_ALSA ) == FALSE )
				return FMOD_INIT_ERROR_ALSA;
#endif

		// set device
		if ( FSOUND_SetDriver( nSndDevice ) == FALSE )
			return FMOD_INIT_ERROR_DEVICE_NOT_READY;

		// initialize system
		FSOUND_SetBufferSize( 100 );
		if ( FSOUND_Init( nSndRate , MAXCHANNELS, 0 ) == FALSE )
			return FMOD_INIT_ERROR_INIT;
	}
	return FMOD_INIT_SUCCESS;
}

void CMusikPlayer::SetPlaymode( )
{
	if( g_Prefs.nShuffle == 1 )
		m_Playmode = MUSIK_PLAYMODE_RANDOM;
	else if( g_Prefs.nRepeat == 1 )
		m_Playmode = MUSIK_PLAYMODE_LOOPLIST;
	else
		m_Playmode = MUSIK_PLAYMODE_NORMAL;
}

void CMusikPlayer::PlayCurSel()
{
	m_LastSong	= m_SongIndex;
	
	int nCurSel = g_PlaylistCtrl->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if ( nCurSel > -1 )
	{
		if ( g_PlaylistChanged )
		{
			SetPlaylist( g_Playlist );
			g_PlaylistChanged = false;
		}
		Play( nCurSel );
	}	
}

bool CMusikPlayer::Play( size_t nItem, int nStartPos, int nFadeType )
{
	//--- check for an invalid playlist ---//
	if ( ( nItem >= m_Playlist.GetCount() ) || ( m_Playlist.GetCount() < 1 ) )
	{
		m_Playing = false;
		return false;
	}

	else
	{
		//-----------------------------//
		//--- start with the basics ---//
		//-----------------------------//
		m_SongIndex		= nItem;	
		m_CurrentFile	= m_Playlist.Item( nItem ).Filename;

		//---------------------------------------------//
		//--- if there is already a fade in			---//
		//--- progress, then we need to abort it	---//
		//---------------------------------------------//
		if ( g_FaderThread->IsCrossfaderActive() )
			g_FaderThread->CrossfaderAbort();

		//---------------------------------------------//
		//--- open a new stream and push it to the	---//
		//--- bottom of the g_ActiveStreams array	---//
		//---------------------------------------------//
		char* pFilename = StringToANSI( m_CurrentFile );
		FSOUND_Stream_SetBufferSize( g_Prefs.nSndBuffer );
		FSOUND_STREAM* pNewStream = FSOUND_Stream_OpenFile( pFilename, FSOUND_2D, 0 );
		free( pFilename );

		//---------------------------------------------//
		//--- set the crossfade type...				---//
		//---------------------------------------------//
		SetCrossfadeType( nFadeType );
		
		//---------------------------------------------//
		//--- start playback on the new stream on	---//
		//--- the designated channel.				---//
		//---------------------------------------------//
		IncNextChannel();
		FSOUND_Stream_Play( GetCurrChannel(), pNewStream );
		FSOUND_SetVolume( GetCurrChannel(), 0 );
		FSOUND_Stream_SetTime( pNewStream, nStartPos * 1000 );
		
		//---------------------------------------------//
		//--- playback has been started, update the	---//
		//--- user interface to reflect it			---//
		//---------------------------------------------//
		if ( nStartPos <= 0 )
		UpdateUI();

		//---------------------------------------------//
		//--- update the global arrays containing	---//
		//--- active channels and streams			---//
		//---------------------------------------------//
		g_ActiveStreams.Add( pNewStream );
		g_ActiveChannels.Add( GetCurrChannel() );

		//---------------------------------------------//
		//--- if fading is not enabled, shut down	---//
		//--- all of the old channels, and set the	---//
		//--- active stream to full volume			---//
		//---------------------------------------------//
		if ( g_Prefs.nFadeEnable == 0 )
		{
			FSOUND_SetVolume( g_ActiveChannels.Item( g_ActiveChannels.GetCount() - 1 ), g_Prefs.nSndVolume );
			ClearOldStreams();
		}

		//---------------------------------------------//
		//--- tell the listening thread its time to	---//
		//--- start fading							---//
		//---------------------------------------------//
		else if ( g_Prefs.nFadeEnable == 1 )
			SetFadeStart();
		
		g_NowPlayingCtrl->PlayBtnToPauseBtn();
	}

	m_Playing = true;
	return true;
}

void CMusikPlayer::UpdateUI()
{
	//-------------------------------------------------//
	//--- this will resynch the current item if the	---//
	//--- playlist appears to be the same. a better	---//
	//--- check is not necessary.						---//
	//-------------------------------------------------//
	if ( m_Playlist.GetCount() == g_Playlist.GetCount() )
		g_PlaylistCtrl->ResynchItem	( m_SongIndex, m_LastSong );
	
	g_Library.UpdateItemLastPlayed	( m_CurrentFile );
	g_NowPlayingCtrl->UpdateInfo	( m_CurrentFile );
}

void CMusikPlayer::ClearOldStreams()
{
	int nStreamCount = g_ActiveStreams.GetCount() - 1;
	for ( int i = 0; i < nStreamCount; i++ )
	{
		FSOUND_Stream_Stop	( g_ActiveStreams.Item( 0 ) );
		FSOUND_Stream_Close	( g_ActiveStreams.Item( 0 ) );
		g_ActiveStreams.RemoveAt( 0 );
		g_ActiveChannels.RemoveAt( 0 );
	}
}

void CMusikPlayer::Pause()
{
	m_Paused = true;
	FSOUND_SetPaused( FSOUND_ALL, TRUE );

	// if we pause during a crossfade, just delete the channel and stop fading
	if ( m_Fading )
		SetFadeComplete();

	g_NowPlayingCtrl->PauseBtnToPlayBtn();
}

void CMusikPlayer::Resume()
{
	m_Paused = false;
	FSOUND_SetPaused( FSOUND_ALL, FALSE );

	g_NowPlayingCtrl->PlayBtnToPauseBtn();
}

void CMusikPlayer::Stop()
{
	if ( g_FaderThread->IsCrossfaderActive() )
		g_FaderThread->CrossfaderAbort();

	m_Playing = false;

	int nStreamCount = g_ActiveStreams.GetCount();
	for ( int i = 0; i < nStreamCount; i++ )
	{
		FSOUND_Stream_Stop	( g_ActiveStreams.Item( ( nStreamCount - 1 ) - i ) );
		FSOUND_Stream_Close	( g_ActiveStreams.Item( ( nStreamCount - 1 ) - i ) );
	}
	g_ActiveStreams.Clear();
	g_ActiveChannels.Clear();

	m_SongIndex	= 0;
	m_LastSong	= 0;

	g_NowPlayingCtrl->PauseBtnToPlayBtn();
}

size_t CMusikPlayer::GetRandomSong()
{
	size_t r;

	if( !m_RandomIndex || !m_Playlist.GetCount() )
		return 0;

	//--- clear history ---//
	m_History[0] = m_History[1] = m_History[2] = m_History[3] = m_History[4] = ~0;

	//--- prevent stream from getting so long it might bog down the machine :) ---//
	m_RandomIndex &= 65535;

	//--- iterate random stream to current position ---//
	SeedRandom( m_RandomSeed );
	for ( size_t i = 0; i < m_RandomIndex; i++ )
	{
		bool repeat = false;
		do {
			r = GetRandomNumber() % m_Playlist.GetCount();

			//--- check for repeats ---//
			for ( size_t j = 0; j < m_Playlist.GetCount()-1 && j < WXSIZEOF(m_History); j++ )
				if ( r == m_History[j] )
				{
					repeat = true;
					break;
				}
		} while ( repeat );

		//--- rotate history ---//
		for ( int j = WXSIZEOF(m_History) - 1; j > 0; j-- )
			m_History[j] = m_History[j-1];
		m_History[0] = r;
	}

	return r;
}

void CMusikPlayer::NextSong()
{
	m_LastSong	= m_SongIndex;
	
	switch ( m_Playmode )
	{
	case MUSIK_PLAYMODE_NORMAL:
		m_SongIndex++;
		Play( m_SongIndex );
		break;

	case MUSIK_PLAYMODE_LOOPSONG:
		Play( m_SongIndex );
		break;

	case MUSIK_PLAYMODE_LOOPLIST:
		if ( m_SongIndex == ( m_Playlist.GetCount()-1 ) )
			m_SongIndex = 0;
		else m_SongIndex++;
		Play( m_SongIndex );
		break;

	case MUSIK_PLAYMODE_RANDOM:
		m_RandomIndex++;
		Play( GetRandomSong() );
		break;
	}
}

void CMusikPlayer::PrevSong()
{
	m_LastSong	= m_SongIndex;
	
	if ( g_ActiveStreams.GetCount() && GetTime( FMOD_MSEC ) > 2000 )	// 2 second grace period to go to previous track
	{
		Play( m_SongIndex );
		return;
	} 
	else
	{
		switch ( m_Playmode )
		{
		case MUSIK_PLAYMODE_NORMAL:
			if( m_SongIndex > 0 )
				m_SongIndex--;
			Play( m_SongIndex );
			break;
	
		case MUSIK_PLAYMODE_LOOPSONG:
			Play( m_SongIndex );
			break;
	
		case MUSIK_PLAYMODE_LOOPLIST:
			if ( m_SongIndex <= 0 )
				m_SongIndex = ( m_Playlist.GetCount() - 1 );
			else m_SongIndex--;
			Play( m_SongIndex );
			break;
	
		case MUSIK_PLAYMODE_RANDOM:
			if ( m_RandomIndex > 0 ) m_RandomIndex--;
			Play( GetRandomSong() );
			break;
		}
	}
}

void CMusikPlayer::SetVolume()
{
	if ( IsPlaying() )
		FSOUND_SetVolume( g_ActiveChannels.Item( g_ActiveChannels.GetCount() - 1 ), g_Prefs.nSndVolume );
}

int CMusikPlayer::GetDuration( int nType )
{
	if ( nType == FMOD_SEC )
		return ( FSOUND_Stream_GetLengthMs( g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ) ) / 1000 );
	else if ( nType == FMOD_MSEC )
		return ( FSOUND_Stream_GetLengthMs( g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ) ) );
	else
		return ( -1 );
}

int CMusikPlayer::GetTime( int nType )
{
	if ( nType == FMOD_SEC )
		return ( FSOUND_Stream_GetTime( g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ) ) / 1000 );
	else if ( nType == FMOD_MSEC )
		return ( FSOUND_Stream_GetTime( g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ) ) );
	else
		return ( -1 );
}

int CMusikPlayer::GetTimeLeft( int nType )
{
	if ( nType == FMOD_SEC )
		return ( GetDuration( FMOD_SEC ) - GetTime( FMOD_SEC ) );
	else if ( nType == FMOD_MSEC )
		return ( GetDuration( FMOD_MSEC ) - GetTime( FMOD_MSEC ) );
	else
		return ( -1 );
}

wxString CMusikPlayer::GetTimeStr()
{
	return SecToStr( GetTime( FMOD_SEC ) );
}

void CMusikPlayer::SetTime( int nSec )
{
	if ( g_Prefs.nFadeSeekEnable == 0 || m_Paused )
	FSOUND_Stream_SetTime( g_ActiveStreams.Item( g_ActiveStreams.GetCount() - 1 ), nSec * 1000 );

	else
		Play( m_SongIndex, nSec, CROSSFADE_SEEK );
}

wxString CMusikPlayer::SecToStr( int nSec )
{
	wxString result;

	int ms = nSec * 1000;
	int hours = ms / 1000 / 60 / 60;
	ms -= hours * 1000 * 60 * 60;
	int minutes = ms / 1000 / 60;
	ms -= minutes * 1000 * 60;
	int seconds = ms / 1000;

	if ( hours > 0 )
		result.sprintf( wxT( "%d:%02d:%02d" ), hours, minutes, seconds );
	else
		result.sprintf( wxT( "%d:%02d" ), minutes, seconds );
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * \brief Gets the duration of a song by loading and closing a file
 * \param filename 
 * \return duration of song, in milliseconds, -1 if a failure
 */
int CMusikPlayer::GetFileDuration( wxString sFilename, int nType )
{
	int duration = -1;

	// this should be FSOUND_MPEGACCURATE to get an accurate length, but it's way slower..
	FSOUND_STREAM *pStream = FSOUND_Stream_OpenFile( StringToANSI( sFilename ), FSOUND_2D, 0 );

	if ( pStream )
	{
		duration = FSOUND_Stream_GetLengthMs( pStream );
		FSOUND_Stream_Close( pStream );
	}

    if ( nType == FMOD_MSEC )
		return duration;
	else
		return ( duration / 1000 );	
}

void CMusikPlayer::SetFadeComplete()
{
	m_Fading = false; 
	ClearOldStreams();
}

void CMusikPlayer::SetFadeStart()
{
	m_BeginFade = true;
	m_Fading = true; 
}

void CMusikPlayer::IncNextChannel()
{
	if ( ( m_Channels + 1 ) == MAXCHANNELS )
		m_Channels = 0;
	else
		m_Channels++;
}

int CMusikPlayer::GetCurrChannel()
{
	return m_Channels;
}
