#pragma once

#include "MusikSelectionCtrl.h"

class CMusikLibrary;

#ifndef baseCMusikSelectionBar
#define baseCMusikSelectionBar CSizingControlBarG
#endif

class CMusikSelectionBar : public baseCMusikSelectionBar
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikSelectionBar( CMusikLibrary* library, int type );
	virtual ~CMusikSelectionBar();

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikSelectionCtrl* m_wndChild;
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

