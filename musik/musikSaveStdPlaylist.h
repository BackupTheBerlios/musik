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
//   CmusikSaveStdPlaylist
//
// Filename(s): 
//
//   musikSaveStdPlaylist.h, musikSaveStdPlaylist.cpp
//
// Information:
//
//   Dialog used when prompting user to save a standard playlist
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////


#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikPrefs;

///////////////////////////////////////////////////

class CmusikSaveStdPlaylist : public CDialog
{
public:

	// construct and destruct
	CmusikSaveStdPlaylist( CWnd* pParent = NULL, CmusikPrefs* prefs = NULL );
	virtual ~CmusikSaveStdPlaylist();

	// mfc message maps
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedYes();

protected:

	// overrides
	virtual void DoDataExchange( CDataExchange* pDX );

	// prefs
	CmusikPrefs* m_Prefs;

	// checkbox
	CButton m_Remember;

	DECLARE_DYNAMIC(CmusikSaveStdPlaylist)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////