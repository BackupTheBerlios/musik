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
//   ..and has a visualization in the background
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
class CmusikPlaylistCtrl;

///////////////////////////////////////////////////

enum PLAYLIST_INFO_VIZ_STYLE
{
	PLAYLIST_INFO_VIZ_STYLE_NONE = 0,
	PLAYLIST_INFO_VIZ_STYLE_EQ,
};

class CmusikPlaylistInfoCtrl : public CWnd
{
#define VIZ_WIDTH     128
#define VIZ_HEIGHT    19
#define MUSIK_VIZ_TIMER			WM_USER + 2

public:

	// construct and destruct
	CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikPlaylistInfoCtrl();

	// called to update the background image
	void UpdateBG();

	// called to update the info string
	void UpdateInfo();

protected:

	// pointer to the list
	CmusikPlaylistCtrl* m_ListCtrl;

	// draws the eq bars in the bg
	void DrawEQ( HDC hdc );

	HBITMAP m_hBGBitmap;
	COLORREF *m_pBGBitmapBits;
	COLORREF m_EQColor;
	COLORREF m_BGColor;

	// timer
	UINT_PTR m_TimerID;

	CmusikString	m_strInfo;


	// message maps
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
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

