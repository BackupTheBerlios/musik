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

	m_Info = new CMusikNowPlayingInfo( m_Player, m_Prefs );

	if ( !m_Info->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;

	m_Info->Set( "%b20 %a4 %c  in the year %a2" );
	
	GetDC()->SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );

	return 0;
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::UpdateInfo( bool refresh )
{
	m_Info->UpdateInfo();
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect lpRect = CRect( CPoint( 0, 0 ), m_Info->GetSize() );
	m_Info->MoveWindow( lpRect );
	m_Info->Layout();
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	delete m_Info;
}

///////////////////////////////////////////////////
