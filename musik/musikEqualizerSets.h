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
//   CmusikEqualizerSets
//
// Filename(s): 
//
//   musikEqualizerSets.h, musikEqualizerSets.cpp
//
// Information:
//
//   Dialog used for adding / removing / managing
//   equalizer presets. It front ends the CmusikLibrary.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "afxwin.h"

#include "../musikCore/include/musikArrays.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikEQSettings;

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
	afx_msg void OnBnClickedSaveActiveAsSelected();
	afx_msg void OnBnClickedSaveActiveAsNew();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLbnDblclkPresetBox();
	afx_msg void OnBnClickedRenameSel();
	afx_msg void OnLbnSelchangePresetBox();
	afx_msg void OnBnClickedResetDefaults();
	afx_msg void OnBnClickedSetAsDefault();
	afx_msg void OnBnClickedDeleteSel();

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

	// getting active
	void GetActiveEqualizer( CmusikEQSettings* settings );

    // parent window to post events to
	CWnd* m_Parent;

	// controls
	CListBox m_PresetBox;

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerSets)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


