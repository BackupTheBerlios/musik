///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CMusikEditInPlace : public CEdit
{
public:

	// construct and destruct
	CMusikEditInPlace();
	virtual ~CMusikEditInPlace();

	// set string
	void SetString( CString str ){ m_Str = str; }

	// overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

	// mfc message maps
	afx_msg void OnEnKillfocus();

	// last know string
	CString m_Str;

	// what happened?
	void Commit();
	void Cancel();

	// macros
	DECLARE_DYNAMIC(CMusikEditInPlace)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

///////////////////////////////////////////////////

