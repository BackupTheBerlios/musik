///////////////////////////////////////////////////

#pragma once

#include "MusikSourcesCtrl.h"

///////////////////////////////////////////////////

class CMusikLibrary;

///////////////////////////////////////////////////

#define IDC_SOURCES 1000

///////////////////////////////////////////////////

#ifndef baseCMusikSourcesBar
	#define baseCMusikSourcesBar CSizingControlBarCF
#endif

///////////////////////////////////////////////////

class CMusikSourcesBar : public baseCMusikSourcesBar
{
public:

	// construct / destruct
	CMusikSourcesBar( CFrameWnd* parent, CMusikLibrary* library );
	virtual ~CMusikSourcesBar();

	// gets
	CMusikSourcesCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children
	CMusikSourcesCtrl* m_wndChild;
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

