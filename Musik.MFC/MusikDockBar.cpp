///////////////////////////////////////////////////

#include "stdafx.h"

#include "MusikDockBar.h"
#include "MusikPrefs.h"

#include "MEMDC.H"
#include ".\musikdockbar.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikDockBar, baseMusikDockBar);

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikDockBar, baseMusikDockBar)
    ON_WM_NCLBUTTONUP()
    ON_WM_NCHITTEST()
	ON_WM_NCPAINT()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMusikDockBar::CMusikDockBar( CMusikPrefs* prefs, CMenu* menu )
{
	m_bActive = false;

	m_Prefs = prefs;
	m_Menu = menu;

	ShowGripper( true );
    m_cyGripper = 12;

	m_biHide = new CMusikDockbarBtn( prefs, 'r' );
	m_biOptions = new CMusikDockbarBtn( prefs, 'u' );
}

///////////////////////////////////////////////////

CMusikDockBar::~CMusikDockBar()
{
	if ( m_biHide )
		delete m_biHide;

	if ( m_biOptions )
		delete m_biOptions;
}

///////////////////////////////////////////////////

void CMusikDockBar::ShowGripper( bool show )
{
	m_ShowGripper = show;
}

///////////////////////////////////////////////////

void CMusikDockBar::OnNcLButtonUp( UINT nHitTest, CPoint point )
{
    if ( nHitTest == HTCLOSE )
        m_pDockSite->ShowControlBar(this, FALSE, FALSE); // hide

    baseMusikDockBar::OnNcLButtonUp( nHitTest, point );
}

///////////////////////////////////////////////////

void CMusikDockBar::NcCalcClient( LPRECT pRc, UINT nDockBarID )
{
	// save the bar rect
    CRect rcBar( pRc ); 

    // subtract edges
    baseMusikDockBar::NcCalcClient(pRc, nDockBarID);

    if ( !HasGripper() || !m_ShowGripper )
        return;

	// the client rect as calculated by 
	// the base class
    CRect rc(pRc); 

    rc.DeflateRect( 0, m_cyGripper, 0, 0 );

    // set position for the close 
	// and option buttons
    CPoint ptOrgBtn = CPoint( rc.right - 12, rc.top - 13 );
	CPoint ptOptBtn = CPoint( ptOrgBtn.x - 14, ptOrgBtn.y );

    m_biHide->Move( ptOrgBtn - rcBar.TopLeft() );
	m_biOptions->Move( ptOptBtn - rcBar.TopLeft() );

    *pRc = rc;
}

///////////////////////////////////////////////////

void CMusikDockBar::OnNcPaint()
{
    // get window DC that is clipped to the non-client area
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
	mdc.FillSolidRect( rcDraw, m_Prefs->MUSIK_COLOR_BTNFACE );
    NcPaintGripper( &mdc, rcClient );

	// draw border...
	if ( HasGripper() && m_ShowGripper )
	{
		rcClient.InflateRect( 1, 1, 1, 1 );
		mdc.Draw3dRect( rcClient, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );
		rcClient.DeflateRect( 1, 1, 1, 1 );
	}

    // client area is not our bussiness
    dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);
}

///////////////////////////////////////////////////

BOOL CMusikDockBar::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
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

void CMusikDockBar::NcPaintGripper( CDC* pDC, CRect rcClient )
{
	if ( !HasGripper() || !m_ShowGripper )
        return;

    // compute the caption rectangle
    CRect rcGrip = rcClient;
    CRect rcBtn = m_biOptions->GetRect();

	// gripper at top
	rcGrip.left -= 1;
    rcGrip.top -= m_cyGripper + 1;
    rcGrip.bottom = rcGrip.top + 11;
    rcGrip.right = rcBtn.left - 3;
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
		COLORREF clrCptnText;
		if ( m_bActive )
			clrCptnText = m_Prefs->MUSIK_COLOR_CAPTIONTEXT;
		else
			clrCptnText = m_Prefs->MUSIK_COLOR_INACTIVECAPTIONTEXT;

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
    m_biHide->Paint( pDC );
	m_biOptions->Paint( pDC );
}

///////////////////////////////////////////////////

UINT CMusikDockBar::OnNcHitTest( CPoint point )
{
    CRect rcBar;
    GetWindowRect( rcBar );

	// hit client area
    UINT nRet = baseMusikDockBar::OnNcHitTest( point );
    if ( nRet != HTCLIENT )
        return nRet;

	// hit the close button
    CRect rc = m_biHide->GetRect();
    rc.OffsetRect( rcBar.TopLeft() );
    if ( rc.PtInRect( point ) )
        return HTCLOSE;

	// hit options button
	rc = m_biOptions->GetRect();
	rc.OffsetRect( rcBar.TopLeft() );
	if ( rc.PtInRect( point ) )
		return HTOPTIONS;

    return HTCLIENT;
}

///////////////////////////////////////////////////

void CMusikDockBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
    UNUSED_ALWAYS( bDisableIfNoHndler );
    UNUSED_ALWAYS( pTarget );

    if ( !HasGripper() )
        return;

	BOOL active_change = m_bActive;
	CWnd* pFocus = GetFocus();
	m_bActive = ( pFocus->GetSafeHwnd() && IsChild( pFocus ) );

    CPoint pt;
    ::GetCursorPos(&pt);

    BOOL bLButtonDown = ( ::GetKeyState( VK_LBUTTON ) < 0 );
	
	// was the close buton pressed?
	BOOL bHideHit			= ( OnNcHitTest( pt ) == HTCLOSE );
    BOOL bWasHidePushed		= m_biHide->bPushed;
    m_biHide->bPushed		= bHideHit && bLButtonDown;
    BOOL bWasHideRaised		= m_biHide->bRaised;
    m_biHide->bRaised		= bHideHit && !bLButtonDown;

	// was the option button pressed?
    BOOL bOptionsHit		= ( OnNcHitTest( pt ) == HTOPTIONS );
    BOOL bWasOptionsPushed	= m_biOptions->bPushed;
    m_biOptions->bPushed	= bOptionsHit && bLButtonDown;
    BOOL bWasOptionsRaised	= m_biOptions->bRaised;
    m_biOptions->bRaised	= bOptionsHit && !bLButtonDown;

	// do we need to paint?
	BOOL bNeedPaint;
    bNeedPaint |= ( m_biHide->bPushed ^ bWasHidePushed ) ||
                  ( m_biHide->bRaised ^ bWasHideRaised ) ||
				  ( m_biOptions->bPushed ^ bWasOptionsPushed ) ||
				  ( m_biOptions->bRaised ^ bWasOptionsRaised ) ;

	if ( m_bActive != active_change )
		bNeedPaint = true;

    if ( bNeedPaint )
        SendMessage( WM_NCPAINT );
}

///////////////////////////////////////////////////

LRESULT CMusikDockBar::OnSetText( WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult = baseMusikDockBar::OnSetText( wParam, lParam );

    SendMessage( WM_NCPAINT );

    return lResult;
}

///////////////////////////////////////////////////

CMusikDockbarBtn::CMusikDockbarBtn( CMusikPrefs* prefs, char btn )
{
    bRaised = FALSE;
    bPushed = FALSE;
	m_Btn = btn;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

void CMusikDockbarBtn::Paint(CDC* pDC)
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
    pDC->TextOut( ptOrg.x + 2, ptOrg.y + 2, CString( m_Btn ) );

	// select the old object
    pDC->SelectObject( oldfont );
    pDC->SetBkMode( nPrevBkMode );
    pDC->SetTextColor( clrOldTextColor );
}

///////////////////////////////////////////////////

BOOL CMusikDockBar::HasGripper() const
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

BOOL CMusikDockBar::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CMusikDockBar::DrawBorders( CDC* pDC, CRect& rect )
{
	int i;
	// we don't want borders, damnit
}

///////////////////////////////////////////////////

