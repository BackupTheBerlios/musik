///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikNowPlayingCtrl.h"

///////////////////////////////////////////////////

#ifndef baseCMusikNowPlayingBar
#define baseCMusikNowPlayingBar CSizingControlBarG
#endif

///////////////////////////////////////////////////

class CMusikNowPlayingBar : public baseCMusikNowPlayingBar
{
public:
	CMusikNowPlayingBar();
	virtual ~CMusikNowPlayingBar();

protected:
	// default stuff
	CMusikNowPlayingCtrl m_wndChild;
	CFont m_Font;

	// message map
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

