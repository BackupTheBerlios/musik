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
//   CMusikMp3Info
//
// Filename(s): 
//
//   MusikMp3Info.h, MusikMp3Info.cpp
//
// Information:
//
//   CMusikMp3Info reads and writes mp3 tags from a 
//   CMusikSongInfo
// 
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
