#pragma once

#include "MusikSourcesCtrl.h"

#ifndef baseCMusikSourcesBar
#define baseCMusikSourcesBar CSizingControlBarG
#endif

class CMusikSourcesBar : public baseCMusikSourcesBar
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikSourcesBar();
	virtual ~CMusikSourcesBar();

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikSourcesCtrl m_wndChild;
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

