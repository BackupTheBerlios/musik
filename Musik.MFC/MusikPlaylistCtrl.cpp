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
	InitFonts();
}

///////////////////////////////////////////////////

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{
	delete m_Bullets;
	delete m_Items;
}

///////////////////////////////////////////////////

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

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	lpncsp->rgrc[0].bottom -= 6;
	lpncsp->rgrc[0].top += 4;
	lpncsp->rgrc[0].right -= 4;
	lpncsp->rgrc[0].left += 4;
}

///////////////////////////////////////////////////

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

	CMemDC pDC( &dc );

    // draw borders in non-client area
    CRect rcDraw = rcBar;

    // erase the NC background
	CBrush *back = CBrush::FromHandle( GetSysColorBrush( COLOR_BTNFACE ) );
    pDC.FillRect(rcDraw, back );
	pDC.Draw3dRect( 3, 3, rcClient.Width() + 2, rcClient.Height() + 2, GetSysColor( COLOR_BTNSHADOW ), GetSysColor( COLOR_BTNHILIGHT ) );

    // client area is not our bussiness :)
	dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::ResetColumns()
{
	while ( DeleteColumn( 0 ) );

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
	SetRedraw( false );
	SetItemCountEx( m_Playlist->size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	CRect rcClient;
	GetClientRect( &rcClient );
	SetRedraw( true );
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

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	// dummy function, its just here to relay messages to the
	// NM_CUSTOMDRAW function, which will draw the actual items

	//CRect rcItem;
	//GetItemRect( pDispInfo->item.iItem, &rcItem, LVIR_BOUNDS );
	//DrawItem( GetDC(), pDispInfo->item.iItem, rcItem );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect headerRect;
	GetDlgItem(0)->GetWindowRect(&headerRect);
	ScreenToClient(&headerRect);
	dc.ExcludeClipRect(&headerRect);
	   
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, GetSysColor( COLOR_BTNHILIGHT ) );
	   
	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

BOOL CMusikPlaylistCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
	//return CListCtrl::OnEraseBkgnd(pDC);
}

///////////////////////////////////////////////////

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
		
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect item_rect;
		GetItemRect( iRow, &item_rect, LVIR_BOUNDS ); 

		DrawItem( pDC, iRow, item_rect );

		*pResult = CDRF_SKIPDEFAULT;
	}

	else if( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT )
      *pResult = CDRF_DODEFAULT;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::EnableHighlighting( int row, bool bHighlight )
{
	ListView_SetItemState( m_hWnd, row, bHighlight? 0xff: 0, LVIS_SELECTED );
}

///////////////////////////////////////////////////

bool CMusikPlaylistCtrl::IsRowSelected( int row )
{
	return ListView_GetItemState( m_hWnd, row, LVIS_SELECTED ) != 0;
}

///////////////////////////////////////////////////

CString CMusikPlaylistCtrl::GetRating( int item )
{
	int nRating = atoi( m_Playlist->items()->at( item ).GetField( MUSIK_LIBRARY_TYPE_RATING ) );
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

	return sRating;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::DrawItem( CDC* pDC, int item, const CRect& rect )
{
	CMemDC dc( pDC, &rect );

	// setup the font and highlight colors
	COLORREF bg;
	COLORREF text;
	if ( IsRowSelected( item ) )
	{
		bg = GetSysColor( COLOR_HIGHLIGHT );
		text = GetSysColor( COLOR_HIGHLIGHTTEXT );
	}
	else
	{
		if ( item % 2 == 0 )
		{
			bg = m_Prefs->GetPlaylistStripeColor();
			text = GetSysColor( COLOR_WINDOWTEXT );
		}
		else
		{
			bg = GetSysColor( COLOR_BTNHILIGHT );
			text = GetSysColor( COLOR_WINDOWTEXT );
		}
	}

	// draw the text
	CRect rcSubItem;
	CString sSubItem;
	dc.SetTextColor( text );
	for ( size_t i = 0; i < m_Prefs->GetPlaylistColCount(); i++ )
	{
		GetSubItemRect( item, i, LVIR_BOUNDS, rcSubItem );
		if ( m_Prefs->GetPlaylistCol( i ) == MUSIK_LIBRARY_TYPE_RATING )
		{
			sSubItem = GetRating( item );
			dc.FillSolidRect( &rcSubItem, bg );
			dc.SelectObject( m_Bullets );
			dc.TextOut( rcSubItem.left + 6, rcSubItem.top, sSubItem );
		}
		
		else
		{
			sSubItem = m_Playlist->items()->at( item ).GetField( m_Prefs->GetPlaylistCol ( i ) );
			dc.FillSolidRect( &rcSubItem, bg );
			dc.SelectObject( m_Items );
			dc.TextOut( rcSubItem.left + 6, rcSubItem.top, sSubItem );
		}
	}
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::InitFonts()
{
	m_Items = new CFont();
	m_Items->CreateStockObject( DEFAULT_GUI_FONT );

    m_Bullets = new CFont();
	m_Bullets->CreatePointFont( 96, "Marlett" );
}