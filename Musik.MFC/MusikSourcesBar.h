///////////////////////////////////////////////////

#pragma once

#include "MusikSourcesCtrl.h"

///////////////////////////////////////////////////

class CMusikLibrary;

///////////////////////////////////////////////////

#ifndef baseCMusikSourcesBar
	#define baseCMusikSourcesBar CSizingControlBarCF
#endif

///////////////////////////////////////////////////

class CMusikSourcesBar : public baseCMusikSourcesBar
{
public:

	// construct / destruct
	CMusikSourcesBar( CMusikLibrary* library, int id );
	virtual ~CMusikSourcesBar();

protected:

	// children
	CMusikSourcesCtrl* m_wndChild;
	CFont m_Font;
	int m_ChildID;

	// message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

