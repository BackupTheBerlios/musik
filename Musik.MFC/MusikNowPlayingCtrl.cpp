// MusikNowPlayingCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikNowPlayingCtrl.h"
#include ".\musiknowplayingctrl.h"


// CMusikNowPlayingCtrl

IMPLEMENT_DYNAMIC(CMusikNowPlayingCtrl, CWnd)
CMusikNowPlayingCtrl::CMusikNowPlayingCtrl()
{
}

CMusikNowPlayingCtrl::~CMusikNowPlayingCtrl()
{
}


BEGIN_MESSAGE_MAP(CMusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CMusikNowPlayingCtrl message handlers


void CMusikNowPlayingCtrl::OnPaint()
{
	CPaintDC dc(this); 

	CDC memdc;
	memdc.CreateCompatibleDC(&dc);

	CRect rc;
	GetClientRect(rc);

	// draw control background
	memdc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	memdc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY );

	// copy back buffer to the display
	dc.GetClipBox(&rc);
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memdc, rc.left, rc.top, SRCCOPY);
	memdc.DeleteDC();
}
