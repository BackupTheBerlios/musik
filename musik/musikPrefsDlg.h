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
//   CmusikPrefsInterfaceGeneral
//
// Filename(s): 
//
//   musikPrefsDlg.h, musikPrefsDlg.cpp
//
// Information:
//
//   All the stuff relating to preferences.
// 
// Usage: 
//
//   Void.
//
// Some code adapated / borrowed from...
//
//   http://www.codeproject.com/shell/create_shortcut.asp?target=shortcut (create shortcut)
//
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

	// startup
	void EnableStartup();
	void DisableStartup();
	bool GetStartup();

	// controls
	CButton m_EnableTrayIcon;
	CButton m_PromptSavePlaylists;
	CButton m_PromptDropOnPlaylist;
	CButton m_StartWithWindows;

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceGeneral)
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	CButton m_AutoSynchronize;
	CButton m_LibraryShowAll;
};

///////////////////////////////////////////////////

