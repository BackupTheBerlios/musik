///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CMusikPlayer;
class CMusikPrefs;
class CMusikNowPlayingInfo;

///////////////////////////////////////////////////

class CMusikNowPlayingCtrl : public CWnd
{
public:

	// construct and destruct
	CMusikNowPlayingCtrl( CMusikPlayer* player, CMusikPrefs* prefs );
	virtual ~CMusikNowPlayingCtrl();

	// layout and update
	void UpdateInfo( bool redraw = true );
	void RescaleInfo();

protected:

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// info object (title/artist/album)
	CMusikNowPlayingInfo* m_Info1;
	CMusikNowPlayingInfo* m_Info2;

	// our player and prefs
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CMusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

///////////////////////////////////////////////////
