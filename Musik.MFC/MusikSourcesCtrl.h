///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "3rdparty/PropTree.h"
#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CPropTreeItem*> CMusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CMusikPlayer;
class CMusikSourcesDropTarget;

///////////////////////////////////////////////////

class CMusikSourcesCtrl : public CPropTree
{
public:

	// construct / destruct
	CMusikSourcesCtrl( CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, UINT dropid );
	virtual ~CMusikSourcesCtrl();

	// actions
	void KillFocus( bool redraw = true );

	void FocusLibrary();
	void FocusNowPlaying();

	// message maps
	void OnDropFiles(HDROP hDropInfo);

protected:

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// drop target for the playlist
	CMusikSourcesDropTarget* m_DropTarget;

	// create default headers
	void InitItems();
	CPropTreeItem* m_LibrariesRoot;
	CPropTreeItem* m_StdPlaylistRoot;
	CPropTreeItem* m_DynPlaylistRoot;

	// loading the items
	void LoadLibraries();
	void LoadStdPlaylists();
	void LoadDynPlaylists();

	// macros
	DECLARE_DYNAMIC(CMusikSourcesCtrl)
	DECLARE_MESSAGE_MAP()

private:

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