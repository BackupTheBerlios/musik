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
//   CmusikThread
//
// Filename(s): 
//
//   musikThread.h, musikThread.cpp
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

class CmusikThread
{
public:
	
	// construct / destruct
	CmusikThread();
	~CmusikThread();

	// functions
	void Start( ACE_THR_FUNC func, void* args, bool join = true );
	void Suspend( bool wait = false );
	void Resume();
	void Abort();
	void Kill();

	void* GetArgs(){ return m_Args; }

	// attributes
	bool IsRunning(){ return m_Running; }
	bool IsSuspended(){ return m_Suspended; }
	bool IsJoined(){ return m_Joined; }

	// this tells the thread to break whatever
	// it is doing and cleanup. 
	bool m_Abort;

	// should not be set except by a worker
	// function!
	bool m_Finished;

	// the worker thread should exclusivly set
	// this one
	bool m_Asleep;

private:

	bool m_Running;
	bool m_Suspended;
	bool m_Joined;

	ACE_thread_t* m_ThreadID;
	ACE_hthread_t* m_ThreadHND;

	void* m_Args;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

