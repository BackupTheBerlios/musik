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
//
//   CmusikListCtrl
//
// Filename(s): 
//
//   musikListCtrl.h, musikListCtrl.cpp
//
// Information:
//
//   musikCube's custom list control
// 
// Usage: 
//
//   A few specialized functions, like hiding scrollbars
//   and a better header DND implementation
//
// Some code adapted from:
//
//   Lars [Large] Werner: http://www.codeproject.com/listctrl/listctrlsbhide.asp
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