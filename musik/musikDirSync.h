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
//   CmusikDirSync
//
// Filename(s): 
//
//   musikDirSync.h, musikDirSync.cpp
//
// Information:
//
//   Dialog used for adding / removing paths for
//   directory synchronization
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musik.h"

#include "../musikCore/include/musikLibrary.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

class CmusikDirSync : public CDialog
{
public:

	// construct / destruct
	CmusikDirSync( CWnd* pParent = NULL, CmusikLibrary* pLibrary = NULL );
	virtual ~CmusikDirSync();

	// message maps and overrides
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// dialog data
	enum { IDD = IDD_DIR_SYNC };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// vars
	bool m_Changed;

	// controls
	CListBox m_wndPaths;

	// core
	CmusikLibrary* m_Library;

	// helpers
	bool ValidatePath( const CString& path );
	void DeleteSel();

	// macros
	DECLARE_DYNAMIC(CmusikDirSync)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


