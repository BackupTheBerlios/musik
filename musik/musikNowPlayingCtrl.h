///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikPlayer;
class CmusikPrefs;
class CmusikNowPlayingInfo;
class CmusikVolumeCtrl;
class CmusikTimeCtrl;
class CButtonST;

///////////////////////////////////////////////////

enum
{
	MUSIK_NOWPLAYING_BTN_PLAY = 4957,
	MUSIK_NOWPLAYING_BTN_STOP,
	MUSIK_NOWPLAYING_BTN_PREV,
	MUSIK_NOWPLAYING_BTN_NEXT
};

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikNowPlayingCtrl( CFrameWnd* parent, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikNowPlayingCtrl();

	// getting children
	CmusikVolumeCtrl* GetVolumeCtrl(){ return m_Volume; }
	CmusikTimeCtrl* GetTimeCtrl(){ return m_Track; }

	// layout and update
	void UpdateInfo( bool redraw = true );
	void RescaleInfo();
	void UpdateButtonStates();

protected:

	// pointer to the main dialog
	CFrameWnd* m_MainWnd;

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnPrev();
	afx_msg void OnBtnStop();
	afx_msg void OnBtnNext();

	// info object (title/artist/album)
	CmusikNowPlayingInfo* m_Info1;
	CmusikNowPlayingInfo* m_Info2;

	// volume slider
	CmusikVolumeCtrl *m_Volume;

	// seek slider
	CmusikTimeCtrl* m_Track;

	// buttons
	CButtonST* m_Play;
	CButtonST* m_Stop;
	CButtonST* m_Prev;
	CButtonST* m_Next;

	// default font
	CFont m_Font;

	// our player and prefs
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// macros
	DECLARE_DYNAMIC(CmusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()

public:

};

///////////////////////////////////////////////////
