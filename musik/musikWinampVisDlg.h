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
//   CmusikWinampVisDlg
//
// Filename(s): 
//
//   musikWinampVisDlg.h, musikWinampVisDlg.cpp
//
// Information:
//
//   Winamp vis chooser
// 
// Usage: 
//
//   Its a dialog. Use it.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "Resource.h"
#include "musikPrefs.h"

///////////////////////////////////////////////////

class CmusikWinampVisDlg : public CDialog
{

public:

	// construct and destruct
	CmusikWinampVisDlg( CFrameWnd* parent, CmusikStringArray* vislist, CmusikPrefs* prefs ); 
	virtual ~CmusikWinampVisDlg();

	// messages and overrides
	afx_msg void OnClose();
	afx_msg void OnPluginBoxChange();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSettings();
	
	// helpers
	int GetIndex();

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);
	enum { IDD = IDD_WINAMP_VIS };

protected:

	// objects
	CFrameWnd* m_Parent;
	CmusikStringArray* m_VisList;
	CComboBox m_VisDropDown;
	CListBox m_VisListBox;
	CmusikPrefs* m_Prefs;
	
	// macros
	DECLARE_DYNAMIC(CmusikWinampVisDlg)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
