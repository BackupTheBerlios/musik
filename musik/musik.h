///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikApp
//
// Filename(s): 
//
//   musik.h, musik.cpp
//
// Information:
//
//   musikCube Entry Point
// 
// Usage: 
//
//   Void.
//
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
	virtual ~CmusikApp();

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