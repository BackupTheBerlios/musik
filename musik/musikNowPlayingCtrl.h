///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikPlayer;
class CmusikPrefs;
class CmusikNowPlayingInfo;
class CmusikVolumeCtrl;

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikNowPlayingCtrl( CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikNowPlayingCtrl();

	// layout and update
	void UpdateInfo( bool redraw = true );
	void RescaleInfo();

protected:

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// info object (title/artist/album)
	CmusikNowPlayingInfo* m_Info1;
	CmusikNowPlayingInfo* m_Info2;

	// volume slider
	CmusikVolumeCtrl *m_Volume;

	// our player and prefs
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CmusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

///////////////////////////////////////////////////
