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
	bool WriteInfo( CMusikSongInfo info );

	// gets
	CStdString GetField( int field );
	CMusikSongInfo* Get(){ return &m_Info; }

private:

	CStdString GetTempFilename( CStdString fn, bool check_exists = true );
	CMusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////