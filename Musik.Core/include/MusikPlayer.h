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
	MUSIK_PLAYER_INIT_START = 0,
	MUSIK_PLAYER_INIT_STOP,
	MUSIK_PLAYER_INIT_RESTART
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

	int  InitSound( int mode = MUSIK_PLAYER_INIT_START );

	void SetPlaylist( CMusikPlaylist* playlist );
	
	bool Play( int index = 0 );
	bool Next();
	bool Prev();
	bool Pause();
	bool Resume();

	bool IsPlaying();
	bool IsPaused();

private:
	bool m_IsPlaying;
	bool m_IsPaused;

	CMusikFunctor* m_Functor;

	CMusikStreamPtrArray* m_ActiveStreams;
	CIntArray* m_ActiveChannels;

	ACE_Thread_Mutex main_mutex;
	ACE_Thread* main_thread;

	CMusikLibrary* m_Library;
	CMusikPlaylist* m_Playlist;

	void* m_NewSong;
	void CallNewSongPtr();
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////