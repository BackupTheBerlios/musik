///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"
#include "MusikPrefs.h"

#include "MusikPlaylistCtrl.h"

#include "../Musik.Core/include/MusikPlaylist.h"
#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikSongInfoCache.h"

#include "MEMDC.H"
#include ".\musikplaylistctrl.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikPlaylistCtrl, CListCtrl)
CMusikPlaylistCtrl::CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPrefs* prefs, CMusikPlaylist* playlist )
{
	m_Library	= library;
	m_Prefs		= prefs;
	m_Playlist	= playlist;

	m_SongInfoCache = new CMusikSongInfoCache( m_Playlist, m_Library );

	InitFonts();
}

///////////////////////////////////////////////////

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{
	delete m_SongInfoCache;
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
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnLvnOdcachehint)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	lpncsp->rgrc[0].bottom -= 6;
	lpncsp->rgrc[0].top += 4;
	lpncsp->rgrc[0].right -= 4;
	lpncsp->rgrc[0].left += 4;

	CListCtrl::OnNcCalcSize( bCalcValidRects, lpncsp );
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

    // client area is not our bussiness
	dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::ResetColumns()
{
	while ( DeleteColumn( 0 ) );

	for ( size_t i = 0; i < m_Prefs->GetPlaylistColCount(); i++ )
	{
		InsertColumn( (int)i, m_Library->GetSongField( m_Prefs->GetPlaylistCol( i ) ) );
		SetColumnWidth( (int)i, m_Prefs->GetPlaylistColWidth( i ) );
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
	SetItemCountEx( m_Playlist->size(), LVSICF_NOINVALIDATEALL /*| LVSICF_NOSCROLL*/ );

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

	LV_ITEM* pItem = &( pDispInfo->item );

	if ( pItem->mask & LVIF_TEXT )
	{
		int nItem = pItem->iItem;
		int nSub  = pItem->iSubItem;
		int nType = m_Prefs->GetPlaylistCol( nSub );

		pItem->mask |= LVIF_DI_SETITEM;

		CString sValue;
		
		if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_RATING )
			sValue = GetRating( nItem );
		else
			sValue = m_Playlist->GetField( nItem, nType );

		char* pStr = sValue.GetBuffer();

		pItem->cchTextMax = sizeof( *pStr );
		lstrcpy( pItem->pszText, pStr );
	}

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
	
	// we got a paint item event, ignore it, we want to draw the 
	// sub items one by one.
	else if ( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT  )
		*pResult = CDRF_NOTIFYSUBITEMDRAW;

	// draw the sub items
	else if ( pLVCD->nmcd.dwDrawStage == ( CDDS_ITEMPREPAINT | CDDS_SUBITEM ) )
	{
		CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		int nSubType = m_Prefs->GetPlaylistCol( pLVCD->iSubItem );

		if ( nSubType == MUSIK_LIBRARY_TYPE_RATING )
		{
			pDC->SelectObject( m_Bullets );
			pDC->SetTextColor( GetSysColor( COLOR_BTNFACE ) );
		}
		else
			pDC->SelectObject( m_Items );
			
		*pResult = CDRF_NEWFONT;
	}
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

void CMusikPlaylistCtrl::InitFonts()
{
	m_Items.CreateStockObject( DEFAULT_GUI_FONT );
	m_Bullets.CreatePointFont( 100, "Marlett" );
}
///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

///////////////////////////////////////////////////
void CMusikPlaylistCtrl::OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

	m_SongInfoCache->Set( pCacheHint->iFrom, pCacheHint->iTo );

	*pResult = 0;
}
