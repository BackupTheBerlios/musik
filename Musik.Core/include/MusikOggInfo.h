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
//   CMusikOggInfo
//
// Filename(s): 
//
//   MusikOggInfo.h, MusikOggInfo.cpp
//
// Information:
//
//   CMusikOggInfo reads and writes ogg tags from a 
//   CMusikSongInfo
// 
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
