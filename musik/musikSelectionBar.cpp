#include "stdafx.h"
#include "musik.h"
#include "musikSelectionBar.h"
#include "MainFrm.h"

#include "../musikCore/include/musikLibrary.h"
#include ".\musikselectionbar.h"


CmusikSelectionBar::CmusikSelectionBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, int dropid )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikSelectionCtrl( parent, library, prefs, type, ctrlid, dropid );
}

CmusikSelectionBar::~CmusikSelectionBar()
{
	delete m_wndChild;
}


BEGIN_MESSAGE_MAP(CmusikSelectionBar, baseCmusikSelectionBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CmusikSelectionBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikSelectionBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_ANY );

	// child
	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_NOCOLUMNHEADER;
	long dwStyleEx =  LVS_EX_FULLROWSELECT;

	if ( !m_wndChild->Create( dwStyle, CRect( 0, 0, 0, 0), this, 123 ) )
		return -1;

	m_wndChild->SetExtendedStyle( dwStyleEx );
	m_wndChild->ModifyStyleEx( WS_EX_STATICEDGE, NULL );


	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

void CmusikSelectionBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
	m_wndChild->RescaleColumn();
}
