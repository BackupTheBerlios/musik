// MusikSelectionCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionCtrl.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include ".\musikselectionctrl.h"

// CMusikSelectionCtrl

IMPLEMENT_DYNAMIC(CMusikSelectionCtrl, CMusikListCtrl)
CMusikSelectionCtrl::CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, int type, int ctrl_id )
{
	m_Library = library;
	m_Type = type;
	m_Parent = parent;
	m_ID = ctrl_id;
	m_Updating = false;
	HideScrollBars( LCSB_NCOVERRIDE, SB_HORZ );
}

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikSelectionCtrl, CMusikListCtrl)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
END_MESSAGE_MAP()



// CMusikSelectionCtrl message handlers


void CMusikSelectionCtrl::OnSize(UINT nType, int cx, int cy)
{
	CMusikListCtrl::OnSize(nType, cx, cy);

	SetColumnWidth( 0, cx );
}

int CMusikSelectionCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMusikListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	InsertColumn( 0, m_Library->GetSongField( m_Type ) );

	CRect client_size;
	GetClientRect( &client_size );

	SetColumnWidth( 0, client_size.Width() );

	UpdateV();

	return 0;
}

void CMusikSelectionCtrl::UpdateV()
{
	m_Library->GetAllDistinct( m_Type, m_Items );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );
}

void CMusikSelectionCtrl::UpdateV( CStdString query )
{
	m_Library->GetRelatedItems( query, m_Type, m_Items );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	CRect rcClient;
	GetClientRect( &rcClient );
	RedrawWindow( rcClient );
}

void CMusikSelectionCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem= &(pDispInfo)->item;

	int index= pItem->iItem;

	if ( pItem->mask & LVIF_TEXT )
	{
		switch ( pItem->iSubItem )
		{
		case 0:
			lstrcpy( pItem->pszText, m_Items.at( index ).c_str() );
			break;
		}

		*pResult = 0;
	}
}

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
				m_Parent->PostMessage( WM_SELBOXUPDATE, GetCtrlID() );
			}
		}
	}

	*pResult = 0;
}

CString CMusikSelectionCtrl::GetTypeDB()
{
	return m_Library->GetSongFieldDB( m_Type );
}

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
