#pragma once


// CMusikPlaylistView
#include "MusikPlaylistCtrl.h"

class CMusikPlaylistView : public CWnd
{
	DECLARE_DYNAMIC(CMusikPlaylistView)

public:
	CMusikPlaylistView();
	virtual ~CMusikPlaylistView();

protected:
	CMusikPlaylistCtrl m_Playlist;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

