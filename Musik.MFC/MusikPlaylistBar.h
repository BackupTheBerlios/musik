#pragma once

#include "MusikPlaylistCtrl.h"

#ifndef baseCMusikPlaylistBar
#define baseCMusikPlaylistBar CSizingControlBarG
#endif

class CMusikPlaylistBar : public baseCMusikPlaylistBar
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikPlaylistBar();
	virtual ~CMusikPlaylistBar();

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikPlaylistCtrl m_wndChild;
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
};

