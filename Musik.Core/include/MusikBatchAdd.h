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
//   CMusikBatchAdd
//
// Filename(s): 
//
//   CMusikBatchAdd.h, CMusikBatchAdd.cpp
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

#include "../include/MusikPlaylist.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlaylist;
class CMusikFunctor;

class CMusikBatchAdd;

///////////////////////////////////////////////////

static void MusikBatchAddWorker( CMusikBatchAdd* params );

///////////////////////////////////////////////////

class CMusikBatchAdd
{
public: 

	// construct / destruct
	CMusikBatchAdd();
	CMusikBatchAdd( CStdStringArray* pFiles, CMusikPlaylist* pPlaylist, 
		CMusikLibrary* pLibrary, CMusikFunctor* pFunctor, bool bUpdatePlaylist = false, 
		bool bDeleteFilelist = true );
	~CMusikBatchAdd();

	// thread execution
	void Run();
	void Kill(){ m_Kill = true; }
	void Pause();
	void Resume();

	// we'll allow users to manually set
	// these variables.
	CStdStringArray* m_Files;
	CMusikPlaylist* m_Playlist;
	CMusikLibrary* m_Library;
	CMusikFunctor* m_Functor;
	bool m_UpdatePlaylist;
	bool m_DeleteFilelist;

	bool m_Kill;

	// user shouldn't call these, they should
	// be left to the thread...
	bool IsKilling(){ return m_Kill; }


private:

    CMusikThread* m_pThread;	
};

///////////////////////////////////////////////////