///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/MusikDir.h"

///////////////////////////////////////////////////

static void MusikDirAddWorker( CMusikDir* params )
{
	if ( params->m_Functor )
		params->m_Functor->OnThreadStart();

	OpenDir( params->m_Dir, params->m_Target, params->m_Functor, true );

	if ( params->m_Functor )
		params->m_Functor->OnThreadEnd();
}

///////////////////////////////////////////////////

CMusikDir::CMusikDir( CStdString dir, CStdStringArray* target, CMusikFunctor* functor, bool threaded, bool start )
{
	m_Target = target;
	m_Dir = dir;
	m_Functor = functor;
	m_Threaded = threaded;

	if ( dir && start )
		Run();
}

///////////////////////////////////////////////////

CMusikDir::~CMusikDir()
{
	if ( m_pThread )
	{
		m_pThread->Kill();
		delete m_pThread;
	}
}


///////////////////////////////////////////////////

// avoid multiple constructor calls
// during file find recursion...
CStdStringArray* g_Target;
CMusikFunctor* g_Functor;
int g_Count;
int g_Found;

///////////////////////////////////////////////////

#ifdef WIN32

void OpenDir( CStdString dir, CStdStringArray* target, CMusikFunctor* functor, bool reset )
{
	if ( target )
		g_Target = target;
	if ( functor )
		g_Functor = functor;
	if ( reset )
	{
		g_Count = 0;
		g_Found = 0;
	}

	// find the first file in the directory...
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hDir = FindFirstFile( dir.c_str(), &lpFindFileData );

	// assure the first file exists...
	if ( hDir != INVALID_HANDLE_VALUE )
	{
		CStdString fn, temp;
		while ( true )
		{
			fn = lpFindFileData.cFileName;

			if ( !fn.IsEmpty() && fn != "." && fn != ".." )
			{
				// file was a directory
				if ( lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					fn += CStdString( "\\*.*" );
					OpenDir( fn );
				}

				// file was a file
				else
				{
					temp = fn.Right( 3 );
					temp.MakeLower();

					if ( temp == "ogg" || temp == "mp3" )
					{
						target->push_back( fn );
						g_Found++;
					}

					if ( g_Functor )
					{
						g_Count++;
						g_Functor->OnThreadNewFile( g_Found, g_Count );
					}	
				}
			}

			// find next
			if ( !FindNextFile( hDir, &lpFindFileData ) )
				break;
		}
	}
	FindClose( hDir );
}

#else

void OpenDir( CStdString dir, CStdStringArray* target, CMusikFunctor* functor, bool reset )
{
	if ( target )
		g_Target = target;
	if ( functor )
		g_Functor = functor;

	DIR *pDir;
	struct dirent *pEntry;

	if ( pDir == opendir( dir.c_str() ) )
	{
		while( entry = readdir( pDir ) )
		{
			temp = (CStdString)pEntry->d_name;
			temp = temp.Right( 3 );
			temp.MakeLower();

			if ( temp.c_str() == "ogg" || temp.c_str() == "mp3" )
				target->push_back( (CStdString)pEntry->d_name );
		}
		closedir( pDir );
	}
}

#endif

///////////////////////////////////////////////////

void CMusikDir::Run()
{
	if ( m_Threaded && !m_pThread )
	{
		if ( m_Target && !m_Dir.IsEmpty() && m_Functor )
		{
			m_pThread = new CMusikThread();
			m_pThread->Start( (ACE_THR_FUNC)MusikDirAddWorker, this );
			return;
		}
	}
	else
		OpenDir( m_Dir, m_Target, m_Functor, true );

	TRACE0( "CMusikDir::Run() failed\n" );
}

///////////////////////////////////////////////////

void CMusikDir::Pause()
{
	if ( m_pThread )
		m_pThread->Pause();
}

///////////////////////////////////////////////////

void CMusikDir::Resume()
{
	if ( m_pThread )
		m_pThread->Resume();
}

///////////////////////////////////////////////////

void CMusikDir::Kill()
{
	if ( m_pThread )
		m_pThread->Kill();
}

///////////////////////////////////////////////////