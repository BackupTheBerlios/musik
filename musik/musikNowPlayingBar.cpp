///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikNowPlayingBar.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

CmusikNowPlayingBar::CmusikNowPlayingBar( CFrameWnd* parent, CmusikPlayer* player, CmusikPrefs* prefs )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikNowPlayingCtrl( parent, player, prefs );
}

///////////////////////////////////////////////////

CmusikNowPlayingBar::~CmusikNowPlayingBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNowPlayingBar, baseCmusikNowPlayingBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikNowPlayingBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikNowPlayingBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_BOTTOM );

	ShowGripper( false );

	// child
	if ( !m_wndChild->Create( NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0), this, 123) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );
	
	return 0;
}

///////////////////////////////////////////////////

void CmusikNowPlayingBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( &rcClient );
}

///////////////////////////////////////////////////