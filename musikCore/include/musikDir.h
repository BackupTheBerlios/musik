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

	// construct and destruct
	CmusikDir();
	CmusikDir( CStdString dir, CStdStringArray* target = NULL, CmusikFunctor* functor = NULL );
	~CmusikDir();

	// the few functions we have
	void SetTarget( CStdStringArray* target ){	m_Target = target;	}
	void Run();

	// values we need
	CStdStringArray* m_Target;
	CStdString m_Dir;
	CmusikFunctor* m_Functor;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////