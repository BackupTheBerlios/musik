///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikPlaylistView.h"
#include "MusikPrefs.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikArrays.h"
#include "../Musik.Core/include/MusikPlayer.h"
#include "../Musik.Core/include/MusikBatchAdd.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikPlaylistView, CWnd)

///////////////////////////////////////////////////

int WM_BATCHADD_PROGRESS	= RegisterWindowMessage( "BATCHADD_PROGRESS" );
int WM_BATCHADD_END			= RegisterWindowMessage( "BATCHADD_END" );

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikPlaylistView, CWnd)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DROPFILES()

	// custom messages
	ON_REGISTERED_MESSAGE( WM_BATCHADD_PROGRESS, OnBatchAddProgress )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_END, OnBatchAddEnd )
END_MESSAGE_MAP()

///////////////////////////////////////////////////
CMusikPlaylistView::CMusikPlaylistView( CFrameWnd* mainwnd, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs )
{
	m_Playlist = new CMusikPlaylistCtrl( mainwnd, library, player, prefs );

	m_Library = library;
	m_Player = player;
	m_Parent = mainwnd;
	m_Prefs = prefs;

	// batch add thread
	m_BatchAddThr = NULL;
	m_BatchAddFnct = NULL;
}

///////////////////////////////////////////////////

CMusikPlaylistView::~CMusikPlaylistView()
{
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

	DragAcceptFiles( true );

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

void CMusikPlaylistView::OnDropFiles(HDROP hDropInfo)
{
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	CStdStringArray* files = NULL;

	// if the thread exists, pause it
	if ( m_BatchAddThr )
		m_BatchAddThr->Pause();

	// otherwise create a new list of files
	else
		files = new CStdStringArray();

	for ( size_t i = 0; i < nNumFiles; i++ )
	{
		if ( DragQueryFile( hDropInfo, i, szNextFile, MAX_PATH ) > 0 )
		{
			if ( m_BatchAddThr )
				m_BatchAddThr->m_Files->push_back( szNextFile );
			else
				files->push_back( szNextFile );
		}
	}

	DragFinish( hDropInfo );

	// if thread exists, resume it.
	if ( m_BatchAddThr )
		m_BatchAddThr->Resume();
	else
	{
		m_BatchAddFnct = new CMusikBatchAddFunctor( this );
		m_BatchAddThr = new CMusikBatchAdd( files, m_Player->GetPlaylist(), m_Library, m_BatchAddFnct );
		m_BatchAddThr->Run();
	}
}

///////////////////////////////////////////////////

LRESULT CMusikPlaylistView::OnBatchAddProgress( WPARAM wParam, LPARAM lParam )
{

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMusikPlaylistView::OnBatchAddEnd( WPARAM wParam, LPARAM lParam )
{
	if ( m_BatchAddThr )
	{
		delete m_BatchAddThr;
		m_BatchAddThr = NULL;
	}

	if ( m_BatchAddFnct )
	{
		delete m_BatchAddFnct;
		m_BatchAddFnct = NULL;
	}

	int WM_SELBOXRESET = RegisterWindowMessage( "SELBOXRESET" );
	m_Parent->PostMessage( WM_SELBOXRESET );

	return 0L;
}

///////////////////////////////////////////////////

