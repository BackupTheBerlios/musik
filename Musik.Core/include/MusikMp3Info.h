///////////////////////////////////////////////////

#ifndef C_MUSIK_MP3_INFO_H
#define C_MUSIK_MP3_INFO_H

///////////////////////////////////////////////////

#include "MusikPlaylist.h"

///////////////////////////////////////////////////

class CMusikMp3Info
{
public:

	// construct and destruct
	CMusikMp3Info();
	~CMusikMp3Info();

	// reading and writing tag
	bool LoadInfo( const CStdString& fn );
	static bool WriteInfo( CMusikSongInfo info, bool clear_old = true );

	// gets
	CStdString GetField( int field );
	CMusikSongInfo* Get(){ return &m_Info; }
	static CStdString GetGenre( CStdString genre );
	static int GetGenreID( CStdString genre );

private:

	CMusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////