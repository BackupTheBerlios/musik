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
//   CmusikTagDlg
//
// Filename(s): 
//
//   musikTagDlg.h, musikTagDlg.cpp
//
// Information:
//
//   The tag editing dialog 
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "Resource.h"
#include "../musikCore/include/musikLibrary.h"

///////////////////////////////////////////////////

class CmusikTagDlg : public CDialog
{
	
public:
	
	// construct and destruct 
	CmusikTagDlg( CWnd* parent, CmusikPlaylist* playlist, CmusikLibrary* library ); 
	virtual ~CmusikTagDlg();

	// helpers
	void UpdateSel( int index = 0 );
	void UpdatePlaylist( CmusikPlaylist* playlist ){ m_Playlist = playlist; }

	// misc
	enum { IDD = IDD_TAG_PROPERTIES };
	virtual void DoDataExchange(CDataExchange* pDX);  
	void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	// mfc messages
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnChangeField();
	afx_msg void OnBnClickedApply();

protected:

	// update info
	void UpdateDlg();
	void SaveCurr();
	void PromptSave();

	void OnEnter();
	void OnShiftEnter();

	void SelectFocused();

	bool m_Modified;
	bool m_Updating;

	// from parent
	CWnd* m_Parent;
	CmusikLibrary* m_Library;
	CmusikPlaylist* m_Playlist;
	CmusikSongInfo m_Song;

	int m_SongIndex;

	// macros
	DECLARE_DYNAMIC(CmusikTagDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
