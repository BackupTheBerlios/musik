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
//   CmusikDir
//
// Filename(s): 
//
//   musikDir.h, musikDir.cpp
//
// Information:
//
//   Cross platform (win32 / linux) directory traverser
// 
// Usage: 
//
//   Construct then call Run()
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikDir.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

CmusikDir::CmusikDir()
{
	m_Target = NULL;
}

///////////////////////////////////////////////////

CmusikDir::CmusikDir( CmusikString dir, CmusikStringArray* target )
{
	m_Target = target;
	m_Dir = dir;
}

///////////////////////////////////////////////////

CmusikDir::~CmusikDir()
{
}

///////////////////////////////////////////////////

#ifdef WIN32

void OpenDir( CmusikString dir, CmusikStringArray* target )
{
	// find the first file in the directory...
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hDir = FindFirstFile( dir.c_str(), &lpFindFileData );

	// assure the first file exists...
	if ( hDir != INVALID_HANDLE_VALUE )
	{
		CmusikString fn, temp;
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

					if ( fn.Left( 0 ) != "\\" )
						fn += "\\*.*";						// add \\*.* to the end of the new path
					else
						fn += "*.*";						// add *.* to the end of the new path

					OpenDir( fn, target );
				}

				// file is a file...
				else
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path...
					fn += lpFindFileData.cFileName;			// append filename
					
					MFN.SetFilename( fn );
					if ( MFN.GetExtension() == "mp3" || MFN.GetExtension() == "ogg" )
						target->push_back( fn );
				}
			}

		}
		while ( FindNextFile( hDir, &lpFindFileData ) );

		FindClose( hDir );
	}
}

#else

void OpenDir( CmusikString dir, CmusikStringArray* target )
{
	DIR* pDir = opendir ( dir.c_str() );

	if ( !pDir) 
		return;

	dirent* pEntry;

	CmusikString fn;
	CmusikFilename MFN;
	while ( ( pEntry = readdir ( pDir ) ) ) 
    {
		// got a directory, so recurse...
        if ( DT_DIR & pEntry->d_type && strcmp ( pEntry->d_name, "." ) && strcmp ( pEntry->d_name, ".." ) ) 
        {
            CmusikString sSubDir = dir + CmusikString ( "/" ) + CmusikString ( pEntry->d_name );
            OpenDir ( sSubDir, target );
		}

		// not a directory, so check file attributes
		else
		{
			if ( fn.Left( 0 ) != "/" )
				fn = dir + CmusikString( "/" );
			else
				fn = dir;

			fn += CmusikString ( pEntry->d_name );
	
			MFN.SetFilename( fn );
			if ( MFN.GetExtension() == "mp3" || MFN.GetExtension() == "ogg" )
				target->push_back( fn );
		}
    }
}

#endif

///////////////////////////////////////////////////

void CmusikDir::Run()
{
	OpenDir( m_Dir, m_Target );
	return;
}

///////////////////////////////////////////////////
