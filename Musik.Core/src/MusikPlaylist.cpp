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
//   CMusikPlaylist,
//   CMusikSong,
//   CMusikSongInfo
//
// Filename(s): 
//
//   MusikPlaylist.h, MusikPlaylist.cpp
//
// Information:
//
//   CMusikPlaylist:
//     is a CMusikSongArray manager. It allows easy
//     adding, removing, and querying of songs.
//
//   CMusikSong:
//     is a small class that only contains the (unique)
//     ID of the song as it exists in the loaded database, as
//     well as a few utility functions to access it's respective
//     information. The library must be setup using
//     CMusikSong::SetLibrary(), which is a static function.
//
//   CMusikSongInfo:
//     is a larger class than CMusikSong, as it actually
//     stores all of the database information in memory,
//     rather than querying the database to retrieve it.
//
// Usage: 
//
//   CMusikPlaylist:
//     Create an instance and Add(), Remove(), etc.
//
//   CMusikSong:
//     Create and set the song with SetID()
//     Use GetField() to retrieve information
//
//   CMusikSongInfo:
//     Create, then use CMusikLibrary::GetSongInfoFromID()
//     to populate. GetField() or various other Get() functions
//     to query.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/MusikPlaylist.h"
#include "../include/MusikLibrary.h"

///////////////////////////////////////////////////

// CMusik Song: a single song ID

///////////////////////////////////////////////////

CMusikLibrary* CMusikSong::m_MusikLibrary = NULL;

///////////////////////////////////////////////////

CMusikSong::CMusikSong()
{
	m_ID = -1;
}

///////////////////////////////////////////////////

CMusikSong::~CMusikSong()
{
}

///////////////////////////////////////////////////

CStdString CMusikSong::GetField( int field )
{
	CStdString ret;
	m_MusikLibrary->GetFieldFromID( m_ID, field, ret );
	return ret;
}

///////////////////////////////////////////////////

void CMusikSong::SetField( int field, CStdString value )
{

}

///////////////////////////////////////////////////

CMusikSongInfo::CMusikSongInfo()
{
	m_ID = -1;
}

///////////////////////////////////////////////////

CMusikSongInfo::~CMusikSongInfo()
{

}

///////////////////////////////////////////////////

CStdString CMusikSongInfo::GetField( int field )
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

// CMusikPlaylist: a playlist object

///////////////////////////////////////////////////

CMusikPlaylist::CMusikPlaylist()
{
	
}

///////////////////////////////////////////////////

CMusikPlaylist::~CMusikPlaylist()
{
	m_Songs.clear();
}

///////////////////////////////////////////////////

CStdString CMusikPlaylist::GetField( int index, int field )
{
	return m_Songs.at( index ).GetField( field );
}

///////////////////////////////////////////////////

void CMusikPlaylist::Clear()
{
	m_Songs.clear();
}	

///////////////////////////////////////////////////

void CMusikPlaylist::Add( const CMusikSong& song )
{ 
	m_Songs.push_back( song ); 
}

///////////////////////////////////////////////////

size_t CMusikPlaylist::GetCount()
{
	return m_Songs.size();
}

///////////////////////////////////////////////////

int CMusikPlaylist::GetSongID( int index )
{
	return m_Songs.at( index ).GetID();
}

///////////////////////////////////////////////////

// CMusikPlaylistInfo

///////////////////////////////////////////////////

CMusikPlaylistInfo::CMusikPlaylistInfo()
{
	m_Type = -1;
	m_ID = -1;
}

///////////////////////////////////////////////////

CMusikPlaylistInfo::CMusikPlaylistInfo( CStdString name, int type, int id )
{
	m_Name = name;
	m_Type = type;
	m_ID = id;
}

///////////////////////////////////////////////////

CMusikPlaylistInfo::~CMusikPlaylistInfo()
{
}

///////////////////////////////////////////////////

void CMusikPlaylistInfo::Set( CStdString name, int type, int id )
{
	m_Name = name;
	m_Type = type;
	m_ID = id;
}

///////////////////////////////////////////////////

void CMusikPlaylistInfo::SaveToLibrary( CMusikLibrary* lib )
{
	
}

///////////////////////////////////////////////////