#pragma once


// CMusikPlaylistView
#include "MusikPlaylistCtrl.h"

class CMusikLibrary;
class CMusikPrefs;
class CMusikPlaylist;

class CMusikPlaylistView : public CWnd
{
	DECLARE_DYNAMIC(CMusikPlaylistView)

public:
	CMusikPlaylistView( CMusikLibrary* library, CMusikPrefs* prefs, CMusikPlaylist* playlist );
	virtual ~CMusikPlaylistView();

	CMusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

protected:
	CMusikPlaylistCtrl* m_Playlist;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


