#include "stdafx.h"
#include "Musik.h"
#include "MusikSelectionBar.h"
#include "MainFrm.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include ".\musikselectionbar.h"


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

	//SetSCBStyle( GetSCBStyle() | SCBS_SIZECHILD );

	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS;
	long dwStyleEx =  LVS_EX_FULLROWSELECT;

	if ( !m_wndChild->Create( dwStyle, CRect( 0, 0, 0, 0), this, 123 ) )
		return -1;

    
	m_wndChild->SetExtendedStyle( dwStyleEx );
	m_wndChild->ModifyStyleEx( dwStyleEx, WS_EX_STATICEDGE );

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

void CMusikSelectionBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarG::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
	m_wndChild->RescaleColumn();
}
