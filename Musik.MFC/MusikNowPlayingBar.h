///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikNowPlayingCtrl.h"

///////////////////////////////////////////////////

#ifndef baseCMusikNowPlayingBar
#define baseCMusikNowPlayingBar CSizingControlBarG
#endif

///////////////////////////////////////////////////

class CMusikPlayer;
class CMusikPrefs;

///////////////////////////////////////////////////

class CMusikNowPlayingBar : public baseCMusikNowPlayingBar
{
public:

	// construct and destruct
	CMusikNowPlayingBar( CMusikPlayer* player, CMusikPrefs* prefs );
	virtual ~CMusikNowPlayingBar();

	// get control
	CMusikNowPlayingCtrl* GetCtrl(){ return m_wndChild; }

protected:
	// default stuff
	CMusikNowPlayingCtrl* m_wndChild;
	CFont m_Font;

	// message map
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );

	// macros
	DECLARE_MESSAGE_MAP()
public:

};

///////////////////////////////////////////////////

