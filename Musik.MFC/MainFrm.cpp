///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
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
// Class(s): 
//
//   CMainFrame,
//
// Filename(s): 
//
//   MainFrm.h, MainFrm.cpp
//
// Information:
//
//   Musik.MFC's main SDI window.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MainFrm.h"
#include "MainFrmFunctor.h"

#include "../Musik.Core/include/StdString.h"
#include "../Musik.Core/include/MusikLibrary.h"
#include "../Musik.Core/include/MusikPlayer.h"
#include "../Musik.Core/include/MusikFilename.h"

#include <io.h>
#include <Direct.h>
#include ".\mainfrm.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

int WM_SELBOXUPDATE			= RegisterWindowMessage( "SELBOXUPDATE" );
int WM_SELBOXRESET			= RegisterWindowMessage( "SELBOXRESET" );

int MW_NEWPLAYLISTOWNER		= RegisterWindowMessage( "NEWPLAYLISTOWNER" );

int WM_SONGCHANGE			= RegisterWindowMessage( "SONGCHANGE" );
int WM_SONGSTOP				= RegisterWindowMessage( "SONGSTOP" );

int WM_DRAGSTART			= RegisterWindowMessage( "DRAGSTART" );
int WM_DRAGEND				= RegisterWindowMessage( "DRAGEND" );

int WM_SOURCESLIBRARY		= RegisterWindowMessage( "SOURCESLIBRARY" );
int WM_SOURCESSTDPLAYLIST	= RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
int WM_SOURCESDYNPLAYLIST	= RegisterWindowMessage( "SOURCESDYNDPLAYLIST" );

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_DESTROY()

	// custom message maps
	ON_REGISTERED_MESSAGE( WM_SELBOXUPDATE, OnUpdateSel )
	ON_REGISTERED_MESSAGE( WM_SONGCHANGE, OnSongChange )
	ON_REGISTERED_MESSAGE( WM_SONGSTOP, OnSongStop )
	ON_REGISTERED_MESSAGE( WM_SOURCESLIBRARY, OnSourcesLibrary )
	ON_REGISTERED_MESSAGE( WM_SOURCESSTDPLAYLIST, OnSourcesStdPlaylist )
	ON_REGISTERED_MESSAGE( WM_SOURCESDYNPLAYLIST, OnSourcesDynPlaylist )
	ON_REGISTERED_MESSAGE( WM_DRAGSTART, OnDragStart )
	ON_REGISTERED_MESSAGE( WM_DRAGEND, OnDragEnd )
	ON_REGISTERED_MESSAGE( MW_NEWPLAYLISTOWNER, OnNewPlaylistOwner )
	ON_REGISTERED_MESSAGE( WM_SELBOXRESET, OnSelBoxesReset )
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMainFrame::CMainFrame()
{
	InitPaths();
	InitMusik();

	m_CurPlaylist = NULL;
	m_DynPlaylist = NULL;
	m_StdPlaylist = NULL;

	m_hIcon16 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_MUSIK_16 ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	m_hIcon32 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_MUSIK_32 ), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );
}

///////////////////////////////////////////////////

CMainFrame::~CMainFrame()
{
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		delete m_wndSelectionBars[i];

	delete m_wndView;
	delete m_wndSources;
	delete m_wndNowPlaying;

	CleanMusik();
}

///////////////////////////////////////////////////

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


///////////////////////////////////////////////////

void CMainFrame::InitMusik()
{
	m_NewSong		= new CMusikFrameFunctor( this );
	m_SelPlaylist	= new CMusikPlaylist();	
	m_Library		= new CMusikLibrary( ( CStdString )m_Database );
	m_Prefs			= new CMusikPrefs( m_PrefsIni );
	m_Player		= new CMusikPlayer( m_NewSong, m_Library );
	m_Player->InitSound( m_Prefs->GetPlayerDevice(), m_Prefs->GetPlayerDriver(), m_Prefs->GetPlayerRate(), m_Prefs->GetPlayerMaxChannels() );
	
	if ( m_Prefs->IsCrossfaderEnabled() )
		m_Player->SetCrossfader( m_Prefs->GetDefaultCrossfader() );
}

///////////////////////////////////////////////////

void CMainFrame::CleanMusik()
{
	if ( m_Library )		delete m_Library;
	if ( m_Prefs )			delete m_Prefs;
	if ( m_SelPlaylist )	delete m_SelPlaylist;
	if ( m_DynPlaylist )	delete m_DynPlaylist;
	if ( m_StdPlaylist )	delete m_StdPlaylist;
	if ( m_Player )			delete m_Player;
	if ( m_NewSong )		delete m_NewSong;
}

///////////////////////////////////////////////////

void CMainFrame::LoadDlgSize()
{
	CSize dlg_size = m_Prefs->GetDlgSize();
	CPoint dlg_pos = m_Prefs->GetDlgPos();

	MoveWindow( dlg_pos.x, dlg_pos.y, dlg_size.cx, dlg_size.cy );

	if ( m_Prefs->IsMaximized() )
	{
		WINDOWPLACEMENT max;
		max.showCmd = SW_SHOWMAXIMIZED;
		SetWindowPlacement( &max );
		return;
	}
}

///////////////////////////////////////////////////

void CMainFrame::ResetUI()
{
	MoveWindow( 0, 0, 800, 600 );

	CSize size;
	
	// now playing
	size.cx = NULL;
	size.cy = 48;
	m_wndNowPlaying->SetSize( size );

	// sources
	size.cx = 160;
	size.cy = NULL;
	m_wndSources->SetSize( size );

	// selection box
	size.cx = NULL;
	size.cy = 160;
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		m_wndSelectionBars[i]->SetSize( size );
}

///////////////////////////////////////////////////

void CMainFrame::ResetSelBoxes()
{
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		m_wndSelectionBars[i]->GetCtrl()->UpdateV( true );
}

///////////////////////////////////////////////////

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

///////////////////////////////////////////////////

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

///////////////////////////////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	SetIcon( m_hIcon32, true );
	SetIcon( m_hIcon16, false );

	// create the background window, which is the playlist
	m_wndView = new CMusikPlaylistView( this, m_Library, m_Player, m_Prefs );
	m_wndView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL );
	m_wndView->ModifyStyleEx( WS_EX_STATICEDGE, NULL );
	m_wndView->ModifyStyle( WS_BORDER, 0 );
	m_wndView->GetCtrl()->UpdateV();
    EnableDocking( CBRS_ALIGN_BOTTOM );
	EnableDocking( CBRS_ALIGN_LEFT );
    EnableDocking( CBRS_ALIGN_RIGHT );
	EnableDocking( CBRS_ALIGN_TOP );

	// now playing control
	m_wndNowPlaying = new CMusikNowPlayingBar( m_Player, m_Prefs );
	m_wndNowPlaying->Create( _T( "Musik Now Playing" ), this, ID_NOWPLAYING );
	m_wndNowPlaying->SetBarStyle( m_wndNowPlaying->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndNowPlaying->EnableDocking( CBRS_ALIGN_BOTTOM );
	m_wndNowPlaying->ShowGripper( false );
	DockControlBar( m_wndNowPlaying, AFX_IDW_DOCKBAR_BOTTOM );

	// selection controls
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		m_wndSelectionBars[i] = new CMusikSelectionBar( this, m_Library, m_Prefs, i, i );
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

	// sources control
	m_wndSources = new CMusikSourcesBar( this, m_Library, m_Prefs );
	m_wndSources->Create( _T( "Sources" ), this, ID_SOURCESBOX );
	m_wndSources->EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );
    m_wndSources->SetBarStyle( m_wndSources->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	DockControlBar( m_wndSources, AFX_IDW_DOCKBAR_LEFT );

	// load dockbar sizes and positions
	if ( m_Prefs->ResetUI() )
	{
		ResetUI();
		m_Prefs->SetDlgResetUI( false );
	}
	else
	{
		LoadDlgSize();
		CSizingControlBar::GlobalLoadState( this, "MusikDockBars" );
	}

	return 0;
}

///////////////////////////////////////////////////

bool CMainFrame::PlayCmd( const CStdString& fn )
{
	if ( fn.IsEmpty() )
		return false;

	if ( m_Library )
	{
		// add song to library, if necessary
		if ( !m_Library->IsSongInLibrary( fn ) )
			m_Library->AddSong( fn );


		// get playlist's internal player
		CMusikPlaylist* pPlaylist = m_wndView->GetCtrl()->GetPlaylist();
		if ( pPlaylist )
		{
			// get the song we just added...
			CMusikSong song;
			m_Library->GetSongFromFilename( fn, song );

			// add to the control's playlist
			if ( song.GetID() >= 0 )
			{
				pPlaylist->Add( song );
				m_wndView->GetCtrl()->UpdateV();

				// set the player's playlist, and play
				if ( m_Player )
				{
					m_Player->SetPlaylist( pPlaylist );
					if ( !m_Player->IsPlaying() )
						m_Player->Play( pPlaylist->GetCount() - 1, MUSIK_CROSSFADER_NEW_SONG );
				}

				ResetSelBoxes();

				return true;
			}
		}
	}

	return false;
}

///////////////////////////////////////////////////

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

///////////////////////////////////////////////////

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

///////////////////////////////////////////////////

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

///////////////////////////////////////////////////

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

///////////////////////////////////////////////////

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	return TRUE;
}

///////////////////////////////////////////////////

void CMainFrame::OnDestroy()
{
	CSizingControlBar::GlobalSaveState( this, "MusikDockBars" );
	
	CRect rc_dlg;
	GetWindowRect( &rc_dlg );

	bool maximized = false;
	WINDOWPLACEMENT max;
	GetWindowPlacement( &max );

	if ( max.showCmd == SW_SHOWMAXIMIZED )
		maximized = true;
	else
		m_Prefs->SetDlgSize( CSize( rc_dlg.right - rc_dlg.left, rc_dlg.bottom - rc_dlg.top ) );

	m_Prefs->SetDlgPos( CPoint( rc_dlg.left, rc_dlg.top ) );
	m_Prefs->SetMaximized( maximized );

	CFrameWnd::OnDestroy();
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnUpdateSel( WPARAM wParam, LPARAM lParam )
{
	size_t selbox_count = m_Prefs->GetSelBoxCount();
	if ( selbox_count < 2 )
		return 0L;

	CMusikSelectionCtrl* pSender	= NULL;
	CMusikSelectionCtrl* pCurr		= NULL;
	CMusikSelectionCtrl* pParent	= NULL;

	int nSender = (int)wParam;

	// find the sender and parent
	for ( size_t i = 0; i < selbox_count; i++ )
	{
		pCurr = m_wndSelectionBars[i]->GetCtrl();
		if ( pCurr->GetCtrlID() == nSender )
			pSender = pCurr;
		if ( pCurr->IsParent() )
			pParent = pCurr;
	}

	// sender not found
	if ( !pSender )
		return 0L;

	// no parent found, so we have a new parent!
	if ( !pParent )
	{
		pParent = pSender;
		pParent->SetParent( true );
	}

	// if the first item was clicked, the user wants
	// to display all the info
	if ( pSender->IsItemSelected( 0 ) )
	{
		CMusikSelectionCtrl::SetUpdating( true );

		for( size_t i = 0; i < selbox_count; i++ )
		{
			pCurr = m_wndSelectionBars[i]->GetCtrl();
			pCurr->SetItemState( -1, 0, LVIS_SELECTED );
		}

		pSender->SetParent( false );
		pSender->UpdateV();

		CMusikSelectionCtrl::SetUpdating( false );

		return 0L;
	}	

	// get a list of the sender's selected items
	CStdStringArray sender_sel;
	pSender->GetSelItems( sender_sel );

	// construct first part of query... this is basically
	// a list of all the selected items from the control
	// that triggered the event
	CStdString sSender = pSender->GetSelQuery();
	if ( pSender != pParent )
	{
		for ( size_t i = 0; i < selbox_count; i++ )
		{
			pCurr = m_wndSelectionBars[i]->GetCtrl();
			if ( pCurr != pSender && pCurr->GetSelectedCount() )
			{
				sSender += _T( " and " );
				sSender += pCurr->GetSelQuery();
			}
		}
	}

	// go through each box that isn't the sender and
	// update based on the new information we got
	CMusikSelectionCtrl::SetUpdating( true );
	for( size_t i = 0; i < selbox_count; i++ )
	{
		pCurr = m_wndSelectionBars[i]->GetCtrl();

		if ( pCurr != pSender && pCurr != pParent )
			pCurr->UpdateV( sSender );
	}
	CMusikSelectionCtrl::SetUpdating( false );

	// get the songs
	m_Library->GetRelatedSongs( sSender, pSender->GetType(), *m_SelPlaylist );

	// make sure the correct playlist is set
	if ( m_wndView->GetCtrl()->GetPlaylist() != m_SelPlaylist )
	{
		// delete any old playlists we don't
		// need in memory any more
		CleanPlaylists();

		// make sure our current playlist is
		// correct, so the playlist can let
		// us know the new address of it once
		// the CMusikPlayer takes ownership
		m_CurPlaylist = m_SelPlaylist;
		m_wndView->GetCtrl()->SetPlaylist( m_SelPlaylist );

		// deselect any sources item
		m_wndSources->GetCtrl()->KillFocus();
	}

	// update the window
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongChange( WPARAM wParam, LPARAM lParam )
{
	if ( m_Player->IsPlaying() )
	{
		CString s;
		s.Format( _T( "%s [ %s - %s ]" ), 
			MUSIK_VERSION_STR,
			m_Player->GetCurrPlaying()->GetTitle().c_str(), 
			m_Player->GetCurrPlaying()->GetArtist().c_str() );

		SetWindowText( s );
		
		m_wndNowPlaying->GetCtrl()->UpdateInfo();
		m_wndView->GetCtrl()->RedrawWindow();
	}
	else
		SetWindowText( MUSIK_VERSION_STR );	

	return 0L;
}	

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongStop( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "Song stop signal caught\n" );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesLibrary( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A library or device was clicked\n" );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A standard playlist was clicked\n" );

	// make sure our current playlist is
	// correct, so the playlist can let
	// us know the new address of it once
	// the CMusikPlayer takes ownership
	m_CurPlaylist = m_StdPlaylist;

	CleanPlaylists();
	m_StdPlaylist = new CMusikPlaylist();

	int nID = m_wndSources->GetCtrl()->GetFocusedItem()->GetPlaylistID();
	m_Library->GetStdPlaylist( nID, *m_StdPlaylist, true );

	m_wndView->GetCtrl()->SetPlaylist( m_StdPlaylist );
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesDynPlaylist( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A dynamic playlist was clicked\n" );

	
	//m_CurPlaylist = m_DynPlaylist;

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnDragStart( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "DND Started.\n" );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnDragEnd( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "DND Ended.\n" );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnNewPlaylistOwner( WPARAM wParam, LPARAM lParam )
{
	if ( m_CurPlaylist == m_SelPlaylist )
		m_SelPlaylist = (CMusikPlaylist*)wParam;

	else if ( m_CurPlaylist == m_DynPlaylist )
		m_DynPlaylist = (CMusikPlaylist*)wParam;

	else if ( m_CurPlaylist == m_StdPlaylist )
		m_StdPlaylist = (CMusikPlaylist*)wParam;

	else
		TRACE0( "No previous playlist found. Musik will crash at some point.\n" );

	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::CleanPlaylists()
{
	if ( m_StdPlaylist )
	{
		delete m_StdPlaylist;
		m_StdPlaylist = NULL;
	}

	if ( m_DynPlaylist )
	{
		delete m_DynPlaylist;
		m_DynPlaylist = NULL;
	}
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSelBoxesReset( WPARAM wParam, LPARAM lParam )
{
	ResetSelBoxes();
	return 0L;
}

///////////////////////////////////////////////////

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	return false;
}


///////////////////////////////////////////////////

void CMainFrame::OnPaint()
{
	CPaintDC dc(this);
}

///////////////////////////////////////////////////

void CMainFrame::OnSysColorChange()
{
	CFrameWnd::OnSysColorChange();

	m_Prefs->ThemeChanged();
	Invalidate();
}
