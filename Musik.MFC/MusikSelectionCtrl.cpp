// MusikSelectionCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionCtrl.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include ".\musikselectionctrl.h"

// CMusikSelectionCtrl

IMPLEMENT_DYNAMIC(CMusikSelectionCtrl, CMusikListCtrl)
CMusikSelectionCtrl::CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, int type )
{
	m_Library = library;
	m_Type = type;
	m_Parent = (CMainFrame*)parent;
	HideScrollBars( LCSB_NCOVERRIDE, /*SB_HORZ*/ SB_BOTH );
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

	m_Library->GetAllDistinct( m_Type, m_Items );
	SetItemCountEx( m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

	return 0;
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

	// only interested in state changes
	if ( pNMLV->uChanged & LVIF_STATE )
	{
		if ( pNMLV->uNewState & LVIS_SELECTED )
		{
		}
	}

	*pResult = 0;
}
