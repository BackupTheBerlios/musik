#pragma once

#include "MusikSelectionCtrl.h"

#ifndef baseCMusikSelectionBar
#define baseCMusikSelectionBar CSizingControlBarG
#endif

class CMusikSelectionBar : public baseCMusikSelectionBar
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikSelectionBar();
	virtual ~CMusikSelectionBar();

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikSelectionCtrl m_wndChild;
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

