#pragma once

#include "MusikNowPlayingCtrl.h"

#ifndef baseCMusikNowPlayingBar
#define baseCMusikNowPlayingBar CSizingControlBarG
#endif

class CMusikNowPlayingBar : public baseCMusikNowPlayingBar
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikNowPlayingBar();
	virtual ~CMusikNowPlayingBar();

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikNowPlayingCtrl m_wndChild;
	CFont m_Font;

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//

//-------------------------------------------------//
//--- overrides									---//
//-------------------------------------------------//

//-------------------------------------------------//
//--- message maps								---//
//-------------------------------------------------//
protected:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};

