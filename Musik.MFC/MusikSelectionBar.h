///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikSelectionCtrl.h"

#include "MusikDockBar.h"

///////////////////////////////////////////////////

class CMusikLibrary;

///////////////////////////////////////////////////

#ifndef baseCMusikSelectionBar
	#define baseCMusikSelectionBar CMusikDockBar
#endif

///////////////////////////////////////////////////

class CMusikSelectionBar : public baseCMusikSelectionBar
{
public:

	// construct / destruct
	CMusikSelectionBar( CFrameWnd* parent, CMusikLibrary* library, CMusikPrefs* prefs, int type, int ctrl_id );
	virtual ~CMusikSelectionBar();

	// gets
	CMusikSelectionCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// child
	CMusikSelectionCtrl* m_wndChild;
	CFont m_Font;

	// mfc message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
