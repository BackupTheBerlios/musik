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
#include <deque>

#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

CmusikFilename::CmusikFilename()
{
}

///////////////////////////////////////////////////

CmusikFilename::CmusikFilename( const CmusikString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CmusikFilename::~CmusikFilename()
{
}

///////////////////////////////////////////////////

void CmusikFilename::SetFilename( const CmusikString& fn )
{
	m_Filename = fn;
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetPathSeparator()
{
	return (CmusikString)MUSIK_PATH_SEPARATOR;
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetJustFilename()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Right( m_Filename.GetLength() - nPos - 1 );
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetJustFilenameNoExt()
{
	CmusikString fn = GetJustFilename();
	int nPos = fn.ReverseFind( "." );

	if ( nPos )
		fn = fn.Left( nPos );

	return fn;
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetFullFilename()
{
	return m_Filename;
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetTrimFilename()
{
#ifdef WIN32
	return m_Filename.Right( m_Filename.length() - 2 );
#endif

	return m_Filename;
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetExtension( bool make_lower )
{
	int nPos = m_Filename.ReverseFind( "." );
	CmusikString ret = m_Filename.Right( m_Filename.GetLength() - nPos - 1 );

	if ( make_lower )
		ret.MakeLower();

	return ret;
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetPath()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	return m_Filename.Left( nPos + 1 );
}

///////////////////////////////////////////////////

CmusikString CmusikFilename::GetTrimPath()
{
	int nPos = m_Filename.ReverseFind( MUSIK_PATH_SEPARATOR );
	CmusikString trimmed = m_Filename.Left( nPos + 1 );

#ifdef WIN32
	trimmed = trimmed.Right( trimmed.length() - 2 );
#endif

	return trimmed;
}

///////////////////////////////////////////////////

bool CmusikFilename::FileExists( const CmusikString& fn )
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

CmusikString CmusikFilename::GetTempFilename( CmusikString fn, bool check_exists )
{
	srand ( ( size_t )time( NULL ) );

	CmusikFilename MFN( fn );
	CmusikString sPath = MFN.GetPath();
	
	CmusikString sName;
	sName.Format( "%s%d", sPath, rand() );
	
	if ( check_exists )
	{
		while ( CmusikFilename::FileExists( sName ) )
			sName.Format( "%s%d.tmp", sPath, rand() );
	}

	return sName;
}

///////////////////////////////////////////////////

void CmusikFilename::GetDelimitedPath( CmusikStringArray& target, bool reverse, bool trimfilename )
{
	target.clear();

	CmusikString path;
	if ( trimfilename )
		path = GetTrimPath();
	else
		path = GetTrimFilename();

	DelimitStr( path, MUSIK_PATH_SEPARATOR, target, reverse );
}

///////////////////////////////////////////////////

void CmusikFilename::DelimitStr( CmusikString path, CmusikString delimiter, CmusikStringArray& target, bool reverse, int count )
{
	target.clear();

	if ( reverse )
		std::reverse( path.begin(), path.end() );

	char* token = strtok( path.GetBuffer(), delimiter.c_str() );
	int cnt = 1;
	CmusikString tokrev;
	while ( token )
	{
		tokrev = token;
		
		if ( reverse )
		{
			std::reverse( tokrev.begin(), tokrev.end() );
			target.insert( target.begin(), tokrev );
		}
		else
			target.push_back( tokrev );

		if ( count != -1 && ( cnt == count ) )
			break;

		token = strtok( NULL, delimiter.c_str() );
		cnt++;
	}
}

///////////////////////////////////////////////////

bool CmusikFilename::GetSongInfo( CmusikStringArray mask, CmusikString fn_delimiter, CmusikSongInfo& target, bool partial )
{
	CmusikString path = GetTrimPath();
	CmusikString fn = GetJustFilenameNoExt();

	fn.Replace( fn_delimiter, MUSIK_PATH_SEPARATOR );
	path += fn;

	CmusikStringArray values;
	DelimitStr( path, MUSIK_PATH_SEPARATOR, values, true, mask.size() );

	if ( !partial && values.size() < mask.size() )
		return false;

	if ( !values.size() )
		return false;

	for ( size_t i = 0; i < mask.size(); i++ )
	{
		const CmusikString current = mask.at( i );

		if ( current == "$TITLE" )
			target.SetTitle( values.at( i ) );
		else if ( current == "$ARTIST" )
			target.SetArtist( values.at( i ) );
		else if ( current == "$ALBUM" )
			target.SetAlbum( values.at( i ) );
		else if ( current == "$TRACK" )
			target.SetTrackNum( values.at( i ) );
		else if ( current == "$GENRE" )
			target.SetGenre( values.at( i ) );
		else if ( current == "$YEAR" )
			target.SetYear( values.at( i ) );
		else if ( current == "$RATING" )
			target.SetRating( values.at( i ) );

		if ( i + 1 >= values.size() )
			break;
	}

	return true;
}

///////////////////////////////////////////////////

