///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"

#include "musikEqualizerCtrl.h"

///////////////////////////////////////////////////

// CmusikEqualizerBar

///////////////////////////////////////////////////

CmusikEqualizerBar::CmusikEqualizerBar( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikEqualizerCtrl( library, player, prefs );
}

///////////////////////////////////////////////////

CmusikEqualizerBar::~CmusikEqualizerBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerBar, baseCmusikEqualizerBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikEqualizerBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikEqualizerBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_ANY );

	// child
	if ( !m_wndChild->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0), this, 123 ) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
}

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerCtrl, CWnd)

///////////////////////////////////////////////////

CmusikEqualizerCtrl::CmusikEqualizerCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikEqualizerCtrl::~CmusikEqualizerCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerCtrl, CWnd)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

