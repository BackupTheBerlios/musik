///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikSelectionCtrl.h"
#include "musikPrefs.h"

#include "MEMDC.H"

#include "../musikCore/include/musikLibrary.h"

///////////////////////////////////////////////////

// CmusikSelectionBar

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikSelectionBar, baseCmusikSelectionBar)

///////////////////////////////////////////////////

CmusikSelectionBar::CmusikSelectionBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, int dropid )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikSelectionCtrl( parent, library, prefs, type, ctrlid, dropid );
}

///////////////////////////////////////////////////

CmusikSelectionBar::~CmusikSelectionBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikSelectionBar, baseCmusikSelectionBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_SELECTIONBOX_RENAME, OnSelectionboxRename)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikSelectionBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikSelectionBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_ANY );

	// child
	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS;
	long dwStyleEx = LVS_EX_FULLROWSELECT;

	if ( !m_wndChild->Create( dwStyle, CRect( 0, 0, 0, 0), this, 123 ) )
		return -1;

	m_wndChild->SetExtendedStyle( dwStyleEx );
	m_wndChild->ModifyStyleEx( WS_EX_STATICEDGE, NULL );

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
	m_wndChild->RescaleColumn();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnOptions()
{
	ShowMenu();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnSelectionboxRename()
{
	GetCtrl()->RenameSel();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::ShowMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_SELECTION_BOX_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	int type = GetCtrl()->GetType();
	switch( type )
	{
	case MUSIK_LIBRARY_TYPE_ARTIST:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_ARTIST, MF_CHECKED );
		break;
	case MUSIK_LIBRARY_TYPE_ALBUM:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_ALBUM, MF_CHECKED );
		break;
	case MUSIK_LIBRARY_TYPE_YEAR:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_YEAR, MF_CHECKED );
		break;
	case MUSIK_LIBRARY_TYPE_GENRE:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_GENRE, MF_CHECKED );
		break;
	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_TRACKNUMBER, MF_CHECKED );
		break;
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_TIMEADDED, MF_CHECKED );
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED );
		break;
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_LASTPLAYED, MF_CHECKED );
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED );
		break;
	case MUSIK_LIBRARY_TYPE_FORMAT:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_FORMAT, MF_CHECKED );
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED );
		break;
	case MUSIK_LIBRARY_TYPE_RATING:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_RATING, MF_CHECKED );
		break;
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_TIMESPLAYED, MF_CHECKED );
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED );
		break;
	case MUSIK_LIBRARY_TYPE_BITRATE:
		popup_menu->CheckMenuItem( ID_CHANGETYPE_BITRATE, MF_CHECKED );
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED );
		break;
	}

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

// CmusikSelectionCtrl

///////////////////////////////////////////////////

bool CmusikSelectionCtrl::m_Updating = false;

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikSelectionCtrl, CmusikListCtrl)

///////////////////////////////////////////////////

int WM_SELECTION_EDIT_COMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
int WM_SELECTION_EDIT_CANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );

///////////////////////////////////////////////////

CmusikSelectionCtrl::CmusikSelectionCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, int dropid )
{
	m_Library = library;
	m_Type = type;
	m_Parent = parent;
	m_ID = ctrlid;
	m_DropID = dropid;
	m_ParentBox = false;
	m_MouseTrack = false;
	m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );
	HideScrollBars( LCSB_NCOVERRIDE, SB_HORZ );
	InitFonts();

	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikSelectionCtrl::~CmusikSelectionCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikSelectionCtrl, CmusikListCtrl)
	// mfc message maps
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
	ON_WM_KEYDOWN()

	// custom message maps
	ON_REGISTERED_MESSAGE(WM_SELECTION_EDIT_COMMIT,OnEditCommit)
	ON_REGISTERED_MESSAGE(WM_SELECTION_EDIT_CANCEL,OnEditCancel)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikSelectionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	CString sTitle = (CString)m_Library->GetSongField( m_Type );
	sTitle += _T( "s" );

	InsertColumn( 0, sTitle );

	GetParent()->SetWindowText( sTitle );

	// edit in place
	m_EditInPlace.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, 123 );
	m_EditInPlace.EnableWindow( FALSE );

	CFont font;
	font.CreateStockObject( DEFAULT_GUI_FONT );
	m_EditInPlace.SetFont( &font );

    // set initial state
	RescaleColumn();
	UpdateV( true );

	return 0;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::RescaleColumn()
{
	CRect client_size;
	GetClientRect( &client_size );

	SetColumnWidth( 0, client_size.Width() );
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::UpdateV( bool update_count )
{
	int nPos = GetScrollPos( SB_VERT );

	CStdString top;
	if ( !update_count )
		top = m_Items.at( 0 );

	m_Library->GetAllDistinct( m_Type, m_Items );

	if ( update_count )
	{
		CString type = GetTypeStr();
		type.MakeLower();
		top.Format( _T( "Show all %ss (%d)" ), type, m_Items.size() );
	}

	m_Items.insert( m_Items.begin(), top );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	SetScrollPos( SB_VERT, nPos, false );

	RedrawWindow();
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::UpdateV( CStdString query, bool update_count )
{
	CStdString top;
	if ( !update_count )
		top = m_Items.at( 0 );

	m_Library->GetRelatedItems( query, m_Type, m_Items );

	if ( update_count )
	{
		CString type = GetTypeStr();
		type.MakeLower();
		top.Format( _T( "Show all %ss ( %d )" ), type, m_Items.size() );
	}

	m_Items.insert( m_Items.begin(), top );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	CRect rcClient;
	GetClientRect( &rcClient );
	RedrawWindow( rcClient );
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem = &(pDispInfo)->item;

	// only need to worry about item text
	if ( pItem->mask & LVIF_TEXT )
	{
		char* pStr = NULL;

		switch( pItem->iSubItem )
		{
		case 0:

			if ( pItem->iItem > 0 )
			{
				// a safeguard... just make sure the item we're
				// getting passed has a respective value in the
				// array of items.
				if ( pItem->iItem >= (int)m_Items.size() )
				{
					CString sNull = _T( "[musik.caching]" );
					pStr = sNull.GetBuffer();
				}

				// got a valid item, so go ahead and add it.
				else
				{
					switch ( m_Type )
					{
					case MUSIK_LIBRARY_TYPE_FORMAT:
						if ( m_Items.at( pItem->iItem ) == _T( "0" ) )
							pStr = _T( "mp3" );
						else if ( m_Items.at( pItem->iItem ) == _T( "1" ) )
							pStr = _T( "ogg" );
						break;

					default:
						pStr = (char*)m_Items.at( pItem->iItem ).c_str();
						break;
					}
				}
			}
			else
				pStr = (char*)m_Items.at( 0 ).c_str();

			if ( pStr )
			{
				pItem->cchTextMax = sizeof( *pStr );
				lstrcpy( pItem->pszText, pStr );
			}

			break;
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ( !m_Updating )
	{
		// only interested in state changes
		if ( pNMLV->uChanged & LVIF_STATE )
		{
			if ( pNMLV->uNewState & LVIS_SELECTED )
			{
				int WM_SELBOXUPDATE = RegisterWindowMessage( "SELBOXUPDATE" );
				m_Parent->SendMessage( WM_SELBOXUPDATE, GetCtrlID() );
			}
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

CString CmusikSelectionCtrl::GetTypeDB()
{
	CStdString ret = m_Library->GetSongFieldDB( m_Type );
	CString sRet = (CString)ret.c_str();
	return sRet;
}

///////////////////////////////////////////////////

CString CmusikSelectionCtrl::GetTypeStr()
{
	return (CString)m_Library->GetSongField( m_Type ).c_str();
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::GetSelItems( CStdStringArray& items, bool format_query )
{
	items.clear();
	
	int item;
	int count = 0;

	CString item_str;
	for ( int i = -1 ; ; )
	{
		item = GetNextItem( i, LVNI_SELECTED );

		if ( item == -1 )
			break;

		else
		{
			switch( m_Type )
			{
			case MUSIK_LIBRARY_TYPE_FORMAT:
				item_str = GetItemText( item, 0 );
				if ( item_str == "mp3" )
					items.push_back( "0" );
				else if ( item_str == "ogg" )
					items.push_back( "1" );

			default:
				items.push_back( ( CStdString)GetItemText( item, 0 ) );
				if ( format_query )
					items.at( count ).Replace( _T( "'" ), _T( "''" ) );

			}

		}

		i = item;
		count++;
	}
}

///////////////////////////////////////////////////

CStdString CmusikSelectionCtrl::GetSelQuery( CStdString other_sel_query )
{
	CStdStringArray selected_items;
	GetSelItems( selected_items );

	CStdString sQuery;
	sQuery += GetTypeDB();
	sQuery += _T( " like " );
	for ( size_t i = 0; i < selected_items.size(); i++ )
	{
		if ( i == selected_items.size() - 1 )
		{
			sQuery += _T( "'" );
			sQuery += selected_items.at( i );
			sQuery += _T( "' " );
			if ( !other_sel_query.IsEmpty() )
			{
				sQuery += _T( " and " );
				sQuery += other_sel_query;
			}
		}

		else
		{
			sQuery += _T( "'" );
			sQuery += selected_items.at( i );
			sQuery += _T( "'" );
			if ( !other_sel_query.IsEmpty() )
			{
				sQuery += _T( " and " );
				sQuery += other_sel_query;
			}
			sQuery += _T( " or " );
			sQuery += GetTypeDB();
			sQuery += _T( " like " );
		}
	}
	
	return sQuery;
}

///////////////////////////////////////////////////

bool CmusikSelectionCtrl::IsItemSelected( int item )
{
	if ( GetItemState( item, LVIS_SELECTED ) )
		return true;
	return false;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    *pResult = CDRF_DODEFAULT;
	
	// this is the first notifcation we'll receive. tell
	// the handler we want to recieve notifactions for 
	// each item in the list
    if ( pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}

	// draw each sub item... we set colors here.
	if ( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

		if ( pLVCD->nmcd.dwItemSpec == 0 )
			pDC->SelectObject( m_Bold );
		else
			pDC->SelectObject( m_Regular );

		pLVCD->clrTextBk = m_Prefs->MUSIK_COLOR_LISTCTRL;
		pLVCD->clrText = m_Prefs->MUSIK_COLOR_LISTCTRLTEXT;
			
		*pResult = CDRF_NEWFONT;
		return;
	}
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::InitFonts()
{
	m_Regular.CreateStockObject( DEFAULT_GUI_FONT );

	LOGFONT pBoldFont;
	m_Regular.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;

	m_Bold.CreateFontIndirect( &pBoldFont );
}

///////////////////////////////////////////////////

BOOL CmusikSelectionCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////


void CmusikSelectionCtrl::OnPaint()
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

LRESULT CmusikSelectionCtrl::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_MouseTrack = false;
	return 0L;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// mouse must cleanly enter the window
	// without the button down to start
	// tracking. once tracking begins,
	// dnd can begin. once the mouse leaves 
	// the window, m_MouseTrack is false and
	// dnd cannot happen
	if ( !m_MouseTrack && !( nFlags & MK_LBUTTON ) )
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::_TrackMouseEvent(&tme);

		m_MouseTrack = true; 	
	}

	if ( ( nFlags & MK_LBUTTON ) && GetSelectedCount() && m_MouseTrack )
	{
		COleDataSource datasrc;
		HGLOBAL        hgDrop;
		DROPFILES*     pDrop;
		CStringList    lsDraggedFiles;
		POSITION       pos;
		CString        sFile;
		UINT           uBuffSize = 0;
		TCHAR*         pszBuff;
		FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

		// get a list of filenames with the currently
		// selected items...
		CStdStringArray files;
		CStdStringArray sel;
		GetSelItems( sel );
		m_Library->GetRelatedItems( GetType(), sel, MUSIK_LIBRARY_TYPE_FILENAME, files );

		if ( !files.size() )
			return;

		// CStringList containing files
		for ( size_t i = 0; i < files.size(); i++ )
		{
			lsDraggedFiles.AddTail( files.at( i ) );
			uBuffSize += files.at( i ).GetLength() + 1;
		}

		sel.clear();
		files.clear();

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

		// Start the drag 'n' drop!
		DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

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
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// returning non-zero ignores the message,
	// and marquee never appears.
	*pResult = 1;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// user pressed f2 to rename an entry
	if ( nChar == VK_F2 )
		RenameSel();

	CmusikListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::RenameSel()
{
	CStdStringArray items;
	GetSelItems( items );

	// make sure somethign is selected
	if ( !items.size() )
		return;

	// first selected item rect
	CRect rcItem;
	GetItemRect( GetSelectionMark(), rcItem, LVIR_BOUNDS );
	rcItem.left += 2;

	if ( rcItem.top < 0 )
	{
		int height = rcItem.bottom - rcItem.top;
		rcItem.top = 0 + 2;
		rcItem.bottom = height + 2;
	}

	m_EditInPlace.EnableWindow( TRUE );
	m_EditInPlace.MoveWindow( rcItem );
	m_EditInPlace.SetFocus();
	m_EditInPlace.SetString( m_Items.at( GetSelectionMark() ).c_str() );
	m_EditInPlace.ShowWindow( SW_SHOWDEFAULT );		
}

///////////////////////////////////////////////////

LRESULT CmusikSelectionCtrl::OnEditCancel( WPARAM wParam, LPARAM lParam )
{
	m_EditInPlace.EnableWindow( FALSE );
	SetFocus();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikSelectionCtrl::OnEditCommit( WPARAM wParam, LPARAM lParam )
{
	m_EditInPlace.EnableWindow( FALSE );
	SetFocus();

	m_CommitStr = m_EditInPlace.GetString();

	if ( GetSelectedCount() == 1 )
	{
		if ( m_Items.at( GetSelectionMark() ) == m_CommitStr )
			return 0L;
	}

	if ( !m_CommitStr.IsEmpty() )
	{
		int WM_SELBOXEDITCOMMIT = RegisterWindowMessage( "SELBOXEDITCOMMIT" );
		m_Parent->SendMessage( WM_SELBOXEDITCOMMIT, (WPARAM)this, (LPARAM)GetType() );
	}

	return 1L;
}

///////////////////////////////////////////////////

CStdString CmusikSelectionCtrl::GetEditCommitStr()
{
	return m_CommitStr;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	CmusikSelectionBar* parent = (CmusikSelectionBar*)GetParent();
	parent->ShowMenu();
}

///////////////////////////////////////////////////

