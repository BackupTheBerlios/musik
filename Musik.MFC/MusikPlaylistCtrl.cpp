// MusikPlaylistCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPrefs.h"

#include "MusikPlaylistCtrl.h"

#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikLibrary.h"
#include ".\musikplaylistctrl.h"


// CMusikPlaylistCtrl

IMPLEMENT_DYNAMIC(CMusikPlaylistCtrl, CListCtrl)
CMusikPlaylistCtrl::CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPrefs* prefs )
{
	m_Library = library;
	m_Prefs = prefs;
}

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikPlaylistCtrl, CListCtrl)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CMusikPlaylistCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	lpncsp->rgrc[0].bottom -= 6;
	lpncsp->rgrc[0].top += 4;
	lpncsp->rgrc[0].right -= 4;
	lpncsp->rgrc[0].left += 4;
}

void CMusikPlaylistCtrl::OnNcPaint()
{
    // get window DC that is clipped to the non-client area
    CWindowDC dc(this);

    CRect rcClient, rcBar;

    GetClientRect(rcClient);
    ClientToScreen(rcClient);
    GetWindowRect(rcBar);

    rcClient.OffsetRect(-rcBar.TopLeft());
    rcBar.OffsetRect(-rcBar.TopLeft());

    CDC mdc;
    mdc.CreateCompatibleDC(&dc);
    
    CBitmap bm;
    bm.CreateCompatibleBitmap(&dc, rcBar.Width(), rcBar.Height());
    CBitmap* pOldBm = mdc.SelectObject(&bm);

    // draw borders in non-client area
    CRect rcDraw = rcBar;

    // erase the NC background
	CBrush *back = CBrush::FromHandle( GetSysColorBrush( COLOR_BTNFACE ) );
    mdc.FillRect(rcDraw, back );
	mdc.Draw3dRect( 3, 3, rcClient.Width() + 2, rcClient.Height() + 2, GetSysColor( COLOR_BTNSHADOW ), GetSysColor( COLOR_BTNHILIGHT ) );

    // client area is not our bussiness :)
    dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);

    dc.BitBlt(0, 0, rcBar.Width(), rcBar.Height(), &mdc, 0, 0, SRCCOPY);

    ReleaseDC(&dc);

    mdc.SelectObject(pOldBm);
    bm.DeleteObject();
    mdc.DeleteDC();
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::ResetColumns()
{
	CIntArray order = m_Prefs->GetPlaylistOrder();
	for ( int i = 0; i < order.size(); i++ )
	{
		InsertColumn( i, m_Library->GetSongField( order.at( i ) ) );
	}
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::SaveColumnOrder()
{
	CIntArray last_order;
	char sCol[256];

	LVCOLUMN pColumn;
	pColumn.mask = LVCF_TEXT;
	pColumn.pszText = sCol;
	pColumn.cchTextMax = sizeof( sCol );

	for ( int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++ )
	{
		GetColumn( i, &pColumn );
		last_order.push_back( m_Library->GetSongFieldID( pColumn.pszText ) );
	}

	m_Prefs->SetPlaylistOrder( last_order );
}

///////////////////////////////////////////////////

int CMusikPlaylistCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	ResetColumns();

	return 0;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnDestroy()
{
	SaveColumnOrder();

	CListCtrl::OnDestroy();
}
