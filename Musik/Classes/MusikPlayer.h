/*
 *  MusikPlayer.h
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

#ifndef MUSIK_PLAYER_H
#define MUSIK_PLAYER_H

//--- wx stuff ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include <wx/dynarray.h>

enum EMUSIK_CROSSFADER_TYPE
{
	CROSSFADE_ERROR = 0,
	CROSSFADE_NORMAL,
	CROSSFADE_PAUSE,
	CROSSFADE_RESUME,
	CROSSFADE_STOP,
	CROSSFADE_EXIT,
	CROSSFADE_SEEK
};

//--- fmod ---//
#include "fmod.h"

//--- CMusikSongArray defined here ---//
#include "MusikLibrary.h"

//--- CMusikStreamArray declaration ---//
WX_DECLARE_OBJARRAY( FSOUND_STREAM*, CMusikStreamArray );

#define MAXCHANNELS 32

enum EMUSIK_PLAYMODE
{
	MUSIK_PLAYMODE_NORMAL = 0,
	MUSIK_PLAYMODE_LOOPSONG,
	MUSIK_PLAYMODE_LOOPLIST,
	MUSIK_PLAYMODE_RANDOM
};

enum EFMOD_INIT
{
	FMOD_INIT_STOP = 0,
	FMOD_INIT_START,
	FMOD_INIT_RESTART,
	FMOD_INIT_ERROR_DSOUND,
	FMOD_INIT_ERROR_WINDOWS_WAVEFORM,
	FMOD_INIT_ERROR_ASIO,
	FMOD_INIT_ERROR_DEVICE_NOT_READY,
	FMOD_INIT_ERROR_INIT,
	FMOD_INIT_ERROR_OSS,
	FMOD_INIT_ERROR_ESD,
	FMOD_INIT_ERROR_ALSA,
	FMOD_INIT_SUCCESS
};

enum EFMOD_MISC
{
	FMOD_SEC = 0,
	FMOD_MSEC
};

class CMusikPlayer : public wxEvtHandler
{
public:
	CMusikPlayer();
	~CMusikPlayer();

	//--------------------//
	//--- sound system ---//
	//--------------------//
	int	 InitializeFMOD		( int nFunction, int nSndOutput, int nSndDevice, int nSndRate );
	void Shutdown			( bool bClose = true );
	void ClearOldStreams	( bool bClearAll = false );
	void SetFrequency		( );
	void InitDSP			( );
	void ActivateDSP		( );
	void FreeDSP			( );

	//------------------------------//
	//--- events threads may use ---//
	//------------------------------//
	void OnNextSongEvt		( wxCommandEvent& WXUNUSED(event) ){ NextSong(); SetStartingNext( false );	}
	void OnFadeCompleteEvt	( wxCommandEvent& WXUNUSED(event) ){ SetFadeComplete();						}
	void OnPlayerStop		( wxCommandEvent& WXUNUSED(event) ){ FinalizeStop();						}
	void OnPlayerPause		( wxCommandEvent& WXUNUSED(event) ){ FinalizePause();						}
	void OnPlayerResume		( wxCommandEvent& WXUNUSED(event) ){ FinalizeResume();						}

	//-------------------------------------//
	//--- crossfader thread calls		---//
	//--- these functions to clean up	---//
	//-------------------------------------//
	void FinalizePause		( );
	void FinalizeResume		( );
	void FinalizeStop		( );

	//----------------//
	//--- booleans ---//
	//----------------//
	bool IsPlaying			( ) { return ( m_Playing );		}
	bool IsPaused			( ) { return ( m_Paused );		}
	bool IsFading			( ) { return ( m_Fading );		}
	bool IsStartingNext		( ) { return ( m_StartingNext );}
	bool IsStopping			( ) { return ( m_Stopping );	}
	bool BeginFade			( )	{ return ( m_BeginFade );	}
	void CaughtBeginFade	( )	{ m_BeginFade = false;		}

	//-------------------------//
	//--- playback controls ---//
	//-------------------------//
	void PlayCurSel	( );
    bool Play		( size_t nItem, int nStartPos = 0, int nFadeType = CROSSFADE_NORMAL );
	void Stop		( bool bFade = true, bool bExit = false );
	void NextSong	( );		
	void PrevSong	( );		
	void Pause		( bool bFade = true );
	void Resume		( bool bFade = true );

	//------------//
	//--- gets ---//
	//------------//
	size_t	GetCurIndex			( ) { return ( m_SongIndex ); }
	int	 GetDuration			( int nType );
	int	 GetFileDuration		( wxString sFilename, int nType );
	int  GetFilesize			( wxString sFilename );
	int	 GetLastTime			( int nType );
	int	 GetTime				( int nType );
	int	 GetTimeLeft			( int nType );
	int	 GetCurrChannel			( );
	void IncNextChannel			( );
	wxString GetTimeStr			( );
	wxString GetCurrentFile		() { return m_CurrentFile; }
	int GetCrossfadeType		() { return m_CrossfadeType; }
	size_t GetRandomSong		();

	//------------//
	//--- sets ---//
	//------------//
	void SetFadeComplete	( );
	void SetFadeBegin		( ){ m_Fading = true; }
	void SetFadeStart		( );
	void SetPlaymode		( );
	void SetVolume			( );
	void SetTime			( int nSec );
	void SetPlaylist		( CMusikSongArray playlist ) { m_Playlist = playlist; }
	void SetStartingNext	( bool bStart = true ){ m_StartingNext = bStart; }
	void SetCrossfadeType	( int nType ){ m_CrossfadeType = nType; }

	//------------//
	//--- misc ---//
	//------------//
	void UpdateUI		( );
	wxString SecToStr	( int nSec );
	
	DECLARE_EVENT_TABLE()
	
private:
	
	CMusikSongArray m_Playlist;			//--- heart and soul.								---//
	int				m_Playmode;			//--- repeat, shuffle, etc							---//
	size_t			m_SongIndex;		//--- current index in playlist						---//
	size_t			m_LastSong;			//--- last song played, used for updating			---//
	bool			m_Playing;			//--- currently playing?							---//
	bool			m_Paused;			//--- currently paused?								---//
	bool			m_BeginFade;		//--- should we begin fading?						---//
	bool			m_Fading;			//--- currently (cross)fading?						---//
	bool			m_StartingNext;		//--- in the middle of starting up a next song?		---//
	bool			m_Stopping;			//--- is the player currently stopping?				---//
	wxString		m_CurrentFile;		//--- filename of current song						---//
	int				m_CrossfadeType;	

	int				m_RandomSeed;		//--- random seed for consistent results			---//
	size_t			m_RandomIndex;		//--- index into the stream of random numbers		---//
	size_t			m_History[5];		//--- history of songs played, to avoid repeats		---//

	FSOUND_DSPUNIT	*m_DSP;

	int m_Channels;
};

#endif
