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

#include "stdafx.h"

#include "../include/musikThread.h"

///////////////////////////////////////////////////

CmusikThread::CmusikThread()
{	
	m_ThreadID = NULL;
	m_ThreadHND = NULL;

	m_Running = false;
	m_Paused = false;
	m_Joined = false;
}

///////////////////////////////////////////////////

CmusikThread::CmusikThread( ACE_THR_FUNC func, void* args, bool join )
{
	m_Running = false;
	m_Paused = false;
	m_Joined = false;

	Start( func, args, join );
}

///////////////////////////////////////////////////

CmusikThread::~CmusikThread()
{
	Kill();
}

///////////////////////////////////////////////////

void CmusikThread::Start( ACE_THR_FUNC func, void* args, bool join )
{
	// kill the old thread if it 
	// is running...
	Kill();

	// create the objects
	m_ThreadID = new ACE_thread_t();
	m_ThreadHND = new ACE_hthread_t();

	// spawn the thread function
	ACE_Thread::spawn( (ACE_THR_FUNC)func,
		args,
		THR_JOINABLE | THR_NEW_LWP,
		m_ThreadID,
		m_ThreadHND );

	// join it to the main thread
	if ( join )
	{
		#ifdef WIN32
			ACE_Thread::join( m_ThreadHND );
		#else
			ACE_Thread::join( 0, m_ThreadHND, 0 );
		#endif
	}
	else

	m_Joined = join;
	m_Running = true;
	m_Paused = false;
}

///////////////////////////////////////////////////

void CmusikThread::Kill()
{
	if ( m_Running )
	{
		Pause();

		// delete 'em
		if ( m_ThreadID )
		{
			delete m_ThreadID;
			m_ThreadID = NULL;
		}
		if ( m_ThreadHND ) 
		{
			delete m_ThreadHND;
			m_ThreadHND = NULL;
		}
	}
}

///////////////////////////////////////////////////

void CmusikThread::Pause()
{
	if ( m_Running && !m_Paused )
	{
		ACE_Thread::suspend( *m_ThreadHND );
		m_Paused = true;
	}
}

///////////////////////////////////////////////////

void CmusikThread::Resume()
{
	if ( m_Paused )
	{
		ACE_Thread::resume( *m_ThreadHND );
		m_Paused = false;
	}
}

///////////////////////////////////////////////////
