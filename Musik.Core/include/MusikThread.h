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
//   CMusikThread
//
// Filename(s): 
//
//   MusikThread.h, MusikThread.cpp
//
// Information:
//
//   A wrapper for ACE::Thread stuff, as well
//   as a simple STL based.
//
//   It is designed to make thread executation much
//   easier with very little overhead.
// 
// Usage: 
//
//   Used by the backend
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_THREAD
#define C_MUSIK_THREAD

///////////////////////////////////////////////////

#include "ace/Thread.h"
#include "ace/Synch.h"

///////////////////////////////////////////////////

class CMusikThread
{
public:
	
	// construct / destruct
	CMusikThread();
	CMusikThread( ACE_THR_FUNC func, void* args, bool join = true );
	~CMusikThread();

	// functions
	void Start( ACE_THR_FUNC func, void* args, bool join = true );
	void Pause();
	void Resume();
	void Kill();

	// attributes
	bool IsRunning(){ return m_Running; }
	bool IsPaused(){ return m_Paused; }
	bool IsJoined(){ return m_Joined; }

private:

	bool m_Running;
	bool m_Paused;
	bool m_Joined;

	ACE_thread_t* m_ThreadID;
	ACE_hthread_t* m_ThreadHND;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////