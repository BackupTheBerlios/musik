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

	int GetCtrlID(){ return m_wndChild->GetCtrlID(); }
	void SetUpdating( bool updating = true ){ m_wndChild->SetUpdating( updating ); }

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

