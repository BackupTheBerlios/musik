#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesBar.h"

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
END_MESSAGE_MAP()


int CMusikSourcesBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCMusikSourcesBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	SetSCBStyle( GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD );

	if ( !m_wndChild.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT, CRect(0,0,0,0), this, 123) )
		return -1;

	m_wndChild.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild.SetFont( &m_Font );

	return 0;
}
