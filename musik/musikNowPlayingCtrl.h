///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikPlayer;
class CmusikPrefs;
class CmusikNowPlayingInfo;
class CmusikVolumeCtrl;
class CmusikTimeCtrl;

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikNowPlayingCtrl( CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikNowPlayingCtrl();

	// getting children
	CmusikVolumeCtrl* GetVolumeCtrl(){ return m_Volume; }
	CmusikTimeCtrl* GetTimeCtrl(){ return m_Track; }

	// layout and update
	void UpdateInfo( bool redraw = true );
	void RescaleInfo();

protected:

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	// info object (title/artist/album)
	CmusikNowPlayingInfo* m_Info1;
	CmusikNowPlayingInfo* m_Info2;

	// volume slider
	CmusikVolumeCtrl *m_Volume;

	// seek slider
	CmusikTimeCtrl* m_Track;

	// our player and prefs
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CmusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()

public:

};

///////////////////////////////////////////////////
