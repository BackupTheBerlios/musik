///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikTrackCtrl.h"

#include "musikPrefs.h"

#include "MEMDC.H"
#include ".\musiktrackctrl.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTrackCtrl, CSliderCtrl)

///////////////////////////////////////////////////

CmusikTrackCtrl::CmusikTrackCtrl( CmusikPrefs* prefs )
{
	m_Channel = -1;
	m_Prefs = prefs;
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
		else if ( lpcd->dwItemSpec ==  TBCD_THUMB )
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
		rcChannel.InflateRect( 0, 2, 0, 2 );
	}
	else
	{
		rcChannel.left += 1;
		rcChannel.right += 1;
		rcChannel.InflateRect( 1, 0, 1, 0 );
	}

	CMemDC pMemDC( pDC, &rcChannel );

	pMemDC.FillSolidRect( rcChannel, m_Prefs->MUSIK_COLOR_BTNFACE );
	pMemDC.Draw3dRect( rcChannel, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

	if ( bHoriz )
		m_Channel = rcChannel.Height();
	else
		m_Channel = rcChannel.Width();
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::DrawHorizontalThumb( CDC* pDC, const CRect& rect )
{
	CMemDC pMemDC( pDC, &rect );

	CString s;
	s.Format( "%d, %d, %d, %d\n", rect.top, rect.left, rect.bottom, rect.right );
	TRACE0( s );

	pMemDC.Draw3dRect( rect, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNSHADOW );
}

///////////////////////////////////////////////////

void CmusikTrackCtrl::DrawVerticalThumb( CDC* pDC, const CRect& rect )
{
	CMemDC pMemDC( pDC, &rect );

	CString s;
	s.Format( "%d, %d, %d, %d\n", rect.top, rect.left, rect.bottom, rect.right );
	TRACE0( s );

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
