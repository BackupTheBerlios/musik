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
	CMusikSourcesBar( CMusikLibrary* library );
	virtual ~CMusikSourcesBar();

protected:

	// children
	CMusikSourcesCtrl* m_wndChild;
	CFont m_Font;

	// message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

