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
	CMusikSelectionBar( CFrameWnd* parent, CMusikLibrary* library, int type, int ctrl_id );
	virtual ~CMusikSelectionBar();

	CMusikSelectionCtrl* GetCtrl(){ return m_wndChild; }

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

};

