// Michael Dunn -- http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop -- DND

///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikPrefs.h"

#include "musikPlaylistCtrl.h"
#include "musikSourcesCtrl.h"
#include "musikBatchAddFunctor.h"

#include "../musikCore/include/musikPlaylist.h"
#include "../musikCore/include/musikArrays.h"
#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikDynDspInfo.h"
#include "../musikCore/include/musikPlayer.h"
#include "../musikCore/include/musikBatchAdd.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPlaylistCtrl, CmusikListCtrl)

///////////////////////////////////////////////////

int WM_BATCHADD_PROGRESS_PLAYLIST	= RegisterWindowMessage( "BATCHADD_PROGRESS" );
int WM_BATCHADD_END_PLAYLIST		= RegisterWindowMessage( "BATCHADD_END" );

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
	
	// custom messages
	ON_REGISTERED_MESSAGE( WM_BATCHADD_PROGRESS_PLAYLIST, OnBatchAddProgress )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_END_PLAYLIST, OnBatchAddEnd )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPlaylistCtrl::CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid )
{
	// core
	m_Playlist	= NULL;
	m_Library	= library;
	m_Prefs		= prefs;
	m_Player	= player;

	// musik will always start up
	// with library default selected
	m_PlaylistType = MUSIK_SOURCES_TYPE_LIBRARY;

	// dnd drop id
	m_DropID = dropid;

	// batch add thread
	m_BatchAddThr = NULL;
	m_BatchAddFnct = NULL;

	// main window
	m_MainWnd = mainwnd;

	// misc
	m_RatingWidth = -1;
	m_Changed = true;
	m_SongInfoCache = new CmusikDynDspInfo( NULL, m_Library );

	// dnd variables
	m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );

	// fonts and colors
	InitFonts();
	InitColors();
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

void CmusikPlaylistCtrl::UpdateV()
{
	if ( m_Playlist )
	{
		SetRedraw( false );
		m_SongInfoCache->Set( 0, 0, true );
		SetItemCountEx( m_Playlist->GetCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

		CRect rcClient;
		GetClientRect( &rcClient );
		SetRedraw( true );
		RedrawWindow( rcClient );
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
			sValue = m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), nType );

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
			pDC->SetTextColor( m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );
		}

		else
		{
			if ( m_Player->GetCurrPlaying()->GetID() == m_Playlist->GetSongID( pLVCD->nmcd.dwItemSpec ) && 
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

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InitFonts()
{
	m_ItemFont.CreateStockObject( DEFAULT_GUI_FONT );
	m_StarFont.CreatePointFont( 100, "musik" );

	LOGFONT pBoldFont;
	m_ItemFont.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;

	m_BoldFont.CreateFontIndirect( &pBoldFont );}

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

	m_SongInfoCache->Set( pCacheHint->iFrom, pCacheHint->iTo );

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
	if ( m_SongInfoCache )
		m_SongInfoCache->SetPlaylist( playlist );
    
	m_Playlist = playlist;
	m_PlaylistType = m_Type;
	
	m_Changed = true;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// give the current playlist to the player,
	// unless the player already owns it.
	if ( m_Changed )
	{
		// give the current playlist to the player,
		// if it does not belong to the player. otherwise,
		// it will delete itself, then try to copy itself.
		// confused yet? i am...
		if ( m_PlaylistType != MUSIK_SOURCES_TYPE_NOWPLAYING )
			GivePlaylistToPlayer();		

		// the current playlist always becomes the
		// now playing after an item is activated,
		// so our type is now "now playing" and our
		// playlist points to the player
		m_PlaylistType = MUSIK_SOURCES_TYPE_NOWPLAYING;
		m_Playlist = m_Player->GetPlaylist();
	}

	m_Player->Play( pNMIA->iItem, MUSIK_CROSSFADER_NEW_SONG );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GivePlaylistToPlayer()
{
	// if the type was now playing, then we don't
	// need to post any messages to clean up
	// the playlist... as its just a pointer to
	// the player's internal playlist -- which 
	// itself, not an object. 
	m_Player->SetPlaylist( m_Playlist );
	m_Playlist = NULL;

	int WM_PLAYERNEWPLAYLIST = RegisterWindowMessage( "PLAYERNEWPLAYLIST" );
	m_MainWnd->SendMessage( WM_PLAYERNEWPLAYLIST, (WPARAM)m_PlaylistType );

	// playlist is now updated, so flag
	// it as unchanged
	m_Changed = false;
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

void CmusikPlaylistCtrl::OnDropFiles(HDROP hDropInfo)
{
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	CStdStringArray* files = NULL;

	// if the thread exists, pause it
	if ( m_BatchAddThr )
	{
		m_BatchAddThr->Pause();
		files = m_BatchAddThr->m_Files;
	}

	// otherwise create a new list of files
	else
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
				m_Dir.m_Threaded = false;
				m_Dir.m_Target = files;

				m_Dir.Run();
			}		

			// otherwise it was just a file... add it...
			else
				files->push_back( szNextFile );
		}
	}

	// if thread exists, resume it.
	if ( m_BatchAddThr )
		m_BatchAddThr->Resume();
	else
	{
		if ( files->size() > 0 )
		{
			if ( m_BatchAddFnct )
			{
				TRACE0( "Last thread ended but functor still active? This is a bug...\n" );
				delete m_BatchAddFnct;
				m_BatchAddFnct = NULL;
			}	

			m_BatchAddFnct = new CmusikBatchAddFunctor( this );
			m_BatchAddThr = new CmusikBatchAdd( files, m_Player->GetPlaylist(), m_Library, m_BatchAddFnct );
			m_BatchAddThr->Run();
		}
	}
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnBatchAddProgress( WPARAM wParam, LPARAM lParam )
{
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnBatchAddEnd( WPARAM wParam, LPARAM lParam )
{
	if ( m_BatchAddThr )
	{
		delete m_BatchAddThr;
		m_BatchAddThr = NULL;
	}

	if ( m_BatchAddFnct )
	{
		delete m_BatchAddFnct;
		m_BatchAddFnct = NULL;
	}

	int WM_SELBOXRESET = RegisterWindowMessage( "SELBOXRESET" );
	m_MainWnd->PostMessage( WM_SELBOXRESET );

	return 0L;
}

///////////////////////////////////////////////////