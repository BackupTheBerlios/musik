///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikTimeCtrl.h"

#include "musikDynamicText.h"
#include "musikTrackCtrl.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

#include "MEMDC.H"
#include ".\musiktimectrl.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTimeCtrl, CWnd)

///////////////////////////////////////////////////

#define MUSIK_SEEK_TIMER WM_USER + 1

///////////////////////////////////////////////////

CmusikTimeCtrl::CmusikTimeCtrl( CmusikPrefs* prefs, CmusikPlayer* player )
{
	m_TimeCtrl = NULL;
	m_CurTime = NULL;
	m_TotalTime = NULL;

	m_Prefs = prefs;
	m_Player = player;
}

///////////////////////////////////////////////////

CmusikTimeCtrl::~CmusikTimeCtrl()
{
	if ( m_TimeCtrl )
		delete m_TimeCtrl;

	if ( m_CurTime )
		delete m_CurTime;
	
	if ( m_TotalTime )
		delete m_TotalTime;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikTimeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TimeCtrl = new CmusikTrackCtrl( m_Prefs, m_Player );
	if ( !m_TimeCtrl->Create( TBS_HORZ | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return false;
    
	m_TimeCtrl->SetRange( 0, 100 );
	m_TimeCtrl->SetLockIfNotPlaying();

	m_CurTime = new CmusikDynamicText();
	if ( !m_CurTime->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this ) )
		return false;
	m_CurTime->SetDynFont( 11, 1, 0 );
	m_CurTime->SetDynText( _T( "0:00" ) );
	
	m_TotalTime = new CmusikDynamicText();
	if ( !m_TotalTime->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this ) )
		return false;
	m_TotalTime->SetDynFont( 11, 1, 0 );
	m_TotalTime->SetDynText( _T( "0:00" ) );
	
	m_TimerID = SetTimer( MUSIK_SEEK_TIMER, 1000, NULL );

	return 0;
}

///////////////////////////////////////////////////

void CmusikTimeCtrl::OnSize(UINT nType, int cx, int cy)
{
	RescaleInfo( cx );
}

///////////////////////////////////////////////////

void CmusikTimeCtrl::RescaleInfo( int cx )
{
	CRect rcClient;
	GetClientRect( &rcClient );

	CRect rcStatic = CRect( 0, 0, 0, 0 );

	int nRemain = NULL;
	int nTrackStart = NULL;

	m_CurTime->UpdateDynSize();
	m_TotalTime->UpdateDynSize();

	if ( m_CurTime )
	{
		m_CurTime->GetClientRect( &rcStatic );
		m_CurTime->MoveWindow( 0, abs( rcClient.Height() - rcStatic.Height() ) / 2, rcStatic.Width(), rcStatic.Height() );
		nRemain += rcStatic.Width();
		nTrackStart = rcStatic.Width();
	}

	if ( m_TotalTime )
	{
		m_TotalTime->GetClientRect( &rcStatic );
		m_TotalTime->MoveWindow( cx - rcStatic.Width(), abs( rcClient.Height() - rcStatic.Height() ) / 2, cx, rcStatic.Height() );
		nRemain += rcStatic.Width();
	}

	if ( m_TimeCtrl )
	{
		nRemain = cx - nRemain;
		m_TimeCtrl->MoveWindow( nTrackStart, 0, nRemain, 16 );
	}
}

///////////////////////////////////////////////////

BOOL CmusikTimeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikTimeCtrl::OnPaint()
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

void CmusikTimeCtrl::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == MUSIK_SEEK_TIMER )
	{
		if ( m_Player->IsPlaying() && !m_Player->IsPaused() )
		{
			
		}
	}
}

///////////////////////////////////////////////////
