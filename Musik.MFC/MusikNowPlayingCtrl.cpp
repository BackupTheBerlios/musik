///////////////////////////////////////////////////

#include "stdafx.h"

#include "Memdc.h"

#include "Musik.h"
#include "MusikPrefs.h"

#include "MusikNowPlayingCtrl.h"
#include "MusikNowPlayingInfo.h"

#include "../Musik.Core/include/MusikPlayer.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikNowPlayingCtrl, CWnd)

///////////////////////////////////////////////////

CMusikNowPlayingCtrl::CMusikNowPlayingCtrl( CMusikPlayer* player, CMusikPrefs* prefs )
{
	m_Player = player;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CMusikNowPlayingCtrl::~CMusikNowPlayingCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, GetSysColor( COLOR_BTNFACE ) );

	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

int CMusikNowPlayingCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect( &rcClient );

	m_Info1 = new CMusikNowPlayingInfo( m_Player, m_Prefs );
	m_Info2 = new CMusikNowPlayingInfo( m_Player, m_Prefs );

	if ( !m_Info1->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	if ( !m_Info2->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	m_Info1->Set( "%b20 %a4 %c  by %b16 %a0" );
	m_Info2->Set( "%c from the album %b16 %a1" );

	GetDC()->SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );

	return 0;
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::UpdateInfo( bool refresh )
{
	m_Info1->UpdateInfo();
	m_Info2->UpdateInfo();
	RescaleInfo();
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	RescaleInfo();
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	delete m_Info1;
	delete m_Info2;
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::RescaleInfo()
{
	CRect lpRect = CRect( CPoint( 0, 0 ), m_Info1->GetSize() );
	m_Info1->MoveWindow( lpRect );
	m_Info1->Layout();

	lpRect = CRect( CPoint( 0, m_Info1->GetHeight() ), m_Info2->GetSize() );
	m_Info2->MoveWindow( lpRect );
	m_Info2->Layout();
}

///////////////////////////////////////////////////

BOOL CMusikNowPlayingCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////
