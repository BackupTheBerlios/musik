///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//   CmusikDockBar
//
// Filename(s): 
//
//   musikDockBar.h, musikDockBar.cpp
//
// Information:
//
//   A CSizingControlBar specifically adpated for the
//   musikCube UI
// 
// Usage: 
//
//   Same as a CSizingControlBar... see URL below
//
// Some code adapted from:
//
//   Cristi Posea: http://www.datamekanix.com/sizecbar/
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikDockBar.h"
#include "musikPrefs.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDockBar, basemusikDockBar);

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDockBar, basemusikDockBar)
    ON_WM_NCLBUTTONUP()
    ON_WM_NCHITTEST()
	ON_WM_NCPAINT()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikDockBar::CmusikDockBar( CmusikPrefs* prefs, CMenu* menu, bool show_options_button )
{
	m_ForceDrawBorder = false;
	m_Prefs = prefs;
	m_Menu = menu;

	ShowGripper( true );
    m_cyGripper = 12;

	m_ShowButton = show_options_button;

	if ( m_ShowButton )
		m_biOptions = new CmusikDockbarBtn( prefs, '6' );
	else
		m_biOptions = NULL;
}

///////////////////////////////////////////////////

CmusikDockBar::~CmusikDockBar()
{
	if ( m_biOptions )
		delete m_biOptions;
}

///////////////////////////////////////////////////

void CmusikDockBar::ShowGripper( bool show )
{
	m_ShowGripper = show;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnNcLButtonUp( UINT nHitTest, CPoint point )
{
    if ( nHitTest == HTCLOSE )
	{
		OnOptions();
		return;
	}

    basemusikDockBar::OnNcLButtonUp( nHitTest, point );
}

///////////////////////////////////////////////////

void CmusikDockBar::NcCalcClient( LPRECT pRc, UINT nDockBarID )
{
	// save the bar rect
    CRect rcBar( pRc ); 

    // subtract edges
    basemusikDockBar::NcCalcClient(pRc, nDockBarID);

	CRect rc(pRc); 
    if ( IsFloating() || ( HasGripper() && m_ShowGripper ) )
    {
		// the client rect as calculated by 
		// the base class
		rc.DeflateRect( 0, m_cyGripper, 0, 0 );

		// set position for option button
		CPoint ptOptBtn = CPoint( rc.right - 13, rc.top - 14 );

		if ( m_ShowButton && m_biOptions )
			m_biOptions->Move( ptOptBtn - rcBar.TopLeft() );
	}
	else
		rc.DeflateRect( 0, -2, 0, 0 );

    *pRc = rc;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnNcPaint()
{
	CWindowDC dc(this);

    CRect rcClient, rcBar;
    GetClientRect( rcClient );
    ClientToScreen( rcClient );

    GetWindowRect( rcBar );
    rcClient.OffsetRect( -rcBar.TopLeft() );
    rcBar.OffsetRect( -rcBar.TopLeft() );

    CMemDC mdc( &dc );
    
    // erase the NC background
	CRect rcDraw = rcBar;
	rcDraw.InflateRect( 1, 1, 1, 1 );
	mdc.FillSolidRect( rcDraw, m_Prefs->MUSIK_COLOR_BTNFACE );

	// draw border and gripper...
	if ( ( m_ShowGripper || m_ForceDrawBorder ) && !IsFloating() )
	{
		NcPaintGripper( &mdc, rcClient );
		rcClient.InflateRect( 1, 1, 1, 1 );
		mdc.Draw3dRect( rcClient, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );
		rcClient.DeflateRect( 1, 1, 1, 1 );
	}

    // client area is not our bussiness
    dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);
}

///////////////////////////////////////////////////

BOOL CmusikDockBar::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    // must have a parent
    ASSERT_VALID(pParentWnd);

    // cannot be both fixed and dynamic
    // (CBRS_SIZE_DYNAMIC is used for resizng when floating)
    ASSERT ( !( ( dwStyle & CBRS_SIZE_FIXED ) &&
              ( dwStyle & CBRS_SIZE_DYNAMIC ) ) );

	// save the control bar styles
    m_dwStyle = dwStyle & CBRS_ALL; 

    // register and create the window - skip CControlBar::Create()
	HBRUSH back = CreateSolidBrush( (COLORREF)m_Prefs->MUSIK_COLOR_BTNFACE );
    CString wndclass = ::AfxRegisterWndClass( CS_DBLCLKS,
        ::LoadCursor( NULL, IDC_ARROW ), 
		back, 0 );

	// keep only the generic window styles, as 
	// well as WS_CLIPCHILDREN to prevent flashing
    dwStyle &= ~CBRS_ALL;
    dwStyle |= WS_CLIPCHILDREN;

	// create it
    if ( !CWnd::Create( wndclass, lpszWindowName, dwStyle, CRect( 0, 0, 0, 0 ), pParentWnd, nID ) )
        return FALSE;

    return TRUE;
}

///////////////////////////////////////////////////

void CmusikDockBar::NcPaintGripper( CDC* pDC, CRect rcClient )
{
	if ( !IsFloating() && ( !HasGripper() || !m_ShowGripper )  )
        return;

    // compute the caption rectangle
    CRect rcGrip = rcClient;
    
	CRect rcBtn = rcClient;
	if ( m_ShowButton && m_biOptions )
		rcBtn = m_biOptions->GetRect();

	// gripper at top

	if ( IsFloating() )
		rcGrip.left = 2;
	else
		rcGrip.left = 4;

    rcGrip.top = 4;
    rcGrip.bottom = 14;

	if ( m_ShowButton )
	  rcGrip.right = rcBtn.left - 2;

    rcGrip.InflateRect( 0, 1 );

    // draw the caption background
	COLORREF clrCptn = m_Prefs->MUSIK_COLOR_ACTIVECAPTION;
    pDC->FillSolidRect( &rcGrip, clrCptn );

    // select a font for the caption text
	// (7.5 points at 96 ppi)
    CFont font;
    int ppi = pDC->GetDeviceCaps( LOGPIXELSX );
    int pointsize = MulDiv( 75, 96, ppi );
    BOOL bFont = font.CreatePointFont( pointsize, m_sFontFace );

	// draw it
    if ( bFont )
    {
        // get the text color
		COLORREF clrCptnText = m_Prefs->MUSIK_COLOR_CAPTIONTEXT;

		int nOldBkMode = pDC->SetBkMode( TRANSPARENT );
        COLORREF clrOldText = pDC->SetTextColor( clrCptnText );

        CFont* pOldFont = pDC->SelectObject( &font );
        CString sTitle;
        GetWindowText( sTitle );

        CPoint ptOrg = CPoint( rcGrip.left + 3, rcGrip.top );

        pDC->ExtTextOut( ptOrg.x, ptOrg.y,
            ETO_CLIPPED, rcGrip, sTitle, NULL );

        pDC->SelectObject( pOldFont );
        pDC->SetBkMode( nOldBkMode );
        pDC->SetTextColor( clrOldText );
    }

    // draw the buttons
	if ( m_ShowButton && m_biOptions )
		m_biOptions->Paint( pDC );
}

///////////////////////////////////////////////////

UINT CmusikDockBar::OnNcHitTest( CPoint point )
{
    CRect rcBar;
    GetWindowRect( rcBar );

	// hit client area
    UINT nRet = basemusikDockBar::OnNcHitTest( point );
    if ( nRet != HTCLIENT )
        return nRet;

	// hit the options button
	if ( m_ShowButton && m_biOptions )
	{
		CRect rc = m_biOptions->GetRect();
		rc.OffsetRect( rcBar.TopLeft() );
		if ( rc.PtInRect( point ) )
			return HTCLOSE;
	}

    return HTCLIENT;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
    UNUSED_ALWAYS( bDisableIfNoHndler );
    UNUSED_ALWAYS( pTarget );

	// was the options buton pressed?
	if ( HasGripper() && m_ShowButton && m_biOptions )
	{  
		// what was hit?
		CPoint pt;
		::GetCursorPos(&pt);
   		BOOL bLButtonDown = ( ::GetKeyState( VK_LBUTTON ) < 0 );
		UINT nHit = OnNcHitTest( pt );

		BOOL bHideHit			= ( nHit == HTCLOSE );
		BOOL bWasHidePushed		= m_biOptions->bPushed;
		m_biOptions->bPushed	= bHideHit && bLButtonDown;
		BOOL bWasHideRaised		= m_biOptions->bRaised;
		m_biOptions->bRaised	= bHideHit && !bLButtonDown;

		// needs painting
		BOOL bNeedsPaint = FALSE;
		bNeedsPaint |=	( m_biOptions->bPushed ^ bWasHidePushed ) ||
						( m_biOptions->bRaised ^ bWasHideRaised );

		SendMessage( WM_NCPAINT );
			return;
	}
}

///////////////////////////////////////////////////

LRESULT CmusikDockBar::OnSetText( WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult = basemusikDockBar::OnSetText( wParam, lParam );

    SendMessage( WM_NCPAINT );

    return lResult;
}

///////////////////////////////////////////////////

CmusikDockbarBtn::CmusikDockbarBtn( CmusikPrefs* prefs, char btn )
{
    bRaised = FALSE;
    bPushed = FALSE;
	m_Btn = btn;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

void CmusikDockbarBtn::Paint(CDC* pDC)
{
    CRect rc = GetRect();

	// if pushed, redraw as raised
    if ( bPushed )
        pDC->Draw3dRect( rc, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

	// if raised, redraw as pushed
    else if ( bRaised )
            pDC->Draw3dRect( rc, m_Prefs->MUSIK_COLOR_BTNHILIGHT, m_Prefs->MUSIK_COLOR_BTNSHADOW );

	// set text color
    COLORREF clrOldTextColor = pDC->GetTextColor();
    pDC->SetTextColor( m_Prefs->MUSIK_COLOR_BTNTEXT );
    int nPrevBkMode = pDC->SetBkMode( TRANSPARENT );

	// set text font, 6 point at 96 ppi
    CFont font;
    int ppi = pDC->GetDeviceCaps( LOGPIXELSX );
    int pointsize = MulDiv( 60, 96, ppi );
    font.CreatePointFont( pointsize, _T( "Marlett" ) );
    CFont* oldfont = pDC->SelectObject( &font );

	// draw the text
    pDC->TextOut( ptOrg.x + 2, ptOrg.y + 3, CString( m_Btn ) );

	// select the old object
    pDC->SelectObject( oldfont );
    pDC->SetBkMode( nPrevBkMode );
    pDC->SetTextColor( clrOldTextColor );
}

///////////////////////////////////////////////////

BOOL CmusikDockBar::HasGripper() const
{
	#if defined(_SCB_MINIFRAME_CAPTION) || !defined(_SCB_REPLACE_MINIFRAME)
		// if the miniframe has a caption, 
		// don't display the gripper
		if ( IsFloating() )
			return FALSE;
	#endif

    return TRUE;
}

///////////////////////////////////////////////////

BOOL CmusikDockBar::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnOptions()
{
	TRACE0( "Options button pressed\n" );
}

///////////////////////////////////////////////////

void CmusikDockBar::ForceDockedSize( const CSize& size, int dockstate, bool layout )
{
	if ( dockstate == LM_HORZDOCK )
		m_szHorz = size;
	if ( dockstate == LM_VERTDOCK )
		m_szVert = size;

	if ( layout )
		Layout();
}

///////////////////////////////////////////////////

void CmusikDockBar::ForceFloatSize( const CSize& size )
{
	m_szFloat = size;
}

///////////////////////////////////////////////////