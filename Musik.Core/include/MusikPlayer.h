///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
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
//   CMusikPlayer
//
// Filename(s): 
//
//   MusikPlayer.h, MusikPlayer.cpp
//
// Information:
//
//   CMusikPlayer is a front end to FMOD for playing
//   files from a CMusikPlaylist, querying information
//   from it's CMusikLibrary
// 
// Example: 
//
//   A UI can use this class to control song playback easily
//
// Usage: 
//
//   Create the class and using the basic playback controls,
//   such as CMusikPlayer::Play(), CMusikPlayer::Next(), etc.
//
//   To make things easy, CMusikPlayer will manage it's own
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

#include "MusikPlaylist.h"

#include "MusikThread.h"
#include "MusikCrossfader.h"

#include "fmod.h"

///////////////////////////////////////////////////

enum
{
	// playmodes
	MUSIK_PLAYER_PLAYMODE_NORMAL = 0,
	MUSIK_PLAYER_PLAYMODE_RANDOM,
	MUSIK_PLAYER_PLAYMODE_SMART,
	MUSIK_PLAYER_PLAYMODE_LOOP
};

///////////////////////////////////////////////////

enum
{
	// initialization values
	MUSIK_PLAYER_INIT_START = 0,
	MUSIK_PLAYER_INIT_STOP,
	MUSIK_PLAYER_INIT_RESTART,

	// return values
	MUSIK_PLAYER_INIT_SUCCESS,
	MUSIK_PLAYER_INIT_ERROR
};

///////////////////////////////////////////////////

enum
{
	MUSIK_TIME_SECONDS = 0,
	MUSIK_TIME_MS
};

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlaylist;
class CMusikFunctor;
class CMusikPlayer;
class CMusikSongInfo;
class CMusikEqualizer;
class CMusikCrossfader;

///////////////////////////////////////////////////

static void MusikPlayerWorker( CMusikPlayer* player );

///////////////////////////////////////////////////

class CMusikPlayer	
{

	static void* F_CALLBACKAPI MusikEQCallback( void* originalbuffer, void *newbuffer, int length, int param );

public: 	

	// construct and destruct
	CMusikPlayer( CMusikFunctor* functor, CMusikLibrary* library );
	~CMusikPlayer();

	// sound system initialization and
	// deinitialization stuff
	int  InitSound( int device, int driver, int rate, int channels, int mode = MUSIK_PLAYER_INIT_START );
	void CleanSound();
	void CleanPlaylist();

	// crossfader stuff
	void InitCrossfader();
	void CleanCrossfader();
	int GetFadeType(){ return m_FadeType; }
	CMusikCrossfader* GetCrossfader(){ return m_Crossfader; }
	void FinishCrossfade();
	void SetCrossfader( CMusikCrossfader fader, bool force_init = true );

	// equalizer stuff
	void InitEQ_DSP();
	void CleanEQ_DSP();
	void EnableEQ( bool enable, bool force_init = true );

	// contains the information of the 
	// currently playing song
	CMusikSongInfo* GetCurrPlaying(){ return &m_CurrSong; }

	// sets various attributes
	void SetPlaylist( CMusikPlaylist* playlist );
	void SetLibrary( CMusikLibrary* library );

	// time
	int	GetTimeNow		( int mode );
	int	GetTimeRemain	( int mode );
	int	GetDuration		( int mode );

	// volume
	int GetMaxVolume(){ return m_Volume; }
	void SetMaxVolume( int vol, bool set_now = true );

	int GetVolume( int channel_id );
	void SetVolume( int channel_id, int vol );

	// play controls
	bool Play( int index = 0, int fade_type = -1, int start_pos = 0 );
	bool Next();
	bool Prev();
	bool Pause();
	bool Resume();
	void Stop();
	void Exit();

	// status
	bool IsPlaying()			{ return m_IsPlaying; }
	bool IsPaused()				{ return m_IsPaused; }
	bool IsShuttingDown()		{ return m_ShutDown; }
	bool IsEqualizerActive()	{ return m_IsEQActive; }
	bool IsCrossfaderActive();
	bool IsCrossfaderReady()	{ return m_IsCrossfaderReady; }

	// crossfade flags
	void FlagCrossfade()		{ m_IsCrossfaderReady = true; }
	void UnflagCrossfade()		{ m_IsCrossfaderReady = false; }

	// misc
	size_t GetStreamCount();
	int GetChannelID( int n );
	int GetHandle(){ return m_Handle; }
	int GetPlaymode(){ return m_PlayMode; }
	void SetPlaymode( int mode ){ m_PlayMode = mode; }
	void SetSafeShutdown(){ m_ShutDown = true; }
	CMusikPlaylist* GetPlaylist(){ return m_Playlist; }

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

	// start and stop sound. InitSound() will
	// relay to these helper functions
	void StopSound();
	int  StartSound( int device, int driver, int rate, int channels );

	// equalizer
	CMusikEqualizer* m_EQ;
	
	// crossfader
	CMusikCrossfader* m_Crossfader;
	int m_FadeType;

	// status flags
	bool m_IsPlaying;
	bool m_IsPaused;
	bool m_ShutDown;
	bool m_IsEQActive;
	bool m_IsCrossfaderReady;

	// play mode (repeat, random, etc)
	int m_PlayMode;

	// info on currently playing song
	CMusikSongInfo m_CurrSong;

	// a pointer to a functor that will be
	// used to post a next song event
	CMusikFunctor* m_Functor;

	// maximum number of open channels, and
	// little functions to help us manage
	// them...
	int m_MaxChannels;
	int m_CurrChannel;
	void PushNewChannel();
	int GetCurrChannel();

	// a pointer to the currently playing stream
	FSOUND_STREAM* GetCurrStream();

	// active streams and channels
	void CleanOldStreams( bool kill_primary = false );
	CMusikStreamPtrArray* m_ActiveStreams;
	CIntArray* m_ActiveChannels;

	// equalizer DSP unit
	FSOUND_DSPUNIT *m_EQ_DSP;

	// internal functions used to initialize
	// and free any equalizer stuff used
	void InitEqualizer();
	void CleanEqualizer();

	// main thread and mutex
	CMusikThread* m_pThread;
	void InitThread();
	void CleanThread();
	
	// pointer to library and playlist
	CMusikLibrary* m_Library;
	CMusikPlaylist* m_Playlist;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
