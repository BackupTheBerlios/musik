///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikDirSync : public CDialog
{
public:

	// construct / destruct
	CmusikDirSync(CWnd* pParent = NULL);
	virtual ~CmusikDirSync();

	// message maps
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedAdd();

	// dialog data
	enum { IDD = IDD_DIR_SYNC };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// macros
	DECLARE_DYNAMIC(CmusikDirSync)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


