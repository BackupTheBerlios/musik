// MusikSelectionCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionCtrl.h"
#include ".\musikselectionctrl.h"


// CMusikSelectionCtrl

IMPLEMENT_DYNAMIC(CMusikSelectionCtrl, CMusikListCtrl)
CMusikSelectionCtrl::CMusikSelectionCtrl()
{
	HideScrollBars( LCSB_NCOVERRIDE, SB_HORZ );
}

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikSelectionCtrl, CMusikListCtrl)
	ON_WM_SIZE()
	ON_WM_CREATE()
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

	InsertColumn( 0, _T( "Artists" ) );
	CRect client_size;
	GetClientRect( &client_size );
	SetColumnWidth( 0, client_size.Width() );
	return 0;
}
