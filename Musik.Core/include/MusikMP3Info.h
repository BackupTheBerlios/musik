///////////////////////////////////////////////////

#ifndef C_MUSIK_MP3_INFO_H
#define C_MUSIK_MP3_INFO_H

///////////////////////////////////////////////////

#include "MusikPlaylist.h"

///////////////////////////////////////////////////

class CMusikMp3Info
{
public:

	CMusikMp3Info();
	~CMusikMp3Info();

	// reading and writing tag
	bool LoadInfo( const CStdString& fn );
	bool WriteInfo( CMusikSongInfo info, bool clear_old = true );

	// gets
	CStdString GetField( int field );
	CStdString GetGenre( CStdString genre );
	int GetGenreID( CStdString genre );

private:

	CMusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////