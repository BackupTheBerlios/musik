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