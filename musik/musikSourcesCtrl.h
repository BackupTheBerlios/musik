///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "../musikCore/include/musikDir.h"

#include "musikEditInPlace.h"
#include "musikDockBar.h"

#include "3rdparty/musikPropTree.h"

#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CmusikPropTreeItem*> CmusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CmusikPlayer;
class CmusikSourcesDropTarget;
class CmusikSourcesCtrl;

///////////////////////////////////////////////////

#ifndef baseCmusikSourcesBar
	#define baseCmusikSourcesBar CmusikDockBar
#endif

///////////////////////////////////////////////////

#define IDC_SOURCES 1000

///////////////////////////////////////////////////

// CmusikSourcesBar

class CmusikSourcesBar : public baseCmusikSourcesBar
{
public:

	// construct / destruct
	CmusikSourcesBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid );
	virtual ~CmusikSourcesBar();

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

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikSourcesCtrl

class CmusikSourcesCtrl : public CmusikPropTree
{

	friend class CmusikSourcesDropTarget;

public:

	// construct / destruct
	CmusikSourcesCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid );
	virtual ~CmusikSourcesCtrl();

	// actions
	void KillFocus( bool redraw = true );
	void FocusLibrary();
	void FocusNowPlaying();

	// overrides
	void DoDrag( CmusikPropTreeItem* pItem );
	virtual CmusikPropTreeItem* FindItem( const POINT& pt );

	// message maps
	void OnDropFiles(HDROP hDropInfo);

protected:

	// pointer to main frame
	CFrameWnd* m_Parent;

	// item hover fun
	virtual void OnNewHoveredItem( int nIndex );
	CmusikPropTreeItem* FindItemAtIndex( int nIndex );

   	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// custom message maps
	LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );

	// drop target for the playlist
	CmusikSourcesDropTarget* m_DropTarget;
	bool m_DropArrange;

	// create default headers
	void InitItems();
	CmusikPropTreeItem* m_LibrariesRoot;
	CmusikPropTreeItem* m_StdPlaylistRoot;
	CmusikPropTreeItem* m_DynPlaylistRoot;

	// loading and finding the items
	void LoadLibraries();
	void LoadStdPlaylists();
	void LoadDynPlaylists();
	int FindInLibraries( CmusikPropTreeItem* pItem );
	int FindInStdPlaylists( CmusikPropTreeItem* pItem );
	int FindInDynPlaylists( CmusikPropTreeItem* pItem );

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

	// startup bool, if true on startup
	// the library will be selected by
	// default...
	bool m_Startup;

	// stuff relating to the core
	CmusikPlayer* m_Player;

	// items that will show up in the list
	CmusikSourcesItemPtrArray m_Libraries;
	CmusikSourcesItemPtrArray m_StdPlaylists;
	CmusikSourcesItemPtrArray m_DynPlaylists;
};

///////////////////////////////////////////////////