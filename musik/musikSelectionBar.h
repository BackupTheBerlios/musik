///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikSelectionCtrl.h"

#include "musikDockBar.h"

///////////////////////////////////////////////////

class CmusikLibrary;

///////////////////////////////////////////////////

#ifndef baseCmusikSelectionBar
	#define baseCmusikSelectionBar CmusikDockBar
#endif

///////////////////////////////////////////////////

class CmusikSelectionBar : public baseCmusikSelectionBar
{
public:

	// construct / destruct
	CmusikSelectionBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, int dropid );
	virtual ~CmusikSelectionBar();

	// gets
	CmusikSelectionCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// child
	CmusikSelectionCtrl* m_wndChild;
	CFont m_Font;

	// mfc message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
