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
// Class(s): 
//
//   CmusikHeaderCtrl
//
// Filename(s): 
//
//   musikHeaderCtrl.h, musikHeaderCtrl.cpp
//
// Information:
//
//   A custom header control that allows drag and drop rearranging
//   to replace the one that was done very poorly in MFC
// 
// Usage: 
//
//   CmusikListCtrl will subclass the default header with this one,
//   so integration should be seamless.
//
// Some code adapted from:
//
//   http://www.codeguru.com/listview/drag_col.shtml
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikHeaderCtrl : public CHeaderCtrl
{
public:
	// construct and destruct
	CmusikHeaderCtrl();
	CmusikHeaderCtrl(CWnd* pWnd, void (CWnd::*fpDragCol)(int, int));
	virtual ~CmusikHeaderCtrl();

	// functions
	void SetCallback(CWnd* pWnd, void (CWnd::*fpDragCol)(int, int));

	// if we want to enable header DND
	void EnableHeaderDND( bool enable = true ){ m_bEnableDND = enable; }

protected:

	// vars
	BOOL	m_bEnableDND;
	BOOL	m_bCheckForDrag;
	BOOL	m_bDragging;
	BOOL	m_bFirstRun;
	int		*m_pWidth;
	int		m_nDragCol;
	int		m_nDropPos;
	CRect	marker_rect;
	void	(CWnd::*m_fpDragCol)(int, int);
	CWnd	*m_pOwnerWnd;

	// message maps
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	// macros
	DECLARE_DYNAMIC(CmusikHeaderCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
