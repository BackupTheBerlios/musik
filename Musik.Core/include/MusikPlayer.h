///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYER_H
#define C_MUSIK_PLAYER_H

///////////////////////////////////////////////////

#include "MusikPlaylist.h"

#include "../Musik.Core/include/MusikCrossfader.h"
#include "../Musik.Core/include/MusikEqualizer.h"

#include "ace/Thread.h"
#include "ace/Synch.h"

#include "fmod.h"

///////////////////////////////////////////////////

enum
{
	// playmodes
	MUSIK_PLAYER_PLAYMODE_NORMAL = 0,
	MUSIK_PLAYER_PLAYMODE_RANDOM,
	MUSIK_PLAYER_PLAYMODE_SMART,
	MUSIK_PLAYER_PLAYMODE_LOOP,
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

class CMusikLibrary;
class CMusikPlaylist;
class CMusikFunctor;
class CMusikPlayer;
class CMusikSongInfo;

///////////////////////////////////////////////////

static void PlayerWorker( CMusikPlayer* player );

///////////////////////////////////////////////////

class CMusikPlayer	
{
public: 	
	CMusikPlayer( CMusikFunctor* functor );
	~CMusikPlayer();

	int  InitSound( int device, int driver, int rate, int channels, int mode = MUSIK_PLAYER_INIT_START );
	void CleanSound();

	void SetPlaylist( CMusikPlaylist* playlist );
	
	bool Play( int index = 0, int play_type = 0, int start_pos = 0 );
	bool Next();
	bool Prev();
	bool Pause();
	bool Resume();

	bool IsPlaying()		{ return m_IsPlaying; }
	bool IsPaused()			{ return m_IsPaused; }
	bool IsShuttingDown()	{ return m_ShutDown; }

private:
	// start and stop sound. InitSound() will
	// relay to these helper functions
	void StopSound();
	int  StartSound( int device, int driver, int rate, int channels );

	// status flags
	bool m_IsPlaying;
	bool m_IsPaused;
	bool m_ShutDown;

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

	// active streams and channels
	CMusikStreamPtrArray* m_ActiveStreams;
	CIntArray* m_ActiveChannels;

	// main thread and mutex
	void InitThread();
	void CleanThread();
	
	ACE_Thread_Mutex* m_Mutex;
	ACE_thread_t* m_ThreadID;
	ACE_hthread_t* m_ThreadHND;

	// pointer to library and playlist
	CMusikLibrary* m_Library;
	CMusikPlaylist* m_Playlist;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////