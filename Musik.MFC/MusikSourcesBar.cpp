#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesBar.h"
#include ".\musiksourcesbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMusikSourcesBar::CMusikSourcesBar()
{
}

CMusikSourcesBar::~CMusikSourcesBar()
{
}


BEGIN_MESSAGE_MAP(CMusikSourcesBar, baseCMusikSourcesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CMusikSourcesBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCMusikSourcesBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( !m_wndChild.Create( WS_CHILD | WS_VISIBLE | PTS_NOTIFY, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	if ( !m_Font.CreateStockObject( DEFAULT_GUI_FONT ) )
		return -1;

	m_wndChild.SetFont( &m_Font );

	return 0;
}

void CMusikSourcesBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarCF::OnSize(nType, cx, cy);

	CRect client_size;
	GetClientRect( &client_size );

	m_wndChild.MoveWindow( 0, 0, client_size.Width(), client_size.Height() );
}
