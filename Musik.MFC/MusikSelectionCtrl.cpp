///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionCtrl.h"

#include "../Musik.Core/include/MusikLibrary.h"

///////////////////////////////////////////////////

bool CMusikSelectionCtrl::m_Updating = false;

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikSelectionCtrl, CMusikListCtrl)
CMusikSelectionCtrl::CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, int type, int ctrl_id )
{
	m_Library = library;
	m_Type = type;
	m_Parent = parent;
	m_ID = ctrl_id;
	m_ParentBox = false;
	HideScrollBars( LCSB_NCOVERRIDE, SB_HORZ );
	InitFonts();
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
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CMusikSelectionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMusikListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );

	CString sTitle = (CString)m_Library->GetSongField( m_Type );
	sTitle += _T( "s" );

	InsertColumn( 0, sTitle );
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
	SetColumnWidth( 0, client_size.Width() );
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
			items.push_back( ( CStdString)GetItemText( item, 0 ) );
			if ( format_query )
				items.at( count ).Replace( _T( "'" ), _T( "''" ) );
			count++;
		}

		i = item;
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

		pLVCD->clrTextBk = GetSysColor( COLOR_BTNHILIGHT );
			
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
