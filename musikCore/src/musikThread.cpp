///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
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

#include "../include/musikConfig.h"
#include "../include/musikThread.h"

///////////////////////////////////////////////////

CmusikThread::CmusikThread()
{	
	m_ThreadID	= NULL;
	m_ThreadHND = NULL;

	m_Running	= false;
	m_Suspended = false;
	m_Joined	= false;
	m_Finished	= false;
	m_Abort		= false;
	m_Asleep	= false;
	
	m_Args		= NULL;
}

///////////////////////////////////////////////////

CmusikThread::~CmusikThread()
{
	Kill();
}

///////////////////////////////////////////////////

void CmusikThread::Start( ACE_THR_FUNC func, void* args, bool join, int type )
{
	m_Args = args;
	m_Type = type;

	// kill the old thread if it 
	// is running...
	Kill();

	// create the objects
	m_ThreadID = new ACE_thread_t();
	m_ThreadHND = new ACE_hthread_t();

	// spawn the main thread either joined...
	if ( join )
	{
		ACE_Thread::spawn( (ACE_THR_FUNC)func,
			this,
			THR_JOINABLE | THR_NEW_LWP,
			m_ThreadID,
			m_ThreadHND );

		#ifdef WIN32
			ACE_Thread::join( m_ThreadHND );
		#else
			ACE_Thread::join( 0, m_ThreadHND, 0 );
		#endif
	}

	// ... or unjoined to the main thread
	else
	{
		ACE_Thread::spawn( (ACE_THR_FUNC)func,
			this,
			THR_NEW_LWP,
			m_ThreadID,
			m_ThreadHND );
	}

	m_Joined = join;
	m_Running = true;
	m_Suspended = false;
	m_Finished = false;
}

///////////////////////////////////////////////////

void CmusikThread::Kill()
{
	if ( m_Running )
		Suspend();

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

///////////////////////////////////////////////////

void CmusikThread::Abort()
{
	m_Abort = true;
}

///////////////////////////////////////////////////

void CmusikThread::Suspend( bool wait )
{
	if ( m_Running && !m_Suspended )
	{
		m_Suspended = true;

		if ( wait )
		{
			while ( !m_Asleep )
				Sleep( 100 );
		}
	}
}

///////////////////////////////////////////////////

void CmusikThread::Resume()
{
	if ( m_Running & m_Suspended )
	{
		m_Suspended = false;
	}
}

///////////////////////////////////////////////////
