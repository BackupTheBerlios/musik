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
//   CmusikBatchAdd
//
// Filename(s): 
//
//   CmusikBatchAdd.h, CmusikBatchAdd.cpp
//
// Information:
//
//   A thread to add files to a given library. 
//
// Usage: 
//
//   Create the class and call Run(). Upon completion,
//   the functor's OnThreadComplete() function will be
//   called.
//
///////////////////////////////////////////////////

#include "../include/musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlaylist;
class CmusikFunctor;
class CmusikPlayer;
class CmusikBatchAdd;

///////////////////////////////////////////////////

static void musikBatchAddWorker( CmusikBatchAdd* params );

///////////////////////////////////////////////////

class CmusikBatchAdd
{
public: 

	// construct / destruct
	CmusikBatchAdd();
	CmusikBatchAdd( CStdStringArray* pFiles, CmusikPlaylist* pPlaylist, CmusikLibrary* pLibrary, CmusikPlayer* pPlayer, 
		CmusikFunctor* pFunctor, bool bUpdatePlaylist = false, bool bAddToPlayer = false, bool bDeleteFilelist = true );

	~CmusikBatchAdd();

	// thread execution
	void Run();
	void Kill(){ m_Kill = true; }
	void Pause();
	void Resume();

	// we'll allow users to manually set
	// these variables.
	CStdStringArray* m_Files;
	CmusikPlaylist* m_Playlist;
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikFunctor* m_Functor;
	bool m_UpdatePlaylist;
	bool m_DeleteFilelist;
	bool m_AddToPlayer;				// negates UpdatePlaylist() and m_Playlist()... will add files to player's playlist
	bool m_Kill;

	// user shouldn't call these, they should
	// be left to the thread...
	bool IsKilling(){ return m_Kill; }


private:

    CmusikThread* m_pThread;	
};

///////////////////////////////////////////////////