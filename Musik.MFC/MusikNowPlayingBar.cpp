#include "stdafx.h"
#include "Musik.h"
#include "MusikNowPlayingBar.h"
#include ".\musiknowplayingbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMusikNowPlayingBar::CMusikNowPlayingBar()
{
}

CMusikNowPlayingBar::~CMusikNowPlayingBar()
{
}


BEGIN_MESSAGE_MAP(CMusikNowPlayingBar, baseCMusikNowPlayingBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CMusikNowPlayingBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCMusikNowPlayingBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	SetSCBStyle( GetSCBStyle() | SCBS_SIZECHILD );

	if ( !m_wndChild.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0), this, 123) )
		return -1;

	m_wndChild.ModifyStyleEx( 0, WS_EX_STATICEDGE );

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild.SetFont( &m_Font );
	
	return 0;
}

void CMusikNowPlayingBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarG::OnSize(nType, cx, cy);

	//MoveWindow( 0, 0, cx, cy );
}
