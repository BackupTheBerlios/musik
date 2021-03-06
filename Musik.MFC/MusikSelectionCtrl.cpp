///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionCtrl.h"
#include "MusikPrefs.h"

#include "MEMDC.H"

#include "../Musik.Core/include/MusikLibrary.h"

///////////////////////////////////////////////////

bool CMusikSelectionCtrl::m_Updating = false;

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikSelectionCtrl, CMusikListCtrl)

///////////////////////////////////////////////////

CMusikSelectionCtrl::CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, CMusikPrefs* prefs, int type, int ctrlid, int dropid )
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

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikSelectionCtrl, CMusikListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CMusikSelectionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CMusikListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	CString sTitle = (CString)m_Library->GetSongField( m_Type );
	sTitle += _T( "s" );

	InsertColumn( 0, "", LVCFMT_LEFT, 0 );
	InsertColumn( 1, sTitle );

	GetParent()->SetWindowText( sTitle );

	CRect client_size;
	GetClientRect( &client_size );

	RescaleColumn();

	UpdateV( true );

	return 0;
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::RescaleColumn()
{
	CRect client_size;
	GetClientRect( &client_size );

	SetColumnWidth( 0, 0 );
	SetColumnWidth( 1, client_size.Width() );
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::UpdateV( bool update_count )
{
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

	RedrawWindow();
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::UpdateV( CStdString query, bool update_count )
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

void CMusikSelectionCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem = &(pDispInfo)->item;

	// only need to worry about item text
	if ( pItem->mask & LVIF_TEXT )
	{
		switch( pItem->iSubItem )
		{
		case 0:

			// first column is empty, this will disable
			// bounding box selection and make DND easier.
			break;

		case 1:

			// a safeguard... just make sure the item we're
			// getting passed has a respective value in the
			// array of items.
			if ( pItem->iItem >= (int)m_Items.size() )
			{
				CString sNull = _T( "[musik.error]" );
				char* pStr = sNull.GetBuffer();

				pItem->cchTextMax = sizeof( *pStr );
				lstrcpy( pItem->pszText, pStr );
			}

			// got a valid item, so go ahead and add it.
			else
			{
				const char* pStr = m_Items.at( pItem->iItem ).c_str();

				pItem->cchTextMax = sizeof( *pStr );
				lstrcpy( pItem->pszText, pStr );
			}

			break;
		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
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

CString CMusikSelectionCtrl::GetTypeDB()
{
	CStdString ret = m_Library->GetSongFieldDB( m_Type );
	CString sRet = (CString)ret.c_str();
	return sRet;
}

///////////////////////////////////////////////////

CString CMusikSelectionCtrl::GetTypeStr()
{
	return (CString)m_Library->GetSongField( m_Type ).c_str();
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::GetSelItems( CStdStringArray& items, bool format_query )
{
	items.clear();
	
	int item;
	int count = 0;

	for ( int i = -1 ; ; )
	{
		item = GetNextItem( i, LVNI_SELECTED );

		if ( item == -1 )
			break;

		else
		{
			items.push_back( ( CStdString)GetItemText( item, 1 ) );
			if ( format_query )
				items.at( count ).Replace( _T( "'" ), _T( "''" ) );
		}

		i = item;
		count++;
	}
}

///////////////////////////////////////////////////

CStdString CMusikSelectionCtrl::GetSelQuery()
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
		}

		else
		{
			sQuery += _T( "'" );
			sQuery += selected_items.at( i );
			sQuery += _T( "' or " );
			sQuery += GetTypeDB();
			sQuery += _T( " like " );
		}
	}
	
	return sQuery;
}

///////////////////////////////////////////////////

bool CMusikSelectionCtrl::IsItemSelected( int item )
{
	if ( GetItemState( item, LVIS_SELECTED ) )
		return true;
	return false;
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
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

void CMusikSelectionCtrl::InitFonts()
{
	m_Regular.CreateStockObject( DEFAULT_GUI_FONT );

	LOGFONT pBoldFont;
	m_Regular.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;

	m_Bold.CreateFontIndirect( &pBoldFont );
}

///////////////////////////////////////////////////

BOOL CMusikSelectionCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////


void CMusikSelectionCtrl::OnPaint()
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

LRESULT CMusikSelectionCtrl::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_MouseTrack = false;
	return 0L;
}

///////////////////////////////////////////////////

void CMusikSelectionCtrl::OnMouseMove(UINT nFlags, CPoint point)
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
