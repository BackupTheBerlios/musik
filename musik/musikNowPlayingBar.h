///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikNowPlayingCtrl.h"

#include "musikDockBar.h"

///////////////////////////////////////////////////

#ifndef baseCmusikNowPlayingBar
	#define baseCmusikNowPlayingBar CmusikDockBar
#endif

///////////////////////////////////////////////////

class CmusikPlayer;
class CmusikPrefs;

///////////////////////////////////////////////////

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

