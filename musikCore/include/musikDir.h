///////////////////////////////////////////////////

#ifndef C_MUSIK_DIR_H
#define C_MUSIK_DIR_H

///////////////////////////////////////////////////

#include "musikArrays.h"
#include "musikThread.h"
#include "musikFunctor.h"

#ifndef WIN32
#include <dirent.h>
#endif

///////////////////////////////////////////////////

class CmusikDir;

///////////////////////////////////////////////////

static void musikDirAddWorker( CmusikDir* params );
void OpenDir( CStdString dir, CStdStringArray* target = NULL, CmusikFunctor* functor = NULL, bool reset = false );

///////////////////////////////////////////////////

class CmusikDir
{
public:

	CmusikDir();
	CmusikDir( CStdString dir, CStdStringArray* target = NULL, CmusikFunctor* functor = NULL, bool threaded = false, bool start = false );
	~CmusikDir();

	void SetTarget( CStdStringArray* target ){	m_Target = target;	}

	// thread execution
	void Run();
	void Kill();
	void Pause();
	void Resume();

	// values we need
	CStdStringArray* m_Target;
	CStdString m_Dir;
	CmusikFunctor* m_Functor;
	bool m_Threaded;

protected:

	// thread it will run on
	CmusikThread* m_pThread;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////