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
//   CmusikSourcesCtrl, CmusikSourcesBar
//
// Filename(s): 
//
//   musikSourceCtrl.h, musikSourceCtrl.cpp
//
// Information:
//
//   The "Sources" panel. 
// 
// Usage: 
//
//   Void.
//
// Code adapted from:
//
//   CPropTree: http://www.codeproject.com/treectrl/proptree.asp
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikEditInPlace.h"
#include "musikDockBar.h"
#include "musikSourcesDropTarget.h"

#include "../musikCore/include/musikDir.h"
#include "../musikCore/include/musikPlayer.h"

#include "3rdparty/musikPropTree.h"

#include <vector>

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

typedef std::vector<CmusikPropTreeItem*> CmusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CmusikSourcesCtrl;

///////////////////////////////////////////////////

#ifndef baseCmusikSourcesBar
	#define baseCmusikSourcesBar CmusikDockBar
#endif

///////////////////////////////////////////////////

#define IDC_SOURCES 1000

///////////////////////////////////////////////////

enum
{
	MUSIK_SOURCES_EDITINPLACE_RENAME = 493,
	MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH,
	MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST,
	MUSIK_SOURCES_EDITINPLACE_NEWSUBLIBRARYRARY,
};

///////////////////////////////////////////////////

// CmusikSourcesBar

///////////////////////////////////////////////////

class CmusikSourcesBar : public baseCmusikSourcesBar
{
	friend class CmusikSourcesCtrl;

public:

	// construct / destruct
	CmusikSourcesBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid, UINT pldropid_r, UINT sbdropid_r );
	virtual ~CmusikSourcesBar();

	// options
	virtual void OnOptions(){}
	void ShowMenu( bool force_show = false );

	// gets
	CmusikSourcesCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children
	CmusikSourcesCtrl* m_wndChild;
	CFont m_Font;
	CFrameWnd* m_Parent;

	// message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemChanged( NMHDR* pNotifyStruct, LRESULT* plResult );
	afx_msg void OnSourcesRename();
	afx_msg void OnSourcesDelete();
	afx_msg void OnSourcesShuffleplaylist();
	afx_msg void OnShowSublibraries();
	afx_msg void OnShowPlaylists();
	afx_msg void OnShowSearch();

	// macros
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

#define IDC_EDITINPLACE 1538

///////////////////////////////////////////////////

class CmusikSourcesCtrl : public CmusikPropTree
{
	friend class CmusikSourcesBar;
	friend class CmusikSourcesDropTarget;

public:

	// construct / destruct
	CmusikSourcesCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid, UINT pldropid_r, UINT sbdropid_r );
	virtual ~CmusikSourcesCtrl();

	// actions
	void KillFocus( bool redraw = true );
	void FocusLibrary();
	void FocusNowPlaying();
	void FocusQuickSearch();
	void RenameItem( CmusikPropTreeItem* pItem = NULL, int mode = MUSIK_SOURCES_EDITINPLACE_RENAME, CPoint loc = CPoint( -1, -1 ) );
	void DeleteSel();
	void QuickSearch();

	void Reset(){ DeinitItems(); InitItems(); }

	// overrides
	void DoDrag( CmusikPropTreeItem* pItem );

	// message maps
	void OnDropFiles( HDROP hDropInfo, bool right_button = false );

protected:

	// pointer to main frame
	CFrameWnd* m_Parent;

   	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// custom message maps
	afx_msg LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEditChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

	// drop target for the playlist
	CmusikSourcesDropTarget* m_DropTarget;
	bool m_DropArrange;

	// create default headers
	void InitItems();
	void DeinitItems();
	CmusikPropTreeItem* m_LibRoot;
	CmusikPropTreeItem* m_SrcRoot;
	CmusikPropTreeItem* m_SubLibRoot;
	CmusikPropTreeItem* m_StdPlaylistRoot;

	// loading and finding the items
	void LibLoad();
	void SrcLoad();
	void StdPlaylistsLoad( int type );

	// macros
	DECLARE_DYNAMIC(CmusikSourcesCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// for some reason creating a CmusikDir
	// object during a drop operation causes
	// it to fail. i can't imagine why, its 
	// just initializing a few variables to 
	// NULL in the constructor. at any rate,
	// its created here...
	CmusikDir m_Dir;

	// F2...
	CmusikEditInPlace m_EditInPlace;

	// quick search
	void FinishQuickSearch();

	// mouse tracking?
	bool m_MouseTrack;

	// Create... new playlist
	CmusikPropTreeItem* CreateNewStdPlaylist( int type, CmusikStringArray* files = NULL );

	// startup bool, if true on startup
	// the library will be selected by
	// default...
	bool m_Startup;

	// stuff relating to the core
	CmusikPlayer* m_Player;

	// items that will show up in the list
	CmusikSourcesItemPtrArray m_Lib;
	CmusikSourcesItemPtrArray m_Src;
	CmusikSourcesItemPtrArray m_SubLibs;
	CmusikSourcesItemPtrArray m_StdPlaylists;
};

///////////////////////////////////////////////////