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
	m_Filesize = 0;
	m_Duration = 0;
}

///////////////////////////////////////////////////

CmusikSong::~CmusikSong()
{
}

///////////////////////////////////////////////////

CmusikString CmusikSong::GetField( int field )
{
	CmusikString ret;
	m_musikLibrary->GetFieldFromID( m_ID, field, ret );
	return ret;
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

CmusikString CmusikSongInfo::GetField( int field )
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
		return m_TimesPlayed;
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

void CmusikSongInfo::SetField( int field, const CmusikString& str )
{
	switch( field )
	{
	case MUSIK_LIBRARY_TYPE_ARTIST:
		m_Artist = str;
		break;
	case MUSIK_LIBRARY_TYPE_ALBUM:
		m_Album = str;
		break;
	case MUSIK_LIBRARY_TYPE_YEAR:
		m_Year = str;
		break;
	case MUSIK_LIBRARY_TYPE_GENRE:
		m_Genre = str;
		break;
	case MUSIK_LIBRARY_TYPE_TITLE:
		m_Title = str;
		break;
	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		m_TrackNum = str;
		break;
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
		m_TimeAdded = str;
		break;
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
		m_LastPlayed = str;
		break;
	case MUSIK_LIBRARY_TYPE_FILESIZE:
		m_Filesize = str;
		break;
	case MUSIK_LIBRARY_TYPE_FORMAT:
		m_Format = str;
		break;
	case MUSIK_LIBRARY_TYPE_DURATION:
		m_Duration = str;
		break;
	case MUSIK_LIBRARY_TYPE_RATING:
		m_Rating = str;
		break;
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		m_LastPlayed = str;
		break;
	case MUSIK_LIBRARY_TYPE_BITRATE:
		m_Bitrate = str;
		break;
	case MUSIK_LIBRARY_TYPE_FILENAME:
		m_Filename = str;
		break;
	}
}

///////////////////////////////////////////////////

// CmusikPlaylist: a playlist object

///////////////////////////////////////////////////

CmusikPlaylist::CmusikPlaylist()
{
	m_ID = -1;
	m_Type = MUSIK_PLAYLIST_TYPE_UNKNOWN;

	m_TotalTime = 0;
	m_TotalSize = 0.0;
}

///////////////////////////////////////////////////

CmusikPlaylist::~CmusikPlaylist()
{
	m_Songs.clear();
}

///////////////////////////////////////////////////

CmusikString CmusikPlaylist::GetField( int index, int field )
{
	return m_Songs.at( index ).GetField( field );
}

///////////////////////////////////////////////////

void CmusikPlaylist::Clear()
{
	m_Songs.clear();

	m_TotalTime = 0;
	m_TotalSize = 0.0;
}	

///////////////////////////////////////////////////

void CmusikPlaylist::Add( CmusikSong& song )
{ 
	m_Songs.push_back( song ); 
	m_TotalTime += song.GetDuration();
	m_TotalSize += (double)song.GetFilesize();
}

///////////////////////////////////////////////////

size_t CmusikPlaylist::GetCount()
{
	return m_Songs.size();
}

///////////////////////////////////////////////////

int CmusikPlaylist::GetSongID( int index )
{
	if ( index >= (int)m_Songs.size() || index < 0 )
		return -1;

	return m_Songs.at( index ).GetID();
}

///////////////////////////////////////////////////

CmusikSong CmusikPlaylist::GetSong( int index )
{
	if ( index >= (int)m_Songs.size() || index < 0 )
	{
		CmusikSong bad;
		return bad;
	}

	return m_Songs.at( index );
}

///////////////////////////////////////////////////

void CmusikPlaylist::DeleteAt( size_t pos )
{
	if ( pos > m_Songs.size() - 1 )
		ASSERT( 1 );

	m_TotalTime -= m_Songs.at( pos ).GetDuration();
	m_TotalSize -= (double)m_Songs.at( pos ).GetFilesize();
	m_Songs.erase( m_Songs.begin() + pos );
}

///////////////////////////////////////////////////

void CmusikPlaylist::InsertAt( CmusikSong& song, int at )
{
	if ( at == -1 )
		m_Songs.push_back( song );
	else
		m_Songs.insert( m_Songs.begin() + at, song );

	m_TotalTime += song.GetDuration();
	m_TotalSize += (double)song.GetFilesize();
}

///////////////////////////////////////////////////

void CmusikPlaylist::InsertAt( CmusikSongArray& songs, int pos )
{
	for ( size_t i = 0; i < songs.size(); i++ )
	{
		if ( pos == -1 )
			m_Songs.push_back( songs.at( i ) );
		else
			m_Songs.insert( m_Songs.begin() + pos + i, songs.at( i ) );

		m_TotalTime += songs.at( i ).GetDuration();
		m_TotalSize += (double)songs.at( i ).GetFilesize();
	}
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

CmusikPlaylistInfo::CmusikPlaylistInfo( CmusikString name, int type, int id )
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

void CmusikPlaylistInfo::Set( CmusikString name, int type, int id )
{
	m_Name = name;
	m_Type = type;
	m_ID = id;
}

///////////////////////////////////////////////////

