// MusikPlaylistView.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikPlaylistView.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikArrays.h"
#include "MusikPrefs.h"

// CMusikPlaylistView

IMPLEMENT_DYNAMIC(CMusikPlaylistView, CWnd)
CMusikPlaylistView::CMusikPlaylistView( CMusikLibrary* library, CMusikPrefs* prefs, CMusikPlaylist* playlist )
{
	m_Playlist = new CMusikPlaylistCtrl( library, prefs, playlist );
}

CMusikPlaylistView::~CMusikPlaylistView()
{
	delete m_Playlist;
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

	long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA;
	long dwStyleEx = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;
	
	m_Playlist->Create( dwStyle, CRect( 0, 0, 0, 0 ), this, 123 );
	m_Playlist->SetExtendedStyle( dwStyleEx );

	return 0;
}

void CMusikPlaylistView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Playlist->MoveWindow( 0, 0, cx, cy );
}
