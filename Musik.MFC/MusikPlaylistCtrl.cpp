///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"
#include "MusikPrefs.h"

#include "MusikPlaylistCtrl.h"

#include "../Musik.Core/include/MusikPlaylist.h"
#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikDynDspInfo.h"
#include "../Musik.Core/include/MusikPlayer.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikPlaylistCtrl, CListCtrl)
CMusikPlaylistCtrl::CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, CMusikPlaylist* playlist )
{
	m_Library	= library;
	m_Prefs		= prefs;
	m_Playlist	= playlist;
	m_Player	= player;

	m_RatingWidth = -1;

	m_Changed = true;

	m_SongInfoCache = new CMusikDynDspInfo( m_Playlist, m_Library );

	InitFonts();
	InitColors();
}

///////////////////////////////////////////////////

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{
	delete m_SongInfoCache;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikPlaylistCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnLvnOdcachehint)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
END_MESSAGE_MAP()

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

void CMusikPlaylistCtrl::SetPlaylist( CMusikPlaylist* playlist )
{
	m_Playlist = playlist;
	m_Changed = true;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::UpdateV()
{
	SetRedraw( false );
	m_SongInfoCache->Set( 0, 0 );
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

	// only need to worry about item text
	if ( pItem->mask & LVIF_TEXT )
	{
		int nItem = pItem->iItem;
		int nSub  = pItem->iSubItem;
		int nType = m_Prefs->GetPlaylistCol( nSub );

		pItem->mask |= LVIF_DI_SETITEM;

		CString sValue;
		
		// if the item we want to access is out of the
		// cache's range, something bad happened
		if ( nItem > m_SongInfoCache->GetLast() || nItem < m_SongInfoCache->GetFirst() )
			sValue = _T( "[musik.uncached]" );

		// if the current subitem type is rating, then
		// we need to format it correctly... pass the
		// value to GetRating()
		else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_RATING )
			sValue = GetRating( nItem );

		// if we have a time value, it was stored as 
		// milliseconds, so it needs to be formatted
		else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_DURATION )
			sValue = GetTimeStr( nItem );

		// otherwise, just use the value we are supposed to
		else
			sValue = m_SongInfoCache->items()->at( nItem - m_SongInfoCache->GetFirst() ).GetField( nType );

		// copy the buffer of the correct display string
		// to the current LV_ITEM
		char* pStr = sValue.GetBuffer();
		pItem->cchTextMax = sizeof( *pStr );
		lstrcpy( pItem->pszText, pStr );
	}

	*pResult = 0;
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
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	
	// we got a paint item event, ignore it, we want to draw the 
	// sub items one by one.
	else if ( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT  )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}	

	// draw the sub items
	else if ( pLVCD->nmcd.dwDrawStage == ( CDDS_ITEMPREPAINT | CDDS_SUBITEM ) )
	{
		CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		int nSubType = m_Prefs->GetPlaylistCol( pLVCD->iSubItem );

		// if this value is uninitialized we need to find
		// it... it is the width of the rating font
		if ( m_RatingWidth == -1 )
		{
			pDC->SelectObject( m_StarFont );
			CSize size = pDC->GetOutputTextExtent( _T( "-----" ) );
			m_RatingWidth = size.cx;
		}

		if ( nSubType == MUSIK_LIBRARY_TYPE_RATING )
		{
			pDC->SelectObject( m_StarFont );
			pDC->SetTextColor( GetSysColor( COLOR_BTNFACE ) );
		}
		else
		{
			if ( m_Player->GetCurrPlaying()->GetID() == m_Playlist->items()->at( pLVCD->nmcd.dwItemSpec ).GetID() )
				pDC->SelectObject( m_BoldFont );
			else
				pDC->SelectObject( m_ItemFont );
		}

		if ( pLVCD->nmcd.dwItemSpec % 2 != 0 )
			pLVCD->clrTextBk = clrStripe;
			
		*pResult = CDRF_NEWFONT;
		return;
	}
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::InitFonts()
{
	m_ItemFont.CreateStockObject( DEFAULT_GUI_FONT );
	m_StarFont.CreatePointFont( 100, "Musik" );

	LOGFONT pBoldFont;
	m_ItemFont.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;

	m_BoldFont.CreateFontIndirect( &pBoldFont );}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::InitColors()
{
	int r, g, b;
	
	r = GetRValue( GetSysColor( COLOR_BTNHILIGHT ) );
	g = GetGValue( GetSysColor( COLOR_BTNHILIGHT ) );
	b = GetBValue( GetSysColor( COLOR_BTNHILIGHT ) );
	int avg = ( r + g + b ) / 3;

	// color is more bright than dim, so the stripe 
	// color will be slightly darker
	if ( avg > 128 )
	{
		r -= 10;
		g -= 10;
		b -= 10;

		if ( r < 0 ) r = 0;
		if ( g < 0 ) g = 0;
		if ( b < 0 ) b = 0;
	}

	// opposite
	else
	{
		r += 10;
		g += 10;
		b += 10;

		if ( r > 255 ) r = 255;
		if ( g > 255 ) g = 255;
		if ( b > 255 ) b = 255;
	}

	// set color 
	clrStripe = RGB( r, g, b );
}

///////////////////////////////////////////////////
void CMusikPlaylistCtrl::OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

	m_SongInfoCache->Set( pCacheHint->iFrom, pCacheHint->iTo );

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

CString CMusikPlaylistCtrl::GetTimeStr( int item )
{
	int time_ms  = atoi( m_SongInfoCache->items()->at( item - m_SongInfoCache->GetFirst() ).GetDuration() );

	CString sTime;

	int ms = time_ms;
	int hours = ms / 1000 / 60 / 60;
	ms -= hours * 1000 * 60 * 60;
	int minutes = ms / 1000 / 60;
	ms -= minutes * 1000 * 60;
	int seconds = ms / 1000;

	if ( hours > 0 )
		sTime.Format( _T( "%d:%02d:%02d" ), hours, minutes, seconds );
	else
		sTime.Format( _T( "%d:%02d" ), minutes, seconds );
	
	return sTime;
}

///////////////////////////////////////////////////

CString CMusikPlaylistCtrl::GetRating( int item )
{
	int nRating = atoi( m_SongInfoCache->items()->at( item - m_SongInfoCache->GetFirst() ).GetRating() );

	CString sRating;
	switch ( nRating )
	{
	case 1:
		sRating = _T( "-,,,," );
		break;
	case 2:
		sRating = _T( "--,,," );
		break;
	case 3:
		sRating = _T( "---,," );
		break;
	case 4:
		sRating = _T( "----," );
		break;
	case 5:
        sRating = _T( "-----" );
		break;
	case 0:
	default:
		sRating = _T( ",,,,," );
		break;
	}

	return sRating;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	DWORD dwPos = ::GetMessagePos();
	CPoint ptCurr( (int)LOWORD( dwPos ), (int)HIWORD( dwPos ) );
	ScreenToClient( &ptCurr );

	LVHITTESTINFO hit_test;
	hit_test.pt = ptCurr;
	SubItemHitTest( &hit_test );

	// click column to rate
	if( hit_test.flags & LVHT_ONITEMLABEL )
	{
		if ( m_Prefs->GetPlaylistCol( hit_test.iSubItem ) == MUSIK_LIBRARY_TYPE_RATING )
		{
			CRect sub_item_rect;
			GetSubItemRect( hit_test.iItem, hit_test.iSubItem, LVIR_BOUNDS, sub_item_rect );

			for ( int i = 0; i < hit_test.iSubItem; i++ )
				ptCurr.x -= m_Prefs->GetPlaylistColWidth( (size_t)i );

			int nRating;
			if ( ptCurr.x <= 6 )
				nRating = 0;
			else if ( ptCurr.x >= m_RatingWidth )
				nRating = 5;
			else
				nRating = ( ( ptCurr.x - 6 ) / ( m_RatingWidth / 5 ) ) + 1;

            m_Library->SetSongRating( m_Playlist->items()->at( hit_test.iItem ).GetID(), nRating );	
			if ( m_SongInfoCache->ResyncItem( m_Playlist->items()->at( hit_test.iItem ).GetID() ) )
				RedrawItems( hit_test.iItem, hit_test.iItem );
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CMusikPlaylistCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if ( m_Changed )
	{
		m_Player->SetPlaylist( m_Playlist );
		m_Changed = false;
	}			

	m_Player->Play( pNMIA->iItem );

	*pResult = 0;
}

///////////////////////////////////////////////////
