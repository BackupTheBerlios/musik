///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikPlaylistView.h"
#include "MusikPrefs.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikPlayer.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikPlaylistView, CWnd)
CMusikPlaylistView::CMusikPlaylistView( CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, CMusikPlaylist* playlist )
{
	m_Playlist = new CMusikPlaylistCtrl( library, player, prefs, playlist );
}

///////////////////////////////////////////////////

CMusikPlaylistView::~CMusikPlaylistView()
{
	delete m_Playlist;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikPlaylistView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CMusikPlaylistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | WS_VSCROLL | WS_HSCROLL | LVS_SHOWSELALWAYS;
	long dwStyleEx = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;

	m_Playlist->Create( dwStyle, CRect( 0, 0, 0, 0 ), this, 123 );
	m_Playlist->SetExtendedStyle( dwStyleEx );

	m_Playlist->ModifyStyleEx( dwStyleEx | WS_EX_STATICEDGE, 0 );

	return 0;
}

///////////////////////////////////////////////////

void CMusikPlaylistView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Playlist->MoveWindow( 4, 4, cx - 8, cy - 10 );
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
	pDC.FillSolidRect( rcWindow, GetSysColor( COLOR_BTNFACE ) );

	// draw a simple border
	CRect rcBorder = rcWindow;
	rcBorder.left += 3;
	rcBorder.top += 3;
	rcBorder.right -= 3;
	rcBorder.bottom -= 5;
	pDC.Draw3dRect( rcBorder, GetSysColor( COLOR_BTNSHADOW ), GetSysColor( COLOR_BTNHILIGHT ) );

	CRect rcBottom;
	rcBottom.left = 0;
	rcBottom.right = rcClient.Width();
	rcBottom.top = rcClient.Height() - 2;
	rcBottom.bottom = rcClient.Height();
	pDC.Draw3dRect( rcBottom, GetSysColor( COLOR_BTNSHADOW ), GetSysColor( COLOR_BTNHILIGHT ) );

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
	