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
//   musikTask.h
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

#ifndef C_MUSIK_TASK
#define C_MUSIK_TASK

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "ace/Task.h"

///////////////////////////////////////////////////

enum
{
	MUSIK_TASK_TYPE_UNKNOWN = -1,
	MUSIK_TASK_TYPE_BATCHADD = 0,
	MUSIK_TASK_TYPE_BATCHRETAG,
	MUSIK_TASK_TYPE_REMOVEOLD,
	MUSIK_TASK_TYPE_PLAYER_WORKER,
	MUSIK_TASK_TYPE_LAST
};

///////////////////////////////////////////////////

class CmusikTask : public ACE_Task<ACE_MT_SYNCH>
{

public:

	CmusikTask();

	int GetType(){ return m_Type; }
	void StopWait( int timeout_secs );
	int close( u_long flags = 0 );

protected:

	bool m_Active;
	bool m_Finished;
	bool m_Stop;

	int m_Type;
};

typedef std::vector<CmusikTask*> CmusikTaskPtrArray;

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
