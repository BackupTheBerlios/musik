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
//   CmusikPlaylistInfoCtrl
//
// Filename(s): 
//
//   musikPlaylistInfoCtrl.h, musikPlaylistInfoCtrl.cpp
//
// Information:
//
//   Shows information about playlist, such as number of songs, size, etc.
// 
// Usage: 
//
//
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "../musikCore/include/StdString.h"
#include "../musikCore/include/musikFunctor.h"
#include "../musikCore/include/musikDir.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlayer;
class CmusikPrefs;
class CmusikPlaylist;
class CmusikDynDspInfo;
class CmusikDir;

///////////////////////////////////////////////////

class CmusikPlaylistInfoCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikPlaylistInfoCtrl();

	

protected:

	// pointer to the list
	CmusikPlaylistCtrl* m_ListCtrl;

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		
	// default font
	CFont m_Font;

	// our player and prefs
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CmusikPlaylistInfoCtrl)
	DECLARE_MESSAGE_MAP()
};

