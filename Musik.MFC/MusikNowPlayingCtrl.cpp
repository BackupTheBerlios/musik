///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikNowPlayingCtrl.h"

#include "Memdc.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikNowPlayingCtrl, CWnd)

///////////////////////////////////////////////////

CMusikNowPlayingCtrl::CMusikNowPlayingCtrl()
{
}

///////////////////////////////////////////////////

CMusikNowPlayingCtrl::~CMusikNowPlayingCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, GetSysColor( COLOR_BTNFACE ) );

	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

