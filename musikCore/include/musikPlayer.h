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
//   To make things easy, CmusikPlayer will manage it's own
//   playlist. Once it takes control of the playlist, via
//   SetPlaylist(), it will take on freeing, creating, and
//   maintaining it... this includes deleting it. Once a user
//   has passed the playlist to the player, he should never, ever
//   delete it manually, unless he calls CleanPlaylist().
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYER_H
#define C_MUSIK_PLAYER_H

///////////////////////////////////////////////////

#include "musikPlaylist.h"

#include "musikThread.h"
#include "musikCrossfader.h"

#include "fmod.h"

///////////////////////////////////////////////////

enum
{
	// playmodes
	MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE = 1,
	MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST = 2,
	MUSIK_PLAYER_PLAYMODE_INTRO = 4,
};

///////////////////////////////////////////////////

enum
{
	// init states
	MUSIK_PLAYER_INIT_ERROR = -1,
	MUSIK_PLAYER_INIT_UNINITIALIZED = 0,
	MUSIK_PLAYER_INIT_SUCCESS,

	// initialization values
	MUSIK_PLAYER_INIT_START,
	MUSIK_PLAYER_INIT_STOP,
	MUSIK_PLAYER_INIT_RESTART
};

///////////////////////////////////////////////////

enum
{
	MUSIK_TIME_SECONDS = 0,
	MUSIK_TIME_MS
};

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlaylist;
class CmusikFunctor;
class CmusikPlayer;
class CmusikSongInfo;
class CmusikEqualizer;
class CmusikCrossfader;

///////////////////////////////////////////////////

static void musikPlayerWorker( CmusikThread* player );

///////////////////////////////////////////////////

class CmusikPlayer	
{

	static void* F_CALLBACKAPI musikEQCallback( void* originalbuffer, void *newbuffer, int length, int param );

public: 	

	// construct and destruct
	CmusikPlayer( CmusikFunctor* functor, CmusikLibrary* library );
	~CmusikPlayer();

	// sound system initialization and
	// deinitialization stuff
	int  InitSound( int device, int driver, int rate, int channels, int mode = MUSIK_PLAYER_INIT_START );
	bool IsSoundInitialized(){ return ( m_State == MUSIK_PLAYER_INIT_SUCCESS ? true : false ); }
	int  GetInitState(){ return m_State; }
	void CleanSound();
	void CleanPlaylist();

	// crossfader stuff
	int GetFadeType(){ return m_FadeType; }
	CmusikCrossfader* GetCrossfader(){ return m_Crossfader; }
	bool SetCrossfader( const CmusikCrossfader& fader );
	void EnableCrossfader( bool enabled = true );

	// equalizer stuff
	CmusikEqualizer* GetEqualizer(){ return m_EQ; }
	void EnableEqualizer( bool enable );

	// contains the information of the 
	// currently playing song
	CmusikSongInfo* GetCurrPlaying(){ return &m_CurrSong; }

	// sets various attributes
	void SetPlaylist( CmusikPlaylist* playlist );
	void SetLibrary( CmusikLibrary* library );

	// item that is being played
	int GetIndex(){ return m_Index; }

	// time
	int	GetTimeNow		( int mode );
	int GetTimeNowPer	();
	int	GetTimeRemain	( int mode );
	int	GetDuration		( int mode );

	// volume
	int GetMaxVolume(){ return m_Volume; }
	void SetMaxVolume( int vol, bool set_now = true );

	int GetVolume( int channel_id );
	void SetVolume( int channel_id, int vol );

	// play controls
	bool Play( int index = 0, int fade_type = -1, int start_pos = 0 );
	bool Next( bool expired = false );
	bool Prev();
	bool Pause();
	bool Resume();
	void Stop();
	void Exit();

	// used for enquing the next / prev song if
	// playback is currently paused
	void EnquePaused( int index );

	// only the thread should touch these
	void FinalizePause();
	void FinalizeResume();
	void FinalizeNewSong();
	void FinalizeStop();
	void FinalizeExit();
		
	// status
	bool IsPlaying()			{ return m_IsPlaying; }
	bool IsPaused()				{ return m_IsPaused; }
	bool IsShuttingDown()		{ return m_ShutDown; }
	bool IsEqualizerActive()	{ return m_IsEQActive; }
	bool IsEqualizerEnabled()	{ return ( m_EQ == NULL ? false : true ); }
	bool IsCrossfaderEnabled();
	bool IsCrossfaderReady()	{ return m_IsCrossfaderReady; }

	// crossfade flags
	void FlagCrossfade()		{ m_IsCrossfaderReady = true; }
	void UnflagCrossfade()		{ m_IsCrossfaderReady = false; }

	// getting streams, channels, and handles
	size_t GetStreamCount();
	int GetChannelID( int n );
	int GetHandle(){ return m_Handle; }
	int GetCurrChannel();

	// play mode
	unsigned long GetPlaymode(){ return m_Playmode; }
	void SetPlaymode( unsigned long mode ){ m_Playmode = mode; }
	unsigned long m_Playmode;

	// misc
	void SetSafeShutdown(){ m_ShutDown = true; }
	CmusikPlaylist* GetPlaylist(){ return m_Playlist; }
	CStdString GetTimeStr( int time_ms );
	void SetTimeNowPer( int percent );
	CStdString GetTimePerStr( int percent );
	int GetTimePer ( int percent );

private:

	// index of the current song
	int m_Index;

	// volume
	int m_Volume;

	// song's handle.. basically it just
	// gets incremented every time a new song
	// is played, so the crossfader can
	// tell if it should abort what its currently
	// doing
	int m_Handle;

	// state of the player
	int m_State;

	// start and stop sound. InitSound() will
	// relay to these helper functions
	void StopSound();
	int  StartSound( int device, int driver, int rate, int channels );

	// status flags
	bool m_IsPlaying;
	bool m_IsPaused;
	bool m_ShutDown;
	bool m_IsEQActive;
	bool m_IsCrossfaderReady;

	// info on currently playing song
	CmusikSongInfo m_CurrSong;

	// a pointer to a functor that will be
	// used to post a next song event
	CmusikFunctor* m_Functor;

	// maximum number of open channels, and
	// little functions to help us manage
	// them...
	int m_MaxChannels;
	int m_CurrChannel;
	void PushNewChannel();

	// a pointer to the currently playing stream
	FSOUND_STREAM* GetCurrStream();

	// active streams and channels
	void CleanOldStreams( bool kill_primary = false );
	CmusikStreamPtrArray* m_ActiveStreams;
	CIntArray* m_ActiveChannels;

	// equalizer DSP unit
	FSOUND_DSPUNIT *m_EQ_DSP;

	// internal functions used to initialize
	// and free any equalizer stuff used
	CmusikEqualizer* m_EQ;

	void InitEqualizer();
	void CleanEqualizer();

	void InitEQ_DSP();
	void CleanEQ_DSP();

	// internal functions to maintain the
	// current set of crossfader prefs...
	CmusikCrossfader* m_Crossfader;
	
	int m_FadeType;
	bool m_CrossfaderEnabled;

	void InitCrossfader();
	void CleanCrossfader();

	// main thread and mutex
	CmusikThread* m_pThread;
	void InitThread();
	void CleanThread();
	
	// pointer to library and playlist
	CmusikLibrary* m_Library;
	CmusikPlaylist* m_Playlist;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
