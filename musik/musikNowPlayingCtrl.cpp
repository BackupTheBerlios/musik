///////////////////////////////////////////////////

#include "stdafx.h"

#include "Memdc.h"

#include "musik.h"
#include "musikPrefs.h"

#include "musikNowPlayingCtrl.h"
#include "musikNowPlayingInfo.h"
#include "musikVolumeCtrl.h"
#include "musikTimeCtrl.h."

#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikNowPlayingCtrl, CWnd)

///////////////////////////////////////////////////

CmusikNowPlayingCtrl::CmusikNowPlayingCtrl( CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Player = player;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikNowPlayingCtrl::~CmusikNowPlayingCtrl()
{
	delete m_Volume;
	delete m_Track;
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

	m_Info1->Set( "%b20 %a4 %c  by %b16 %a0" );
	m_Info2->Set( "%c from the album %b16 %a1" );

	m_Volume = new CmusikVolumeCtrl( m_Prefs, m_Player );
	if ( !m_Volume->Create( TBS_VERT | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	m_Track = new CmusikTimeCtrl( m_Prefs, m_Player );
	if ( !m_Track->Create( NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	m_Prev = new CButton();
	if ( !m_Prev->Create( "|<", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_PREV ) )
		return -1;

	m_Play = new CButton();
	if ( !m_Play->Create( "|>", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_PLAY ) )
		return -1;

	m_Stop = new CButton();
	if ( !m_Stop->Create( "[ ]", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_STOP ) )
		return -1;

	m_Next = new CButton();
	if ( !m_Next->Create( ">|", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_NEXT ) )
		return -1;

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
	CRect lpRect = CRect( CPoint( 0, 0 ), m_Info1->GetSize() );
	m_Info1->MoveWindow( lpRect );
	m_Info1->Layout();

	lpRect = CRect( CPoint( 0, m_Info1->GetHeight() + 2 ), m_Info2->GetSize() );
	m_Info2->MoveWindow( lpRect );
	m_Info2->Layout();

	CRect rcClient;
	GetClientRect( &rcClient );
	lpRect = CRect( CPoint( rcClient.right - 16, rcClient.top ), CSize( 16, rcClient.bottom ) );
	m_Volume->MoveWindow( lpRect );

	CPoint ptTemp( rcClient.right - 240 - 16, rcClient.bottom - 16 );
	CSize szTemp( 240, 16 );
	m_Track->MoveWindow( CRect( ptTemp, szTemp ) );

	m_Prev->MoveWindow( 0, 50, 24, 24 );
	m_Play->MoveWindow( 24, 50, 24, 24 );
	m_Stop->MoveWindow( 48, 50, 24, 24 );
	m_Next->MoveWindow( 72, 50, 24, 24 );
}

///////////////////////////////////////////////////

BOOL CmusikNowPlayingCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnPlay()
{
	MessageBox( "Play" );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnPrev()
{
	MessageBox( "Prev" );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnStop()
{
	MessageBox( "Stop" );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnNext()
{
	MessageBox( "Next" );
}

///////////////////////////////////////////////////