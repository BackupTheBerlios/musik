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
	void SetArgs( void* pArgs ){ m_Args = pArgs; };

	// attributes
	bool IsVisible();

	// what happened?
	void Commit();
	void Cancel();

	// overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

	// mfc message maps
	afx_msg void OnEnKillfocus();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChange();

	// args
	void* m_Args;

	// last know string
	CString m_Str;

	// visible?
	bool m_IsVisible;

	// macros
	DECLARE_DYNAMIC(CmusikEditInPlace)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

