///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "afxwin.h"

#include "../musikCore/include/musikArrays.h"

///////////////////////////////////////////////////

class CmusikLibrary;

///////////////////////////////////////////////////

class CmusikEqualizerSets : public CDialog
{
public:

	// construct and destruct
	CmusikEqualizerSets( CWnd* pParent = NULL, CmusikLibrary* library = NULL );
	virtual ~CmusikEqualizerSets();

	// mfc message maps / overrides
	afx_msg void OnBnClickedCloseDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLoadActiveFromSelected();
	afx_msg void OnBnClickedSaveActiveAsSelected();
	afx_msg void OnBnClickedSaveActiveAsNew();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// misc
	enum { IDD = IDD_EQUALIZER_SETS };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX); 

	// core
	CmusikLibrary* m_Library;

	int GetIndex();

	// internal list of equalizer ids
	// obtained when when dialog is first
	// intialized, and maintained while the
	// dialog is open
	CIntArray m_IDs;
	CStdStringArray m_Items;

    // parent window to post events to
	CWnd* m_Parent;

	// controls
	CListBox m_PresetBox;

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerSets)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


