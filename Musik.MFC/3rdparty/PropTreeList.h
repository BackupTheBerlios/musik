///////////////////////////////////////////////////
// PropTree.h : header file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.
//
//  Modified by Casey Langen for Musik MFC port...
//  Rather than linking to a dll or external lib, it compiles in.
//  Changes default look and behavior as well. Much thanks to the
//  Author Scott Ramsay.
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CPropTree;

///////////////////////////////////////////////////

class CPropTreeList : public CWnd
{
public:

	// construct / destruct
	CPropTreeList();
	virtual ~CPropTreeList();

	// overrides
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// misc
	void UpdateResize();
	void SetPropOwner(CPropTree* pProp);

	// public message maps
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	// CPropTree class that this class belongs
	CPropTree*		m_pProp;

	// bitmap back buffer for flicker free drawing
	CBitmap			m_BackBuffer;

	// current diminsions of the back buffer
	CSize			m_BackBufferSize;

	// misc
	void RecreateBackBuffer(int cx, int cy);
	void CheckVisibleFocus();

	// protected message maps
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
