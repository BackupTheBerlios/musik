/*
 *  NowPlayingCtrl.h
 *
 *  The "Now Playing" information
 *	This is the control visible on the very bottom of the frame
 *	Includes currently playing songs and playlist navigation
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_NOWPLAYING_CTRL
#define MUSIK_NOWPLAYING_CTRL

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

enum EMUSIK_NOWPLAYINGCTRL_ID
{
	MUSIK_NOWPLAYINGCTRL_PLAYPAUSE = 0,
	MUSIK_NOWPLAYINGCTRL_NEXT,
	MUSIK_NOWPLAYINGCTRL_PREV,
	MUSIK_NOWPLAYINGCTRL_STOP,
	MUSIK_NOWPLAYINGCTRL_VOLUME
};

//--- bitmap button and seek bar event handler ---//
#include "BtnDown.h"
#include "GaugeSeek.h"

class CNowPlayingCtrl : public wxPanel
{
public:
	CNowPlayingCtrl( wxWindow *pParent );
	~CNowPlayingCtrl();

	//--- events ---//
	void PlayerStop			( wxCommandEvent&	WXUNUSED(event) );
	void PlayerPlayPause	( wxCommandEvent&	WXUNUSED(event) );	
	void PlayerNext			( wxCommandEvent&	WXUNUSED(event) );
	void PlayerPrev			( wxCommandEvent&	WXUNUSED(event) );	
	void PlayerVolume		( wxCommandEvent&	WXUNUSED(event) );	
	void OnTimer			( wxTimerEvent&		WXUNUSED(event) );

	//--- play control bitmaps ---//
	wxBitmap	bmPrev,		bmPrevDown;
	wxBitmap	bmNext,		bmNextDown;
	wxBitmap	bmPlay,		bmPlayDown;
	wxBitmap	bmPause,	bmPauseDown;
	wxBitmap	bmStop,		bmStopDown;
	wxBitmap	bmVolume,	bmVolumeDown;

	//--- playlist buttons ---//
	wxBitmapButton	*btnPlayPause;
	wxBitmapButton	*btnNext;
	wxBitmapButton	*btnPrev;
	wxBitmapButton	*btnStop;
	wxBitmapButton	*btnVolume;

	//--- seek bar ---//
	wxGauge *gSeek;

	//--- seek bar custom event ---//
	CGaugeSeekEvt *pSeekEvt;

	//--- now playing text objects ---//
	wxStaticText *stSong;
	wxStaticText *stArtist;
	wxStaticText *stCurtime;

	//--- left and right panels ---//
	wxPanel* pLeftPanel;
	wxPanel* pRightPanel;

	//--- sizers ---//
	wxBoxSizer *hsArtistTime;
	wxBoxSizer *vsLeftCol;
	wxBoxSizer *hsButtons;
	wxBoxSizer *vsRightCol;
	wxBoxSizer *hsCols;

	//--- button down events (win32) ---//
	#ifdef __WXMSW__
	CBtnDownEvt *pPrevEvt;
	CBtnDownEvt *pNextEvt;
	CBtnDownEvt *pPlayEvt;
	CBtnDownEvt *pPauseEvt;
	CBtnDownEvt *pStopEvt;
	CBtnDownEvt *pVolumeEvt;
	#endif

	//--- funcs ---//
	void PlayBtnToPauseBtn();
	void PauseBtnToPlayBtn();
	void UpdateInfo( wxString filename );
	void SetTime( wxString sTimeStr );

	DECLARE_EVENT_TABLE()
private:
	void UpdateTime();
	void StartTimer();
	void KillTimer();
	wxTimer *pSecTimer;
};

#endif
