///////////////////////////////////////////////////

#pragma once

#include "musikSourcesCtrl.h"

#include "musikDockBar.h"

///////////////////////////////////////////////////

class CmusikLibrary;

///////////////////////////////////////////////////

#define IDC_SOURCES 1000

///////////////////////////////////////////////////

#ifndef baseCmusikSourcesBar
	#define baseCmusikSourcesBar CmusikDockBar
#endif

///////////////////////////////////////////////////

class CmusikSourcesBar : public baseCmusikSourcesBar
{
public:

	// construct / destruct
	CmusikSourcesBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid );
	virtual ~CmusikSourcesBar();

	// gets
	CmusikSourcesCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children
	CmusikSourcesCtrl* m_wndChild;
	CFont m_Font;
	CFrameWnd* m_Parent;

	// message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemChanged( NMHDR* pNotifyStruct, LRESULT* plResult );

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

