///////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikNameEntry : public CDialog
{
public:

	// construct and destruct
	CmusikNameEntry( CWnd* pParent, CString* str );
	virtual ~CmusikNameEntry();

	// mfc messages / overrides
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	// dialog data
	enum { IDD = IDD_NAME_ENTRY };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	//controls
	CEdit m_Edit;

	// string
	CString* m_Str;

	// macros
	DECLARE_DYNAMIC(CmusikNameEntry)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

