// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Musik.h"

#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/StdString.h"

#include <io.h>
#include <Direct.h>

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int WM_SELBOXUPDATE = RegisterWindowMessage( "SELBOXUPDATE" );

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE( WM_SELBOXUPDATE, OnUpdateSel )
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	InitPaths();

	m_Library = new CMusikLibrary( ( CStdString )m_Database );
	m_Prefs = new CMusikPrefs( m_PrefsIni );

	//m_hIcon16 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICON16 ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	//m_hIcon32 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICON32 ), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );
}

CMainFrame::~CMainFrame()
{
	for ( int i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		delete m_wndSelectionBars[i];
	delete m_wndView;
	delete m_wndSources;
	delete m_wndNowPlaying;

	delete m_Library;
	delete m_Prefs;
}

void CMainFrame::InitPaths()
{
	char buffer[2000];
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );
	m_UserDir = buffer;
	m_UserDir += _T( "\\.Musik\\" );

	RecurseMkDir( m_UserDir.GetBuffer() );

	m_Database = m_UserDir + _T( "musiklib.db" );
	m_PrefsIni = m_UserDir + _T( "musikprefs.ini" );
}

void CMainFrame::ResetDialogRect()
{
	CSize dlg_size = m_Prefs->GetDlgSize();
	CPoint dlg_pos = m_Prefs->GetDlgPos();

	MoveWindow( dlg_pos.x, dlg_pos.y, dlg_size.cx, dlg_size.cy );
}

void CMainFrame::ResetNowPlaying()
{
	CRect rc;
	GetClientRect( &rc );
	
	//m_wndNowPlaying.SetWindowPos( this, 0, rc.Height() - m_Prefs->GetNowPlayingHeight(), rc.Width(), m_Prefs->GetNowPlayingHeight(), NULL );
}

bool CMainFrame::RecurseMkDir( char* pszDir )
{
    char*   pszLastSlash;
    char    cTmp;

	if( _access( pszDir, 0 ) != -1 )
        return true;

    pszLastSlash = strrchr( pszDir, '\\' );
    if ( pszLastSlash )
    {
        cTmp = *pszLastSlash;
        *pszLastSlash = '\0';

        RecurseMkDir( pszDir );

        *pszLastSlash = cTmp;
    }

    if ( _mkdir( pszDir ) == -1 )
		return false;

    return true;
}

void CMainFrame::DockBarLeftOf( CSizingControlBar* pBar, CSizingControlBar* pTargetBar )
{
    ASSERT(pBar != NULL);
    ASSERT(pTargetBar != NULL);
    ASSERT(pBar != pTargetBar);

    // the neighbour must be already docked
    CDockBar* pDockBar = pTargetBar->m_pDockBar;
    ASSERT(pDockBar != NULL);
    UINT nDockBarID = pTargetBar->m_pDockBar->GetDlgCtrlID();
    ASSERT(nDockBarID != AFX_IDW_DOCKBAR_FLOAT);

    bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

    // dock normally (inserts a new row)
    DockControlBar(pBar, nDockBarID);

    // delete the new row (the bar pointer and the row end mark)
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);

    // find the target bar
    for (int i = 0; i < pDockBar->m_arrBars.GetSize(); i++)
    {
        void* p = pDockBar->m_arrBars[i];
        if (p == pTargetBar) // and insert the new bar after it
            pDockBar->m_arrBars.InsertAt(i + 1, pBar);
    }

    // move the new bar into position
    CRect rBar;
    pTargetBar->GetWindowRect(rBar);
    rBar.OffsetRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
    pBar->MoveWindow(rBar);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	//-------------------------------------------------//
	//--- create a background window				---//
	//-------------------------------------------------//
	m_wndView = new CMusikPlaylistView( m_Library, m_Prefs );
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
	m_wndNowPlaying = new CMusikNowPlayingBar();
	m_wndNowPlaying->Create( _T( "Musik Now Playing" ), this, ID_NOWPLAYING );
	m_wndNowPlaying->SetBarStyle( m_wndNowPlaying->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC & ~SCBS_EDGEALL );
	m_wndNowPlaying->EnableDocking( CBRS_ALIGN_BOTTOM );
	m_wndNowPlaying->ShowGripper( false );
	DockControlBar( m_wndNowPlaying, AFX_IDW_DOCKBAR_BOTTOM );

	//-------------------------------------------------//
	//--- selection controls						---//
	//-------------------------------------------------//
	for ( int i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		m_wndSelectionBars[i] = new CMusikSelectionBar( this, m_Library, i, i );
		m_wndSelectionBars[i]->Create( _T( "Musik Selection Box" ), this, ID_SELECTIONBOX_START + i );
		m_wndSelectionBars[i]->SetBarStyle( m_wndSelectionBars[i]->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
		m_wndSelectionBars[i]->EnableDocking( CBRS_ALIGN_ANY );
		if ( i == 0 )
		{
			DockControlBar( m_wndSelectionBars[i] );
		}
		else
			DockBarLeftOf( m_wndSelectionBars[i], m_wndSelectionBars[i-1] );
	}

	//-------------------------------------------------//
	//--- sources control							---//
	//-------------------------------------------------//
	m_wndSources = new CMusikSourcesBar();
	m_wndSources->Create( _T( "Musik Sources" ), this, ID_SOURCESBOX );
	m_wndSources->EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );
    m_wndSources->SetBarStyle( m_wndSources->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	DockControlBar( m_wndSources, AFX_IDW_DOCKBAR_LEFT );

	//-------------------------------------------------//
	//--- rescale dialog based on prefs				---//
	//-------------------------------------------------//
	ResetDialogRect();

	//-------------------------------------------------//
	//--- load dockbar sizes and positions			---//
	//-------------------------------------------------//
	CSizingControlBar::GlobalLoadState( this, "MusikDockBars" );

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
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	return TRUE;
}

void CMainFrame::OnDestroy()
{
	CSizingControlBar::GlobalSaveState( this, "MusikDockBars" );
	
	CRect rc_dlg;
	GetWindowRect( &rc_dlg );

	m_Prefs->SetDlgSize( CSize( rc_dlg.right - rc_dlg.left, rc_dlg.bottom - rc_dlg.top ) );
	m_Prefs->SetDlgPos( CPoint( rc_dlg.left, rc_dlg.top ) );

	CFrameWnd::OnDestroy();
}

LRESULT CMainFrame::OnUpdateSel( WPARAM wParam, LPARAM lParam )
{
	size_t selbox_count = m_Prefs->GetSelBoxCount();
	if ( selbox_count < 2 )
		return 0L;

	CMusikSelectionCtrl* pSender = NULL;
	CMusikSelectionCtrl* pCurr = NULL;

	int nSender = wParam;

	// find the sender
	for ( size_t i = 0; i < selbox_count; i++ )
	{
		pCurr = m_wndSelectionBars[i]->GetCtrl();
		if ( pCurr->GetCtrlID() == nSender )
			pSender = pCurr;
	}

	// sender not found
	if ( !pSender )
		return 0L;

	// get a list of the sender's selected items
	CStdStringArray sender_sel;
	pSender->GetSelItems( sender_sel );

	// construct first part of query... this is basically
	// a list of all the selected items from the control
	// that triggered the event
	CStdString sSender = pSender->GetSelQuery();

	// go through each box that isn't the sender and
	// update based on the new information we got
	for( size_t i = 0; i < selbox_count; i++ )
	{
		pCurr = m_wndSelectionBars[i]->GetCtrl();
		if ( pCurr != pSender )
		{
			pCurr->SetUpdating( true );			
			pCurr->UpdateV( sSender );
			pCurr->SetUpdating( false );
		}
	}

	return 0L;
}