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

const CStdString& CMusikSongInfo::GetField( int field )
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