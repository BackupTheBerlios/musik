///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikEditInPlace.h"
#include "3rdparty/MusikPropTree.h"

#include "../Musik.Core/include/MusikDir.h"

#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CMusikPropTreeItem*> CMusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CMusikPlayer;
class CMusikSourcesDropTarget;

///////////////////////////////////////////////////

class CMusikSourcesCtrl : public CMusikPropTree
{
public:

	// construct / destruct
	CMusikSourcesCtrl( CFrameWnd* parent, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, UINT dropid );
	virtual ~CMusikSourcesCtrl();

	// actions
	void KillFocus( bool redraw = true );
	void FocusLibrary();
	void FocusNowPlaying();

	// overrides
	void DoDrag( CMusikPropTreeItem* pItem );
	virtual CMusikPropTreeItem* FindItem( const POINT& pt );

	// message maps
	void OnDropFiles(HDROP hDropInfo);

protected:

	// pointer to main frame
	CFrameWnd* m_Parent;

	// item hover fun
	virtual void OnNewHoveredItem( int nIndex );
	CMusikPropTreeItem* FindItemAtIndex( int nIndex );

   	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );

	// drop target for the playlist
	CMusikSourcesDropTarget* m_DropTarget;

	// create default headers
	void InitItems();
	CMusikPropTreeItem* m_LibrariesRoot;
	CMusikPropTreeItem* m_StdPlaylistRoot;
	CMusikPropTreeItem* m_DynPlaylistRoot;

	// loading the items
	void LoadLibraries();
	void LoadStdPlaylists();
	void LoadDynPlaylists();

	// macros
	DECLARE_DYNAMIC(CMusikSourcesCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// for some reason creating a CMusikDir
	// object during a drop operation causes
	// it to fail. i can't imagine why, its 
	// just initializing a few variables to 
	// NULL in the constructor. at any rate,
	// its created here...
	CMusikDir m_Dir;

	// F2...
	CMusikEditInPlace m_EditInPlace;

	// startup bool, if true on startup
	// the library will be selected by
	// default...
	bool m_Startup;

	// stuff relating to the core
	CMusikPlayer* m_Player;

	// items that will show up in the list
	CMusikSourcesItemPtrArray m_Libraries;
	CMusikSourcesItemPtrArray m_StdPlaylists;
	CMusikSourcesItemPtrArray m_DynPlaylists;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

///////////////////////////////////////////////////