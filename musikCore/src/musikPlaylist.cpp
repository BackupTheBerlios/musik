///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
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
//   CmusikPlaylist,
//   CmusikSong,
//   CmusikSongInfo
//
// Filename(s): 
//
//   musikPlaylist.h, musikPlaylist.cpp
//
// Information:
//
//   CmusikPlaylist:
//     is a CmusikSongArray manager. It allows easy
//     adding, removing, and querying of songs.
//
//   CmusikSong:
//     is a small class that only contains the (unique)
//     ID of the song as it exists in the loaded database, as
//     well as a few utility functions to access it's respective
//     information. The library must be setup using
//     CmusikSong::SetLibrary(), which is a static function.
//
//   CmusikSongInfo:
//     is a larger class than CmusikSong, as it actually
//     stores all of the database information in memory,
//     rather than querying the database to retrieve it.
//
// Usage: 
//
//   CmusikPlaylist:
//     Create an instance and Add(), Remove(), etc.
//
//   CmusikSong:
//     Create and set the song with SetID()
//     Use GetField() to retrieve information
//
//   CmusikSongInfo:
//     Create, then use CmusikLibrary::GetSongInfoFromID()
//     to populate. GetField() or various other Get() functions
//     to query.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikPlaylist.h"
#include "../include/musikLibrary.h"

///////////////////////////////////////////////////

// Cmusik Song: a single song ID

///////////////////////////////////////////////////

CmusikLibrary* CmusikSong::m_musikLibrary = NULL;

///////////////////////////////////////////////////

CmusikSong::CmusikSong()
{
	m_ID = -1;
}

///////////////////////////////////////////////////

CmusikSong::~CmusikSong()
{
}

///////////////////////////////////////////////////

CStdString CmusikSong::GetField( int field )
{
	CStdString ret;
	m_musikLibrary->GetFieldFromID( m_ID, field, ret );
	return ret;
}

///////////////////////////////////////////////////

void CmusikSong::SetField( int field, CStdString value )
{

}

///////////////////////////////////////////////////

CmusikSongInfo::CmusikSongInfo()
{
	m_ID = -1;
}

///////////////////////////////////////////////////

CmusikSongInfo::~CmusikSongInfo()
{

}

///////////////////////////////////////////////////

CStdString CmusikSongInfo::GetField( int field )
{
	switch( field )
	{
	case MUSIK_LIBRARY_TYPE_ARTIST:
		return m_Artist;
		break;
	case MUSIK_LIBRARY_TYPE_ALBUM:
		return m_Album;
		break;
	case MUSIK_LIBRARY_TYPE_YEAR:
		return m_Year;
		break;
	case MUSIK_LIBRARY_TYPE_GENRE:
		return m_Genre;
		break;
	case MUSIK_LIBRARY_TYPE_TITLE:
		return m_Title;
		break;
	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		return m_TrackNum;
		break;
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
		return m_TimeAdded;
		break;
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
		return m_LastPlayed;
		break;
	case MUSIK_LIBRARY_TYPE_FILESIZE:
		return m_Filesize;
		break;
	case MUSIK_LIBRARY_TYPE_FORMAT:
		return m_Format;
		break;
	case MUSIK_LIBRARY_TYPE_DURATION:
		return m_Duration;
		break;
	case MUSIK_LIBRARY_TYPE_RATING:
		return m_Rating;
		break;
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		return m_LastPlayed;
		break;
	case MUSIK_LIBRARY_TYPE_BITRATE:
		return m_Bitrate;
		break;
	case MUSIK_LIBRARY_TYPE_FILENAME:
		return m_Filename;
		break;
	}

	return m_Filename;
}	

///////////////////////////////////////////////////

// CmusikPlaylist: a playlist object

///////////////////////////////////////////////////

CmusikPlaylist::CmusikPlaylist()
{
	
}

///////////////////////////////////////////////////

CmusikPlaylist::~CmusikPlaylist()
{
	m_Songs.clear();
}

///////////////////////////////////////////////////

CStdString CmusikPlaylist::GetField( int index, int field )
{
	return m_Songs.at( index ).GetField( field );
}

///////////////////////////////////////////////////

void CmusikPlaylist::Clear()
{
	m_Songs.clear();
}	

///////////////////////////////////////////////////

void CmusikPlaylist::Add( const CmusikSong& song )
{ 
	m_Songs.push_back( song ); 
}

///////////////////////////////////////////////////

size_t CmusikPlaylist::GetCount()
{
	return m_Songs.size();
}

///////////////////////////////////////////////////

int CmusikPlaylist::GetSongID( int index )
{
	return m_Songs.at( index ).GetID();
}

///////////////////////////////////////////////////

// CmusikPlaylistInfo

///////////////////////////////////////////////////

CmusikPlaylistInfo::CmusikPlaylistInfo()
{
	m_Type = -1;
	m_ID = -1;
}

///////////////////////////////////////////////////

CmusikPlaylistInfo::CmusikPlaylistInfo( CStdString name, int type, int id )
{
	m_Name = name;
	m_Type = type;
	m_ID = id;
}

///////////////////////////////////////////////////

CmusikPlaylistInfo::~CmusikPlaylistInfo()
{
}

///////////////////////////////////////////////////

void CmusikPlaylistInfo::Set( CStdString name, int type, int id )
{
	m_Name = name;
	m_Type = type;
	m_ID = id;
}

///////////////////////////////////////////////////

void CmusikPlaylistInfo::SaveToLibrary( CmusikLibrary* lib )
{
	
}

///////////////////////////////////////////////////