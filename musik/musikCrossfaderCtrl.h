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

protected:

	// core
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CmusikCrossfaderCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


