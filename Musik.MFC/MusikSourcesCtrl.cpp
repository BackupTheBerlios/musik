// MusikSourcesCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesCtrl.h"

// CMusikSourcesCtrl

IMPLEMENT_DYNAMIC(CMusikSourcesCtrl, CPropTree)

CMusikSourcesCtrl::CMusikSourcesCtrl()
{
}

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikSourcesCtrl, CPropTree)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMusikSourcesCtrl message handlers
int CMusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPropTree::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CMusikSourcesCtrl::OnSize(UINT nType, int cx, int cy)
{
	CPropTree::OnSize(nType, cx, cy);

}
