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
//   CmusikMp3Info
//
// Filename(s): 
//
//   musikMp3Info.h, musikMp3Info.cpp
//
// Information:
//
//   CmusikMp3Info reads and writes mp3 tags from a 
//   CmusikSongInfo
// 
///////////////////////////////////////////////////

#ifndef C_MUSIK_MP3_INFO_H
#define C_MUSIK_MP3_INFO_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikMp3Info
{
public:

	// construct and destruct
	CmusikMp3Info();
	~CmusikMp3Info();

	// reading and writing tag
	bool LoadInfo( const CmusikString& fn );
	static bool WriteInfo( CmusikSongInfo info, bool clear_old = true );

	// gets
	CmusikString GetField( int field );
	CmusikSongInfo* Get(){ return &m_Info; }
	static CmusikString GetGenre( CmusikString genre );
	static int GetGenreID( CmusikString genre );

private:

	CmusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
