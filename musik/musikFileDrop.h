///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikFileDrop : public CDialog
{
public:

	// construct and destruct
	CmusikFileDrop( CWnd* pParent = NULL );
	virtual ~CmusikFileDrop();

	// dialog data
	enum { IDD = IDD_FILEDROP };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);

	// macros
	DECLARE_DYNAMIC(CmusikFileDrop)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
