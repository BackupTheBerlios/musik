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
//   CMusikFilename
//
// Filename(s): 
//
//   MusikFilename.h, MusikFilename.cpp
//
// Information:
//
//   A crossplatform (Win32/Linux) way to extract certain
//   parts of a filename.
// 
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/MusikFilename.h"

///////////////////////////////////////////////////

CMusikFilename::CMusikFilename()
{
}

///////////////////////////////////////////////////

CMusikFilename::CMusikFilename( const CStdString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CMusikFilename::~CMusikFilename()
{
}

///////////////////////////////////////////////////

void CMusikFilename::SetFilename( const CStdString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetPathSeparator()
{
	return (CStdString)MUSIK_PATH_SEPARATOR;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetJustFilename()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Right( m_Filename.GetLength() - nPos - 1 );
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetFullFilename()
{
	return m_Filename;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetExtension( bool make_lower )
{
	int nPos = m_Filename.ReverseFind( "." );
	CStdString ret = m_Filename.Right( m_Filename.GetLength() - nPos - 1 );

	if ( make_lower )
		ret.MakeLower();

	return ret;
}

///////////////////////////////////////////////////

CStdString CMusikFilename::GetPath()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Left( nPos + 1 );
}

///////////////////////////////////////////////////

bool CMusikFilename::FileExists( const CStdString& fn )
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

CStdString CMusikFilename::GetTempFilename( CStdString fn, bool check_exists )
{
	srand ( ( size_t )time( NULL ) );

	CMusikFilename MFN( fn );
	CStdString sPath = MFN.GetPath();
	
	CStdString sName;
	sName.Format( "%s%d", sPath, rand() );
	
	if ( check_exists )
	{
		while ( CMusikFilename::FileExists( sName ) )
			sName.Format( "%s%d", sPath, rand() );
	}

	return sName;
}

///////////////////////////////////////////////////