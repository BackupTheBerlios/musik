// Musik.h : main header file for the Musik application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CMusikApp:
// See Musik.cpp for the implementation of this class
//

class CMusikApp : public CWinApp
{
public:
	CMusikApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMusikApp theApp;