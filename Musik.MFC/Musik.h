///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "3rdparty/singleinstance.h"
#include "../Musik.Core/include/stdstring.h"

///////////////////////////////////////////////////

class CMusikApp : public CWinApp, CSingleInstance
{
public:

	// construct and destruct
	CMusikApp();

	// overrides
	virtual BOOL InitInstance();
	virtual void WakeUp( LPCSTR aCommandLine ) const;

	// message maps
	afx_msg void OnAppAbout();

	// macros
	DECLARE_MESSAGE_MAP()

private:

	void Play( const CStdString& fn );
};

///////////////////////////////////////////////////

extern CMusikApp theApp;

///////////////////////////////////////////////////