// MusikPlaylistView.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPlaylistView.h"
#include ".\musikplaylistview.h"


// CMusikPlaylistView

IMPLEMENT_DYNAMIC(CMusikPlaylistView, CWnd)
CMusikPlaylistView::CMusikPlaylistView()
{
}

CMusikPlaylistView::~CMusikPlaylistView()
{
}


BEGIN_MESSAGE_MAP(CMusikPlaylistView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMusikPlaylistView message handlers


int CMusikPlaylistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	m_Playlist.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT, CRect( 0, 0, 0, 0 ), this, 123 );

	return 0;
}

void CMusikPlaylistView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Playlist.MoveWindow( 0, 0, cx, cy );
}
