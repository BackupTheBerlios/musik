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
//   CmusikPlaylistView
//
// Filename(s): 
//
//   musikPlaylistView.h, musikPlaylistView.cpp
//
// Information:
//
//   A CWnd derived object that maintains a CmusikPlaylistCtrl
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikPlaylistCtrl.h"
#include "musikPlaylistInfoCtrl.h"
#include "musikPrefs.h"
#include "musikPlaylistDropTarget.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlayer;
class CmusikPrefs;
class CmusikPlaylist;
class CmusikPlaylistDropTarget;
class CFrameWnd;

///////////////////////////////////////////////////

class CmusikPlaylistView : public CWnd
{
public:

	// construct and destruct
	CmusikPlaylistView( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid_l, UINT dropid_r, UINT dropid_sources, UINT dropid_selection_l, UINT dropid_selection_r );
	virtual ~CmusikPlaylistView();

	// get the playlist control
	CmusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

	// playlist info
	void InitPlaylistInfo();
	void DeinitPlaylistInfo();
	void UpdatePlaylistInfo();

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

protected:

	// playlist GUI object
	CmusikPlaylistCtrl* m_Playlist;
	CmusikPlaylistInfoCtrl* m_PlaylistInfo;
	
	// drop target for the playlist
	CmusikPlaylistDropTarget* m_DropTarget;

	// a pointer to the library and player,
	// so we can use it in file dropped operations
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// pointer to main window so we can
	// messages to it, such as updating the UI
	// after files have been dropped
	CWnd* m_Parent;
	
	// macros
	DECLARE_DYNAMIC(CmusikPlaylistView)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
