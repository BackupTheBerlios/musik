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
//   The tag editing dialog is treated as a child of
//   the playlist window -- therefor it has a pointer
//   to its parent. it is allowed to talk back and forth
//   directly without posting messages. 
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
	void UpdateSongInfo( int item = 0 );
	void UpdateSel( int index = 0 );

	// misc
	enum { IDD = IDD_TAG_PROPERTIES };
	virtual void DoDataExchange(CDataExchange* pDX);  
	void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

	// from parent
	CWnd* m_Parent;
	CmusikLibrary* m_Library;
	CmusikPlaylist* m_Playlist;
	CmusikSongInfoArray m_Songs;
	int m_SongIndex;

	DECLARE_DYNAMIC(CmusikTagDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
