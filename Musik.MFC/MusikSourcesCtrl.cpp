// MusikSourcesCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesCtrl.h"
#include ".\musiksourcesctrl.h"


// CMusikSourcesCtrl

IMPLEMENT_DYNAMIC(CMusikSourcesCtrl, CListCtrl)
CMusikSourcesCtrl::CMusikSourcesCtrl()
{
}

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikSourcesCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMusikSourcesCtrl message handlers
int CMusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	InsertColumn( 0, "Sources" );

	return 0;
}

void CMusikSourcesCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	SetColumnWidth( 0, cx );
}
