///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikBatchTagDlg
//
// Filename(s): 
//
//   musikBatchTagDlg.h, musikBatchTagDlg.cpp
//
// Information:
//
//   Dialog used for batch tag editing
// 
// Usage: 
//
//   Ment to be modal.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "Resource.h"
#include "afxwin.h"

#include "../musikCore/include/musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikBatchTagDlg : public CDialog
{

public:

	// construct and destruct
	CmusikBatchTagDlg( CWnd* parent, CmusikSongInfoArray* songs );
	virtual ~CmusikBatchTagDlg();

	// mfc messages and overrides
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedEnbtitle();
	afx_msg void OnBnClickedEnbartist();
	afx_msg void OnBnClickedEnbalbum();
	afx_msg void OnBnClickedEnbtrack();
	afx_msg void OnBnClickedEnbgenre();
	afx_msg void OnBnClickedEnbyear();
	afx_msg void OnBnClickedEnbrating();
	virtual BOOL OnInitDialog();

	// misc
	enum { IDD = IDD_BATCH_TAG };
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	// objects
	CmusikSongInfoArray* m_Songs;
	CWnd* m_Parent;

	// controls
	CEdit m_Title;
	CEdit m_Artist;
	CEdit m_Album;
	CEdit m_Track;
	CComboBox m_Genre;
	CEdit m_Year;
	CComboBox m_Rating;

	// helpers
	void Apply();

	// macros
	DECLARE_DYNAMIC(CmusikBatchTagDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
