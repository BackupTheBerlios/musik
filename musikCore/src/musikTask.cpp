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
//   CmusikTask
//
// Filename(s): 
//
//   musikTask.cpp
//
// Information:
//
//   A wrapper for ACE_Task stuff. A task is basically a
//   thread that acts as an object. See ACE documentation
//   and CmusikBatchAddTask, CmusikBatchRetagTask, and
//   CmusikRemoveOldTask for more information.
//
//   It is designed to make ACE thread / thread executation much
//   more like the musik API.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikTask.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

CmusikTask::CmusikTask()
	: ACE_Task<ACE_MT_SYNCH>()
{
	m_Stop = true;
	m_Finished = true;
	m_Active = false;
}

///////////////////////////////////////////////////

int CmusikTask::close( u_long flags )
{
	return NULL;
}

///////////////////////////////////////////////////

void CmusikTask::StopWait( int timeout_secs )
{
	if ( m_Active )
	{
		m_Stop = true;

		// wait for 20 seconds. if the thread
		// has not shut down in 20 seconds, something
		// bad happened. a string is logged and the
		// deletion continues non-gracefully
		int timeout = 0;
		while ( !m_Finished )
		{
			timeout++;
			Sleep( 500 );

			if ( timeout == ( timeout_secs * 2 ) )
			{
				TRACE0( "CmusikRemoveOldTask: I appear to be deadlocked. Terminating\n" );
				m_Finished = true;
			}
		}

		m_Active = false;
	}
}

///////////////////////////////////////////////////
