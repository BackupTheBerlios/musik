///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"

#include "musikCrossfaderCtrl.h"
#include "musikPrefs.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

// CmusikCrossfaderBar

///////////////////////////////////////////////////

CmusikCrossfaderBar::CmusikCrossfaderBar( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikCrossfaderCtrl( library, player, prefs );
}

///////////////////////////////////////////////////

CmusikCrossfaderBar::~CmusikCrossfaderBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikCrossfaderBar, baseCmusikCrossfaderBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikCrossfaderBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikCrossfaderBar::OnCreate(lpCreateStruct) == -1 )
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

void CmusikCrossfaderBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
}

///////////////////////////////////////////////////

// CmusikCrossfaderCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikCrossfaderCtrl, CWnd)

///////////////////////////////////////////////////

CmusikCrossfaderCtrl::CmusikCrossfaderCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikCrossfaderCtrl::~CmusikCrossfaderCtrl()
{
}


///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikCrossfaderCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikCrossfaderCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, m_Prefs->MUSIK_COLOR_BTNFACE );

	DefWindowProc( WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0 );
}

///////////////////////////////////////////////////

BOOL CmusikCrossfaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////
