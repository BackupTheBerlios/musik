#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionBar.h"
#include "MainFrm.h"

#include "../Musik.Core/include/MusikLibrary.h"


CMusikSelectionBar::CMusikSelectionBar( CFrameWnd* parent, CMusikLibrary* library, int type, int ctrl_id )
{
	m_wndChild = new CMusikSelectionCtrl( parent, library, type, ctrl_id );
}

CMusikSelectionBar::~CMusikSelectionBar()
{
	delete m_wndChild;
}


BEGIN_MESSAGE_MAP(CMusikSelectionBar, baseCMusikSelectionBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CMusikSelectionBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCMusikSelectionBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	SetSCBStyle( GetSCBStyle() | SCBS_SIZECHILD );

	if ( !m_wndChild->Create( WS_CHILD | WS_VISIBLE | LVS_NOCOLUMNHEADER | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS, CRect(0,0,0,0), this, 123 ) )
		return -1;

	m_wndChild->ModifyStyleEx( 0, WS_EX_STATICEDGE );

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

void CMusikSelectionBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarG::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
