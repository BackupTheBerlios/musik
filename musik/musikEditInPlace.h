///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikEditInPlace : public CEdit
{
public:

	// construct and destruct
	CmusikEditInPlace();
	virtual ~CmusikEditInPlace();

	// set string
	void SetString( CString str ){ m_Str = str; }
	CString GetString(){ return m_Str; }

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
	DECLARE_DYNAMIC(CmusikEditInPlace)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

///////////////////////////////////////////////////

