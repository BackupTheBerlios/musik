///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikTrackCtrl.h"

#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTrackCtrl, CSliderCtrl)

///////////////////////////////////////////////////

CmusikTrackCtrl::CmusikTrackCtrl( CmusikPrefs* prefs, CmusikPlayer* player )
{
	m_Prefs = prefs;
	m_Player = player;

	m_LeftDown = false;
	m_IsCapturing = false;
	m_LockIfNotPlaying = false;
	m_LastPos = -1;
	m_Cursor.x = -1;
	m_Cursor.y = -1;
}

///////////////////////////////////////////////////

CmusikTrackCtrl::~CmusikTrackCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikTrackCtrl, CSliderCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikTrackCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW lpcd = (LPNMCUSTOMDRAW)pNMHDR;

	// CDDS_PREPAINT is at the beginning of the paint cycle. 
	// this will fire first, so we tell it we want
	// all item drawing information...
	if ( lpcd->dwDrawStage == CDDS_PREPAINT )
	{
		// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;         
		return;
	}

	// CDDS_ITEMPREPAINT is sent when an item is about
	// to be drawn..
	if ( lpcd->dwDrawStage == CDDS_ITEMPREPAINT )
	{	
		CDC *pDC = CDC::FromHandle( lpcd->hdc );
		CRect rect( lpcd->rc );

		// we don't want to draw any ticks,
		// so just skip the default action
		// and return...
		if ( lpcd->dwItemSpec == TBCD_TICS )
		{
			*pResult = CDRF_SKIPDEFAULT;
			return;
		}

		// drawing the thumb
		else if ( lpcd->dwItemSpec == TBCD_THUMB )
		{
			CRect rcThumbDraw;
			if ( rect.Height() > rect.Width() )
				DrawHorizontalThumb( pDC, rect );
			else
				DrawVerticalThumb( pDC, rect );
		}

		// drawing the channel, the space
		// below the thumb
		else if ( lpcd->dwItemSpec == TBCD_CHANNEL )
		{
			CRect rcThumb;
			GetThumbRect(rcThumb);

			DrawChannel( pDC, rect, ( rcThumb.Height() > rcThumb.Width() ) );
		}

		*pResult = CDRF_SKIPDEFAULT;
		return;
	}

	*pResult = CDRF_SKIPDEFAULT;
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::DrawChannel( CDC* pDC, const CRect& rect, BOOL bHoriz )
{
	CRect rcChannel = rect;

	if ( bHoriz )
	{
		rcChannel.top += 1;
		rcChannel.bottom += 1;
		rcChannel.InflateRect( 0, 1, 0, 1 );
	}
	else
	{
		rcChannel.left += 1;
		rcChannel.right += 1;
		rcChannel.InflateRect( 1, 0, 1, 0 );
	}

	CMemDC pMemDC( pDC, &rcChannel );

	pMemDC.FillSolidRect( rcChannel, m_Prefs->MUSIK_COLOR_BTNHILIGHT );
	pMemDC.Draw3dRect( rcChannel, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

	int nMax = GetRangeMax();
	int nPos = GetPos();
	float fPercent = (float)GetPos() / (float)GetRangeMax();

	if ( bHoriz )
	{
		float fWidth = ( (float)rcChannel.Width() * fPercent ) + 1;
		pMemDC->FillSolidRect( CRect( rcChannel.left + 1, rcChannel.top + 1, rcChannel.left + (int)fWidth, rcChannel.bottom - 1 ), m_Prefs->MUSIK_COLOR_ACTIVECAPTION );
	}
	else
	{
		float fHeight = ( (float)rcChannel.Height() * fPercent ) + 1;
		pMemDC->FillSolidRect( CRect( rcChannel.left + 1, rcChannel.top + (int)fHeight, rcChannel.right - 1, rcChannel.bottom - 1 ), m_Prefs->MUSIK_COLOR_ACTIVECAPTION );
	}

	Invalidate( false );
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::DrawHorizontalThumb( CDC* pDC, const CRect& rect )
{
	CMemDC pMemDC( pDC, &rect );

	pMemDC.FillSolidRect( rect, m_Prefs->MUSIK_COLOR_BTNFACE );
	pMemDC.Draw3dRect( rect, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNSHADOW );
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::DrawVerticalThumb( CDC* pDC, const CRect& rect )
{
	CMemDC pMemDC( pDC, &rect );

	pMemDC.FillSolidRect( rect, m_Prefs->MUSIK_COLOR_BTNFACE );
	pMemDC.Draw3dRect( rect, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNSHADOW );
}

///////////////////////////////////////////////////

int CmusikTrackCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSliderCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnSetFocus(CWnd* pOldWnd)
{
	// this is kludge but it gets the job done...
	// when the tracker is selected, a bounding
	// rectangle with marching ants is drawn
	// around it. to fix it, we set the focus
	// to the control's parent...
	if ( GetParent() )
		GetParent()->SetFocus();
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_Player && m_LockIfNotPlaying && !m_Player->IsPlaying() )
		return;

    if ( nFlags & MK_LBUTTON )
	{
		if ( !m_IsCapturing )
		{
			m_LeftDown = true;
			SetCapture();
			SetCursor( NULL );

			m_IsCapturing = true;
		}

		SetPosFromMouse();
	}
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( m_Player && m_LockIfNotPlaying && !m_Player->IsPlaying() )
		return;

	GetCursorPos( &m_Cursor );
	SetPosFromMouse();

	OnBeginDrag();
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_Player && m_LockIfNotPlaying && !m_Player->IsPlaying() )
		return;

	if ( m_LeftDown )
	{
        CPoint posCurr;
		GetCursorPos( &posCurr );
		ScreenToClient( &posCurr );

		CRect rcClient;
		GetClientRect( &rcClient );

		// vertical
		if ( GetStyle() & TBS_VERT )
		{
			if ( posCurr.y >= rcClient.Height() )
			{
				ClientToScreen( &rcClient );
				m_Cursor.y = rcClient.bottom;
				SetCursorPos( m_Cursor.x, m_Cursor.y );
			}
			else if ( posCurr.y <= 0 )
			{
				ClientToScreen( &rcClient );
				m_Cursor.y = rcClient.top;	
				SetCursorPos( m_Cursor.x, m_Cursor.y );
			}
			else
			{
				ClientToScreen( &posCurr );
				SetCursorPos( m_Cursor.x, posCurr.y );
			}
		}

		// horizontal
		else
		{
			if ( posCurr.x >= rcClient.Width() )
			{
				ClientToScreen( &rcClient );
				m_Cursor.x = rcClient.right;
				SetCursorPos( m_Cursor.x, m_Cursor.y );
			}
			else if ( posCurr.x <= 0 )
			{
				ClientToScreen( &rcClient );
				m_Cursor.x = rcClient.left;	
				SetCursorPos( m_Cursor.x, m_Cursor.y );
			}
			else
			{
				ClientToScreen( &posCurr );
				SetCursorPos( posCurr.x, m_Cursor.y );
			}
		}

		ReleaseCapture();
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}

	OnFinishDrag();
	m_LeftDown = false;
	m_IsCapturing = false;
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::SetPosFromMouse()
{
	CPoint pos;
	GetCursorPos( &pos );
	ScreenToClient( &pos );

	CRect rcClient;
	GetClientRect( &rcClient );

	// vertical
	if ( GetStyle() & TBS_VERT )
	{
		if ( pos.y >= rcClient.bottom && GetPos() != GetRangeMax() )
		{
			SetPos( GetRangeMax() );
			m_LastPos = GetRangeMax();
			OnPosChanged();
			return;
		}

		else if ( pos.y <= rcClient.top && GetPos() != GetRangeMin() )
		{
			SetPos( GetRangeMin() );
			m_LastPos = GetRangeMin();
			OnPosChanged();
			return;
		}
		
		else
		{
			int nLoc = rcClient.Height() - pos.y;
			float fRatio = (float)nLoc / rcClient.Height();
			float nPos = (float)fRatio * (float)GetRangeMax();

			int nFinal = GetRangeMax() - (int)nPos;

			if ( nFinal != m_LastPos )
			{
				SetPos( nFinal );
				m_LastPos = nFinal;
				OnPosChanged();
			}
			return;
		}
	}

	// horizontal
	else
	{
		if ( pos.x >= rcClient.right && GetPos() != GetRangeMax() )
		{
			SetPos( GetRangeMax() );
			m_LastPos = GetRangeMax();
			OnPosChanged();
			return;
		}

		else if ( pos.x <= rcClient.left && GetPos() != GetRangeMin() )
		{
			SetPos( GetRangeMin() );
			m_LastPos = GetRangeMin();
			OnPosChanged();
			return;
		}

		else
		{
			int nLoc = rcClient.Width() - pos.x;
			float fRatio = (float)nLoc / rcClient.Width();
			float nPos = (float)fRatio * (float)GetRangeMax();

			int nFinal = GetRangeMax() - (int)nPos;

			if ( nFinal != m_LastPos )
			{
                SetPos( nFinal );
				m_LastPos = nFinal;
				OnPosChanged();
			}

			return;
		}
	}
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnPosChanged()
{
	if ( GetParent() )
	{
		int WM_TRACKCHANGE = RegisterWindowMessage( "TRACKCHANGE" );		
		GetParent()->SendMessage( WM_TRACKCHANGE, NULL, NULL );
	}

	TRACE0( "CmusikTrackCtrl::OnPosChanged()\n" );
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnFinishDrag()
{
	if ( GetParent() )
	{
		int WM_TRACKDRAGFINISH = RegisterWindowMessage( "TRACKDRAGFINISH" );		
		GetParent()->SendMessage( WM_TRACKDRAGFINISH, NULL, NULL );
	}

	TRACE0( "CmusikTrackCtrl::OnFinishDrag()\n" );
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::OnBeginDrag()
{
	if ( GetParent() )
	{
		int WM_TRACKDRAGBEGIN = RegisterWindowMessage( "TRACKDRAGBEGIN" );		
		GetParent()->SendMessage( WM_TRACKDRAGBEGIN, NULL, NULL );
	}

	TRACE0( "CmusikTrackCtrl::OnBeginDrag()\n" );
}

///////////////////////////////////////////////////