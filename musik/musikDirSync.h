///////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikLibrary;

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


