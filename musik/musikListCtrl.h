///////////////////////////////////////////////////
//
// Class(s): 
//
//   CmusikListCtrl 
//
// Filename(s): 
//
//   CmusikListCtrl.h,
//   CmusikListCtrl.cpp
//
// Information:
//
//   A CListCtrl instance that does things that musik needs,
//   such as drag and drop and hiding scrollbars.
//
// Example: 
//
//   CmusikSourcesCtrl, CmusikPlaylistCtrl
//
// Usage: 
//
//   See examples.
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//
//   Casey Langen   : Lead Developer, Project Manager
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
// 
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Hide Scrollbar code taken from: 
//   Lars [Large] Werner
//   http://www.codeproject.com/listctrl/listctrlsbhide.asp
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikHeaderCtrl.h"

///////////////////////////////////////////////////

#define LCSB_CLIENTDATA 1
#define LCSB_NCOVERRIDE 2

#define SB_HORZ             0
#define SB_VERT             1
#define SB_BOTH             3

///////////////////////////////////////////////////

class CmusikListCtrl : public CListCtrl
{
public:
	CmusikListCtrl();
	virtual ~CmusikListCtrl();

	// hide scrollbars
	void HideScrollBars( int Type, int Which=SB_BOTH );

	// call this to subclass the header to our
	// custom column arranging version. this should
	// only be done once, and cannot be undone
	void SubclassHeader();

protected:

	// message maps
	afx_msg void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp );
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );

	// custom header
	CmusikHeaderCtrl* m_HeaderCtrl;

	// function the custom header control
	// will use as a callback after a column
	// has been moved
	virtual void OnDragColumn( int source, int dest );

	// macros
	DECLARE_MESSAGE_MAP()

private:

	// Prevent default compiler generation of these copy constructors (no bitwise copy)
	CmusikListCtrl& operator=(const CmusikListCtrl& x);
	CmusikListCtrl(const CmusikListCtrl& x);

	// scrollbars
	BOOL NCOverride;
	int Who;
};

///////////////////////////////////////////////////