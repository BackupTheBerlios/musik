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
//   CmusikOggInfo
//
// Filename(s): 
//
//   musikOggInfo.h, musikOggInfo.cpp
//
// Information:
//
//   CmusikOggInfo reads and writes ogg tags from a 
//   CmusikSongInfo
// 
///////////////////////////////////////////////////

#ifndef C_MUSIK_OGG_INFO_H
#define C_MUSIK_OGG_INFO_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikOggInfo
{
public:

	// construct and destruct
	CmusikOggInfo();
	~CmusikOggInfo();

	// reading and writing tag
	bool LoadInfo( const CmusikString& fn );
	static bool WriteInfo( CmusikSongInfo info );

	// gets
	CmusikString GetField( int field );
	CmusikSongInfo* Get(){ return &m_Info; }

private:

	CmusikString GetTempFilename( CmusikString fn, bool check_exists = true );
	CmusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
