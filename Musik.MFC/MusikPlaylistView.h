///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikPlaylistCtrl.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlayer;
class CMusikPrefs;
class CMusikPlaylist;
class CMusikPlaylistDropTarget;

///////////////////////////////////////////////////

class CMusikPlaylistView : public CWnd
{
public:

	// construct and destruct
	CMusikPlaylistView( CFrameWnd* parent, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, int dropid );
	virtual ~CMusikPlaylistView();

	// get the playlist control
	CMusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

protected:

	// playlist GUI object
	CMusikPlaylistCtrl* m_Playlist;

	// drop target for the playlist
	CMusikPlaylistDropTarget* m_DropTarget;

	// a pointer to the library and player,
	// so we can use it in file dropped operations
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

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
	DECLARE_DYNAMIC(CMusikPlaylistView)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
