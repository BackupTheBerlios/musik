///////////////////////////////////////////////////

#ifndef C_MUSIK_DIR_H
#define C_MUSIK_DIR_H

///////////////////////////////////////////////////

#include "MusikArrays.h"
#include "MusikThread.h"
#include "MusikFunctor.h"

#ifndef WIN32
#include <dirent.h>
#endif

///////////////////////////////////////////////////

class CMusikDir;

///////////////////////////////////////////////////

static void MusikDirAddWorker( CMusikDir* params );
void OpenDir( CStdString dir, CStdStringArray* target = NULL, CMusikFunctor* functor = NULL, bool reset = false );

///////////////////////////////////////////////////

class CMusikDir
{
public:

	CMusikDir();
	CMusikDir( CStdString dir, CStdStringArray* target = NULL, CMusikFunctor* functor = NULL, bool threaded = false, bool start = false );
	~CMusikDir();

	void SetTarget( CStdStringArray* target ){	m_Target = target;	}

	// thread execution
	void Run();
	void Kill();
	void Pause();
	void Resume();

	// values we need
	CStdStringArray* m_Target;
	CStdString m_Dir;
	CMusikFunctor* m_Functor;
	bool m_Threaded;

	void Start();

protected:

	// thread it will run on
	CMusikThread* m_pThread;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////