///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikTimeCtrl.h"

#include "musikDynamicText.h"
#include "musikTrackCtrl.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTimeCtrl, CWnd)

///////////////////////////////////////////////////

int WM_TRACKCHANGE = RegisterWindowMessage( "TRACKCHANGE" );
int WM_TRACKDRAGBEGIN = RegisterWindowMessage( "TRACKDRAGBEGIN" );	
int WM_TRACKDRAGFINISH = RegisterWindowMessage( "TRACKDRAGFINISH" );

///////////////////////////////////////////////////

#define MUSIK_SEEK_TIMER WM_USER + 1

///////////////////////////////////////////////////

CmusikTimeCtrl::CmusikTimeCtrl( CmusikPrefs* prefs, CmusikPlayer* player )
{
	m_TimeCtrl = NULL;
	m_CurTime = NULL;
	m_TotalTime = NULL;

	m_CurChars = -1;
	m_TotalChars = -1;

	m_TimeDrag = false;

	m_Size = CSize( 200, 16 );

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
	ON_REGISTERED_MESSAGE(WM_TRACKCHANGE, OnTrackChange)
	ON_REGISTERED_MESSAGE(WM_TRACKDRAGBEGIN, OnTrackBegin)
	ON_REGISTERED_MESSAGE(WM_TRACKDRAGFINISH, OnTrackFinish)
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
	m_CurTime->SetDynText( _T( "0:00" ), false, false );
	
	m_TotalTime = new CmusikDynamicText();
	if ( !m_TotalTime->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this ) )
		return false;
	m_TotalTime->SetDynFont( 11, 1, 0 );
	m_TotalTime->SetDynText( _T( "0:00" ), false, false );
	
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
	
	if ( cx == -1 )
		cx = rcClient.Width();

	// stuff we'll use
	CRect rcStatic;
	m_CurTime->GetClientRect( &rcStatic );
	CPoint ptCurr;
	CSize szCurr, szUpdate;
	CString sCurTime, sTotalTime;
	size_t text_width = 0;
	size_t track_start = 0;

	// figure out how big to make
	// the windows...
	m_CurTime->GetWindowText( sCurTime );
	m_TotalTime->GetWindowText( sTotalTime );

	if ( sCurTime.GetLength() < sTotalTime.GetLength() )
		szUpdate = m_TotalTime->GetTextSize( sTotalTime );
	else
		szUpdate = m_CurTime->GetTextSize( sCurTime );

	// current time
	ptCurr.x = 0;
	ptCurr.y = ( rcClient.Height() - rcStatic.Height() ) / 2;
	m_CurTime->MoveWindow( CRect( ptCurr, szUpdate ) );

	text_width += szUpdate.cx;
	track_start = text_width;

	// total time
	ptCurr.x = rcClient.Width() - szUpdate.cx;
	ptCurr.y = ( rcClient.Height() - rcStatic.Height() ) / 2;
	m_TotalTime->MoveWindow( CRect( ptCurr, szUpdate ) );

	text_width += szUpdate.cx;

	// seeker
	ptCurr.x = track_start;
	ptCurr.y = 0;
	szCurr.cx = rcClient.Width() - text_width;
	szCurr.cy = 16;
	m_TimeCtrl->MoveWindow( CRect( ptCurr, szCurr ) );
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
		if ( m_Player->IsPlaying() && !m_Player->IsPaused() && !m_TimeDrag )
		{
			CString sTimeStr = m_Player->GetTimeStr( m_Player->GetTimeNow( MUSIK_TIME_MS ) );
			m_TimeCtrl->SetPos( m_Player->GetTimeNowPer() );
			m_CurTime->SetDynText( sTimeStr, false, false );

			// avoid unnecessary RescaleInfo() calls
			if ( sTimeStr.GetLength() != m_CurChars )
			{
				RescaleInfo( -1 );
				m_CurChars = sTimeStr.GetLength();
				m_CurTime->RedrawWindow();
			}
		}
	}
}

///////////////////////////////////////////////////

void CmusikTimeCtrl::OnNewSong()
{
	if ( !m_Player->IsPaused() && m_Player->IsPlaying() )
	{
		CString sTimeStr = m_Player->GetTimeStr( m_Player->GetDuration( MUSIK_TIME_MS ) );
		m_TotalTime->SetDynText( sTimeStr, false, false );

		// avoid un necessary RescaleInfo() calls
		if ( sTimeStr.GetLength() != m_TotalChars )
		{
			RescaleInfo( -1 );
			m_TotalChars = sTimeStr.GetLength();
			RedrawWindow();
		}

		return;
	}
	else
	{
		m_TotalTime->SetDynText( _T( "0:00" ), false, false );
		m_CurTime->SetDynText( _T( "0:00" ), false, false );
		m_TimeCtrl->SetPos( 0 );
		RescaleInfo( -1 );
		RedrawWindow();
	}
}

///////////////////////////////////////////////////

LRESULT CmusikTimeCtrl::OnTrackChange( WPARAM wParam, LPARAM lParam )
{
	CString sTimeStr = m_Player->GetTimePerStr( m_TimeCtrl->GetPos() );
	m_CurTime->SetDynText( sTimeStr, false, false );

	// avoid unnecessary RescaleInfo() calls
	if ( sTimeStr.GetLength() != m_CurChars )
	{
		RescaleInfo( -1 );
		m_CurChars = sTimeStr.GetLength();
		m_CurTime->RedrawWindow();
	}	
	
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikTimeCtrl::OnTrackBegin( WPARAM wParam, LPARAM lParam )
{
	m_TimeDrag = true;

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikTimeCtrl::OnTrackFinish( WPARAM wParam, LPARAM lParam )
{
	m_TimeDrag = false;

	if ( m_Player )
		m_Player->SetTimeNowPer( m_TimeCtrl->GetPos() );

	return 0L;
}

///////////////////////////////////////////////////
