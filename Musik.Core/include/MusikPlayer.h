///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYER_H
#define C_MUSIK_PLAYER_H

///////////////////////////////////////////////////

#include "ace/Thread.h"
#include "ace/Synch.h"

#include "MusikPlaylist.h"
#include "fmod.h"

///////////////////////////////////////////////////

enum
{
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

class CMusikLibrary;
class CMusikPlayerMain;
class CMusikPlaylist;
class CMusikFunctor;

///////////////////////////////////////////////////

class CMusikPlayer	
{
public: 	
	CMusikPlayer( CMusikFunctor* functor );
	~CMusikPlayer();

	int  InitSound( int device, int driver, int rate, int channels, int mode = MUSIK_PLAYER_INIT_START );

	void SetPlaylist( CMusikPlaylist* playlist );
	
	bool Play( int index = 0 );
	bool Next();
	bool Prev();
	bool Pause();
	bool Resume();

	bool IsPlaying();
	bool IsPaused();

private:
	// start and stop sound. InitSound() will
	// relay to these helper functions
	void StopSound();
	int  StartSound( int device, int driver, int rate, int channels );

	// status flags
	bool m_IsPlaying;
	bool m_IsPaused;

	// a pointer to a functor that will be
	// used to post a next song event
	CMusikFunctor* m_Functor;

	// active streams and channels
	CMusikStreamPtrArray* m_ActiveStreams;
	CIntArray* m_ActiveChannels;

	// main thread and mutex
	ACE_Thread_Mutex main_mutex;
	ACE_Thread* main_thread;

	// pointer to library and playlist
	CMusikLibrary* m_Library;
	CMusikPlaylist* m_Playlist;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////