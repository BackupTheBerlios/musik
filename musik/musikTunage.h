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
//   CmusikTunage
//
// Filename(s): 
//
//   musikTunage.h, musikTunage.cpp
//
// Information:
//
//   Does something related to current song info, such as post to a website,
//   write to a text file, etc.
// 
// Usage: 
//
//	 Called when a song changes or is stopped
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikPrefs.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

class CmusikTunage
{
public:
	CmusikTunage( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	~CmusikTunage(void);

	void Execute();

	void Execute_PostURL();
	void Execute_RunApp();
	void Execute_WriteFile();

private:

	// our player and prefs
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	void ParseTags( CmusikString& str, bool encode );
	void URLEncode( CmusikString& str );
};

///////////////////////////////////////////////////
