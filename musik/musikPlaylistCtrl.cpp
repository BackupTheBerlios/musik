///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikPlaylistCtrl
//
// Filename(s): 
//
//   musikPlaylistCtrl.h, musikPlaylistCtrl.cpp
//
// Information:
//
//   musikCube's playlist control, based on a CmusikListCtrl
// 
// Usage: 
//
//   The control is virtual and fairly specialized, read
//   the header file for more information.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikPrefs.h"
#include "musikPlaylistView.h"
#include "musikPlaylistCtrl.h"
#include "musikSourcesCtrl.h"
#include "musikFileDrop.h"
#include "musikSaveStdPlaylist.h"
#include "musikBatchTagDlg.h"

#include "../musikCore/include/musikBatchRetag.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

#pragma warning( disable : 4244 )

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPlaylistCtrl, CWnd )

///////////////////////////////////////////////////

// messages that we can receive

int WM_TAGPROPERTIESDESTROY = RegisterWindowMessage( "TAGPROPERTIESDESTROY" );
int WM_TAGNEXT = RegisterWindowMessage( "TAGNEXT" );
int WM_TAGPREV = RegisterWindowMessage( "TAGPREV" );
int WM_TAGUPDATE = RegisterWindowMessage( "TAGUPDATE" );

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistCtrl, CmusikListCtrl)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnLvnOdcachehint)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
	ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnLvnBeginrdrag)
	ON_NOTIFY(HDN_ENDTRACKA, 0, OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, OnHdnEndtrack)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)

	// menu
	ON_COMMAND(ID_PLAYLISTCOLUMNS_ARTIST, OnPlaylistcolumnsArtist)
	ON_COMMAND(ID_PLC_DELETE_FROMPLAYLIST, OnPlcDeleteFromplaylist)
	ON_COMMAND(ID_PLC_DELETE_FROMLIBRARY, OnPlcDeleteFromlibrary)
	ON_COMMAND(ID_PLC_DELETE_FROMCOMPUTER, OnPlcDeleteFromcomputer)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, OnPlaylistcontextmenuShufflenowplaying)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PROPERTIES, OnPlaylistcontextmenuProperties)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_ALBUM, OnPlaylistcolumnsAlbum)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_YEAR, OnPlaylistcolumnsYear)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_GENRE, OnPlaylistcolumnsGenre)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TITLE, OnPlaylistcolumnsTitle)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TRACKNUMBER, OnPlaylistcolumnsTracknumber)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TIMEADDED, OnPlaylistcolumnsTimeadded)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_LASTPLAYED, OnPlaylistcolumnsLastplayed)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_FILESIZE, OnPlaylistcolumnsFilesize)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_FORMAT, OnPlaylistcolumnsFormat)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_DURATION, OnPlaylistcolumnsDuration)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_RATING, OnPlaylistcolumnsRating)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TIMESPLAYED, OnPlaylistcolumnsTimesplayed)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_BITRATE, OnPlaylistcolumnsBitrate)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_FILENAME, OnPlaylistcolumnsFilename)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_BATCH, OnPlaylistcontextmenuBatch)

	// custom messages
	ON_REGISTERED_MESSAGE( WM_TAGPROPERTIESDESTROY, OnTagEditDestroy )
	ON_REGISTERED_MESSAGE( WM_TAGNEXT, OnTagNext )
	ON_REGISTERED_MESSAGE( WM_TAGPREV, OnTagPrev )
	ON_REGISTERED_MESSAGE( WM_TAGUPDATE, OnTagUpdate )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPlaylistCtrl::CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid_l, UINT dropid_r )
{
	// core
	m_Playlist	= NULL;
	m_Library	= library;
	m_Prefs		= prefs;
	m_Player	= player;

	// tag dialog
	m_TagEdit = NULL;

	// no sorting yet
	m_LastCol	= -1;

	// is a column being rearranged?
	m_Arranging = false;

	// dnd drop id
	m_DropID_L = dropid_l;
	m_DropID_R = dropid_r;

	// main window
	m_Parent = mainwnd;

	// info ctrl
	m_InfoCtrl = NULL;

	// misc
	m_DropArrange = false;
	m_PlaylistNeedsSave = false;
	m_SongInfoCache = new CmusikDynDspInfo( NULL, m_Library );

	// dnd variables
	m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );

	// fonts and colors
	InitFonts();
	InitColors();

	// get rating extent 
	GetRatingExtent();
}

///////////////////////////////////////////////////

CmusikPlaylistCtrl::~CmusikPlaylistCtrl()
{
	if ( m_Playlist && m_Playlist->m_Type != MUSIK_PLAYLIST_TYPE_NOWPLAYING )
		delete m_Playlist;

	if ( m_TagEdit )
		delete m_TagEdit;

	delete m_SongInfoCache;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ResetColumns( bool update )
{
	SetRedraw( FALSE );
	GetHeaderCtrl()->SetRedraw( FALSE );

	DeleteAllItems();

	CmusikPlaylist* pList = GetPlaylist();
	m_Playlist = NULL;
	UpdateV();

	while ( DeleteColumn( 0 ) );

	CIntArray cols = m_Prefs->GetPlaylistOrder();
	CIntArray sizes = m_Prefs->GetPlaylistSizes();
	bool rewrite = false;

	size_t times = cols.size();
	for ( size_t i = 0; i < times; i++ )
	{
		if ( cols.at( i ) > -1 )
		{
			InsertColumn( (int)i, m_Library->GetSongField( cols.at( i ) ) );
			SetColumnWidth( (int)i, sizes.at( i ) );
		}
		else
		{
			cols.erase( cols.begin() + i );
			sizes.erase( sizes.begin() + i );
			
			i--;
			times--;

			rewrite = true;
		}
	}

	if ( rewrite )
	{
		m_Prefs->SetPlaylistOrder( cols );
		m_Prefs->SetPlaylistSizes( sizes );
	}

	SetRedraw( TRUE );
	GetHeaderCtrl()->SetRedraw( TRUE );

	m_Playlist = pList;

	if ( update )
		UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SaveColumns()
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

void CmusikPlaylistCtrl::UpdateV( bool redraw, bool select_none )
{
	if ( m_Playlist )
	{
		if ( select_none )
			SetItemState( -1, 0, LVIS_SELECTED );

		int nPos = GetScrollPos( SB_VERT );

		SetRedraw( false );
		m_SongInfoCache->Set( 0, 0, true );
		SetItemCountEx( m_Playlist->GetCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

		SetScrollPos( SB_VERT, nPos, false );

		if ( redraw )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			SetRedraw( true );
			RedrawWindow( rcClient );
		}
	}
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	ResetColumns( false );

	return 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDestroy()
{
	SaveColumns();

	CmusikListCtrl::OnDestroy();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
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
		if ( nItem >= m_SongInfoCache->GetFirst() && nItem <= m_SongInfoCache->GetLast() )
		{
			// if the current subitem type is rating, then
			// we need to format it correctly... pass the
			// value to GetRating()
			if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_RATING )
				sValue = GetRating( nItem );

			// if we have a time value, it was stored as 
			// milliseconds, so it needs to be formatted
			else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_DURATION )
				sValue = GetTimeStr( nItem );

			else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_TRACKNUM )
			{
				sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_TRACKNUM );
				if ( sValue == _T( "0" ) )
					sValue = _T( "" );
			}

			else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_YEAR )
			{
				sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_YEAR );
				if ( sValue == _T( "0" ) )
					sValue = _T( "" );
			}

			else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_TIMESPLAYED )
			{
				sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_TIMESPLAYED );
				if ( sValue == _T( "0" ) )
					sValue = _T( "" );
			}

			else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_FILESIZE )
			{
				sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_FILESIZE );
				float fval = (float)atoi( sValue.GetBuffer() );

				fval /= ( 1024.0 * 1024.0 );
				sValue.Format( _T( "%.2f mb" ), fval );
			}

			else if ( m_Prefs->GetPlaylistCol( nSub ) == MUSIK_LIBRARY_TYPE_FORMAT )
			{
				sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_FORMAT );
				if ( sValue == _T( "1" ) )
					sValue = _T( "ogg" );
				else
					sValue = _T( "mp3" );
			}

			// otherwise, just use the value we are supposed to
			else
				sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), nType );
		}
		else
			sValue = _T( "[musik.caching]" );

		// copy the buffer of the correct display string
		// to the current LV_ITEM

		char* pStr = sValue.GetBuffer();
		pItem->cchTextMax = sizeof( *pStr );
		lstrcpy( pItem->pszText, pStr );
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
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
		if ( m_Playlist && pLVCD->nmcd.dwItemSpec < m_Playlist->GetCount() && !m_Arranging )
		{
			CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
			int nSubType = m_Prefs->GetPlaylistCol( pLVCD->iSubItem );

			if ( nSubType == MUSIK_LIBRARY_TYPE_RATING )
			{
				pDC->SelectObject( m_StarFont );
				pDC->SetTextColor( m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );
			}
			else if ( m_Player->GetPlaylist() == m_Playlist && m_Player->IsPlaying() && m_Playlist->GetSongID( pLVCD->nmcd.dwItemSpec ) == m_Player->GetCurrPlaying()->GetID() )
			{
				pDC->SelectObject( m_BoldFont );
				pDC->SetTextColor( m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );				
			}
			else
				pDC->SelectObject( m_ItemFont );

			// which stripe color to use
			if ( pLVCD->nmcd.dwItemSpec % 2 != 0 )
				pLVCD->clrTextBk = clrStripe;
			else
				pLVCD->clrTextBk = m_Prefs->MUSIK_COLOR_LISTCTRL;

			pLVCD->clrText = m_Prefs->MUSIK_COLOR_LISTCTRLTEXT;		

			*pResult = CDRF_NEWFONT;
			return;
		}
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InitFonts()
{
	m_ItemFont.CreateStockObject( DEFAULT_GUI_FONT );
	m_StarFont.CreatePointFont( 100, "musik" );

	LOGFONT pBoldFont;
	m_ItemFont.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;

	m_BoldFont.CreateFontIndirect( &pBoldFont );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InitColors()
{
	int r, g, b;
	
	r = GetRValue( m_Prefs->MUSIK_COLOR_LISTCTRL );
	g = GetGValue( m_Prefs->MUSIK_COLOR_LISTCTRL );
	b = GetBValue( m_Prefs->MUSIK_COLOR_LISTCTRL );
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
void CmusikPlaylistCtrl::OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

	m_SongInfoCache->Set( pCacheHint->iFrom, pCacheHint->iTo, false );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPaint()
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
	memDC.FillSolidRect( clip, m_Prefs->MUSIK_COLOR_LISTCTRL );
	   
	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

CString CmusikPlaylistCtrl::GetTimeStr( int item )
{
	int time_ms = atoi( m_SongInfoCache->GetValue( item - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_DURATION ).c_str() );
	return (CString)m_Player->GetTimeStr( time_ms );	
}

///////////////////////////////////////////////////

CString CmusikPlaylistCtrl::GetRating( int item )
{
	int nRating = atoi( m_SongInfoCache->GetValue( item - m_SongInfoCache->GetFirst(), MUSIK_LIBRARY_TYPE_RATING ).c_str() );

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

void CmusikPlaylistCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// get cursor pos at time of click in relation
	// to the client
	CPoint ptCurr;
	GetCursorPos( &ptCurr );
	ScreenToClient( &ptCurr );

	// see which item was clicked
	LVHITTESTINFO hit_test;
	hit_test.pt.y = ptCurr.y;
	hit_test.pt.x = 0;
	SubItemHitTest( &hit_test );
	int nItem = hit_test.iItem;

	if ( nItem > -1 )
	{
		int rating_pos = -1;
		for ( size_t i = 0; i < m_Prefs->GetPlaylistColCount(); i++ )
		{
			if ( m_Prefs->GetPlaylistCol( i ) == MUSIK_LIBRARY_TYPE_RATING )
				rating_pos = (int)i;
		}

		if ( rating_pos == -1 )
			return;

		CRect rcRating;
		GetSubItemRect( nItem, rating_pos, LVIR_LABEL, rcRating );

		if ( rcRating.PtInRect( ptCurr ) )
		{
			// if an item is a sub item, there are two
			// spaces in front of the item text -- so
			// shift point that far in the X axis
			int nOffset;
			if ( rating_pos > 0 )
				nOffset = m_TwoSpace;
			else
				nOffset = 2;

			// width of the column and the
			// recalculated click locations...
			int col_width = GetColumnWidth( rating_pos );
			int clk_loc = ( ptCurr.x - rcRating.left );

			int nRating;
			if ( clk_loc <= nOffset )
				nRating = 0;
			else if ( clk_loc >= m_RatingExtent + nOffset )
				nRating = 5;
			else 
			{
				nRating = ( clk_loc / ( m_RatingExtent / 5 ) );

				if ( rating_pos == 0 )
					nRating++;
			}

            m_Library->SetSongRating( m_Playlist->GetSongID( hit_test.iItem ), nRating );	
			ResyncItem( m_Playlist->GetSongID( hit_test.iItem ), hit_test.iItem );
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ResyncItem( size_t songid, int item )
{
	if ( m_SongInfoCache->ResyncItem( songid ) )
	{
		if ( item != -1 )
			RedrawItems( item, item );
		else
			RedrawWindow();
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SavePlaylist( bool check_prompt )
{
	// if the user wants to be prompted...
	if ( m_PlaylistNeedsSave && 
		( m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_STANDARD || m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_SUBLIBRARY ) )
	{
		if ( m_Playlist->GetPlaylistID() == -1 )
			return;

		// user needs to be prompted
		bool save = true;
		if ( check_prompt )
		{
			// throw dialog
			if ( m_Prefs->GetStdPlaylistPrompt() == -1 )
			{
				CmusikSaveStdPlaylist* pDlg;
				pDlg = new CmusikSaveStdPlaylist( this, m_Prefs );
				int nRes = pDlg->DoModal();

				// write the playlist if "ok" was pressed
				if ( nRes != IDOK )
					save = false;

				delete pDlg;
			}

			// otherwise just take the pref's default
			else if ( m_Prefs->GetStdPlaylistPrompt() != 1 )
				save = false;
		}

		// save
		if ( save )
		{
			if ( UseTempTable() )
			{
				CmusikPlaylist temp;
				temp.m_ID = m_Playlist->m_ID;

				m_Library->GetAllSongs( temp, true );
				m_Library->RewriteStdPlaylist( &temp );
			}
			else
				m_Library->RewriteStdPlaylist( m_Playlist );

			m_PlaylistNeedsSave = false;
		}
	}
}


///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SetPlaylist( CmusikPlaylist* playlist )
{
	if ( playlist == NULL )
	{
		TRACE0( "Passed a NULL playlist\n" );
		return;
	}

	SetItemState( -1, 0, LVIS_SELECTED );

	// delete any old playlist that is not a now
	// playing type...
	if ( m_Playlist && m_Playlist->m_Type != MUSIK_PLAYLIST_TYPE_NOWPLAYING )
		delete m_Playlist;

	// set the playlist and cache it
	m_Playlist = playlist;

	if ( m_SongInfoCache )
		m_SongInfoCache->SetPlaylist( playlist );

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	

	if ( m_TagEdit )
		m_TagEdit->UpdatePlaylist( m_Playlist );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SetInfoCtrl( CmusikPlaylistInfoCtrl *ctrl )
{
	m_InfoCtrl = ctrl;
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem( int n )
{
	if ( m_Prefs->AddEntireToNP() )
		return PlayItem_ReplaceNP( n );

	return PlayItem_AddNP( n );
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem_ReplaceNP( int n )
{
	if ( !m_Playlist )
		return false;

	if ( n == -1 )
	{
		POSITION pos = GetFirstSelectedItemPosition();
		n = GetNextSelectedItem ( pos );
	}	

	if ( m_Player->GetPlaylist() != m_Playlist )
		m_Player->SetPlaylist( m_Playlist );

	m_Player->Play( n, MUSIK_CROSSFADER_NEW_SONG );

	SetItemState( n, 0, LVIS_SELECTED );

	return true;
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem_AddNP( int n )
{
	if ( !m_Playlist )
		return false;

	if ( n == -1 )
	{
		POSITION pos = GetFirstSelectedItemPosition();
		n = GetNextSelectedItem ( pos );
	}	

	int insert_at = -1;

	// get activated item's songid
	CmusikSong song = m_Playlist->GetSong( n );

	// assure player has a playlist
	if ( !m_Player->GetPlaylist() )
		m_Player->SetPlaylist( new CmusikPlaylist() );

	// insert song into player's playlist
	if ( GetKeyState( VK_MENU ) >= 0 )
	{
		if ( m_Player->GetPlaylist() != m_Playlist )
		{
			if ( m_Player->GetIndex() + 1 >= (int)m_Player->GetPlaylist()->GetCount() )
			{
				m_Player->GetPlaylist()->Add( song );
				insert_at = m_Player->GetPlaylist()->GetCount() - 1;
			}
			else
			{
				insert_at = m_Player->GetIndex() + 1;
				m_Player->GetPlaylist()->InsertAt( song, insert_at ); 
			}
		}
		else
			insert_at = n;
	}
	else
	{
		CmusikPlaylist* copy = new CmusikPlaylist( *m_Playlist );
		m_Player->SetPlaylist( copy );
		insert_at = n;
	}
	
	// play it
	if ( insert_at > -1 && GetKeyState( VK_CONTROL ) >= 0 )
		m_Player->Play( insert_at, MUSIK_CROSSFADER_NEW_SONG );

	SetItemState( n, 0, LVIS_SELECTED );

	return true;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = NULL;

	int item = pNMIA->iItem;

	PlayItem( item );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::BeginDrag( NMHDR* pNMHDR, bool right_button )
{
	// set cursor back to an arrow
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	int            nSelItem;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // For every selected item in the list, put the filename into lsDraggedFiles.
    pos = GetFirstSelectedItemPosition();
	while ( pos )
    {
        nSelItem = GetNextSelectedItem ( pos );
		sFile = m_Playlist->GetField( nSelItem, MUSIK_LIBRARY_TYPE_FILENAME );
        
        lsDraggedFiles.AddTail ( sFile );

        // Calculate the # of chars required to hold this string.
        uBuffSize += lstrlen ( sFile ) + 1;
    }

	if ( !lsDraggedFiles.GetCount() )
		return;

    // Add 1 extra for the final null char, and the size of the DROPFILES struct.
    uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

    // Allocate memory from the heap for the DROPFILES struct.
    hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );

    if ( !hgDrop )
        return;

    pDrop = (DROPFILES*) GlobalLock ( hgDrop );

    if ( !pDrop )
	{
		GlobalFree ( hgDrop );
		return;
	}

    // Fill in the DROPFILES struct.
    pDrop->pFiles = sizeof(DROPFILES);

	// If we're compiling for Unicode, set the Unicode flag in the struct to
	// indicate it contains Unicode strings.
	#ifdef _UNICODE
		pDrop->fWide = TRUE;
	#endif;

    // Copy all the filenames into memory after the end of the DROPFILES struct.
    pos = lsDraggedFiles.GetHeadPosition();
    pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

    while ( pos )
	{
        lstrcpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
        pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	}

    GlobalUnlock ( hgDrop );

    // Put the data in the data source.
    datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );

    // Add in our own custom data, so we know that the drag originated from our 
    // window.  CMyDropTarget::DragEnter() checks for this custom format, and
    // doesn't allow the drop if it's present.  This is how we prevent the user
    // from dragging and then dropping in our own window.
    // The data will just be a dummy bool.
	HGLOBAL hgBool;

    hgBool = GlobalAlloc ( GHND | GMEM_SHARE, sizeof(bool) );

    if ( NULL == hgBool )
    {
        GlobalFree ( hgDrop );
        return;
    }

    // Put the data in the data coosource.
	if ( right_button )
		etc.cfFormat = m_DropID_R;
	else 
		etc.cfFormat = m_DropID_L;

	datasrc.CacheGlobalData ( right_button ? m_DropID_R : m_DropID_L, hgBool, &etc );

	// post a message to the main frame, letting
	// it know that drag and drop has started
	int WM_DRAGSTART = RegisterWindowMessage( "DRAGSTART" );
	m_Parent->SendMessage( WM_DRAGSTART, NULL );

    // Start the drag 'n' drop!
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

	int WM_DRAGEND = RegisterWindowMessage( "DRAGEND" );
	m_Parent->SendMessage( WM_DRAGEND, NULL );

    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
    switch ( dwEffect )
    {
        case DROPEFFECT_COPY:
        case DROPEFFECT_MOVE:
        {
			// the copy completed successfully
			// do whatever we need to do here
			TRACE0( "DND from playlist completed successfully. The data has a new owner.\n" );
        }
        break;

        case DROPEFFECT_NONE:
        {
            // This needs special handling, because on NT, DROPEFFECT_NONE
            // is returned for move operations, instead of DROPEFFECT_MOVE.
            // See Q182219 for the details.
            // So if we're on NT, we check each selected item, and if the
            // file no longer exists, it was moved successfully and we can
            // remove it from the list.
            if ( m_IsWinNT )
            {
				// the copy completed successfully
				// on a windows nt machine.
				// do whatever we need to do here

                bool bDeletedAnything = false;
                for ( size_t i = GetNextItem ( -1, LVNI_SELECTED );	i != -1; i = GetNextItem ( i, LVNI_SELECTED ) )
                {
                    CString sFilename = GetItemText ( nSelItem, 0 );

                    if ( GetFileAttributes ( sFile ) == 0xFFFFFFFF  &&
                         GetLastError() == ERROR_FILE_NOT_FOUND )
                    {
                        // We couldn't read the file's attributes, and GetLastError()
                        // says the file doesn't exist, so remove the corresponding 
                        // item from the list.
                        DeleteItem ( nSelItem );
                    
                        nSelItem--;
                        bDeletedAnything = true;
                    }
                }

                if ( ! bDeletedAnything )
                {
                    // The DnD operation wasn't accepted, or was canceled, so we 
                    // should call GlobalFree() to clean up.
                    GlobalFree ( hgDrop );
                    GlobalFree ( hgBool );

					TRACE0( "DND had a problem. We had to manually free the data.\n" );
				}
			}

			// not windows NT
			else
			{
				// We're on 9x, and a return of DROPEFFECT_NONE always means
				// that the DnD operation was aborted.  We need to free the
				// allocated memory.
				GlobalFree ( hgDrop );
				GlobalFree ( hgBool );

				TRACE0( "DND had a problem. We had to manually free the data.\n" );
			}
		}

        break;
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	BeginDrag( pNMHDR, false );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDropFiles( HDROP hDropInfo )
{
	// if the drag originated from ourself, then
	// we want to rearrange items, and nothing more.
	if ( m_DropArrange )
	{
		CPoint ptCursor;
		::GetCursorPos( &ptCursor );
		ScreenToClient( &ptCursor );
		
		int nPos = HitTest( ptCursor );

		// get first selected item.. if we can't
		// find one, return.
		int nFirstSel = GetFirstSelected();
		if ( nFirstSel == -1 )
			return;

		// item dragged onto itself
		if ( nPos == nFirstSel )
			return;

		// get selected
		CIntArray sel;
		GetSelectedItems( sel );
		
		CmusikPlaylist sel_songs;
		GetSelectedSongs( sel_songs );

		// remove selected items from their
		// current position
		DeleteItems( sel, false );
		sel.clear();

		// get the id of the currently playing
		// song, we'll find it once the drag is done
		// to assure the correct song is bold.
		int songid = m_Player->GetCurrPlaying()->GetID();

		// insert the items back to their new
		// position...
		InsertItems( sel_songs, nFirstSel, nPos );
		sel_songs.Clear();

		// grab the new index..
		m_Player->FindNewIndex( songid );

		if ( m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_STANDARD )
			m_PlaylistNeedsSave = true;

		m_DropArrange = false;
		return;
	}

	// see if we need to prompt the user for
	// which playlist to insert files to...
	int nRet = m_Prefs->GetFileDropPrompt();

	if ( nRet == -1 )
	{
		CmusikFileDrop* pDlg = new CmusikFileDrop( this, m_Prefs );
		nRet = pDlg->DoModal();
		delete pDlg;

		if ( nRet == MUSIK_FILEDROP_CANCEL )
			return;
	}

	// if we get here, the drag originated from 
	// somewhere else, such as explorer...
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	CmusikStringArray* files = NULL;

	files = new CmusikStringArray();

	CmusikString sTemp;
	for ( size_t i = 0; i < nNumFiles; i++ )
	{
		if ( DragQueryFile( hDropInfo, i, szNextFile, MAX_PATH ) > 0 )
		{
			// get the filetype. if its a directory
			// that was dropped, we'll want to 
			// recurse it and add all the supported 
			// media files...
			SHGetFileInfo( szNextFile, 0, &rFileInfo, sizeof( rFileInfo ), SHGFI_ATTRIBUTES );
			if ( rFileInfo.dwAttributes & SFGAO_FOLDER )
			{
				sTemp = szNextFile;
				sTemp += "\\*.*";

				m_Dir.m_Dir = sTemp;
				m_Dir.m_Target = files;

				m_Dir.Run();
			}		

			// otherwise it was just a file... add it...
			else
				files->push_back( szNextFile );
		}
	}

	if ( files->size() > 0 )
	{
		CmusikBatchAdd* params;

		if ( nRet == MUSIK_FILEDROP_ADDNOWPLAYING )
			params = new CmusikBatchAdd( files, m_Player->GetPlaylist(), m_Library, m_Player, NULL, 0, 1, 1 );

		else if ( nRet == MUSIK_FILEDROP_ADDPLAYLIST )
		{
			params = new CmusikBatchAdd( files, m_Playlist, m_Library, NULL, NULL, 1, 0, 1 );
			m_PlaylistNeedsSave = true;
		}

		else if ( nRet == MUSIK_FILEDROP_ADDLIBRARY )
			params = new CmusikBatchAdd( files, m_Player->GetPlaylist(), m_Library, NULL, NULL );

		int WM_BATCHADD_NEW = RegisterWindowMessage( "BATCHADD_NEW" );
		m_Parent->SendMessage( WM_BATCHADD_NEW, (WPARAM)params );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedItems( CIntArray& items )
{
	items.clear();

    POSITION pos = GetFirstSelectedItemPosition();
	while ( pos )
        items.push_back( GetNextSelectedItem ( pos ) );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedSongs( CmusikPlaylist& items )
{
	items.Clear();
	
	POSITION pos = GetFirstSelectedItemPosition();
	while ( pos )
        items.Add( m_Playlist->GetSong( GetNextSelectedItem( pos ) ) );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedSongs( CmusikSongInfoArray& items )
{
	items.clear();

	CmusikSong curr_song;
	CmusikSongInfo curr_info;
	
	POSITION pos = GetFirstSelectedItemPosition();
	m_Library->BeginTransaction();
	while ( pos )
	{
		curr_song = m_Playlist->GetSong( GetNextSelectedItem( pos ) );
		m_Library->GetSongInfoFromID( curr_song.GetID(), &curr_info );
		items.push_back( curr_info );
	}
	m_Library->EndTransaction();
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::GetFirstSelected()
{
    POSITION pos = GetFirstSelectedItemPosition();
	int nRet = GetNextSelectedItem ( pos );
    
	return nRet;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetItemIDs( const CIntArray& items, CIntArray* target )
{
	target->clear();

	for ( size_t i = 0; i < items.size(); i++ )
		target->push_back( m_Playlist->GetSongID( items.at( i ) ) );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::DeleteItems( const CIntArray& items, bool update )
{
	if ( items.size() <= 0 )
		return;

	int nScrollPos = GetScrollPos( SB_VERT );

	for ( size_t i = 0; i < items.size(); i++ )
	{
		if ( m_Player->GetPlaylist() == m_Playlist )
		{
			if ( m_Player->m_Index >= (int)items.at( i ) - (int)i )
				m_Player->m_Index--;
		}

		m_Playlist->DeleteAt( items.at( i ) - i );
	}

	if ( m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_STANDARD
		|| m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_SUBLIBRARY )
		m_PlaylistNeedsSave = true;

	if ( update )
	{
		UpdateV();

		// deselect all, then select first...
		SetItemState( -1, 0, LVIS_SELECTED );
		if ( GetItemCount() > 0 )
		{
			if ( items.at( 0 ) > GetItemCount() )
				SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
			else
				SetItemState( items.at( 0 ), LVIS_SELECTED, LVIS_SELECTED );
		}

		SetScrollPos( SB_VERT, nScrollPos );
	}

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InsertItems( CmusikPlaylist& items, int firstsel, int at, bool update )
{
	if ( items.GetCount() <= 0 )
		return;

	int nScrollPos = GetScrollPos( SB_VERT );

	// see if we need to insert items above
	// or below the target... depends if we
	// are dragging up or down.
	if ( firstsel < at )
	{
		at -= ( items.GetCount() - 1 );
		if ( at < -1 )
			at = -1;
	}

	m_Playlist->InsertAt( items.m_Songs, at );

	if ( update )
	{
		UpdateV();

		// deselect all, then select the items that
		// were just rearranged...
		SetItemState( -1, 0, LVIS_SELECTED );
		if ( at == -1 )
			at = GetItemCount() - items.GetCount();
		for ( size_t i = at; i < at + items.GetCount(); i++ )
			SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

		SetScrollPos( SB_VERT, nScrollPos );
	}

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// returning non-zero ignores the message,
	// and marquee never appears.
	*pResult = 1;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDragColumn( int source, int dest )
{
	if ( source == dest )
		return;

	m_Arranging = true;

	CIntArray order = m_Prefs->GetPlaylistOrder();
	CIntArray sizes = m_Prefs->GetPlaylistSizes();

	order.insert( order.begin() + dest, order.at( source ) );
	sizes.insert( sizes.begin() + dest, sizes.at( source ) );

	if ( source > dest )
		source++;

	order.erase( order.begin() + source );
	sizes.erase( sizes.begin() + source );

	m_Prefs->SetPlaylistOrder( order );
	m_Prefs->SetPlaylistSizes( sizes );

	m_Arranging = false;

	ResetColumns();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetRatingExtent()
{
	int spc = 0;
	CSize szText;

	HDC	hMemDC	= NULL;
	hMemDC = CreateCompatibleDC( NULL );
	if ( hMemDC )
	{
		CDC* pDC = CDC::FromHandle( hMemDC );
		if ( pDC )
		{
			CFont* pOldFont = pDC->SelectObject( &m_StarFont );

			// list control always adds two spaces after
			// each string in a column, this must be
			// subtracted.
			szText = pDC->GetTextExtent( _T( "  " ) );
			m_TwoSpace = szText.cx;

			// supposed extent of the five stars
			szText = pDC->GetTextExtent( ",,,,," );

			pOldFont = pDC->SelectObject( pOldFont );
		}
	}
	DeleteDC( hMemDC );

	m_RatingExtent = szText.cx;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ( pNMLV->iSubItem >= 0 )
	{
		// draw sort arrow
		int nCurrCol = pNMLV->iSubItem;
		DrawSortArrow( nCurrCol );

		// sort
		int songid = -1;
		if ( m_Player->IsPlaying() )
			songid = m_Player->GetPlaylist()->GetSongID( m_Player->GetIndex() );

		int type = m_Prefs->GetPlaylistCol( pNMLV->iSubItem );
		m_Library->SortPlaylist( m_Playlist, type, m_Ascend ? false : true );

		if ( m_Player->IsPlaying() && songid != -1 )
			m_Player->FindNewIndex( songid );

		UpdateV();

		if ( m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_STANDARD )
			m_PlaylistNeedsSave = true;
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::DrawSortArrow( int nCurrCol )
{
	if ( nCurrCol == m_LastCol )
		m_Ascend = ( m_Ascend == true ) ? false : true;
	else
		m_Ascend = true;

	// set new arrow
	HDITEM header_item;
	CHeaderCtrl* header_ctrl = GetHeaderCtrl();

	header_item.mask = HDI_FORMAT | HDI_BITMAP;
	header_ctrl->GetItem( nCurrCol, &header_item );

	if ( header_item.hbm != 0 ) 
	{
		DeleteObject( header_item.hbm );
		header_item.hbm = 0;
	}

	header_item.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
	header_item.hbm  = (HBITMAP)LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE( m_Ascend ? IDB_UP : IDB_DOWN ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );

	header_ctrl->SetItem( nCurrCol, &header_item );

	// unset old arrow
	if ( m_LastCol != -1 && m_LastCol != nCurrCol )
	{
		header_ctrl->GetItem( m_LastCol, &header_item );
		header_item.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);

		if ( header_item.hbm != 0 ) 
		{
			DeleteObject( header_item.hbm );
			header_item.hbm = 0;
		}

		header_ctrl->SetItem( m_LastCol, &header_item );
	}

	m_LastCol = nCurrCol;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::HideSortArrow()
{
	HDITEM header_item;
	CHeaderCtrl* header_ctrl = GetHeaderCtrl();

	if ( m_LastCol > -1 && m_LastCol < (int)m_Prefs->GetPlaylistColCount() )
	{
		header_ctrl->GetItem( m_LastCol, &header_item );
		header_item.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);

		if ( header_item.hbm != 0 ) 
		{
			DeleteObject( header_item.hbm );
			header_item.hbm = 0;
		}

		header_ctrl->SetItem( m_LastCol, &header_item );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	BeginDrag( pNMHDR, true );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ScrollToCurr()
{
	if ( m_Player->IsPlaying() )
		EnsureVisible( m_Player->GetIndex(), FALSE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ShowContextMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_PLAYLIST_CONTEXT_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	if ( GetSelectedCount() == 0 )
	{
		popup_menu->EnableMenuItem( ID_PLC_DELETE_FROMPLAYLIST, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLC_DELETE_FROMLIBRARY, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLC_DELETE_FROMCOMPUTER, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_PROPERTIES, MF_DISABLED | MF_GRAYED );
	}
	if ( GetSelectedCount() <= 1 )
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_BATCH, MF_DISABLED | MF_GRAYED );

	if ( m_Playlist != m_Player->GetPlaylist() )
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, MF_DISABLED | MF_GRAYED );

	if ( !m_Player->GetPlaylist()->GetCount() )
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, MF_DISABLED | MF_GRAYED );

	if ( m_TagEdit )
		popup_menu->CheckMenuItem( ID_PLAYLISTCONTEXTMENU_PROPERTIES, MF_CHECKED );

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::GetColumnPos( int field )
{
	for ( size_t i = 0; i < m_Prefs->GetPlaylistColCount(); i++ )
	{
		if ( m_Prefs->GetPlaylistCol( i ) == field )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ShowHeaderMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_PLAYLIST_COLUMNS_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	CIntArray items = m_Prefs->GetPlaylistOrder();

	for ( size_t i = 0; i < items.size(); i++ )
	{
		switch( items.at( i ) )
		{
		case MUSIK_LIBRARY_TYPE_ARTIST:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_ARTIST, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_ALBUM:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_ALBUM, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_YEAR:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_YEAR, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_GENRE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_GENRE, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_TITLE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TITLE, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_TRACKNUM:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TRACKNUMBER, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_TIMEADDED:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TIMEADDED, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_LASTPLAYED:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_LASTPLAYED, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_FILESIZE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_FILESIZE, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_FORMAT:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_FORMAT, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_DURATION:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_DURATION, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_RATING:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_RATING, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TIMESPLAYED, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_BITRATE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_BITRATE, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_FILENAME:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_FILENAME, MF_CHECKED );
			break;

		case MUSIK_LIBRARY_TYPE_EQUALIZER:
			popup_menu->CheckMenuItem( ID_EQUALIZER_LOCKCHANNELS, MF_CHECKED );
			break;
		}
	}
		
	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::AddColumn( int field, bool refresh_cols )
{
	m_Prefs->AppendPlaylistColOrder( field );
	m_Prefs->AppendPlaylistColSize( 50 );

	if ( refresh_cols )
		ResetColumns();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::RemoveColumn( int at, bool refresh_cols )
{
	m_Prefs->RemovePlaylistColOrder( at );
	m_Prefs->RemovePlaylistColSize( at );

	if ( refresh_cols )
		ResetColumns();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	SaveColumns();
	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ToggleColumn( int field )
{
	int pos = GetColumnPos( field );
	if ( pos == -1 )
		AddColumn( field );
	else
		RemoveColumn( pos );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsArtist()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_ARTIST );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsAlbum()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_ALBUM );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsYear()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_YEAR );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsGenre()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_GENRE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTitle()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_TITLE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTracknumber()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_TRACKNUM );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTimeadded()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_TIMEADDED );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsLastplayed()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_LASTPLAYED );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsFilesize()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_FILESIZE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsFormat()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_FORMAT );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsDuration()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_DURATION );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsRating()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_RATING );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTimesplayed()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_TIMESPLAYED );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsBitrate()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_BITRATE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsFilename()
{
	ToggleColumn( MUSIK_LIBRARY_TYPE_FILENAME );
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::UseTempTable()
{
	if ( m_Playlist->m_Type == MUSIK_PLAYLIST_TYPE_SUBLIBRARY )
		return true;

	return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;

	WORD nChar = pLVKeyDow->wVKey;

	if ( nChar == VK_DELETE )
	{
		bool delete_from_lib = false;
		bool delete_from_comp = false;

		// delete from library
		if ( GetKeyState( VK_MENU ) < 0 || UseTempTable() )
			delete_from_lib = true;

		// delete from computer
		else if ( GetKeyState( VK_CONTROL ) < 0 )
		{
			delete_from_lib = true;
			delete_from_comp = true;
		}

		// delete from playlist
		DeleteSelectedItems( delete_from_lib, delete_from_comp );
		return;
	}

	if ( nChar == 'A' || nChar == 'a' )
	{
		if ( GetKeyState( VK_CONTROL ) < 0 )
			SetItemState( -1, LVIS_SELECTED, LVIS_SELECTED );

		return;
	}

	if ( nChar == 'D' || nChar == 'd' )
	{
		if ( GetKeyState( VK_CONTROL ) < 0 )
			SetItemState( -1, 0, LVIS_SELECTED );

		return;
	}

	if ( nChar == 'B' || nChar == 'b' )
	{
		if ( GetKeyState( VK_CONTROL ) < 0 )
			OnPlaylistcontextmenuBatch();

		return;
	}

	if ( nChar == 'T' || nChar == 't' )
	{
		if ( GetKeyState( VK_CONTROL ) < 0 )
			OnPlaylistcontextmenuProperties();

		return;
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::DeleteSelectedItems( bool from_library, bool from_computer )
{
	if ( from_library )
	{
		CmusikPlaylist sel_songs;
		GetSelectedSongs( sel_songs );

		if ( from_computer )
		{
			if ( MessageBox( _T( "Are you sure you want to permanently delete these files from your computer?" ), MUSIK_VERSION_STR, MB_ICONWARNING | MB_YESNO ) == IDNO )
				return;	
		}

		m_Library->DeleteSongs( sel_songs, from_computer, UseTempTable() );
	}

	CIntArray sel;
	GetSelectedItems( sel );
	DeleteItems( sel, true );

	if ( from_library || ( from_computer && UseTempTable() ) )
	{
		int WM_SELBOXREQUESTUPDATE = RegisterWindowMessage( "SELBOXREQUESTUPDATE" );
		m_Parent->SendMessage( WM_SELBOXREQUESTUPDATE );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlcDeleteFromplaylist()
{
	DeleteSelectedItems();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlcDeleteFromlibrary()
{
	DeleteSelectedItems( true, false );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlcDeleteFromcomputer()
{
	DeleteSelectedItems( true, true );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuShufflenowplaying()
{
	m_Player->Shuffle();

	if ( GetPlaylist() == m_Player->GetPlaylist() )
	{
		UpdateV();
		ScrollToCurr();
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuProperties()
{
	if ( m_TagEdit )
		OnTagEditDestroy();

	else
	{
		m_TagEdit = new CmusikTagDlg( this, m_Playlist, m_Library );
		m_TagEdit->Create( IDD_TAG_PROPERTIES, this );
		m_TagEdit->ShowWindow( SW_SHOWNORMAL );

		m_TagEdit->UpdateSel( GetFirstSelected() );
	}
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagEditDestroy( WPARAM wParam, LPARAM lParam )
{
	m_TagEdit->DestroyWindow();
	delete m_TagEdit;
	m_TagEdit = NULL;

	return 0L;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ( m_TagEdit )
		m_TagEdit->UpdateSel( pNMLV->iItem );
	
	*pResult = 0;
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagNext( WPARAM wParam, LPARAM lParam )
{
	int sel = GetFirstSelected();
	if ( sel + 1 == m_Playlist->GetCount() )
		sel = 0;
	else
		sel++;

	SetSelectionMark( sel );
	SetItemState( -1, 0, LVIS_SELECTED );
	SetItemState( sel, LVIS_SELECTED, LVIS_SELECTED );
	EnsureVisible( sel, FALSE );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagPrev( WPARAM wParam, LPARAM lParam )
{
	int sel = GetFirstSelected();
	if ( sel - 1 == -1 )
		sel = m_Playlist->GetCount() - 1;
	else
		sel--;

	SetSelectionMark( sel );
	SetItemState( -1, 0, LVIS_SELECTED );
	SetItemState( sel, LVIS_SELECTED, LVIS_SELECTED );
	EnsureVisible( sel, FALSE );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagUpdate( WPARAM wParam, LPARAM lParam )
{
	if ( wParam )
	{
		int songid = (int)wParam;
		ResyncItem( songid );
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuBatch()
{
	CmusikSongInfoArray* selected = new CmusikSongInfoArray();
	GetSelectedSongs( *selected );

	CmusikBatchTagDlg batch( this, selected );
	
	if ( batch.DoModal() == IDOK )
	{
		CmusikBatchRetag* params;

		params = new CmusikBatchRetag( m_Library, NULL, selected );
		params->m_DeleteUpdatedTags = true;
		params->m_WriteToFile = false;

		int WM_BATCHRETAG_NEW = RegisterWindowMessage( "BATCHRETAG_NEW" );
		m_Parent->SendMessage( WM_BATCHRETAG_NEW, (WPARAM)params );		
	}
	else
		delete selected;
}

///////////////////////////////////////////////////


