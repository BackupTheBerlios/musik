// Michael Dunn -- http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop -- DND

///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikPrefs.h"

#include "musikPlaylistCtrl.h"
#include "musikSourcesCtrl.h"
#include "musikFileDrop.h"

#include "musikSaveStdPlaylist.h"

#include "../musikCore/include/musikPlaylist.h"
#include "../musikCore/include/musikArrays.h"
#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikDynDspInfo.h"
#include "../musikCore/include/musikPlayer.h"
#include "../musikCore/include/musikBatchAdd.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

#pragma warning( disable : 4244 )

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPlaylistCtrl, CmusikListCtrl)

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
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPlaylistCtrl::CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid )
{
	// core
	m_Playlist	= NULL;
	m_Library	= library;
	m_Prefs		= prefs;
	m_Player	= player;

	// is a column being rearranged?
	m_Arranging = false;

	// musik will always start up
	// with library default selected
	m_PlaylistType = MUSIK_SOURCES_TYPE_LIBRARY;

	// dnd drop id
	m_DropID = dropid;

	// main window
	m_MainWnd = mainwnd;

	// misc
	m_Changed = true;
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
	delete m_SongInfoCache;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ResetColumns()
{
	while ( DeleteColumn( 0 ) );

	for ( size_t i = 0; i < m_Prefs->GetPlaylistColCount(); i++ )
	{
		InsertColumn( (int)i, m_Library->GetSongField( m_Prefs->GetPlaylistCol( i ) ) );
		SetColumnWidth( (int)i, m_Prefs->GetPlaylistColWidth( i ) );
	}
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

void CmusikPlaylistCtrl::UpdateV( bool redraw )
{
	if ( m_Playlist )
	{
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

	ResetColumns();

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

			else
			{
				if ( m_Player->IsPlaying() &&
					m_Player->GetCurrPlaying()->GetID() == m_Playlist->GetSongID( pLVCD->nmcd.dwItemSpec ) && 
					GetPlaylist() == m_Player->GetPlaylist() && 
					pLVCD->nmcd.dwItemSpec == m_Player->GetIndex() )
					pDC->SelectObject( m_BoldFont );
				else
					pDC->SelectObject( m_ItemFont );
			}

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

	// construct a hit test for the listctrl
	LVHITTESTINFO hit_test;
	hit_test.pt = ptCurr;
	SubItemHitTest( &hit_test );

	// check to see if the click was over a
	// (sub)item's label...
	if( hit_test.flags & LVHT_ONITEMLABEL )
	{
		// if the current column is the rating
		if ( m_Prefs->GetPlaylistCol( hit_test.iSubItem ) == MUSIK_LIBRARY_TYPE_RATING )
		{
			// normalize along x axis
			for ( int i = 0; i < hit_test.iSubItem; i++ )
				ptCurr.x -= m_Prefs->GetPlaylistColWidth( (size_t)i );

			// if an item is a sub item, there are two
			// spaces in front of the item text -- so
			// shift point that far in the X axis
			int nOffset;
			if ( hit_test.iSubItem > 0 )
				nOffset = m_TwoSpace;
			else
				nOffset = 2;

			int nRating;
			if ( ( ptCurr.x ) <= nOffset )
				nRating = 0;
			else if ( ( ptCurr.x - nOffset ) >= m_RatingExtent + nOffset )
				nRating = 5;
			else
				nRating = ( ( ptCurr.x - nOffset ) / ( m_RatingExtent / 5 ) ) + 1;

            m_Library->SetSongRating( m_Playlist->GetSongID( hit_test.iItem ), nRating );	
			if ( m_SongInfoCache->ResyncItem( m_Playlist->GetSongID( hit_test.iItem ) ) )
				RedrawItems( hit_test.iItem, hit_test.iItem );
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SetPlaylist( CmusikPlaylist* playlist, int m_Type )
{
	if ( playlist == NULL )
	{
		TRACE0( "Well, something messed up, our playlist is now NULL...\n" );
	}

	// if the last item was a standard playlist,
	// we may need to save it. so check...
	if ( m_PlaylistType == MUSIK_PLAYLIST_TYPE_STANDARD )
	{
		// if the user wants to be prompted...
		if ( m_PlaylistNeedsSave )
		{
			// user wants to be prompted
			if ( m_Prefs->GetStdPlaylistPrompt() == -1 )
			{
				CmusikSaveStdPlaylist* pDlg;
				pDlg = new CmusikSaveStdPlaylist( this, m_Prefs );
				int nRes = pDlg->DoModal();

				if ( nRes == IDOK )
					m_Library->RewriteStdPlaylist( m_Playlist->GetPlaylistID(), m_Playlist );
			}

			// otherwise just take the pref's default
			else
			{
				if ( m_Prefs->GetStdPlaylistPrompt() == 1 )
					m_Library->RewriteStdPlaylist( m_Playlist->GetPlaylistID(), m_Playlist );
			}

			m_PlaylistNeedsSave = false;
		}
	}

	if ( playlist != m_Playlist )
	{
		if ( m_SongInfoCache )
			m_SongInfoCache->SetPlaylist( playlist );
	    
		m_Playlist = playlist;
		m_PlaylistType = m_Type;
		
		m_Changed = true;
	}
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem( int n )
{
	if ( !m_Playlist )
		return false;

	if ( n == -1 )
	{
		POSITION pos = GetFirstSelectedItemPosition();
		n = GetNextSelectedItem ( pos );
	}	

	// give the current playlist to the player,
	// unless the player already owns it.
	if ( m_Changed )
	{
		// give the current playlist to the player,
		// if it does not belong to the player. otherwise,
		// it will delete itself, then try to copy itself.
		// confused yet? i am...
		if ( m_Playlist != m_Player->GetPlaylist() )
		{
			// the player now owns the playlist, we
			// just continue to point to it. later,
			// a message will be sent back to it's previous
			// owner, letting it know its ok to create
			// a new one in it's place.
			m_Player->SetPlaylist( m_Playlist );
		}

		// post a message to our parent, letting it
		// know the player owns the playlist.
		int WM_PLAYERNEWPLAYLIST = RegisterWindowMessage( "PLAYERNEWPLAYLIST" );
		m_MainWnd->SendMessage( WM_PLAYERNEWPLAYLIST, (WPARAM)m_PlaylistType );

		// the current playlist always becomes the
		// now playing after an item is activated,
		// so our type is now "now playing" and our
		// playlist points to the player
		m_PlaylistType = MUSIK_SOURCES_TYPE_NOWPLAYING;

		// playlist up to date!
		m_Changed = false;
	}

	// play the song in the (new) playlist at
	// the acitvated item.
	if ( n > -1 )
	{
		m_Player->Play( n, MUSIK_CROSSFADER_NEW_SONG );
		return true;
	}

	return false;
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

void CmusikPlaylistCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
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

    // Put the data in the data source.
    etc.cfFormat = m_DropID;
    datasrc.CacheGlobalData ( m_DropID, hgBool, &etc );

	// post a message to the main frame, letting
	// it know that drag and drop has started
	int WM_DRAGSTART = RegisterWindowMessage( "DRAGSTART" );
	m_MainWnd->SendMessage( WM_DRAGSTART, NULL );

    // Start the drag 'n' drop!
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

	// post a message to the main frame, letting
	// it know that drag and drop has completed
	int WM_DRAGEND = RegisterWindowMessage( "DRAGEND" );
	m_MainWnd->SendMessage( WM_DRAGEND, NULL );

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

		// save the items that are selected
		CIntArray sel, selids;
		GetSelectedItems( &sel );
		GetItemIDs( sel, &selids );

		// remove selected items from their
		// current position
		DeleteItems( sel, false );

		// insert the items back to their new
		// position...
		InsertItems( selids, nFirstSel, nPos );

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
	CStdStringArray* files = NULL;

	files = new CStdStringArray();

	CStdString sTemp;
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
		m_MainWnd->SendMessage( WM_BATCHADD_NEW, (WPARAM)params );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedItems( CIntArray* items )
{
	items->clear();

    POSITION pos = GetFirstSelectedItemPosition();
	while ( pos )
        items->push_back( GetNextSelectedItem ( pos ) );
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
		m_Playlist->DeleteAt( items.at( i ) - i );

	if ( m_PlaylistType == MUSIK_PLAYLIST_TYPE_STANDARD )
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
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InsertItems( const CIntArray& items, int firstsel, int at, bool update )
{
	if ( items.size() <= 0 )
		return;

	int nScrollPos = GetScrollPos( SB_VERT );

	// see if we need to insert items above
	// or below the target... depends if we
	// are dragging up or down.
	if ( firstsel < at )
	{
		at -= ( items.size() - 1 );
		if ( at < -1 )
			at = -1;
	}

	m_Playlist->InsertAt( items, at );

	if ( update )
	{
		UpdateV();

		// deselect all, then select the items that
		// were just rearranged...
		SetItemState( -1, 0, LVIS_SELECTED );
		if ( at == -1 )
			at = GetItemCount() - items.size();
		for ( size_t i = at; i < at + items.size(); i++ )
			SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

		SetScrollPos( SB_VERT, nScrollPos );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nChar == VK_DELETE )
	{
		CIntArray sel;
		GetSelectedItems( &sel );
		DeleteItems( sel, true );
		return;
	}

	CmusikListCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
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

bool CmusikPlaylistCtrl::SavePlaylist()
{
	// if this is a standard playlist, it
	// will have an ID. all other playlists
	// will have an id as -1.
	if ( m_Playlist->GetPlaylistID() != -1 )
	{
		m_Library->RewriteStdPlaylist( m_Playlist->GetPlaylistID(), m_Playlist );
		m_PlaylistNeedsSave = false;
		return true;
	}

	return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDragColumn( int source, int dest )
{
	if ( source == dest )
		return;

	m_Arranging = true;

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

	m_RatingExtent = szText.cx;
}