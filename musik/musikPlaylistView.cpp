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
//   CmusikPlaylistView
//
// Filename(s): 
//
//   musikPlaylistView.h, musikPlaylistView.cpp
//
// Information:
//
//   A CWnd derived object that maintains a CmusikPlaylistCtrl
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikPlaylistView.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikArrays.h"
#include "../musikCore/include/musikPlayer.h"

#include "musikPlaylistDropTarget.h"

#include "MEMDC.H"
#include ".\musikplaylistview.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPlaylistView, CWnd)

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistView, CWnd)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

///////////////////////////////////////////////////
CmusikPlaylistView::CmusikPlaylistView( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid_l, UINT dropid_r )
{
	m_Playlist = new CmusikPlaylistCtrl( mainwnd, library, player, prefs, dropid_l, dropid_r );
	m_DropTarget = new CmusikPlaylistDropTarget( m_Playlist, dropid_l );

	m_Library = library;
	m_Player = player;
	m_Parent = mainwnd;
	m_Prefs = prefs;

	m_PlaylistInfo = NULL;
}

///////////////////////////////////////////////////

CmusikPlaylistView::~CmusikPlaylistView()
{
	m_DropTarget->Revoke();
	delete m_DropTarget;
	delete m_Playlist;
	CleanPlaylistInfo();
}

///////////////////////////////////////////////////

int CmusikPlaylistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// view
	ModifyStyleEx( WS_EX_STATICEDGE, NULL );
	ModifyStyle( WS_BORDER, 0 );

	m_DropTarget->Register( this );

	// child
	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | WS_VSCROLL | WS_HSCROLL | LVS_SHOWSELALWAYS;
	long dwStyleEx = LVS_EX_FULLROWSELECT /*| LVS_EX_HEADERDRAGDROP*/;

	m_Playlist->Create( dwStyle, CRect( 0, 0, 0, 0 ), this, 123 );
	m_Playlist->SetExtendedStyle( dwStyleEx );

	m_Playlist->SubclassHeader();

	if ( m_Prefs->PlaylistInfoVisible() )
		InitPlaylistInfo();

	return 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if ( m_Prefs->PlaylistInfoVisible() )
	{
		if ( !m_PlaylistInfo )
			InitPlaylistInfo();

		m_PlaylistInfo->MoveWindow( 3, 3, cx - 6, 15 );
		m_Playlist->MoveWindow( 3, 24, cx - 6, cy - 27 );

		return;
	}

	// variable not set, but playlist info
	// window still exists, destroy it..
	else if ( m_PlaylistInfo )
		CleanPlaylistInfo();

	m_Playlist->MoveWindow( 3, 3, cx - 6, cy - 8 );
}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnNcPaint()
{
    // get window DC that is clipped to the non-client area
    CWindowDC dc(this);

	// rect of the CListCtrl
    CRect rcClient;
    GetClientRect( rcClient );
    ClientToScreen( rcClient );
    rcClient.OffsetRect( -rcClient.TopLeft() );

	// rect of entire window
	CRect rcWindow;
	GetWindowRect( rcWindow );
	rcWindow.OffsetRect( -rcWindow.TopLeft() );

	// memory device context to render to
	CMemDC pDC( &dc );

    // erase the NC background
	pDC.FillSolidRect( rcWindow, m_Prefs->MUSIK_COLOR_BTNFACE );

	// draw a simple border
	CRect rcBorder = rcWindow;
	CRect rcInfo;
	if ( !m_Prefs->PlaylistInfoVisible() )
	{
		rcBorder.left += 2;
		rcBorder.top += 2;
		rcBorder.right -= 2;
		rcBorder.bottom -= 4;
		pDC.Draw3dRect( rcBorder, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );
	}
	else if ( m_Prefs->PlaylistInfoVisible() )
	{
		rcBorder.left += 2;
		rcBorder.top += 23;
		rcBorder.right -= 2;
		rcBorder.bottom -= 4;
		pDC.Draw3dRect( rcBorder, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

		rcInfo = rcBorder;
		rcInfo.top = 2;
		rcInfo.bottom = 19;
		pDC.Draw3dRect( rcInfo, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );
	}

	// line at bottom
	CRect rcBottom;
	rcBottom.left = 0;
	rcBottom.right = rcClient.Width();
	rcBottom.top = rcClient.Height() - 2;
	rcBottom.bottom = rcClient.Height();
	pDC.Draw3dRect( rcBottom, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

    // client area is not our bussiness
	GetWindowRect( rcWindow );
	ScreenToClient( rcWindow );
    dc.IntersectClipRect( rcWindow );

	CRect rcDraw = rcBorder;
	rcDraw.left += 1;
	rcDraw.top += 1;
	rcDraw.right -= 1;
	rcDraw.bottom -= 1;
	dc.ExcludeClipRect( rcDraw );
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistView::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistView::InitPlaylistInfo() 
{
	if ( !m_PlaylistInfo )
	{
		m_PlaylistInfo = new CWnd();
		m_PlaylistInfo->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistView::CleanPlaylistInfo()
{
	if ( m_PlaylistInfo )
	{	
		delete m_PlaylistInfo;
		m_PlaylistInfo = NULL;
	}	
}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	GetCtrl()->ShowContextMenu();
}

///////////////////////////////////////////////////


