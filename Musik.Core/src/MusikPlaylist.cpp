#include "../include/MusikPlaylist.h"

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
	return _T( "" );
}

void CMusikSong::SetField( int field, CStdString value )
{

}


CMusikSongInfo::CMusikSongInfo()
{
	m_Filename = m_Title = m_Artist = m_Album = m_Year =
	m_Genre = m_LastPlayed = m_TimeAdded = m_Notes = _T( "" );

	m_ID = m_Rating = m_Bitrate = m_Duration = m_Dirty =
	m_Filesize = m_TimesPlayed = m_TrackNum = m_Format =
	m_VBR = -1;
}

CMusikSongInfo::~CMusikSongInfo()
{

}