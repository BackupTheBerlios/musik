///////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

enum
{
	MUSIK_FILEDROP_CANCEL = -1,
	MUSIK_FILEDROP_ADDNOWPLAYING,
	MUSIK_FILEDROP_ADDPLAYLIST,
	MUSIK_FILEDROP_ADDLIBRARY
};	

///////////////////////////////////////////////////

class CmusikPrefs;

///////////////////////////////////////////////////

class CmusikFileDrop : public CDialog
{
public:

	// construct and destruct
	CmusikFileDrop( CWnd* pParent = NULL, CmusikPrefs* pPrefs = NULL );
	virtual ~CmusikFileDrop();
	virtual INT_PTR DoModal();

	// message maps
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadioAddPlaylist();
	afx_msg void OnBnClickedRadioAddNowPlaying();
	afx_msg void OnBnClickedRadioAddLibrary();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedOk();

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// prefs
	CmusikPrefs* m_Prefs;

	// return
	int m_Ret;

	// remember checkbox
	CButton m_Remember;

	// first run
	bool m_FirstRun;

	// macros
	DECLARE_DYNAMIC(CmusikFileDrop)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
