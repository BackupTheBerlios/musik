///////////////////////////////////////////////////

#include "stdafx.h"

#include "Memdc.h"

#include "musik.h"
#include "musikPrefs.h"

#include "musikNowPlayingCtrl.h"
#include "musikNowPlayingInfo.h"
#include "musikVolumeCtrl.h"
#include "musikTimeCtrl.h."
#include "3rdparty/BtnST.h"

#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikNowPlayingCtrl, CWnd)

///////////////////////////////////////////////////

CmusikNowPlayingCtrl::CmusikNowPlayingCtrl( CFrameWnd* parent, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Player = player;
	m_Prefs = prefs;
	m_MainWnd = parent;
}

///////////////////////////////////////////////////

CmusikNowPlayingCtrl::~CmusikNowPlayingCtrl()
{
	delete m_Volume;
	delete m_Track;

	delete m_Play;
	delete m_Stop;
	delete m_Prev;
	delete m_Next;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_PREV, OnBtnPrev)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_NEXT, OnBtnNext)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, m_Prefs->MUSIK_COLOR_BTNFACE );

	DefWindowProc( WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0 );
}

///////////////////////////////////////////////////

int CmusikNowPlayingCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect( &rcClient );

	m_Info1 = new CmusikNowPlayingInfo( m_Player, m_Prefs );
	if ( !m_Info1->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	m_Info2 = new CmusikNowPlayingInfo( m_Player, m_Prefs );
	if ( !m_Info2->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	m_Info1->Set( "%b20 %a4" );
	m_Info2->Set( "%c by %b16 %a0 %c from the album %b14 %a1" );

	m_Volume = new CmusikVolumeCtrl( m_Prefs, m_Player );
	if ( !m_Volume->Create( TBS_VERT | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	m_Track = new CmusikTimeCtrl( m_Prefs, m_Player );
	if ( !m_Track->Create( NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	m_Font.CreateStockObject( DEFAULT_GUI_FONT );

	m_Prev = new CButtonST();
	if ( !m_Prev->Create( "prev", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_PREV ) )
		return -1;
	m_Prev->SetFont( &m_Font );

	m_Play = new CButtonST();
	if ( !m_Play->Create( "play", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_PLAY ) )
		return -1;
	m_Play->SetFont( &m_Font );

	m_Stop = new CButtonST();
	if ( !m_Stop->Create( "stop", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_STOP ) )
		return -1;
	m_Stop->SetFont( &m_Font );

	m_Next = new CButtonST();
	if ( !m_Next->Create( "next", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_NEXT ) )
		return -1;
	m_Next->SetFont( &m_Font );

	GetDC()->SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );

	return 0;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::UpdateInfo( bool refresh )
{
	m_Info1->UpdateInfo();
	m_Info2->UpdateInfo();
	RescaleInfo();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::UpdateButtonStates()
{
	if ( m_Player->IsPlaying() && m_Player->IsPaused() )
		m_Play->SetWindowText( "resume" );
	else if ( m_Player->IsPlaying() && !m_Player->IsPaused() )
		m_Play->SetWindowText( "pause" );
	else 
		m_Play->SetWindowText( "play" );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnSize(UINT nType, int cx, int cy)
{
	RescaleInfo();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	delete m_Info1;
	delete m_Info2;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::RescaleInfo()
{
	CRect rcClient;
	GetClientRect( &rcClient );

	CSize szSize = m_Info1->GetSize();
	if ( szSize.cx > ( rcClient.Width() - 240 ) )
		szSize.cx = rcClient.Width() - 240;

	CRect lpRect = CRect( CPoint( 0, 0 ), szSize );
	m_Info1->MoveWindow( lpRect );
	m_Info1->Layout();

	szSize = m_Info2->GetSize();
	if ( szSize.cx > ( rcClient.Width() - 240 ) )
		szSize.cx = rcClient.Width() - 240;

	lpRect = CRect( CPoint( 0, m_Info1->GetHeight() + 2 ), szSize );
	m_Info2->MoveWindow( lpRect );
	m_Info2->Layout();

	lpRect = CRect( CPoint( rcClient.right - 16, rcClient.top ), CSize( 16, rcClient.bottom ) );
	m_Volume->MoveWindow( lpRect );

	CPoint ptTemp( rcClient.right - 240 - 16 - 8, rcClient.bottom - 16 );
	CSize szTemp( 240, 16 );
	m_Track->MoveWindow( CRect( ptTemp, szTemp ) );

	const int nWidth = 240 / 4;
	m_Prev->MoveWindow( rcClient.right - 16 - ( nWidth * 4 ) - 8, 0, nWidth, 20 );
	m_Play->MoveWindow( rcClient.right - 16 - ( nWidth * 3 ) - 8, 0, nWidth, 20 );
	m_Stop->MoveWindow( rcClient.right - 16 - ( nWidth * 2 ) - 8, 0, nWidth, 20 );
	m_Next->MoveWindow( rcClient.right - 16 - ( nWidth * 1 ) - 8, 0, nWidth, 20 );
}

///////////////////////////////////////////////////

BOOL CmusikNowPlayingCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

// send the play command to the main UI, it will
// translate messages and (hopefully) send us
// an event back so we know it started..

void CmusikNowPlayingCtrl::OnBtnPlay()
{
	// if it's not playing, play a song
	if ( !m_Player->IsPlaying() )
	{
		int WM_PLAYER_PLAYSEL = RegisterWindowMessage( "PLAYER_PLAYSEL" );
		m_MainWnd->SendMessage( WM_PLAYER_PLAYSEL );
	}

	else if ( m_Player->IsPlaying() )
	{
		// if it is playing, pause...
		if ( !m_Player->IsPaused() )
			m_Player->Pause();

		// is paused, resume
		else
			m_Player->Resume();
	}
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnPrev()
{
	if ( m_Player->IsPlaying() )
		m_Player->Prev();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnStop()
{
	if ( m_Player->IsPlaying() )
		m_Player->Stop();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnNext()
{
	if ( m_Player->IsPlaying() )
		m_Player->Next();
}

///////////////////////////////////////////////////