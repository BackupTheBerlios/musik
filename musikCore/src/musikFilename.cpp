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

#include <io.h>
#include <Direct.h>

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

	if ( nPos == -1 )
		return (CmusikString)"";

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

bool CmusikFilename::GetSongInfo( CmusikStringArray mask, CmusikStringArray& fn_delimiters, CmusikSongInfo& target, bool clear_info, bool partial )
{
	CmusikString path = GetTrimPath();
	CmusikString fn = GetJustFilenameNoExt();

	SortBySize( fn_delimiters );

	for ( size_t i = 0; i < fn_delimiters.size(); i++ )
		fn.Replace( fn_delimiters.at( i ), MUSIK_PATH_SEPARATOR );

	path += fn;

	if ( path.IsEmpty() )
		return false;

	CmusikStringArray values;
	DelimitStr( path, MUSIK_PATH_SEPARATOR, values, true, mask.size() );

	if ( !partial && values.size() < mask.size() )
		return false;

	if ( !values.size() )
		return false;

	if ( clear_info )
	{
		target.SetTitle( _T( "" ) );
		target.SetArtist( _T( "" ) );
		target.SetAlbum( _T( "" ) );
		target.SetTrackNum( _T( "" ) );
		target.SetGenre( _T( "" ) );
		target.SetYear( _T( "" ) );
		target.SetRating( _T( "" ) );
	}

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

bool CmusikFilename::RenameFromSongInfo( CmusikString mask, CmusikSongInfo& src )
{
	if ( m_Filename.IsEmpty() )
		return false;

	CmusikString out_path = GetPath();
	CmusikString out_dir;

	// parase mask
	if ( mask.Right( mask.length() ) != MUSIK_PATH_SEPARATOR )
		mask += MUSIK_PATH_SEPARATOR;

	mask.Replace( _T( "$TITLE" ), src.GetTitle() );
	mask.Replace( _T( "$ARTIST" ), src.GetArtist() );
	mask.Replace( _T( "$ALBUM" ), src.GetAlbum() );
	mask.Replace( _T( "$TRACK" ), src.GetTrackNum() );
	mask.Replace( _T( "$GENRE" ), src.GetGenre() );
	mask.Replace( _T( "$YEAR" ), src.GetYear() );
	mask.Replace( _T( "$RATING" ), src.GetRating() );

	// get final output filename
	out_path += mask;
	CmusikString ext = GetExtension();
	out_path = out_path.Left( out_path.length() - 1 );
	out_path += ".";
	out_path += ext;

	// create directory
	CmusikFilename fn( out_path );
	if ( CmusikFilename::RecurseMkDir( fn.GetPath().GetBuffer() ) )
		return false;

	// rename file
	if ( rename( m_Filename.c_str(), out_path.c_str() ) )
		return false;

	src.SetFilename( out_path );
	return true;
}

///////////////////////////////////////////////////

bool CmusikFilename::RecurseMkDir( char* pszDir )
{
    char*   pszLastSlash;
    char    cTmp;

#ifdef WIN32
	char path_sep = '\\';
#else
	char path_sep = '/';
#endif

    pszLastSlash = strrchr( pszDir, path_sep );
    if ( pszLastSlash )
    {
        cTmp = *pszLastSlash;
        *pszLastSlash = '\0';

        RecurseMkDir( pszDir );

        *pszLastSlash = cTmp;
    }

	if ( mkdir( pszDir ) )
		return false;

    return true;
}

///////////////////////////////////////////////////

// a quick bubble sort for delimiters

void CmusikFilename::SortBySize( CmusikStringArray& array )
{
	if ( !array.size() )
		return;

	CmusikString hold;
	for ( size_t i = 0; i < array.size() - 1; i++ )
	{
		for ( size_t j = 0; j < array.size() - 1 - i; j++ )
		{
			if ( array.at( j ).length() < array.at( j + 1 ).length() )
			{
				hold = array.at( j + 1 );
				array.at( j + 1 ) = array.at( j );
				array.at( j ) = hold;
			}
		}
	}
}

///////////////////////////////////////////////////

