///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikDir.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

CmusikDir::CmusikDir()
{
	m_Target = NULL;
	m_Functor = NULL;
}

///////////////////////////////////////////////////

CmusikDir::CmusikDir( CStdString dir, CStdStringArray* target, CmusikFunctor* functor )
{
	m_Target = target;
	m_Dir = dir;
	m_Functor = functor;
}

///////////////////////////////////////////////////

CmusikDir::~CmusikDir()
{
}


///////////////////////////////////////////////////

// avoid multiple constructor calls
// during file find recursion...

CStdStringArray* g_Target;
CmusikFunctor* g_Functor;
int g_Count;
int g_Found;

///////////////////////////////////////////////////

#ifdef WIN32

void OpenDir( CStdString dir, CStdStringArray* target, CmusikFunctor* functor, bool reset )
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
		CmusikFilename MFN;
		do
		{
			if ( lpFindFileData.cFileName[0] != '.' )
			{
				// file is a directory
				if ( lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path
					fn += lpFindFileData.cFileName;			// append to make new path
					fn += "\\*.*";							// add *.* to the end of the new path
					OpenDir( fn, NULL, NULL, false );
				}

				// file is a file...
				else
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path...
					fn += lpFindFileData.cFileName;			// append filename
					
					MFN.SetFilename( fn );
					if ( MFN.GetExtension() == "mp3" || MFN.GetExtension() == "ogg" )
						g_Target->push_back( fn );
				}
			}

		}
		while ( FindNextFile( hDir, &lpFindFileData ) );

		FindClose( hDir );
	}
}

#else

void OpenDir( CStdString dir, CStdStringArray* target, CmusikFunctor* functor, bool reset )
{
	if ( target )
		g_Target = target;
	if ( functor )
		g_Functor = functor;

	DIR *pDir;
	struct dirent *pEntry;

	if ( pDir == opendir( dir.c_str() ) )
	{
		CStdString temp;
		while( pEntry = readdir( pDir ) )
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

void CmusikDir::Run()
{
	OpenDir( m_Dir, m_Target, m_Functor, true );
	return;
}

///////////////////////////////////////////////////
