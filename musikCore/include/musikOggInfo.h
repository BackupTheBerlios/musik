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
// Class(s): 
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

#include "musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikOggInfo
{
public:

	// construct and destruct
	CmusikOggInfo();
	~CmusikOggInfo();

	// reading and writing tag
	bool LoadInfo( const CStdString& fn );
	static bool WriteInfo( CmusikSongInfo info );

	// gets
	CStdString GetField( int field );
	CmusikSongInfo* Get(){ return &m_Info; }

private:

	CStdString GetTempFilename( CStdString fn, bool check_exists = true );
	CmusikSongInfo m_Info;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
