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
// Class(s): 
//
//   CmusikFilename
//
// Filename(s): 
//
//   musikFilename.h, musikFilename.cpp
//
// Information:
//
//   A crossplatform (Win32/Linux) way to extract certain
//   parts of a filename.
// 
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/musikFilename.h"

///////////////////////////////////////////////////

CmusikFilename::CmusikFilename()
{
}

///////////////////////////////////////////////////

CmusikFilename::CmusikFilename( const CStdString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CmusikFilename::~CmusikFilename()
{
}

///////////////////////////////////////////////////

void CmusikFilename::SetFilename( const CStdString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CStdString CmusikFilename::GetPathSeparator()
{
	return (CStdString)MUSIK_PATH_SEPARATOR;
}

///////////////////////////////////////////////////

CStdString CmusikFilename::GetJustFilename()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Right( m_Filename.GetLength() - nPos - 1 );
}

///////////////////////////////////////////////////

CStdString CmusikFilename::GetFullFilename()
{
	return m_Filename;
}

///////////////////////////////////////////////////

CStdString CmusikFilename::GetExtension( bool make_lower )
{
	int nPos = m_Filename.ReverseFind( "." );
	CStdString ret = m_Filename.Right( m_Filename.GetLength() - nPos - 1 );

	if ( make_lower )
		ret.MakeLower();

	return ret;
}

///////////////////////////////////////////////////

CStdString CmusikFilename::GetPath()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Left( nPos + 1 );
}

///////////////////////////////////////////////////

bool CmusikFilename::FileExists( const CStdString& fn )
{
	FILE* pIn = fopen( fn.c_str(), "rb" );
	if ( pIn )
	{
		fclose( pIn );
		return true;
	}

	return false;
}

///////////////////////////////////////////////////

CStdString CmusikFilename::GetTempFilename( CStdString fn, bool check_exists )
{
	srand ( ( size_t )time( NULL ) );

	CmusikFilename MFN( fn );
	CStdString sPath = MFN.GetPath();
	
	CStdString sName;
	sName.Format( "%s%d", sPath, rand() );
	
	if ( check_exists )
	{
		while ( CmusikFilename::FileExists( sName ) )
			sName.Format( "%s%d.tmp", sPath, rand() );
	}

	return sName;
}

///////////////////////////////////////////////////
