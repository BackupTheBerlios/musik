///////////////////////////////////////////////////

#include "stdafx.h"

#include "MusikDockBar.h"

#include "MusikPrefs.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikDockBar, baseMusikDockBar);

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikDockBar, baseMusikDockBar)
    ON_WM_NCLBUTTONUP()
    ON_WM_NCHITTEST()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMusikDockBar::CMusikDockBar( CMusikPrefs* prefs, CMenu* menu )
{
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

void CMusikDockBar::NcPaintGripper( CDC* pDC, CRect rcClient )
{
	if ( !HasGripper() || !m_ShowGripper )
        return;

    // compute the caption rectangle
    CRect rcGrip = rcClient;
    CRect rcBtn = m_biOptions->GetRect();

	// gripper at top
    rcGrip.top -= m_cyGripper + 1;
    rcGrip.bottom = rcGrip.top + 11;
    rcGrip.right = rcBtn.left - 3;
    rcGrip.InflateRect( 0, 1 );

    // draw the caption background
	COLORREF clrCptn = m_Prefs->MUSIK_COLOR_ACTIVECAPTION;
    pDC->FillSolidRect( &rcGrip, clrCptn );

    // select a font for the caption text
	// (8.5 points at 96 ppi)
    CFont font;
    int ppi = pDC->GetDeviceCaps( LOGPIXELSX );
    int pointsize = MulDiv( 85, 96, ppi );
    BOOL bFont = font.CreatePointFont( pointsize, m_sFontFace );

	// draw it
    if (bFont)
    {
        // get the text color
        COLORREF clrCptnText = m_bActive ?
            m_Prefs->MUSIK_COLOR_CAPTIONTEXT:
            m_Prefs->MUSIK_COLOR_INACTIVECAPTIONTEXT;

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

    BOOL bNeedPaint = FALSE;

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
    bNeedPaint |= (m_biHide->bPushed ^ bWasHidePushed) ||
                  (m_biHide->bRaised ^ bWasHideRaised) ||
				  (m_biOptions->bPushed ^ bWasOptionsPushed) ||
				  (m_biOptions->bRaised ^ bWasOptionsRaised) ;

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
