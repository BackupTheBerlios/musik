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

	// saving
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// dialog data
	enum { IDD = IDD_PROPPAGE_INTERFACE_GENERAL };

	// overrides
	virtual BOOL OnInitDialog();

protected:

	// misc
	virtual void DoDataExchange( CDataExchange* pDX ); 

	// controls
	CButton m_EnableTrayIcon;
	CButton m_PromptSavePlaylists;
	CButton m_PromptDropOnPlaylist;

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceGeneral)
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

///////////////////////////////////////////////////

