/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarCF          Version 2.43
// 
// Created: Dec 21, 1998        Last Modified: August 03, 2000
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998-2000 by Cristi Posea. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// cristi@datamekanix.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////
// The Musik Team attempted to gain permission to distribute these files
// with the source, but no response was ever recieved. The code is 
// somewhat modified, and if the author wishes us to remove it, he
// should contact Casey Langen at casey@bak.rr.com
/////////////////////////////////////////////////////////////////////////

// sizecbar.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarG

IMPLEMENT_DYNAMIC(CSizingControlBarG, baseCSizingControlBarG);

CSizingControlBarG::CSizingControlBarG()
{
	ShowGripper( true );
    m_cyGripper = 12;

	m_biHide = new CSCBButton( 'r' );
	m_biOptions = new CSCBButton( 'u' );
}

CSizingControlBarG::~CSizingControlBarG()
{
	delete m_biHide;
	delete m_biOptions;
}

BEGIN_MESSAGE_MAP(CSizingControlBarG, baseCSizingControlBarG)
    //{{AFX_MSG_MAP(CSizingControlBarG)
    ON_WM_NCLBUTTONUP()
    ON_WM_NCHITTEST()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarG message handlers

/////////////////////////////////////////////////////////////////////////
// Mouse Handling
//

void CSizingControlBarG::ShowGripper( bool show )
{
	m_ShowGripper = show;
}

void CSizingControlBarG::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCLOSE)
        m_pDockSite->ShowControlBar(this, FALSE, FALSE); // hide

    baseCSizingControlBarG::OnNcLButtonUp(nHitTest, point);
}

void CSizingControlBarG::NcCalcClient(LPRECT pRc, UINT nDockBarID)
{
    CRect rcBar(pRc); // save the bar rect

    // subtract edges
    baseCSizingControlBarG::NcCalcClient(pRc, nDockBarID);

    if (!HasGripper() || !m_ShowGripper )
        return;

    CRect rc(pRc); // the client rect as calculated by the base class

    rc.DeflateRect(0, m_cyGripper, 0, 0);

    // set position for the "x" (hide bar) button
    CPoint ptOrgBtn = CPoint(rc.right - 12, rc.top - 13);
	CPoint ptOptBtn = CPoint(ptOrgBtn.x - 14, ptOrgBtn.y );

    m_biHide->Move(ptOrgBtn - rcBar.TopLeft());
	m_biOptions->Move( ptOptBtn - rcBar.TopLeft() );

    *pRc = rc;
}

void CSizingControlBarG::NcPaintGripper(CDC* pDC, CRect rcClient)
{
    if (!HasGripper() || !m_ShowGripper )
        return;

    CRect gripper = rcClient;
    CRect rcbtn = m_biOptions->GetRect();

    gripper.DeflateRect(1, 1);
	gripper.top -= m_cyGripper - 2;
	gripper.bottom = gripper.top + 3;
	gripper.right = rcbtn.left - 3;

    pDC->Draw3dRect(gripper, ::GetSysColor(COLOR_BTNHIGHLIGHT),
        ::GetSysColor(COLOR_BTNSHADOW));

	m_biHide->Paint(pDC);
	m_biOptions->Paint(pDC);
}

UINT CSizingControlBarG::OnNcHitTest(CPoint point)
{
    CRect rcBar;
    GetWindowRect(rcBar);

    UINT nRet = baseCSizingControlBarG::OnNcHitTest(point);
    if (nRet != HTCLIENT)
        return nRet;

    CRect rc = m_biHide->GetRect();
    rc.OffsetRect(rcBar.TopLeft());
    if (rc.PtInRect(point))
        return HTCLOSE;

	rc = m_biOptions->GetRect();
	rc.OffsetRect(rcBar.TopLeft());
	if (rc.PtInRect(point))
		return HTOPTIONS;

    return HTCLIENT;
}

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarG implementation helpers

void CSizingControlBarG::OnUpdateCmdUI(CFrameWnd* pTarget,
                                      BOOL bDisableIfNoHndler)
{
    UNUSED_ALWAYS(bDisableIfNoHndler);
    UNUSED_ALWAYS(pTarget);

    if (!HasGripper())
        return;

    BOOL bNeedPaint = FALSE;

    CPoint pt;
    ::GetCursorPos(&pt);

    BOOL bLButtonDown = (::GetKeyState(VK_LBUTTON) < 0);
	
	BOOL bHideHit = (OnNcHitTest(pt) == HTCLOSE);
    BOOL bWasHidePushed = m_biHide->bPushed;
    m_biHide->bPushed = bHideHit && bLButtonDown;
    BOOL bWasHideRaised = m_biHide->bRaised;
    m_biHide->bRaised = bHideHit && !bLButtonDown;

    BOOL bOptionsHit = (OnNcHitTest(pt) == HTOPTIONS);
    BOOL bWasOptionsPushed = m_biOptions->bPushed;
    m_biOptions->bPushed = bOptionsHit && bLButtonDown;
    BOOL bWasOptionsRaised = m_biOptions->bRaised;
    m_biOptions->bRaised = bOptionsHit && !bLButtonDown;

    bNeedPaint |= (m_biHide->bPushed ^ bWasHidePushed) ||
                  (m_biHide->bRaised ^ bWasHideRaised) ||
				  (m_biOptions->bPushed ^ bWasOptionsPushed) ||
				  (m_biOptions->bRaised ^ bWasOptionsRaised) ;

    if (bNeedPaint)
        SendMessage(WM_NCPAINT);
}

/////////////////////////////////////////////////////////////////////////
// CSCBButton

CSCBButton::CSCBButton( char btn )
{
    bRaised = FALSE;
    bPushed = FALSE;
	m_Btn = btn;
}

void CSCBButton::Paint(CDC* pDC)
{
    CRect rc = GetRect();

    if (bPushed)
        pDC->Draw3dRect(rc, ::GetSysColor(COLOR_BTNSHADOW),
            ::GetSysColor(COLOR_BTNHIGHLIGHT));
    else
        if (bRaised)
            pDC->Draw3dRect(rc, ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));

    COLORREF clrOldTextColor = pDC->GetTextColor();
    pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);
    CFont font;
    int ppi = pDC->GetDeviceCaps(LOGPIXELSX);
    int pointsize = MulDiv(60, 96, ppi); // 6 points at 96 ppi
    font.CreatePointFont(pointsize, _T("Marlett"));
    CFont* oldfont = pDC->SelectObject(&font);

    pDC->TextOut(ptOrg.x + 2, ptOrg.y + 2, CString( m_Btn ) ); // x-like

    pDC->SelectObject(oldfont);
    pDC->SetBkMode(nPrevBkMode);
    pDC->SetTextColor(clrOldTextColor);
}

BOOL CSizingControlBarG::HasGripper() const
{
#if defined(_SCB_MINIFRAME_CAPTION) || !defined(_SCB_REPLACE_MINIFRAME)
    // if the miniframe has a caption, don't display the gripper
    if (IsFloating())
        return FALSE;
#endif //_SCB_MINIFRAME_CAPTION

    return TRUE;
}