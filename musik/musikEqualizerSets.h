///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikEqualizerSets : public CDialog
{
public:

	// construct and destruct
	CmusikEqualizerSets(CWnd* pParent = NULL);
	virtual ~CmusikEqualizerSets();

	// mfc message maps
	afx_msg void OnBnClickedCloseDlg();

	// misc
	enum { IDD = IDD_EQUALIZER_SETS };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);  

    // parent window to post events to
	CWnd* m_Parent;

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerSets)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


