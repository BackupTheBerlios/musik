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
	CmusikSaveStdPlaylist( CWnd* pParent = NULL, CmusikPrefs* prefs = NULL );   // standard constructor
	virtual ~CmusikSaveStdPlaylist();

	// mfc message maps
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedYes();

	// Dialog Data
	enum { IDD = IDD_SAVESTDPLAYLIST };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	// prefs
	CmusikPrefs* m_Prefs;

	DECLARE_DYNAMIC(CmusikSaveStdPlaylist)
	DECLARE_MESSAGE_MAP()
public:
	CButton m_Remember;
};

///////////////////////////////////////////////////