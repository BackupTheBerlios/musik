///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikNowPlayingStatic.h"

///////////////////////////////////////////////////

class CMusikPlayer;

///////////////////////////////////////////////////

class CMusikNowPlayingCtrl : public CWnd
{
public:

	// construct and destruct
	CMusikNowPlayingCtrl( CMusikPlayer* player );
	virtual ~CMusikNowPlayingCtrl();

	// update
	void UpdateInfo();

	// layout the stuff
	void Layout( bool redraw = true );

protected:

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// our player
	CMusikPlayer* m_Player;

	// font stuff
	void InitFonts();
	CFont m_ArtistFont;
	CFont m_AlbumFont;
	CFont m_TitleFont;
	CFont m_DefaultFont;

	// artist / album / title
	CNowPlayingStatic m_Artist;
	CNowPlayingStatic m_Album;
	CNowPlayingStatic m_Title;
	CNowPlayingStatic m_By;
	CNowPlayingStatic m_From;
	
	// macros
	DECLARE_DYNAMIC(CMusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
