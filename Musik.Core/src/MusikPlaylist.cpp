#include "stdafx.h"
#include "../include/MusikPlaylist.h"
#include "../include/MusikLibrary.h"

//-----------------------------------------------------//
//--- CMusik Song: a single song ID					---//
//-----------------------------------------------------//
CMusikLibrary* CMusikSong::m_MusikLibrary = NULL;

CMusikSong::CMusikSong()
{
	m_ID = -1;
}

CMusikSong::~CMusikSong()
{
}

CStdString CMusikSong::GetField( int field )
{
	CStdString ret;
	m_MusikLibrary->GetFieldFromID( m_ID, field, ret );
	return ret;
}

void CMusikSong::SetField( int field, CStdString value )
{

}


CMusikSongInfo::CMusikSongInfo()
{
	m_ID = m_Rating = m_Bitrate = m_Duration = m_Dirty =
	m_Filesize = m_TimesPlayed = m_TrackNum = m_Format =
	m_VBR = -1;
}

CMusikSongInfo::~CMusikSongInfo()
{

}

//-----------------------------------------------------//
//--- CMusikPlaylist: a playlist object				---//
//-----------------------------------------------------//
CMusikPlaylist::CMusikPlaylist()
{
	
}

CMusikPlaylist::~CMusikPlaylist()
{
	m_Songs.clear();
}

CStdString CMusikPlaylist::GetField( int index, int field )
{
	return m_Songs.at( index ).GetField( field );
}