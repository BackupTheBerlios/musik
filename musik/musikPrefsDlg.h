///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikPropertyPage.h"
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikPrefs;

///////////////////////////////////////////////////

// General::Interface

///////////////////////////////////////////////////

class CmusikPrefsInterfaceGeneral : public CmusikPropertyPage
{
public:

	// construct / destruct
	CmusikPrefsInterfaceGeneral( CmusikPrefs* prefs );
	virtual ~CmusikPrefsInterfaceGeneral();

	// dialog data
	enum { IDD = IDD_PROPPAGE_INTERFACE_GENERAL };

protected:

	// misc
	virtual void DoDataExchange( CDataExchange* pDX ); 

	// controls
	CButton m_EnableTrayIcon;

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceGeneral)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

