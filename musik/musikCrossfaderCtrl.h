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
//   CmusikCrossfaderBar, CmusikCrossfaderCtrl
//
// Filename(s): 
//
//   musikCrossfaderCtrl.h, musikCrossfaderCtrl.cpp
//
// Information:
//
//   musikCube's crossfader control.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikDockBar.h"

///////////////////////////////////////////////////

class CmusikCrossfaderCtrl;
class CmusikLibrary;
class CmusikPlayer;
class CmusikPrefs;
class CmusikDockBar;

///////////////////////////////////////////////////

#ifndef baseCmusikCrossfaderBar
	#define baseCmusikCrossfaderBar CmusikDockBar
#endif

///////////////////////////////////////////////////

// CmusikCrossfaderBar

///////////////////////////////////////////////////

class CmusikCrossfaderBar : public baseCmusikCrossfaderBar
{
public:

	// construct / destruct
	CmusikCrossfaderBar( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikCrossfaderBar();

	// gets
	CmusikCrossfaderCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children
	CmusikCrossfaderCtrl* m_wndChild;

	// child's font
	CFont m_Font;

	// message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikCrossfaderCtrl

///////////////////////////////////////////////////

class CmusikCrossfaderCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikCrossfaderCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikCrossfaderCtrl();

	// mfc message maps
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:

	// core
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// controls
	CComboBox* m_AllCrossfaders;

	// macros
	DECLARE_DYNAMIC(CmusikCrossfaderCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


