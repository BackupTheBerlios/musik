///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikNowPlayingBar.h"
#include "MusikPrefs.h"

#include "../Musik.Core/include/MusikPlayer.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

CMusikNowPlayingBar::CMusikNowPlayingBar( CMusikPlayer* player, CMusikPrefs* prefs )
	: CMusikDockBar( prefs )
{
	m_wndChild = new CMusikNowPlayingCtrl( player, prefs );
}

///////////////////////////////////////////////////

CMusikNowPlayingBar::~CMusikNowPlayingBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikNowPlayingBar, baseCMusikNowPlayingBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CMusikNowPlayingBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCMusikNowPlayingBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_BOTTOM );

	ShowGripper( false );

	// child
	if ( !m_wndChild->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0), this, 123) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );
	
	return 0;
}

///////////////////////////////////////////////////

void CMusikNowPlayingBar::OnSize(UINT nType, int cx, int cy)
{
	CMusikDockBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( &rcClient );
}

///////////////////////////////////////////////////