///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "3rdparty/WinAppEx.h"
#include "../musikCore/include/stdstring.h"

///////////////////////////////////////////////////

class CmusikApp : public CWinAppEx
{
public:

	// construct and destruct
	CmusikApp();

	// overrides
	virtual BOOL InitInstance();
	virtual BOOL OnAnotherInstanceMessage( LPMSG pMsg );

	// message maps
	afx_msg void OnAppAbout();

	// macros
	DECLARE_MESSAGE_MAP()

private:

	void Play( const CString& fn );
};

///////////////////////////////////////////////////

extern CmusikApp theApp;

///////////////////////////////////////////////////