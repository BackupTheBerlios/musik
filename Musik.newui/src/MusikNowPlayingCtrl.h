#ifndef C_MUSIK_NOW_PLAYING_CTRL
#define C_MUSIK_NOW_PLAYING_CTRL

#include "wx/wxprec.h"

enum EMUSIK_NOWPLAYINGCTRL_ID
{
	MUSIK_NOWPLAYINGCTRL_PLAYPAUSE = 0,
	MUSIK_NOWPLAYINGCTRL_NEXT,
	MUSIK_NOWPLAYINGCTRL_PREV,
	MUSIK_NOWPLAYINGCTRL_STOP,
	MUSIK_NOWPLAYINGCTRL_VOLUME
};

class CMusikNowPlayingCtrl : public wxPanel
{
public:
	CMusikNowPlayingCtrl( wxWindow *pParent, wxWindowID id );
	~CMusikNowPlayingCtrl();

	void ResetInfo();


	//--- left and right panels ---//
	wxPanel* pLeftPanel;
	wxPanel* pRightPanel;

	//--- sizers ---//
	wxBoxSizer	*m_TopSizer;
	wxBoxSizer	*m_BottomSizer;
	wxBoxSizer	*m_MainSizer;

	wxBoxSizer *hsArtistTime;
	wxBoxSizer *vsLeftCol;
	wxBoxSizer *hsButtons;
	wxBoxSizer *vsRightCol;
	wxBoxSizer *hsCols;

	DECLARE_EVENT_TABLE()
private:

	//---------------------------------//
	//--- playback control buttons	---//
	//---------------------------------//
	wxBitmapButton	*m_btnPlayPause;
	wxBitmapButton	*m_btnNext;
	wxBitmapButton	*m_btnPrev;
	wxBitmapButton	*m_btnStop;
	wxBitmapButton	*m_btnVolume;

	//---------------------------------//
	//--- playback control icons	---//
	//---------------------------------//
	wxBitmap	m_bmPrev,	m_bmPrevDown;
	wxBitmap	m_bmNext,	m_bmNextDown;
	wxBitmap	m_bmPlay,	m_bmPlayDown;
	wxBitmap	m_bmPause,	m_bmPauseDown;
	wxBitmap	m_bmStop,	m_bmStopDown;
	wxBitmap	m_bmVolume,	m_bmVolumeDown;

	//---------------------------------//
	//--- now playing text objects	---//
	//---------------------------------//
	wxStaticText *m_stSong;
	wxStaticText *m_stArtist;
	wxStaticText *m_stCurtime;

	//---------------------------------//
	//--- seek bar					---//
	//---------------------------------//
	wxGauge* m_Seek;
	
	//---------------------------------//
	//--- fonts						---//
	//---------------------------------//
	wxFont	m_fntSong;
	wxFont	m_fntArtist;
	wxFont	m_fntTime;

	void SetupFonts();
	void SetupIcons();
};

#endif
