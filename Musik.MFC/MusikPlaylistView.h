///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikPlaylistCtrl.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlayer;
class CMusikPrefs;
class CMusikPlaylist;

///////////////////////////////////////////////////

class CMusikPlaylistView : public CWnd
{
public:

	// construct and destruct
	CMusikPlaylistView( CFrameWnd* parent, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, CMusikPlaylist* playlist );
	virtual ~CMusikPlaylistView();

	// get the playlist control
	CMusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

protected:

	// playlist GUI object
	CMusikPlaylistCtrl* m_Playlist;

	// a pointer to the library, so we can
	// use it in file dropped operations
	CMusikLibrary* m_Library;

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// macros
	DECLARE_DYNAMIC(CMusikPlaylistView)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

///////////////////////////////////////////////////


