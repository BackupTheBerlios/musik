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

	CRect rc;
	GetClientRect(rc);

	//CDC memdc;
	//memdc.CreateCompatibleDC(&dc);

	// draw control background
	dc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	dc.PatBlt(0, 0, rc.Width(), rc.Height(), PATCOPY );

	// copy back buffer to the display
	//dc.GetClipBox(&rc);
	//dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memdc, rc.left, rc.top, SRCCOPY);
	//memdc.DeleteDC();
}
