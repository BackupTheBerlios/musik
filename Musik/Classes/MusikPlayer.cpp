/*
 *  MusikPlayer.cpp
 *
 *  Object which controls audio playback.
 *
 *  Uses FMOD sound API.
 *  Information about FMOD is available at http://www.fmod.org
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "MusikPlayer.h"
#include "../Frames/MusikFrame.h"
//--- globals: library / player / prefs ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#include "fmod_errors.h"
#include "../MusikApp.h"
//--- CMusikStreamArray ---//
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( CMusikStreamArray );

#define MUSIK_FMOD_VERSION 0x0370 //--- 0x0363 or 0x0370 ---//

void * F_CALLBACKAPI dspcallback(void *WXUNUSED(originalbuffer), void *newbuffer, int length, int WXUNUSED(param))
{
	// 2 channels (stereo), 16 bit sound
	g_FX.ProcessSamples( newbuffer, length, 2, 16 );
	return newbuffer;
}


BEGIN_EVENT_TABLE( CMusikPlayer, wxEvtHandler )
	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU			( MUSIK_PLAYER_NEXT_SONG,		CMusikPlayer::OnNextSongEvt		)
    EVT_MENU			( MUSIK_PLAYER_FADE_COMPLETE,	CMusikPlayer::OnFadeCompleteEvt )
    EVT_MENU			( MUSIK_PLAYER_NEW_METADATA,	CMusikPlayer::_UpdateNetstreamMetadata )
	EVT_MENU			( MUSIK_PLAYER_STOP,			CMusikPlayer::OnPlayerStop		)
	EVT_MENU			( MUSIK_PLAYER_PLAY_RESTART,	CMusikPlayer::OnPlayRestart		)
	EVT_MENU			( MUSIK_PLAYER_RESUME,			CMusikPlayer::OnPlayerResume	)
END_EVENT_TABLE()

CMusikPlayer::CMusikPlayer()
	: wxEvtHandler()
	
{
	m_Playing		= false;
	m_Paused		= false;
	m_Fading		= false;
	m_BeginFade		= false;
	m_StartingNext	= false;
	m_Stopping		= false;
	m_SongIndex		= 0;
	m_Channels		= -1;
	m_CrossfadeType	= 0;
	m_DSP			= NULL;
	m_Playmode = MUSIK_PLAYMODE_NORMAL;
	m_NETSTREAM_read_percent = 0;
	m_NETSTREAM_bitrate = 0;
	m_NETSTREAM_status = -1;
	m_NETSTREAM_flags = 0;
	m_b_NETSTREAM_AbortConnect = false;
	//--- initialize random playback ---//
	long RandomSeed = wxGetLocalTime();
	SeedRandom( RandomSeed );
	m_arrHistory.Alloc(g_Prefs.nMaxShuffleHistory);
	//set stop watch into pause mode.
	m_bStreamIsWorkingStopWatchIsRunning = false ;
	m_NETSTREAM_last_read_percent = 0;
	m_nLastSongTime = 0;
}

void CMusikPlayer::Init(bool bSuppressAutoPlay)
{
	int nPlayStartPos = 0;
// load old playlist
	if(wxFileExists(MUSIK_PLAYERLIST_FILENAME))
	{
		wxTextFile In;
		In.Open(MUSIK_PLAYERLIST_FILENAME);
		if (In.IsOpened() )
		{
			if(In.GetLineCount() > 1)
			{
				wxArrayString arr;
				DelimitStr(In.GetLine( 0 ),wxT(":"),arr);
				if(arr.GetCount()==2)
				{
					arr[0].ToULong((unsigned long *)&m_SongIndex);
					arr[1].ToLong((long*)&nPlayStartPos);
					nPlayStartPos = max(0,(nPlayStartPos - 10 * 1000)/1000);
				}
			}
			wxArrayString aFilelist;
			for ( size_t i = 1; i < In.GetLineCount(); i++ )
			{
				aFilelist.Add( In.GetLine( i ) );	
			}
			In.Close();
			g_Library.GetFilelistSongs( aFilelist, m_Playlist );
		}
	}
	if(!bSuppressAutoPlay && g_Prefs.bAutoPlayOnAppStart && m_Playlist.GetCount())
		_PostPlayRestart( nPlayStartPos ); 
}
CMusikPlayer::~CMusikPlayer()
{
		wxRemoveFile( MUSIK_PLAYERLIST_FILENAME );
		wxTextFile Out;
		Out.Create( MUSIK_PLAYERLIST_FILENAME );
		Out.Open();
		if ( Out.IsOpened() )
		{
			Out.AddLine( wxString::Format(wxT("%d:%d"),m_SongIndex, m_nLastSongTime));
			for ( size_t i = 0; i < m_Playlist.GetCount(); i++ )
			{
				Out.AddLine( m_Playlist[i].Filename );
			}
			
			Out.Write( Out.GuessType() );
			Out.Close();
		}


	//---------------------------------------------------------//
	//--- this will only get called when the program exits	---//
	//--- so nothing needs to be done. FMOD will always		---//
	//--- clean up after itself								---//
	//---------------------------------------------------------//
}

void CMusikPlayer::Shutdown( bool bClose )
{
	if ( bClose )
		Stop( true, true );
	else
		Stop();

	g_FX.EndEQ();
	FSOUND_Close();
}

int CMusikPlayer::InitializeFMOD( int nFunction )
{
	if ( ( nFunction == FMOD_INIT_RESTART ) || ( nFunction == FMOD_INIT_STOP ) )
		Shutdown( false );

	g_FX.InitEQ();
	g_FX.SetFrequency( 44100 );

	//---------------------//
	//--- setup driver	---//
	//---------------------//
	if ( ( nFunction == FMOD_INIT_START ) || ( nFunction == FMOD_INIT_RESTART ) )
	{
		//-----------------//
		//--- windows	---//
		//-----------------//
		#if defined(__WXMSW__)
			if ( g_Prefs.nSndOutput == 0 )
			{
				if( FSOUND_SetOutput( FSOUND_OUTPUT_DSOUND ) == FALSE )
					return FMOD_INIT_ERROR_DSOUND;
			}
			else if ( g_Prefs.nSndOutput == 1 )
			{
				if ( FSOUND_SetOutput( FSOUND_OUTPUT_WINMM ) == FALSE )
					return FMOD_INIT_ERROR_WINDOWS_WAVEFORM;
			}
			else if ( g_Prefs.nSndOutput == 2 )
			{
				if ( FSOUND_SetOutput( FSOUND_OUTPUT_ASIO ) == FALSE )
					return FMOD_INIT_ERROR_ASIO;
			}
		//-----------------//
		//--- linux		---//
		//-----------------//
		#elif defined (__WXGTK__)
			if ( g_Prefs.nSndOutput == 0 )
			{
				if( FSOUND_SetOutput( FSOUND_OUTPUT_OSS ) == FALSE )
					return FMOD_INIT_ERROR_OSS;
			}
			else if ( g_Prefs.nSndOutput == 1 )
			{
				if ( FSOUND_SetOutput( FSOUND_OUTPUT_ESD ) == FALSE )
					return FMOD_INIT_ERROR_ESD;
			}
			else if ( g_Prefs.nSndOutput == 2 )
			{
				if ( FSOUND_SetOutput( FSOUND_OUTPUT_ALSA ) == FALSE )
					return FMOD_INIT_ERROR_ALSA;
			}
		#endif
		//---------------------//
		//--- setup device	---//
		//---------------------//
		if ( FSOUND_SetDriver( g_Prefs.nSndDevice ) == FALSE )
			return FMOD_INIT_ERROR_DEVICE_NOT_READY;

		// initialize system
		FSOUND_SetBufferSize( 100 );
		InitFMOD_NetBuffer();	
		InitFMOD_ProxyServer();
		if ( FSOUND_Init( g_Prefs.nSndRate , g_Prefs.nSndMaxChan, 0 ) == FALSE )
			return FMOD_INIT_ERROR_INIT;
//		g_Prefs.nSndOutput = FSOUND_GetOutput();
	}
	return FMOD_INIT_SUCCESS;
}
void CMusikPlayer::InitFMOD_NetBuffer	( )
{
	FSOUND_Stream_Net_SetBufferProperties(g_Prefs.nStreamingBufferSize , g_Prefs.nStreamingPreBufferPercent, g_Prefs.nStreamingReBufferPercent);
}
void CMusikPlayer::InitFMOD_ProxyServer	( )
{
	if(g_Prefs.bUseProxyServer)
	{
		wxString sProxyString(  g_Prefs.sProxyServer );
		if(	!g_Prefs.sProxyServerPort.IsEmpty() )
		{
			sProxyString += wxT(":") + 	g_Prefs.sProxyServerPort;
		}
		if(	!g_Prefs.sProxyServerUser.IsEmpty() )	 
		{
			sProxyString =  g_Prefs.sProxyServerUser + wxT(":") + g_Prefs.sProxyServerPassword + wxT("@")  +  sProxyString;
		}
		FSOUND_Stream_Net_SetProxy(ConvW2A(sProxyString));
	}
	else
		FSOUND_Stream_Net_SetProxy("");


}
void CMusikPlayer::SetPlaymode(EMUSIK_PLAYMODE pm )
{
	m_Playmode = pm;
}
void CMusikPlayer::PlayPause()
{
	//--- paused, so play ---//
	if ( IsPlaying() && !IsPaused() )
		Pause();

	//--- paused, so resume ---//
	else if ( IsPlaying() && IsPaused() )
		Resume();

	//--- start playing ---//
	else if ( !IsPlaying() )
	{
		if(GetPlaymode() == MUSIK_PLAYMODE_SHUFFLE)
		{
/*
			// if playlist has changed set new playlist for the player
			// in random mode
			if ( g_PlaylistChanged )
			{
				SetPlaylist( g_Playlist );
				g_PlaylistChanged = false;
			}
*/
			NextSong();
		}
		else
		{
			Play( m_SongIndex );
		}
	}
}
void CMusikPlayer::PlayReplaceList(int nItemToPlay,const CMusikSongArray & playlist)
{
	if ( IsPaused() )
	{
		ClearOldStreams();
		m_Paused = false;
	}
	
	SetPlaylist( playlist );
	Play( nItemToPlay );
}
signed char F_CALLBACKAPI CMusikPlayer::MetadataCallback(char *name, char *value, int userdata)
{
 	CMusikPlayer * _this = (CMusikPlayer *)userdata;
	_this->_SetMetaData(name, value);
	wxCommandEvent MetaDataEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_NEW_METADATA );	
	wxPostEvent( _this, MetaDataEvt );

    return TRUE;
}
void CMusikPlayer::_SetMetaData(char *name, char *value)
{
	wxCriticalSectionLocker locker( m_critMetadata );
	wxString sMetadataName = ConvA2W(name).MakeUpper();
	wxString sMetadataValue = ConvA2W(value);
	if (wxT("ARTIST") == sMetadataName)
	{
		m_MetaDataSong.Artist = sMetadataValue;
	}
	else if (wxT("TITLE")== sMetadataName)
	{
		m_MetaDataSong.Title = sMetadataValue;
	}
	else if (wxT("ALBUM") == sMetadataName)
	{
		m_MetaDataSong.Album = sMetadataValue;
	}
	else if (wxT("GENRE") == sMetadataName)
	{
		m_MetaDataSong.Genre = sMetadataValue;
	}
	else if (wxT("TRACKNUMBER") == sMetadataName)
	{
		m_MetaDataSong.TrackNum = wxStringToInt(sMetadataValue);
	}

	::wxLogDebug(wxT("metadate received: name=%s,value=%s"),(const wxChar*)sMetadataName,(const wxChar*)sMetadataValue);
}
void CMusikPlayer::_UpdateNetstreamMetadata(wxCommandEvent& WXUNUSED(event))
{
	if(_CurrentSongIsNetStream())
	{
		{
			{
				wxCriticalSectionLocker locker( m_critMetadata );
				CMusikSong & cursong = m_Playlist.Item( m_SongIndex );
				cursong.Album = m_MetaDataSong.Album;
				cursong.Artist = m_MetaDataSong.Artist;
				cursong.Title = m_MetaDataSong.Title;
				cursong.Genre = m_MetaDataSong.Genre;
				cursong.TrackNum = m_MetaDataSong.TrackNum;
			}
			wxCriticalSectionLocker locker( m_critInternalData );
			m_CurrentSong = m_Playlist.Item( m_SongIndex );
		}
		UpdateUI();
	}
}
void CMusikPlayer::_PostPlayRestart( int nStartPos )
{
		if(IsPlaying())
			Stop();	 // stop current stream
		wxCommandEvent PlayRestartEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_PLAYER_PLAY_RESTART );
		PlayRestartEvt.SetInt( nStartPos );
		wxPostEvent( this,  PlayRestartEvt);
}   

void CMusikPlayer::OnPlayRestart( wxCommandEvent& event )
{//this method is used in case of a network problem when streaming
		Play(  m_SongIndex , event.GetInt() ); // start playing of current stream
}
bool CMusikPlayer::Play( size_t nItem, int nStartPos, int nFadeType )
{
	if(m_Playlist.GetCount() < g_Prefs.nAutoDJChooseSongsToPlayInAdvance && MUSIK_PLAYMODE_AUTO_DJ == m_Playmode)
	{
		if(m_Playlist.GetCount() == 0)
			nItem = 0;
		_AddRandomSongs();
		if(m_Playlist.GetCount() == 0)   // no songs could be chosen
		{
			m_Playing = false;
			return false;
		}
	}
	//--- check for an invalid playlist ---//
	if ( ( nItem >= m_Playlist.GetCount() ) || ( m_Playlist.GetCount() == 0 ) )
	{
		m_Playing = false;
		return false;
	}

	if (IsPlaying() && _CurrentSongIsNetStream())
	{	
		if(m_CurrentSong.Filename == m_Playlist.Item( nItem ).Filename)
		{
			return true; // already playing this stream
		}

//		_PostPlayRestart(); // will restart playing postponed
//		return true;
	}
	else if (_IsNETSTREAMConnecting())
	{
		return true;
	}
	else if((m_Playlist.Item( nItem ).Format != MUSIK_FORMAT_NETSTREAM) && !::wxFileExists(m_Playlist.Item( nItem ).Filename))
	{	// not a network stream
		// and
		// file does not exist
			wxMessageBox( _( "Cannot find file.\n Filename:" ) + m_Playlist.Item( nItem ).Filename, MUSIKAPPNAME_VERSION, wxICON_STOP );
			Stop();
			return false;
	}

	if(m_CurrentSong.songid >= 0 &&IsPlaying())
		g_Library.RecordSongHistory(m_CurrentSong,GetTime(FMOD_MSEC),true);
	//---------------------------------------------//
	//--- start with the basics					---//
	//---------------------------------------------//
	m_Stopping		= false;
	m_SongIndex		= nItem;	
	m_CurrentSong	= m_Playlist.Item( m_SongIndex );

	//---------------------------------------------//
	//--- if there is already a fade in			---//
	//--- progress, then we need to abort it	---//
	//---------------------------------------------//
	SetCrossfadeType( nFadeType );

	//---------------------------------------------//
	//--- open a new stream and push it to the	---//
	//--- bottom of the g_ActiveStreams array	---//
	//---------------------------------------------//
	FSOUND_Stream_SetBufferSize( g_Prefs.nSndBuffer );
	int nFlags = FSOUND_NORMAL;
	if( _CurrentSongNeedsMPEGACCURATE())
		nFlags |= FSOUND_MPEGACCURATE;
	if(_CurrentSongIsNetStream())
		nFlags |= FSOUND_NONBLOCKING;

#if ( MUSIK_FMOD_VERSION >= 0x0370 )
	FSOUND_STREAM* pNewStream = FSOUND_Stream_Open( ( const char* )ConvFNToFieldMB( m_CurrentSong.Filename ), nFlags , 0, 0 );
#else
	FSOUND_STREAM* pNewStream = FSOUND_Stream_OpenFile( ( const char* )ConvFNToFieldMB( m_CurrentSong.Filename ), nFlags , 0);
#endif
	if(pNewStream == NULL)
	{
		wxMessageBox( _( "Playback will be stopped, because loading failed.\n Filename:" ) + m_CurrentSong.Filename, MUSIKAPPNAME_VERSION, wxICON_STOP );
		Stop(false);
		return false;
	}
	InitDSP();
	if(_CurrentSongIsNetStream()&& _IsNETSTREAMConnecting() == false)
	{
		ClearOldStreams(true);// clear all streams
		m_MetaDataSong = CMusikSong();
		m_p_NETSTREAM_Connecting = pNewStream;
		m_Playing = true;
		m_b_NETSTREAM_AbortConnect = false;
		bool bExit = false;
		do
		{
			int  openstate = FSOUND_Stream_GetOpenState(pNewStream);
			if ((openstate == -1) || (openstate == -3))
			{
				wxMessageBox(_("ERROR: failed to open stream:")+ ConvA2W(FSOUND_Stream_Net_GetLastServerStatus()));
				m_b_NETSTREAM_AbortConnect = true;
				break;
			}
			
		    switch(_NetStreamStatusUpdate(pNewStream))
			{
			case FSOUND_STREAM_NET_READY:
				if(openstate == 0)
					bExit = true;
				break;
			case FSOUND_STREAM_NET_ERROR:
				m_b_NETSTREAM_AbortConnect = true;
				break;
			}
			wxGetApp().Yield();

		} while (!bExit && !m_b_NETSTREAM_AbortConnect);
		m_p_NETSTREAM_Connecting = NULL;
		if( m_b_NETSTREAM_AbortConnect )
		{
			FSOUND_Stream_Close( pNewStream );
			Stop(false);
			m_b_NETSTREAM_AbortConnect = false;
			return false;
		}
		
	}
	//---------------------------------------------//
	//--- start playback on the new stream on	---//
	//--- the designated channel.				---//
	//---------------------------------------------//
	m_Channels = FSOUND_Stream_PlayEx( FSOUND_FREE, pNewStream,NULL,TRUE);//start paused
	if(m_Channels  == -1)
	{
		wxMessageBox(_("Play failed, please try again."));
		wxLogDebug(wxT("play failed:%s"),(const wxChar *) ConvA2W(FMOD_ErrorString(FSOUND_GetError())));
		FSOUND_Stream_Close( pNewStream );
		Stop(false);
		return false;

	}
	if(_CurrentSongIsNetStream())
	{
		FSOUND_Stream_Net_SetMetadataCallback(pNewStream, MetadataCallback, (int)this);
	}
	FSOUND_SetVolume( GetCurrChannel(), 0 );
	FSOUND_Stream_SetTime( pNewStream, nStartPos * 1000 );
	FSOUND_SetPaused(GetCurrChannel(), FALSE);
	g_FaderThread->CrossfaderAbort();
	//---------------------------------------------//
	//--- update the global arrays containing	---//
	//--- active channels and streams			---//
	//---------------------------------------------//
	{
		wxCriticalSectionLocker lock(g_protectingStreamArrays);
		for(size_t i = 0; i < g_ActiveChannels.GetCount();i++)
		{
			if(g_ActiveChannels[i] == GetCurrChannel())
			{
				FSOUND_Stream_Stop ( g_ActiveStreams[i] );
				FSOUND_Stream_Close( g_ActiveStreams[i] );
				g_ActiveStreams.RemoveAt(i);
				g_ActiveChannels.RemoveAt(i);
				break;
			}
		}
		g_ActiveStreams.Add( pNewStream );
		g_ActiveChannels.Add( GetCurrChannel() );
	}
	m_Playing = true;

	SetFrequency();

	
	//---------------------------------------------//
	//--- playback has been started, update the	---//
	//--- user interface to reflect it			---//
	//---------------------------------------------//
	g_NowPlayingCtrl->PlayBtnToPauseBtn();
	UpdateUI();

	//---------------------------------------------//
	//--- if fading is not enabled, shut down	---//
	//--- all of the old channels, and set the	---//
	//--- active stream to full volume			---//
	//---------------------------------------------//
	if ( g_Prefs.bFadeEnable == 0 || g_Prefs.bGlobalFadeEnable == 0 )
	{
		if(g_ActiveChannels.GetCount())
			FSOUND_SetVolume( g_ActiveChannels.Item( g_ActiveChannels.GetCount() - 1 ), 255 );
		ClearOldStreams();
	}

	//---------------------------------------------//
	//--- tell the listening thread its time to	---//
	//--- start fading							---//
	//---------------------------------------------//
	else if ( g_Prefs.bFadeEnable && g_Prefs.bGlobalFadeEnable )
		SetFadeStart();		
	return true;
}
int CMusikPlayer::_NetStreamStatusUpdate(FSOUND_STREAM * pStream)
{
	int status = 0;
	CMusikSong & song = m_CurrentSong;
	FSOUND_Stream_Net_GetStatus(pStream, &status, &m_NETSTREAM_read_percent, &m_NETSTREAM_bitrate, &m_NETSTREAM_flags);
	if(m_NETSTREAM_status != status)
	{
		m_NETSTREAM_status = status;
		switch(status)
		{
		case FSOUND_STREAM_NET_NOTCONNECTED:
			break;
		case FSOUND_STREAM_NET_CONNECTING:
			{
				song.Notes = _("connecting ...");
				g_NowPlayingCtrl->UpdateInfo(song);
			}
			break;
		case FSOUND_STREAM_NET_BUFFERING:
			{
				song.Notes = _("buffering ...");
				g_NowPlayingCtrl->UpdateInfo(song);
			}
			break;
		case FSOUND_STREAM_NET_READY:
			{
				song.Notes = _("streaming");
				g_NowPlayingCtrl->UpdateInfo(song);
			}
			break;
		case FSOUND_STREAM_NET_ERROR:
			wxMessageBox(_("ERROR: failed to open stream:")+ ConvA2W(FSOUND_Stream_Net_GetLastServerStatus()));
			break;

		}
	}
	if(song.Bitrate != m_NETSTREAM_bitrate)
	{
		song.Bitrate = m_NETSTREAM_bitrate;
		UpdateUI();
	}
	

	return status;
}
void CMusikPlayer::InitDSP()
{
	if ( !m_DSP )
		m_DSP = FSOUND_DSP_Create( &dspcallback, FSOUND_DSP_DEFAULTPRIORITY_USER, 0 );			

	if ( m_DSP )
		FSOUND_DSP_SetActive( m_DSP, g_Prefs.bUseEQ );
}

void CMusikPlayer::ActivateDSP()
{
	FSOUND_DSP_SetActive( m_DSP, g_Prefs.bUseEQ );
}

void CMusikPlayer::FreeDSP()
{
	if ( m_DSP )
	{
		FSOUND_DSP_Free( m_DSP );
		m_DSP = NULL;
	}
}

void CMusikPlayer::SetFrequency()
{
	//-------------------------------------------------//
	//--- SiW: this causes problems. mp3s play fine	---//
	//--- but ogg's are slow. or vice versa.		---//
	//--- commented this out for the release.		---//
	//--- See Also: MusikFXFrame::MusikFXFrame()	---//
	//-------------------------------------------------//
	if ( g_Prefs.bUsePitch )
		FSOUND_SetFrequency( FSOUND_ALL, (int)g_FX.GetFrequency() );
	
}

void CMusikPlayer::UpdateUI()
{
	CMusikSong song;
	if(_CurrentSongIsNetStream())
	{
		g_PlaylistBox->PlaylistCtrl().ResynchItem		( m_SongIndex,m_CurrentSong);
	}
	else
	{
		g_PlaylistBox->PlaylistCtrl().Update(false);
		g_Library.UpdateItemLastPlayed	( m_CurrentSong );
	}
	if(g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING)
	{
		g_PlaylistBox->PlaylistCtrl().EnsureVisible(m_SongIndex);
	}
	g_NowPlayingCtrl->UpdateInfo	( m_CurrentSong );
}

void CMusikPlayer::ClearOldStreams( bool bClearAll )
{
	if (!bClearAll && g_FaderThread )
	{
		if ( g_FaderThread->GetWorkerCount() )
			return;
	}
	if(g_FaderThread)
	{
		g_FaderThread->CrossfaderAbort();
		g_FaderThread->CrossfaderStop();
	}
	wxCriticalSectionLocker lock(g_protectingStreamArrays);
	int nStreamCount = g_ActiveStreams.GetCount();

	//-------------------------------------------------//
	//--- if we aren't clearing all streams, keep	---//
	//--- the primary one going.					---//
	//-------------------------------------------------//
	if ( !bClearAll )
		nStreamCount--;

	for ( int i = 0; i < nStreamCount; i++ )
	{
		if(g_ActiveStreams.Item( 0 ))
		{
		FSOUND_Stream_Stop	( g_ActiveStreams.Item( 0 ) );
		FSOUND_Stream_Close	( g_ActiveStreams.Item( 0 ) );
		}
		g_ActiveStreams.RemoveAt( 0 );
		g_ActiveChannels.RemoveAt( 0 );
	}
}

void CMusikPlayer::Pause( bool bCheckFade )
{
	if(_IsNETSTREAMConnecting())
		return;
	//-------------------------------------------------//
	//--- set the fade type, let the thread worry	---//
	//--- about cleaning any existing fades.		---//
	//-------------------------------------------------//
	SetCrossfadeType( CROSSFADE_PAUSE );

	//-------------------------------------------------//
	//--- update the UI.							---//
	//-------------------------------------------------//
	m_Paused = true;
	g_NowPlayingCtrl->PauseBtnToPlayBtn();
	
	//-------------------------------------------------//
	//--- if this type of crossfade is enabled,		---//
	//--- then just setup a fade signal. an event	---//
	//--- will get posted back to actually pause	---//
	//--- once the fade is complete.				---//
	//-------------------------------------------------//
	if ( bCheckFade && g_Prefs.bFadePauseResumeEnable && g_Prefs.bGlobalFadeEnable )
		SetFadeStart();
	else
		FinalizePause();
}

void CMusikPlayer::FinalizePause()
{
	FSOUND_SetPaused( FSOUND_ALL, TRUE );
}

void CMusikPlayer::Resume( bool bCheckFade )
{
	//-------------------------------------------------//
	//--- set the fade type, let the thread worry	---//
	//--- about cleaning any existing fades.		---//
	//-------------------------------------------------//
	SetCrossfadeType( CROSSFADE_RESUME );

	//-------------------------------------------------//
	//--- update the UI.							---//
	//-------------------------------------------------//
	g_NowPlayingCtrl->PlayBtnToPauseBtn();
	FSOUND_SetPaused( FSOUND_ALL, FALSE );
	m_Paused = false;

	//-----------------------------------------------------//
	//--- setup crossfader and return, if	the prefs	---//
	//--- say so.										---//
	//-----------------------------------------------------//
	if ( bCheckFade && g_Prefs.bFadePauseResumeEnable && g_Prefs.bGlobalFadeEnable )
			SetFadeStart();
	else
		FinalizeResume();
}

void CMusikPlayer::FinalizeResume()
{
	SetVolume();
}

void CMusikPlayer::Stop( bool bCheckFade, bool bExit )
{
	if(m_Stopping) return;
	m_b_NETSTREAM_AbortConnect = true;
	m_Stopping = true;
	//-------------------------------------------------//
	//--- set the fade type, let the thread worry	---//
	//--- about cleaning any existing fades.		---//
	//-------------------------------------------------//
	if ( !bExit )
		SetCrossfadeType( CROSSFADE_STOP );
	else
		SetCrossfadeType( CROSSFADE_EXIT );


	//-------------------------------------------------//
	//--- update the ui.							---//
	//-------------------------------------------------//
	g_MusikFrame->SetTitle( MUSIKAPPNAME_VERSION );
	g_NowPlayingCtrl->ResetInfo();
	g_NowPlayingCtrl->PauseBtnToPlayBtn();

	

	//-------------------------------------------------//
	//--- setup crossfader and return, if the prefs	---//
	//--- say so.									---//
	//-------------------------------------------------//
	if ((!_CurrentSongIsNetStream() || bExit) && bCheckFade && g_Prefs.bFadeExitEnable && g_Prefs.bGlobalFadeEnable )
	{
		//--- use exit duration ---//
		if ( bExit )
		{
			if ( IsPlaying() )
			{
				SetFadeStart();
				return;
			}
		}
		
		//--- use stop duration ---//
		else
		{
			if ( IsPlaying() )
			{
				SetFadeStart();
				return;
			}				
		}
	}
	else
		FinalizeStop();
}

void CMusikPlayer::FinalizeStop()
{
	if(m_CurrentSong.songid >= 0 &&IsPlaying())
	{
		g_Library.RecordSongHistory(m_CurrentSong,m_nLastSongTime,true);
	}
	else
		m_nLastSongTime = 0;
	m_CurrentSong = CMusikSong();
	m_Playing = m_Paused = false;
	m_Stopping = false;
	g_PlaylistBox->PlaylistCtrl().ResynchItem( m_SongIndex );
	int nStreamCount = g_ActiveStreams.GetCount();
	for ( int i = 0; i < nStreamCount; i++ )
	{
		FSOUND_Stream_Stop	( g_ActiveStreams.Item( ( nStreamCount - 1 ) - i ) );
		FSOUND_Stream_Close	( g_ActiveStreams.Item( ( nStreamCount - 1 ) - i ) );
	}
	wxCriticalSectionLocker lock(g_protectingStreamArrays);
	g_ActiveStreams.Clear();
	g_ActiveChannels.Clear();

	//-----------------------------------------//
	//--- free up the DSP object. FX stuff.	---//
	//-----------------------------------------//
	FreeDSP();
}

size_t CMusikPlayer::GetShuffledSong()
{
	size_t r = 0;

	if( m_Playlist.GetCount() == 0 )
		return 0;

	
	bool repeat = false;
	int nMaxRepeatCount = 30;
	do {
		repeat = false;
		r = GetRandomNumber() % m_Playlist.GetCount();

		if(m_arrHistory.GetCount() == m_Playlist.GetCount())
		{	// history is as large as the playlist
			// clear the history and start anew
			m_arrHistory.Clear();
			break;
		}
		if(nMaxRepeatCount--) // only check for repeats nMaxRepeatCount times, to prevent endless do loop
		{
			if(::wxFileExists(m_Playlist.Item( r ).Filename))
			{
				//--- check for repeats ---//
				for ( size_t j = 0;  j < m_arrHistory.GetCount(); j++ )
				{
					if ( r == m_arrHistory[j] )
					{
						repeat = true; 
						break;
					}
				}
			}
			else
			{
				repeat = true; 
			}
		}
	} while ( repeat );

	if( m_arrHistory.GetCount() >= g_Prefs.nMaxShuffleHistory )
	{
	//--- rotate history ---//
		m_arrHistory.RemoveAt(0);
	}
	m_arrHistory.Add(r);
	return r;
}

void CMusikPlayer::NextSong()
{
	switch ( m_Playmode )
	{
	case MUSIK_PLAYMODE_AUTO_DJ:

		_AddRandomSongs();
		
		// fall through
	case MUSIK_PLAYMODE_NORMAL:
		m_SongIndex++;
		if ( m_SongIndex >= m_Playlist.GetCount() )
		{
			Stop();
			m_SongIndex = 0;
		}
		else
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

	case MUSIK_PLAYMODE_SHUFFLE:
		Play( GetShuffledSong() );
		break;
	}
}

void CMusikPlayer::PrevSong()
{
	//---------------------------------------------------------//
	//--- 2  second grace period to go to previous track	---//
	//---------------------------------------------------------//
	if ( g_ActiveStreams.GetCount() && GetTime( FMOD_MSEC ) > 2000 )	
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
	
		case MUSIK_PLAYMODE_SHUFFLE:
		case MUSIK_PLAYMODE_AUTO_DJ:
			NextSong();
			break;
		}
	}
}

void CMusikPlayer::_AddRandomSongs()
{

	CMusikSongArray  arrSongs;
	int nSongsToAdd = (g_Prefs.nAutoDJChooseSongsToPlayInAdvance - (m_Playlist.GetCount() - m_SongIndex) + 1);
	if(nSongsToAdd <= 0)
		return;
	int nMaxRepeatCount = 30;
	bool repeat = true;
	int maxsongid = g_Library.QueryCount("select max(songid) from songs;");

	while ( nMaxRepeatCount-- && (repeat || (arrSongs.GetCount() < nSongsToAdd)))
	{
		repeat = false;
#if 0
		int r = GetRandomNumber() % (g_Library.GetSongCount());
		wxString sQueryRandomSong = wxString::Format(wxT(" select songs.songid from songs " 
					"where songs.songid not in(select songid from songhistory "
					"where date_played > julianday('now','-%d hours')) limit 1 offset %d;"),nMaxRepeatCount == 5 ? 1 : g_Prefs.nAutoDjDoNotPlaySongPlayedTheLastNHours,r);
	
#else
		int r = GetRandomNumber() % (maxsongid + 1);
	   wxString sQueryRandomSong = wxString::Format(wxT(" select songs.songid from songs " 
		   "where songs.songid = %d and songid not in(select songid from songhistory "
		   "where date_played > julianday('now','-%d days'));"),r,nMaxRepeatCount == 5 ? 1 : g_Prefs.nAutoDjDoNotPlaySongPlayedTheLastNHours);


#endif
		int songid = g_Library.QueryCount(ConvQueryToMB(sQueryRandomSong));
		if(songid > -1)
		{
			CMusikSong song;
			if(g_Library.GetSongFromSongid(songid,&song))
			{
					if(::wxFileExists(song.Filename))
					{
						//--- check for repeats ---//
						for ( size_t j = 0;  j < arrSongs.GetCount(); j++ )
						{
							if ( songid == arrSongs[j].songid )
							{
								repeat = true; 
								break;
							}
						}
						if(!repeat)
							arrSongs.Add(song);
					}
					else
					{
						repeat = true; 
					}
			}
		}
	}; 
	AddToPlaylist(arrSongs,false);
}
int CMusikPlayer::GetFilesize( wxString sFilename )
{
	int filesize = -1;
#if ( MUSIK_FMOD_VERSION >= 0x0370 )
	FSOUND_STREAM *pStream = FSOUND_Stream_Open( ( const char* )ConvFNToFieldMB( sFilename ), FSOUND_2D, 0, 0 );
#else
	FSOUND_STREAM* pStream = FSOUND_Stream_OpenFile( ( const char* )ConvFNToFieldMB( sFilename ), FSOUND_2D, 0);
#endif

	if ( pStream )
	{
		filesize = FSOUND_Stream_GetLength( pStream );
		FSOUND_Stream_Close( pStream );
	}
	
	return filesize;
}

void CMusikPlayer::SetVolume()
{
	FSOUND_SetSFXMasterVolume(g_Prefs.nSndVolume);
}

int CMusikPlayer::GetDuration( int nType )
{
	if(_CurrentSongIsNetStream())
	{
		return 100;
	}
	if(g_ActiveChannels.GetCount() == 0)
		return 0;
	int nDuration = FSOUND_Stream_GetLengthMs( g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ) );
	if ( nType == FMOD_SEC )
		nDuration /= 1000;

	return nDuration;
}

int CMusikPlayer::GetTime( int nType )
{
	if(_CurrentSongIsNetStream())
	{
		FSOUND_STREAM * p_NETSTREAM = NULL;
		if(g_ActiveStreams.GetCount() && g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ))
		{
			p_NETSTREAM = g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 );	
		}
		else if (m_p_NETSTREAM_Connecting)
		{
			return m_NETSTREAM_read_percent;
		}
		int status = 0;
		if(p_NETSTREAM )
		{
			m_NETSTREAM_last_read_percent =	 m_NETSTREAM_read_percent;
			status = _NetStreamStatusUpdate(p_NETSTREAM);
			int openstate = FSOUND_Stream_GetOpenState(p_NETSTREAM);
			if( openstate == 0) 
			{
				
				int off = FSOUND_Stream_GetTime(p_NETSTREAM);
				int len = FSOUND_Stream_GetLengthMs(p_NETSTREAM);
	              
				if((status == FSOUND_STREAM_NET_ERROR) || (off >= len))
				{// something is wrong, we try to restart the stream
					_PostPlayRestart();
				}
				else
					m_bStreamIsWorkingStopWatchIsRunning = false;// everything is okay
 			}
			else 
			{   // openstate is not okay
				if(m_bStreamIsWorkingStopWatchIsRunning 
						&& m_NETSTREAM_last_read_percent == m_NETSTREAM_read_percent
						&&	m_StreamIsWorkingStopWatch.Time() > 10 * 1000) // 10 sec
				{  // stream is not working ( read percent did not change and FSOUND_Stream_GetOpenState failed)for 10 secs now
				   m_bStreamIsWorkingStopWatchIsRunning = false;
				   _PostPlayRestart();
				}
				else
				{	// stream is disturbed   ,start watch ( if not already running)
					if(!m_bStreamIsWorkingStopWatchIsRunning)					
					{
						m_NETSTREAM_last_read_percent =	 m_NETSTREAM_read_percent;
						m_StreamIsWorkingStopWatch.Start();
						m_bStreamIsWorkingStopWatchIsRunning = true;
					}
				}
			}
		}
		return m_NETSTREAM_read_percent;
	}
	// no critical section here, because crossfader thread is stopped before g_ActiveStreams array is changed(add/remove)
	if(g_ActiveStreams.GetCount() == 0)
		m_nLastSongTime = 0;
	else 
		m_nLastSongTime = FSOUND_Stream_GetTime( g_ActiveStreams.Item( g_ActiveStreams.GetCount()-1 ));
	if ( nType == FMOD_SEC )
		 m_nLastSongTime /= 1000;
	return  m_nLastSongTime;
}

int CMusikPlayer::GetTimeLeft( int nType )
{
	wxCriticalSectionLocker locker( m_critInternalData); // lock because this method is called from fader thread , protect m_Playlist
	if(_CurrentSongIsNetStream())
	{
		return 10000000;
	}
	int nTimeLeft = GetDuration( FMOD_MSEC ) - GetTime( FMOD_MSEC ); 
	if ( nType == FMOD_SEC )
		nTimeLeft /= 1000;
	return nTimeLeft;
}

wxString CMusikPlayer::GetTimeStr()
{
	if(_CurrentSongIsNetStream())
	{
		wxString sProgress;
		// Notes field is (ab)used for (buffering, connecting ) message
		sProgress.sprintf( wxT( "%s (%d%%)"), (const wxChar *)m_CurrentSong.Notes, GetTime( FMOD_SEC ));
		return sProgress;
	}
	else
		return SecToStr( GetTime( FMOD_SEC ) );
}
wxString CMusikPlayer::GetTimeLeftStr()
{
	if(_CurrentSongIsNetStream())
	{
		wxString sProgress;
		// Notes field is (ab)used for (buffering, connecting ) message
		sProgress.sprintf( wxT( "%s (%d%%)"), (const wxChar *)m_CurrentSong.Notes, GetTime( FMOD_SEC ));
		return sProgress;
	}
	else
		return SecToStr( GetTimeLeft( FMOD_SEC ) );
}
void CMusikPlayer::SetTime( int nSec )
{
	if(_IsSeekCrossFadingDisabled())
	{
		FSOUND_Stream_SetTime( g_ActiveStreams.Item( g_ActiveStreams.GetCount() - 1 ), nSec * 1000 );
	}
	else
	{
		Play( m_SongIndex, nSec, CROSSFADE_SEEK );
	}
}

bool CMusikPlayer::_IsSeekCrossFadingDisabled()
{
	return ( g_Prefs.bGlobalFadeEnable == 0 || g_Prefs.bFadeSeekEnable == 0 || m_Paused 
		||  _CurrentSongNeedsMPEGACCURATE() // no seek crossfadeing, because _CurrentSongNeedsMPEGACCURATE files are slow in opening
		|| _CurrentSongIsNetStream());
}
bool CMusikPlayer::_CurrentSongNeedsMPEGACCURATE()
{
	return (g_Prefs.bUse_MPEGACCURATE_ForMP3VBRFiles
			&&(m_SongIndex <  m_Playlist.GetCount())
			&& m_CurrentSong.VBR 
			&& (m_CurrentSong.Format == MUSIK_FORMAT_MP3) ); 
	//  mp3 vbr files needs FSOUND_MPEGACCURATE flag, but takes much too long for other filetypes
}
bool CMusikPlayer::_CurrentSongIsNetStream()
{
	return ((m_SongIndex <  m_Playlist.GetCount())
			&& (m_CurrentSong.Format == MUSIK_FORMAT_NETSTREAM) ); 
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

int CMusikPlayer::GetFileDuration( wxString sFilename, int nType )
{
	int duration = -1;

	//-------------------------------------------------//
	//--- this should be FSOUND_MPEGACCURATE to get	---//
	//--- an accurate length, but it's way slower..	---//
	//-------------------------------------------------//
#if (MUSIK_FMOD_VERSION >= 0x0370)
	FSOUND_STREAM *pStream = FSOUND_Stream_Open( ( const char* )ConvFNToFieldMB( sFilename ), FSOUND_2D, 0, 0 );
#else
	FSOUND_STREAM* pStream = FSOUND_Stream_OpenFile( ( const char* )ConvFNToFieldMB( sFilename ), FSOUND_2D, 0);
#endif

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



int CMusikPlayer::GetCurrChannel()
{
	return m_Channels;
}

void CMusikPlayer::AddToPlaylist( CMusikSongArray & songstoadd ,bool bPlayFirstAdded )
{
	size_t size = songstoadd.GetCount();
	if(size)
	{
		wxCriticalSectionLocker locker( m_critInternalData);
		int plsize = m_Playlist.GetCount();
		for(size_t i = 0; i < size ; i++)
		{
			m_Playlist.Add(songstoadd.Detach(0));
		}
		if(bPlayFirstAdded)
		{
			Stop();
			m_SongIndex = plsize;
			Play(m_SongIndex);
		}
		if(g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING)
		{
			g_Playlist = m_Playlist;
			g_PlaylistBox->Update(false);
		}
	}
}
void CMusikPlayer::InsertToPlaylist( CMusikSongArray & songstoadd ,bool bPlayFirstInserted)
{
	wxCriticalSectionLocker locker( m_critInternalData);
	size_t size = songstoadd.GetCount();
	size_t plsize = m_Playlist.GetCount();
	if(plsize == 0 || !IsPlaying())
	{// list empty, add
		for(size_t i = 0; i < size ; i++)
			m_Playlist.Add(songstoadd.Detach(0));
		if(bPlayFirstInserted)
		{
			Stop();
			m_SongIndex = plsize;
			Play(m_SongIndex);
		}
		else
			m_SongIndex = plsize;
		return;

	}
// list not empty ,insert
	for(size_t i = 0; i < size ; i++)
	{
		m_Playlist.Insert(songstoadd.Detach(0), m_SongIndex + 1 + i );
	}
	if(bPlayFirstInserted)
	{
		Stop();
		m_SongIndex++;
		Play(m_SongIndex);
	}
}
void CMusikPlayer::RemovePlaylistEntry( size_t index )
{
	wxCriticalSectionLocker locker( m_critInternalData);
	wxASSERT(index < m_Playlist.GetCount()); 
	
	if(index < m_SongIndex)
	{
		m_SongIndex--;
	}
	else if(m_SongIndex == index)
	{
		Stop();
	}
	m_Playlist.RemoveAt(index);
}

void  CMusikPlayer::MovePlaylistEntrys(size_t nMoveTo ,const wxArrayInt &arrToMove)
{
	wxCriticalSectionLocker locker( m_critInternalData);
	wxASSERT(nMoveTo >= 0 && nMoveTo <= m_Playlist.GetCount()); 
	int i = arrToMove.GetCount() - 1;
	// first move all entrys which are behind nMoveTo;
	for(;i >= 0 ; i--)
	{

		if(nMoveTo > arrToMove[i])
			break;
		size_t ToMoveIdx = arrToMove[i] + ( arrToMove.GetCount() - 1 - i);
		m_Playlist.Insert(m_Playlist.Detach(ToMoveIdx),nMoveTo);

		if(ToMoveIdx > m_SongIndex && nMoveTo <= m_SongIndex)
			m_SongIndex++;
		else if(ToMoveIdx == m_SongIndex)
			m_SongIndex = nMoveTo;
	}
	// now move all entry which are before
	for(int j = i; j >= 0; j--)
	{
		size_t MoveToIdx = nMoveTo - (i - j) - 1;
		m_Playlist.Insert(m_Playlist.Detach(arrToMove[j]),MoveToIdx);
		if(arrToMove[j] < m_SongIndex && MoveToIdx  > m_SongIndex)
			m_SongIndex--;
		else if( m_SongIndex  == MoveToIdx)
			m_SongIndex++;
		else if(arrToMove[j] == m_SongIndex)
			m_SongIndex = MoveToIdx;

	}
}