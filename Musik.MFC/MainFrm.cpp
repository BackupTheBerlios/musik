// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Musik.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/StdString.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	InitPaths();
	m_Library = new CMusikLibrary( ( CStdString )m_Database );
	//m_hIcon16 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICON16 ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	//m_hIcon32 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICON32 ), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );
}

CMainFrame::~CMainFrame()
{
	delete m_Library;

	for ( int i = 0; i < 4; i++ )
		delete m_wndSelectionBars[i];

	delete m_wndView;
}

void CMainFrame::InitPaths()
{
	char buffer[2000];
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );
	m_UserDir = buffer;
	m_UserDir += _T( "\\.Musik\\" );

	m_Database = m_UserDir + _T( "musiklib.db" );
}

void CMainFrame::DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf )
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	//-------------------------------------------------//
	//--- create a background window				---//
	//-------------------------------------------------//
	m_wndView = new CMusikPlaylistView( m_Library );
	m_wndView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL );
	m_wndView->ModifyStyleEx( WS_EX_STATICEDGE, NULL );
	m_wndView->ModifyStyle( WS_BORDER, 0 );
    EnableDocking( CBRS_ALIGN_BOTTOM );
	EnableDocking( CBRS_ALIGN_LEFT );
    EnableDocking( CBRS_ALIGN_RIGHT );
	EnableDocking( CBRS_ALIGN_TOP );

	//-------------------------------------------------//
	//--- now playing control						---//
	//-------------------------------------------------//
	m_wndNowPlaying.Create( _T( "Musik Now Playing" ), this, 123 );
	m_wndNowPlaying.SetBarStyle( m_wndNowPlaying.GetBarStyle() | CBRS_TOOLTIPS /*| CBRS_FLYBY*/ | CBRS_SIZE_DYNAMIC & ~SCBS_EDGEALL );
	m_wndNowPlaying.EnableDocking( CBRS_ALIGN_BOTTOM );
	m_wndNowPlaying.ShowGripper( false );
	DockControlBar( &m_wndNowPlaying, AFX_IDW_DOCKBAR_BOTTOM );

	//-------------------------------------------------//
	//--- selection controls						---//
	//-------------------------------------------------//
	for ( size_t i = 0; i < 4; i++ )
	{
		m_wndSelectionBars[i] = new CMusikSelectionBar( m_Library, i );
		m_wndSelectionBars[i]->Create( _T( "Musik Selection Box" ), this, 123 );
		m_wndSelectionBars[i]->SetBarStyle( m_wndSelectionBars[i]->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
		m_wndSelectionBars[i]->EnableDocking( CBRS_ALIGN_ANY );
		if ( i == 0 )
			DockControlBar( m_wndSelectionBars[i] );
		else
			DockBarLeftOf( m_wndSelectionBars[i], m_wndSelectionBars[i-1] );
	}

	//-------------------------------------------------//
	//--- sources control							---//
	//-------------------------------------------------//
	m_wndSourcesBar.Create( _T( "Musik Sources" ), this, 123 );
	m_wndSourcesBar.EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );
    m_wndSourcesBar.SetBarStyle( m_wndSourcesBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	DockControlBar( &m_wndSourcesBar, AFX_IDW_DOCKBAR_LEFT );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	CRect client_size;
	GetClientRect( &client_size );

	return TRUE;
}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

}
