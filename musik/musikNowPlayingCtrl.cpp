///////////////////////////////////////////////////

#include "stdafx.h"

#include "Memdc.h"

#include "musik.h"
#include "musikPrefs.h"

#include "musikNowPlayingCtrl.h"
#include "musikNowPlayingInfo.h"
#include "musikVolumeCtrl.h"

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
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
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
	m_Info2 = new CmusikNowPlayingInfo( m_Player, m_Prefs );

	if ( !m_Info1->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	if ( !m_Info2->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	m_Info1->Set( "%b20 %a4 %c  by %b16 %a0" );
	m_Info2->Set( "%c from the album %b16 %a1" );

	m_Volume = new CmusikVolumeCtrl( m_Prefs, m_Player );
	
	if ( !m_Volume->Create( TBS_VERT | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect( 50, 50, 100, 200 ), this, 123 ) )
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
	CWnd::OnSize(nType, cx, cy);
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
}

///////////////////////////////////////////////////

BOOL CmusikNowPlayingCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////
