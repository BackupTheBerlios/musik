// MusikPlaylistCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPlaylistCtrl.h"

// CMusikPlaylistCtrl

IMPLEMENT_DYNAMIC(CMusikPlaylistCtrl, CListCtrl)
CMusikPlaylistCtrl::CMusikPlaylistCtrl()
{
}

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikPlaylistCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()



// CMusikPlaylistCtrl message handlers

