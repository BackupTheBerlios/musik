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
//   CmusikSelectionCtrl, CmusikSelectionBar
//
// Filename(s): 
//
//   musikSelectionCtrl.h, musikSelectionCtrl.cpp
//
// Information:
//
//   musikCube's artist / album / genre / etc selection control
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikSelectionCtrl.h"
#include "musikPrefs.h"
#include "MainFrm.h"

#include "MEMDC.H"

#include "../musikCore/include/musikLibrary.h"

///////////////////////////////////////////////////

// CmusikSelectionBar

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikSelectionBar, baseCmusikSelectionBar)

///////////////////////////////////////////////////

CmusikSelectionBar::CmusikSelectionBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, UINT dropid_l, UINT dropid_r )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikSelectionCtrl( parent, library, prefs, type, ctrlid, dropid_l, dropid_r );
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
	ON_COMMAND(ID_SELECTIONBOXCONTEXT_RENAME, OnSelectionboxRename)
	ON_COMMAND(ID_SELECTIONBOX_CLOSEVIEW, OnSelectionboxRemove)
	ON_COMMAND(ID_SELECTIONBOX_ADDVIEW, OnSelectionboxAddnew)
	ON_COMMAND(ID_CHANGETYPE_ARTIST, OnChangetypeArtist)
	ON_COMMAND(ID_CHANGETYPE_ALBUM, OnChangetypeAlbum)
	ON_COMMAND(ID_CHANGETYPE_YEAR, OnChangetypeYear)
	ON_COMMAND(ID_CHANGETYPE_GENRE, OnChangetypeGenre)
	ON_COMMAND(ID_CHANGETYPE_TRACKNUMBER, OnChangetypeTracknumber)
	ON_COMMAND(ID_CHANGETYPE_TIMEADDED, OnChangetypeTimeadded)
	ON_COMMAND(ID_CHANGETYPE_LASTPLAYED, OnChangetypeLastplayed)
	ON_COMMAND(ID_CHANGETYPE_FORMAT, OnChangetypeFormat)
	ON_COMMAND(ID_CHANGETYPE_RATING, OnChangetypeRating)
	ON_COMMAND(ID_CHANGETYPE_TIMESPLAYED, OnChangetypeTimesplayed)
	ON_COMMAND(ID_CHANGETYPE_BITRATE, OnChangetypeBitrate)
	ON_COMMAND(ID_DELETE_FROMLIBRARY, OnDeleteFromlibrary)
	ON_COMMAND(ID_DELETE_FROMCOMPUTER, OnDeleteFromcomputer)
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
	ShowOptionsMenu();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnSelectionboxRename()
{
	GetCtrl()->RenameSel();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::ShowContextMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_SELECTION_BOX_CONTEXT_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	if ( GetCtrl()->GetSelectedCount() == 0 )
	{
		popup_menu->EnableMenuItem( ID_SELECTIONBOXCONTEXT_RENAME, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_DELETE_FROMLIBRARY, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_DELETE_FROMCOMPUTER, MF_DISABLED | MF_GRAYED );
	}
		
	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

void CmusikSelectionBar::ShowOptionsMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_SELECTION_BOX_OPTIONS_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	if ( m_Prefs->GetSelBoxCount() == 1 )
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_CLOSEVIEW, MF_DISABLED | MF_GRAYED );

	if ( GetCtrl()->GetSelectedCount() == 0 )
		popup_menu->EnableMenuItem( ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED );
		
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

void CmusikSelectionBar::OnSelectionboxRemove()
{
	int WM_SELBOXADDREMOVE = RegisterWindowMessage( "SELBOXADDREMOVE" );
	GetCtrl()->m_Parent->SendMessage( WM_SELBOXADDREMOVE, (WPARAM)FALSE, (LPARAM)this );
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnSelectionboxAddnew()
{
	int WM_SELBOXADDREMOVE = RegisterWindowMessage( "SELBOXADDREMOVE" );
	GetCtrl()->m_Parent->SendMessage( WM_SELBOXADDREMOVE, (WPARAM)TRUE, NULL );
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeArtist()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_ARTIST );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeAlbum()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_ALBUM );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeYear()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_YEAR );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeGenre()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_GENRE );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeTracknumber()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_TRACKNUM );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeTimeadded()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_TIMEADDED );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeLastplayed()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_LASTPLAYED );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeFormat()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_FORMAT );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeRating()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_RATING );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeTimesplayed()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_TIMESPLAYED );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnChangetypeBitrate()
{
	GetCtrl()->SetType( MUSIK_LIBRARY_TYPE_BITRATE );
	ReqSelBoxUpdate();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::ReqSelBoxUpdate()
{
	int WM_SELBOXREQUESTUPDATE = RegisterWindowMessage( "SELBOXREQUESTUPDATE" );
	GetCtrl()->m_Parent->SendMessage( WM_SELBOXREQUESTUPDATE );
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnDeleteFromlibrary()
{
	GetCtrl()->DelSel();
}

///////////////////////////////////////////////////

void CmusikSelectionBar::OnDeleteFromcomputer()
{
	GetCtrl()->DelSel( true );
}

///////////////////////////////////////////////////

// CmusikSelectionCtrl

///////////////////////////////////////////////////

bool CmusikSelectionCtrl::m_Updating = false;
int CmusikSelectionCtrl::m_Count = 1;

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikSelectionCtrl, CmusikListCtrl)

///////////////////////////////////////////////////

int WM_SELECTION_EDIT_COMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
int WM_SELECTION_EDIT_CANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );

///////////////////////////////////////////////////

CmusikSelectionCtrl::CmusikSelectionCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, UINT dropid_l, UINT dropid_r )
{
	m_Prefs = prefs;

	m_Library = library;
	m_Type = type;
	m_Parent = parent;
	m_ID = ctrlid;
	m_DropID_L = dropid_l;
	m_DropID_R = dropid_r;
	m_ParentBox = false;
	m_ChildOrder = -1;
	m_IsFocused = false;

	m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );
	HideScrollBars( LCSB_NCOVERRIDE, SB_HORZ );


	InitFonts();
	InitColors();
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
	ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnLvnBeginrdrag)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()

	// custom message maps
	ON_REGISTERED_MESSAGE(WM_SELECTION_EDIT_COMMIT,OnEditCommit)
	ON_REGISTERED_MESSAGE(WM_SELECTION_EDIT_CANCEL,OnEditCancel)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikSelectionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	// set caption to reflect type
	InsertColumn( 0, _T( "" ) );
	SetWindowCap();

	// edit in place
	m_EditInPlace.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, 123 );
	m_EditInPlace.EnableWindow( FALSE );

	CFont font;
	font.CreateStockObject( DEFAULT_GUI_FONT );
	m_EditInPlace.SetFont( &font );

    // set initial state
	RescaleColumn();
	UpdateV();

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

void CmusikSelectionCtrl::UpdateV()
{
	int nPos = GetScrollPos( SB_VERT );

	// get new items
	if ( m_Type == MUSIK_LIBRARY_TYPE_RATING )
	{
		m_Items.clear();
		m_Items.push_back( _T( "0" ) );
		m_Items.push_back( _T( "5" ) );
		m_Items.push_back( _T( "4" ) );
		m_Items.push_back( _T( "3" ) );
		m_Items.push_back( _T( "2" ) );
		m_Items.push_back( _T( "1" ) );
	}
	else
		m_Library->GetAllDistinct( m_Type, m_Items, true, UseTempTable() );

	// get Select by...
	CmusikString top;
	int item_count = (int)m_Items.size();
	CString type = GetTypeStr();
	type.MakeLower();

	switch ( m_Type )
	{
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		top.Format( _T( "Select all %s (%d)" ), type, item_count );
		break;
	default:
		top.Format( _T( "Select all %ss (%d)" ), type, item_count );
		break;
	}

	// update
	m_Items.insert( m_Items.begin(), top );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	SetScrollPos( SB_VERT, nPos, 0 );
	RedrawWindow();
}

///////////////////////////////////////////////////

bool CmusikSelectionCtrl::UseTempTable()
{
	CMainFrame* ptrMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	return ptrMain->UseTempTable();
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::UpdateV( CmusikString query )
{
	int nPos = GetScrollPos( SB_VERT );

	// run query to get related items
	bool sub_query = true;
	if ( query.Left( 1 ) == "W" )
		sub_query = false;

	m_Library->GetRelatedItems( query, m_Type, m_Items, sub_query, UseTempTable() );

	// format "Select all..."
	CmusikString top;
	CString type = GetTypeStr();
	int item_count = (int)m_Items.size();
	type.MakeLower();
	top.Format( _T( "Select all %ss (%d)" ), type, item_count );

	// update
	m_Items.insert( m_Items.begin(), top );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	SetScrollPos( SB_VERT, nPos, 0 );
	RedrawWindow();
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
		CmusikString sCurrItem;
		switch( pItem->iSubItem )
		{
		case 0:
			if ( pItem->iItem >= 0 )
			{

				// get current item
				bool caching = false;
				if ( pItem->iItem >= (int)m_Items.size() )
				{
					pStr = _T( "[musik.caching]" );
					caching = true;
				}
				else
					sCurrItem = m_Items.at( pItem->iItem );

				// got a valid item, so go ahead and add it.
				if ( !caching )
				{
					switch ( m_Type )
					{
					case MUSIK_LIBRARY_TYPE_FORMAT:
						if ( sCurrItem == _T( "0" ) )
							pStr = _T( "mp3" );
						else if ( sCurrItem == _T( "1" ) )
							pStr = _T( "ogg" );
						break;

					case MUSIK_LIBRARY_TYPE_RATING:
						{
							if ( pItem->iItem > 0 )
							{
								if ( sCurrItem == _T( "0" ) )
									pStr = _T( "Unrated" );

								else if ( sCurrItem == _T( "1" ) )
									pStr = _T( "-,,,," );

								else if ( sCurrItem == _T( "2" ) )
									pStr = _T( "--,,," );

								else if ( sCurrItem == _T( "3" ) )
									pStr = _T( "---,," );

								else if ( sCurrItem == _T( "4" ) )
									pStr = _T( "----," );

								else if ( sCurrItem == _T( "5" ) )
									pStr = _T( "-----" );
							}
							else
								pStr = (char*)sCurrItem.c_str();
						}

						break;

					case MUSIK_LIBRARY_TYPE_LASTPLAYED:
					case MUSIK_LIBRARY_TYPE_TIMESPLAYED:

						if ( sCurrItem.IsEmpty() || sCurrItem == _T( "0" ) )
						{
							pStr = _T( "(never)" );
							break;
						}

					case MUSIK_LIBRARY_TYPE_BITRATE:

						if ( sCurrItem == _T( "0" ) )
						{
							pStr = _T( "(unknown)" );
							break;
						}

					case MUSIK_LIBRARY_TYPE_TRACKNUM:

						if ( sCurrItem == _T( "0" ) )
						{
							pStr = _T( "(unknown)" );
							break;
						}

					default:
						pStr = (char*)sCurrItem.c_str();
						break;
					}
				}

				if ( pStr )
				{
					pItem->cchTextMax = sizeof( *pStr );
					lstrcpy( pItem->pszText, pStr );
				}
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
	*pResult = 0;

	// set the new order of presedence
	if ( !IsParent() && GetChildOrder() == -1 )
	{
		CmusikSelectionCtrl::IncChildOrder();
		m_ChildOrder = m_Count;
	}

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

}

///////////////////////////////////////////////////

CString CmusikSelectionCtrl::GetTypeDB()
{
	CmusikString ret = m_Library->GetSongFieldDB( m_Type );
	CString sRet = (CString)ret.c_str();
	return sRet;
}

///////////////////////////////////////////////////

CString CmusikSelectionCtrl::GetTypeStr()
{
	return (CString)m_Library->GetSongField( m_Type ).c_str();
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::GetSelItems( CmusikStringArray& items, bool format_query )
{
	items.clear();
	
	int item;
	int count = 0;

	CmusikString item_str;

	for ( int i = -1 ; ; )
	{
		item = GetNextItem( i, LVNI_SELECTED );

		if ( item == -1 )
			break;

		else
		{
			item_str = m_Items.at( item );

			switch( m_Type )
			{
			case MUSIK_LIBRARY_TYPE_FORMAT:
				if ( item_str == "mp3" )
					items.push_back( "0" );
				else if ( item_str == "ogg" )
					items.push_back( "1" );

				break;

			default:

				items.push_back( item_str );
				if ( format_query )
					items.at( count ).Replace( _T( "'" ), _T( "''" ) );

				break;
			}
		}

		i = item;
		count++;
	}
}

///////////////////////////////////////////////////

CmusikString CmusikSelectionCtrl::GetSelQuery()
{
	CmusikString sQuery;
	
	CmusikStringArray selected_items;
	GetSelItems( selected_items );

	if ( selected_items.size() )
	{
		CmusikString sSel;
		for ( size_t i = 0; i < selected_items.size(); i++ )
		{
			sSel = selected_items.at( i );

			sQuery += m_Library->GetSongFieldDB( m_Type );
			sQuery += _T( " like " );	
			sQuery += _T( "'" );
			sQuery += sSel;
			sQuery += _T( "'" );

			if ( i < selected_items.size() - 1 )
				sQuery += _T( " or " );
		}
	}
	else
	{
		sQuery += m_Library->GetSongFieldDB( m_Type );
		sQuery += _T( " like '%' " );	
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
		CDC *pDC = CDC::FromHandle( pLVCD->nmcd.hdc );

		CmusikString item_str;

		if ( pLVCD->nmcd.dwItemSpec < m_Items.size() )
			item_str = m_Items.at( pLVCD->nmcd.dwItemSpec );

		if ( pLVCD->nmcd.dwItemSpec == 0 )
			pDC->SelectObject( m_Bold );
		else
		{
			if ( m_Type == MUSIK_LIBRARY_TYPE_RATING )
			{
				if ( item_str != _T( "0" ) && item_str != ( "Unrated" ) )
					pDC->SelectObject( m_StarFont );				
			}
			else
				pDC->SelectObject( m_Regular );
				
		}

		if ( IsParent() )
			pLVCD->clrTextBk = m_Prefs->MUSIK_COLOR_LISTCTRL;
		else
			pLVCD->clrTextBk = clrChild;

		pLVCD->clrText = m_Prefs->MUSIK_COLOR_LISTCTRLTEXT;
			
		*pResult = CDRF_NEWFONT;

		return;
	}
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::InitFonts()
{
	m_StarFont.CreatePointFont( 100, "musik" );

	m_Regular.CreateStockObject( DEFAULT_GUI_FONT );

	LOGFONT pBoldFont;
	m_Regular.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;

	m_Bold.CreateFontIndirect( &pBoldFont );
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::InitColors()
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
	clrChild = RGB( r, g, b );
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

	if ( IsParent() )
		memDC.FillSolidRect( clip, m_Prefs->MUSIK_COLOR_LISTCTRL );
	else
		memDC.FillSolidRect( clip, clrChild );
	   
	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
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

void CmusikSelectionCtrl::RenameSel()
{
	CmusikStringArray items;
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

	CmusikString item_str;

	if ( GetSelectionMark() < (int)m_Items.size() )
		item_str = m_Items.at( GetSelectionMark() );

	m_EditInPlace.EnableWindow( TRUE );
	m_EditInPlace.MoveWindow( rcItem );
	m_EditInPlace.SetFocus();
	m_EditInPlace.SetString( item_str.c_str() );
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

	bool failed = false;

	switch( m_Type )
	{
	case MUSIK_LIBRARY_TYPE_YEAR:

		{
			int year = atoi( m_CommitStr.c_str() );
			if ( year == 0 || year < 1000 || year > 9999 )
			{
				MessageBox( _T( "Invalid year entered. Please enter a four digit number, such as 2004." ), MUSIK_VERSION_STR, MB_ICONINFORMATION );
				failed = true;
				break;
			}
		}

		break;

	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		if ( !m_CommitStr.IsEmpty() )
		{
			int track = atoi( m_CommitStr.c_str() );
			if ( track == 0 )
			{
				MessageBox( _T( "Invalid track number entered. Please enter any number greater than 0, or leave the field blank for no track number." ), MUSIK_VERSION_STR, MB_ICONINFORMATION );
				failed = true;
				break;
			}
		}
		else
			m_CommitStr = "0";

		break;

	}

	if ( GetSelectedCount() == 1 )
	{
		CmusikString item_str;


		if ( GetSelectionMark() < (int)m_Items.size() )
			item_str = m_Items.at( GetSelectionMark() );

		if ( item_str == m_CommitStr )
			failed = true;
	}

	if ( !m_CommitStr.IsEmpty() && !failed )
	{
		int WM_SELBOXEDITCOMMIT = RegisterWindowMessage( "SELBOXEDITCOMMIT" );
		m_Parent->SendMessage( WM_SELBOXEDITCOMMIT, (WPARAM)this, (LPARAM)GetType() );
	}

	return 1L;
}

///////////////////////////////////////////////////

CmusikString CmusikSelectionCtrl::GetEditCommitStr()
{
	return m_CommitStr;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	CmusikSelectionBar* parent = (CmusikSelectionBar*)GetParent();
	parent->ShowContextMenu();
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::SetWindowCap()
{
	CString sTitle = (CString)m_Library->GetSongField( m_Type );

	if ( m_Type != MUSIK_LIBRARY_TYPE_TIMEADDED && m_Type != MUSIK_LIBRARY_TYPE_LASTPLAYED && m_Type != MUSIK_LIBRARY_TYPE_TIMESPLAYED )
		sTitle += _T( "s" );

	LV_COLUMN column;
	column.mask = LVCF_TEXT;
	column.cchTextMax = 255;
	column.pszText = sTitle.GetBuffer();
	
	SetColumn( 0, &column );

	GetParent()->SetWindowText( sTitle );
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::SetType( int type, bool update )
{
	m_Type = type;
	SetWindowCap();

	int WM_SELBOXREQUESTUPDATE = RegisterWindowMessage( "SELBOXREQUESTUPDATE" );
	m_Parent->PostMessage( WM_SELBOXREQUESTUPDATE, (WPARAM)this );
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	BeginDrag( false );
	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	BeginDrag( true );
	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::BeginDrag( bool right_button )
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
	CmusikStringArray files;
	CmusikStringArray sel;
	GetSelItems( sel );
	m_Library->GetRelatedItems( GetType(), sel, MUSIK_LIBRARY_TYPE_FILENAME, files, UseTempTable() );

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
	if ( right_button )
		etc.cfFormat = m_DropID_R;
	else
		etc.cfFormat = m_DropID_L;

	datasrc.CacheGlobalData ( right_button ? m_DropID_R : m_DropID_L, hgBool, &etc );

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

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	if ( GetKeyState( VK_SHIFT ) < 0 )
	{
		if ( pLVKeyDow->wVKey == VK_END || pLVKeyDow->wVKey == VK_HOME || pLVKeyDow->wVKey == VK_DOWN || pLVKeyDow->wVKey == VK_UP || pLVKeyDow->wVKey == VK_PRIOR || pLVKeyDow->wVKey == VK_NEXT )
		{
			int WM_SELBOXUPDATE = RegisterWindowMessage( "SELBOXUPDATE" );
			m_Parent->PostMessage( WM_SELBOXUPDATE, GetCtrlID() );
		}
	}
	else if ( GetKeyState( VK_MENU ) < 0 )
	{
		if ( pLVKeyDow->wVKey == VK_DELETE )
		{
			DelSel();
		}
	}
	else if ( GetKeyState( VK_CONTROL ) < 0 )
	{
		if ( pLVKeyDow->wVKey == VK_DELETE )
			DelSel( true );
	}
	else
	{
		if ( pLVKeyDow->wVKey == VK_F2 )
			RenameSel();
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CmusikListCtrl::OnKillFocus(pNewWnd);
	m_IsFocused = false;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char org_key = nChar;
	CString s_in_key = (CString)org_key;
	s_in_key.MakeLower();

	CString s_cmp_key;
	for ( size_t i = 1; i < m_Items.size(); i++ )
	{
		s_cmp_key = m_Items.at( i ).Left( 1 );
		s_cmp_key.MakeLower();

		if ( s_cmp_key == s_in_key )
		{
			EnsureVisible( i, FALSE );
			return;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// catch this before the focus event, and 
	// test the one possible case clicking on
	// an unfocused list control would cause the
	// update not to trigger
	if ( !m_IsFocused )
	{
		if ( GetSelectedCount() == 1 )
		{
			CRect rcItem;
			GetItemRect( GetSelectionMark(), rcItem, LVIR_BOUNDS );

			if ( rcItem.PtInRect( point ) )
			{
				int WM_SELBOXUPDATE = RegisterWindowMessage( "SELBOXUPDATE" );
				m_Parent->SendMessage( WM_SELBOXUPDATE, GetCtrlID() );
			}
		}
	}

	CmusikListCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CmusikListCtrl::OnSetFocus(pOldWnd);
	m_IsFocused = true;
}

///////////////////////////////////////////////////

void CmusikSelectionCtrl::DelSel( bool from_file )
{
	int WM_SELBOXDELSEL = RegisterWindowMessage( "SELBOXDELSEL" );
	m_Parent->SendMessage( WM_SELBOXDELSEL, (WPARAM)this, (LPARAM)from_file );
}

///////////////////////////////////////////////////

