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

#if !defined(__SCBARG_H__)
#define __SCBARG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////
// CSCBButton (button info) helper class

class CSCBButton
{
public:
    CSCBButton( char btn = 'r' );

    void Move(CPoint ptTo) {ptOrg = ptTo; };
    CRect GetRect() { return CRect(ptOrg, CSize(11, 11)); };
    void Paint(CDC* pDC);

    BOOL    bPushed;
    BOOL    bRaised;

protected:
    CPoint  ptOrg;
	char	m_Btn;
};

/////////////////////////////////////////////////////////////////////////
// CSizingControlBar control bar

#ifndef baseCSizingControlBarG
#define baseCSizingControlBarG CSizingControlBar
#endif

class CSizingControlBarG : public baseCSizingControlBarG
{
    DECLARE_DYNAMIC(CSizingControlBarG);

// Construction
public:
    CSizingControlBarG();

// Attributes
public:
    virtual BOOL HasGripper() const;
	void ShowGripper( bool show = true );

// Operations
public:

// Overridables
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

// Overrides
public:

// Implementation
public:
    virtual ~CSizingControlBarG();
    
protected:
    // implementation helpers
    virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
    virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);

protected:
    int     m_cyGripper;
	bool	m_ShowGripper;

    CSCBButton* m_biHide;
	CSCBButton* m_biOptions;

// Generated message map functions
protected:
    afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);

    DECLARE_MESSAGE_MAP()
};

#endif

