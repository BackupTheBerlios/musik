///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikPlaylistCtrl.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlayer;
class CmusikPrefs;
class CmusikPlaylist;
class CmusikPlaylistDropTarget;

///////////////////////////////////////////////////

class CmusikPlaylistView : public CWnd
{
public:

	// construct and destruct
	CmusikPlaylistView( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, int dropid );
	virtual ~CmusikPlaylistView();

	// get the playlist control
	CmusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

protected:

	// playlist GUI object
	CmusikPlaylistCtrl* m_Playlist;

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
	
	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// macros
	DECLARE_DYNAMIC(CmusikPlaylistView)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
