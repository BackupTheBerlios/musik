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
//   CmusikNowPlayingCtrl, CmusikNowPlayingBar
//
// Filename(s): 
//
//   musikNowPlayingCtrl.h, musikNowPlayingCtrl.cpp
//
// Information:
//
//   The "Now Playing Control"... the dock window at the bottom
//   of musikCube that displays song information with playback
//   controls.
// 
// Usage: 
//
//    Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikDockBar.h"
#include "musikNowPlayingInfo.h"
#include "musikVolumeCtrl.h"
#include "musikTimeCtrl.h."
#include "musikPrefs.h"

#include "3rdparty/BtnST.h"

#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl;

///////////////////////////////////////////////////

enum
{
	MUSIK_NOWPLAYING_BTN_PLAY = 4957,
	MUSIK_NOWPLAYING_BTN_PREV,
	MUSIK_NOWPLAYING_BTN_NEXT
};

///////////////////////////////////////////////////

// CmusikNowPlayingBar

///////////////////////////////////////////////////

#ifndef baseCmusikNowPlayingBar
	#define baseCmusikNowPlayingBar CmusikDockBar
#endif


class CmusikNowPlayingBar : public baseCmusikNowPlayingBar
{
public:

	// construct and destruct
	CmusikNowPlayingBar( CFrameWnd* parent, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikNowPlayingBar();

	// get control
	CmusikNowPlayingCtrl* GetCtrl(){ return m_wndChild; }

protected:
	// default stuff
	CmusikNowPlayingCtrl* m_wndChild;
	CFont m_Font;

	// message map
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikNowPlayingCtrl

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikNowPlayingCtrl( CFrameWnd* parent, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikNowPlayingCtrl();

	// getting children
	CmusikVolumeCtrl* GetVolumeCtrl(){ return m_Volume; }
	CmusikTimeCtrl* GetTimeCtrl(){ return m_Track; }

	// layout and update
	void UpdateInfo( bool redraw = true );
	void RescaleInfo();
	void ResetBtnColors();
	void SetPaused();
	void SetResumed();
	void SetPlaying();
	void SetStopped();

protected:

	// pointer to the main dialog
	CFrameWnd* m_MainWnd;

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnPrev();
	afx_msg void OnBtnNext();

	// info object (title/artist/album)
	CmusikNowPlayingInfo* m_Info1;
	CmusikNowPlayingInfo* m_Info2;

	// volume slider
	CmusikVolumeCtrl *m_Volume;

	// seek slider
	CmusikTimeCtrl* m_Track;

	// buttons
	CButtonST* m_Play;
	CButtonST* m_Prev;
	CButtonST* m_Next;

	// default font
	CFont m_Font;

	// our player and prefs
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CmusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
