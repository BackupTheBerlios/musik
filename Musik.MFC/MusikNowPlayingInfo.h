///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikNowPlayingStatic.h"

///////////////////////////////////////////////////

class CMusikPlayer;
class CMusikPrefs;

///////////////////////////////////////////////////

class CMusikNowPlayingInfo : public CWnd
{
public:
	// construct and destruct
	CMusikNowPlayingInfo( CMusikPlayer* player, CMusikPrefs *prefs );
	virtual ~CMusikNowPlayingInfo();

	// updating
	void UpdateInfo( bool refresh = true );
	void Layout( bool refresh = true );

protected:

	// core stuff
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// font stuff
	void InitFonts();
	CFont m_ArtistFont;
	CFont m_AlbumFont;
	CFont m_TitleFont;
	CFont m_DefaultFont;
	int m_DefaultHeight;

	// artist / album / title
	CNowPlayingStatic m_Artist;
	CNowPlayingStatic m_Album;
	CNowPlayingStatic m_Title;
	CNowPlayingStatic m_By;
	CNowPlayingStatic m_From;

	// macros
	DECLARE_DYNAMIC(CMusikNowPlayingInfo)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

///////////////////////////////////////////////////

