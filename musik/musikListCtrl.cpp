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

#include "stdafx.h"
#include "musikListCtrl.h"

#include <sys/stat.h>
#include <afxdisp.h>	// OLE stuff
#include <shlwapi.h>	// Shell functions (PathFindExtension() in this case)
#include <afxpriv.h>	// ANSI to/from Unicode conversion macros

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

CmusikListCtrl::CmusikListCtrl()
{
	NCOverride = FALSE;	//False as default...
	Who = SB_BOTH;		//Default remove both...

	m_HeaderCtrl = NULL;
}

///////////////////////////////////////////////////

CmusikListCtrl::~CmusikListCtrl()
{
	if ( m_HeaderCtrl )
		delete m_HeaderCtrl;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikListCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

///////////////////////////////////////////////////

void CmusikListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default

	if(NCOverride==TRUE) //If the NCOverride is true we remove each time the CListCtrl calc its rect.
	{
		  if(Who==SB_HORZ) ModifyStyle(WS_HSCROLL ,0,0); //Just by modifing the style we remove the scrollbar
		  if(Who==SB_VERT) ModifyStyle(WS_VSCROLL,0,0);
		  if(Who==SB_BOTH) ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0);
	}

	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

///////////////////////////////////////////////////

void CmusikListCtrl::HideScrollBars(int Type, int Which)
{
	if(Type==LCSB_CLIENTDATA) //This is the clientrect function
	{
		RECT ierect;
		int cxvs, cyvs;
		GetClientRect(&ierect); //Get client width and height

		cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT
		cyvs = GetSystemMetrics (SM_CYVSCROLL); //Get the system metrics - HORZ
		
		if(Which==SB_HORZ) cxvs=0; //Set VERT to zero when choosen HORZ
		if(Which==SB_VERT) cyvs=0; //Set HORZ to zero when choosen VERT

		//Here we set the position of the window to the clientrect + the size of the scrollbars
		SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom+cyvs, SWP_NOMOVE | SWP_NOZORDER);

		//Her we modify the rect so the right part is subbed from the rect.
		if(Which==SB_BOTH||Which==SB_HORZ) ierect.bottom -= ierect.top;
		if(Which==SB_BOTH||Which==SB_VERT) ierect.right -= ierect.left;

		//Just to be safe that the left/top corner is 0...
		ierect.top = 0;
		ierect.left = 0;
		
		HRGN iehrgn = NULL; //This range is created base on which scrollbar that is going to be removed!

		//The -2 is probably a border of some kind that we also need to remove. I could not find any good
		//metrics that gave me an 2 as an answer. So insted we makes it static with -2.
		if(Which==SB_BOTH) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom-2);
		if(Which==SB_HORZ) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom-2);
		if(Which==SB_VERT) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom);
		
		//After the range has been made we add it...
		SetWindowRgn (iehrgn, TRUE);

		//Reset of NCOverride
		NCOverride=FALSE;
	}


	//This is the NcCalcSize override
	if( Type == LCSB_NCOVERRIDE ) 
	{
		NCOverride=TRUE;	//Set to true, so we run the code on each OnNcCalcSize.
		Who=Which;			//Selects which scrollbars to get hidden.
	}
}

///////////////////////////////////////////////////

void CmusikListCtrl::OnDragColumn( int source, int dest )
{
	// virtual
}

///////////////////////////////////////////////////

void CmusikListCtrl::SubclassHeader()
{
	if ( !m_HeaderCtrl )
	{
		ASSERT( GetHeaderCtrl() );

		// init custom header
		m_HeaderCtrl = new CmusikHeaderCtrl();
		m_HeaderCtrl->SetCallback( this, (void (CWnd::*)(int, int))OnDragColumn );
		m_HeaderCtrl->SubclassWindow( GetHeaderCtrl()->m_hWnd );
	}
}

///////////////////////////////////////////////////
