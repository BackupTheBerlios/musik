#pragma once


// CmusikEqualizerSets dialog

class CmusikEqualizerSets : public CDialog
{
	DECLARE_DYNAMIC(CmusikEqualizerSets)

public:
	CmusikEqualizerSets(CWnd* pParent = NULL);   // standard constructor
	virtual ~CmusikEqualizerSets();

// Dialog Data
	enum { IDD = IDD_EQUALIZER_SETS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCloseDlg();
};
