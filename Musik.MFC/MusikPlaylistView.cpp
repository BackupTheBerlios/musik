///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikPlaylistView.h"
#include "MusikPrefs.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikPlayer.h"

#include "MusikPlaylistDropTarget.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikPlaylistView, CWnd)

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikPlaylistView, CWnd)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////
CMusikPlaylistView::CMusikPlaylistView( CFrameWnd* mainwnd, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, int dropid )
{
	m_Playlist = new CMusikPlaylistCtrl( mainwnd, library, player, prefs, dropid );
	m_DropTarget = new CMusikPlaylistDropTarget( m_Playlist, dropid );

	m_Library = library;
	m_Player = player;
	m_Parent = mainwnd;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CMusikPlaylistView::~CMusikPlaylistView()
{
	m_DropTarget->Revoke();
	delete m_DropTarget;
	delete m_Playlist;
}

///////////////////////////////////////////////////

int CMusikPlaylistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// view
	ModifyStyleEx( WS_EX_STATICEDGE, NULL );
	ModifyStyle( WS_BORDER, 0 );

	m_DropTarget->Register( this );

	// child
	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | WS_VSCROLL | WS_HSCROLL;
	long dwStyleEx = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;

	m_Playlist->Create( dwStyle, CRect( 0, 0, 0, 0 ), this, 123 );
	m_Playlist->SetExtendedStyle( dwStyleEx );

	return 0;
}

///////////////////////////////////////////////////

void CMusikPlaylistView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Playlist->MoveWindow( 3, 3, cx - 6, cy - 8 );
}

///////////////////////////////////////////////////

void CMusikPlaylistView::OnNcPaint()
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
	rcBorder.left += 2;
	rcBorder.top += 2;
	rcBorder.right -= 2;
	rcBorder.bottom -= 4;
	pDC.Draw3dRect( rcBorder, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

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

BOOL CMusikPlaylistView::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

