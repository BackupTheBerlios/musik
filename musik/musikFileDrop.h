///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

enum
{
	MUSIK_FILEDROP_CANCEL = -1,
	MUSIK_FILEDROP_ADDNOWPLAYING,
	MUSIK_FILEDROP_ADDPLAYLIST,
	MUSIK_FILEDROP_ADDLIBRARY
};	

///////////////////////////////////////////////////

class CmusikFileDrop : public CDialog
{
public:

	// construct and destruct
	CmusikFileDrop( CWnd* pParent = NULL );
	virtual ~CmusikFileDrop();
	virtual INT_PTR DoModal();

	// dialog data
	enum { IDD = IDD_FILEDROP };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// return
	int m_Ret;

	// first run
	bool m_FirstRun;

	// macros
	DECLARE_DYNAMIC(CmusikFileDrop)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadioAddPlaylist();
	afx_msg void OnBnClickedRadioAddNowPlaying();
	afx_msg void OnBnClickedRadioAddLibrary();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

///////////////////////////////////////////////////
