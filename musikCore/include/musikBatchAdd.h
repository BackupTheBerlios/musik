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
//   CmusikBatchAdd
//
// Filename(s): 
//
//   CmusikBatchAdd.h
//
// Information:
//
//   A worker thread function and class to batch add
//   files or directories to the library...
//
// Usage: 
//
//   Create the new CmusikBatchAddTask and a CmusikBatchAdd
//   pass the CmusikBatchAdd to the task's argument.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_BATCHADD_H
#define C_MUSIK_BATCHADD_H

///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikPlayer.h"
#include "musikFunctor.h"
#include "musikTask.h"

///////////////////////////////////////////////////

namespace musik {

///////////////////////////////////////////////////

class CmusikBatchAdd
{
public: 

	// construct / destruct
	CmusikBatchAdd();
	CmusikBatchAdd( CmusikStringArray* pFiles, CmusikPlaylist* pPlaylist, CmusikLibrary* pLibrary, CmusikPlayer* pPlayer, 
		CmusikFunctor* pFunctor, bool bUpdatePlaylist = false, bool bAddToPlayer = false, bool bDeleteFilelist = true );

	~CmusikBatchAdd(){}

	// we'll allow users to manually set
	// these variables.
	CmusikStringArray* m_Files;
	CmusikPlaylist* m_Playlist;
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikFunctor* m_Functor;
	bool m_UpdatePlaylist;
	bool m_DeleteFilelist;
	bool m_AddToPlayer;				// negates UpdatePlaylist() and m_Playlist()... will add files to player's playlist
	bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

class CmusikBatchAddTask : public CmusikTask
{

public:

	CmusikBatchAddTask();
	~CmusikBatchAddTask();

	int open( void* params );
	int close( u_long flags );
	int svc();

private:

	CmusikBatchAdd* m_Params;

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////