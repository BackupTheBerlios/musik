// MusikPlaylistCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPrefs.h"

#include "MusikPlaylistCtrl.h"

#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikLibrary.h"

#include "MEMDC.H"
#include ".\musikplaylistctrl.h"

// CMusikPlaylistCtrl

IMPLEMENT_DYNAMIC(CMusikPlaylistCtrl, CListCtrl)
CMusikPlaylistCtrl::CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPrefs* prefs, CMusikPlaylist* playlist )
{
	m_Library = library;
	m_Prefs = prefs;
	m_Playlist = playlist;
}

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikPlaylistCtrl, CListCtrl)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
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
	for ( size_t i = 0; i < m_Prefs->GetPlaylistColCount(); i++ )
	{
		InsertColumn( i, m_Library->GetSongField( m_Prefs->GetPlaylistCol( i ) ) );
		SetColumnWidth( i, m_Prefs->GetPlaylistColWidth( i ) );
	}
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::SaveColumns()
{
	CIntArray last_order;
	CIntArray last_sizes;

	char sCol[256];

	LVCOLUMN pColumn;
	pColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	pColumn.pszText = sCol;
	pColumn.cchTextMax = sizeof( sCol );

	int nColCount = GetHeaderCtrl()->GetItemCount();
	LPINT lpiCols = new int[nColCount];

	// get header order array
	GetHeaderCtrl()->GetOrderArray( lpiCols, nColCount );

	for (int i = 0; i < nColCount; i++)
	{
		GetColumn( lpiCols[i], &pColumn );
		last_order.push_back( m_Library->GetSongFieldID( pColumn.pszText ) );
		last_sizes.push_back( pColumn.cx );
	} 
	delete[] lpiCols;

	m_Prefs->SetPlaylistOrder( last_order );
	m_Prefs->SetPlaylistSizes( last_sizes );
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::UpdateV()
{
	SetItemCountEx( m_Playlist->size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	CRect rcClient;
	GetClientRect( &rcClient );
	RedrawWindow( rcClient );
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
	SaveColumns();

	CListCtrl::OnDestroy();
}

void CMusikPlaylistCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem= &(pDispInfo)->item;

	int index= pItem->iItem;

	if ( pItem->mask & LVIF_TEXT )
	{
		if ( m_Prefs->GetPlaylistCol( pItem->iSubItem ) == MUSIK_LIBRARY_TYPE_RATING )
		{
			int nRating = atoi( m_Library->GetSongField( MUSIK_LIBRARY_TYPE_RATING ) );
			CString sRating;
			switch ( nRating )
			{
			case 1:
				sRating = _T( "hiiii" );
				break;
			case 2:
				sRating = _T( "hhiii" );
				break;
			case 3:
				sRating = _T( "hhhii" );
				break;
			case 4:
				sRating = _T( "hhhhi" );
				break;
			case 5:
                sRating = _T( "hhhhh" );
				break;
			case 0:
			default:
				sRating = _T( "iiiii" );
				break;
			}
			lstrcpy( pItem->pszText, sRating );
		}
		else
			lstrcpy( pItem->pszText, m_Playlist->GetField( index, m_Prefs->GetPlaylistCol( pItem->iSubItem ) ) );
	}

	*pResult = 0;
}

void CMusikPlaylistCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, rect);
	
	CRect headerRect;
	GetDlgItem(0)->GetWindowRect(&headerRect);
	ScreenToClient(&headerRect);
	dc.ExcludeClipRect(&headerRect);
	   
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, GetSysColor( COLOR_BTNHILIGHT ) );

	this->SetTextBkColor( GetSysColor( COLOR_BTNHILIGHT ) );
	   
	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

BOOL CMusikPlaylistCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
	//return CListCtrl::OnEraseBkgnd(pDC);
}

void CMusikPlaylistCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;
	
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
        *pResult = CDRF_NOTIFYITEMDRAW;
	
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        int iRow = (int)pLVCD->nmcd.dwItemSpec;
		
		COLORREF bgCol;

		if ( iRow % 2 == 0 )
			bgCol = m_Prefs->GetPlaylistStripeColor();
		else
			bgCol = GetSysColor( COLOR_BTNHILIGHT );


		// 	
		//	*pResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT | CDRF_NEWFONT;
	}

	else if( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT )
      *pResult = CDRF_DODEFAULT;
}

void CMusikPlaylistCtrl::EnableHighlighting( HWND hWnd, int row, bool bHighlight )
{
	ListView_SetItemState(hWnd, row, bHighlight? 0xff: 0, LVIS_SELECTED);
}

bool CMusikPlaylistCtrl::IsRowSelected( HWND hWnd, int row )
{
	return ListView_GetItemState(hWnd, row, LVIS_SELECTED) != 0;
}
