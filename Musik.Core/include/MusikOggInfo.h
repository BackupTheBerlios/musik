///////////////////////////////////////////////////

#ifndef C_MUSIK_OGG_INFO_H
#define C_MUSIK_OGG_INFO_H

///////////////////////////////////////////////////

#include "MusikPlaylist.h"

///////////////////////////////////////////////////

class CMusikOggInfo
{
public:

	// construct and destruct
	CMusikOggInfo();
	~CMusikOggInfo();

	// reading and writing tag
	bool LoadInfo( const CStdString& fn );
	static bool WriteInfo( CMusikSongInfo info, bool clear_old = true );

	// gets
	CStdString GetField( int field );
	CMusikSongInfo* Get(){ return &m_Info; }

private:
	CMusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////