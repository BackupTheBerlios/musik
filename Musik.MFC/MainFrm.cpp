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

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

int WM_SELBOXUPDATE = RegisterWindowMessage( "SELBOXUPDATE" );
int WM_SONGCHANGE = RegisterWindowMessage( "SONGCHANGE" );

int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage( "SOURCESDYNDPLAYLIST" );

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE( WM_SELBOXUPDATE, OnUpdateSel )
	ON_REGISTERED_MESSAGE( WM_SONGCHANGE, OnSongChange )
	ON_REGISTERED_MESSAGE( WM_SOURCESLIBRARY, OnSourcesLibrary )
	ON_REGISTERED_MESSAGE( WM_SOURCESSTDPLAYLIST, OnSourcesStdPlaylist )
	ON_REGISTERED_MESSAGE( WM_SOURCESDYNPLAYLIST, OnSourcesDynPlaylist )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMainFrame::CMainFrame()
{
	InitPaths();
	InitMusik();

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
	m_LibPlaylist	= new CMusikPlaylist();	
	m_Library		= new CMusikLibrary( ( CStdString )m_Database );
	m_Prefs			= new CMusikPrefs( m_PrefsIni );

	m_Player		= new CMusikPlayer( m_NewSong, m_Library, m_LibPlaylist );
	m_Player->InitSound( m_Prefs->GetPlayerDevice(), m_Prefs->GetPlayerDriver(), m_Prefs->GetPlayerRate(), m_Prefs->GetPlayerMaxChannels() );
	
	if ( m_Prefs->IsCrossfaderEnabled() )
		m_Player->SetCrossfader( m_Prefs->GetDefaultCrossfader() );
}

///////////////////////////////////////////////////

void CMainFrame::CleanMusik()
{
	if ( m_Library )		delete m_Library;
	if ( m_Prefs )			delete m_Prefs;
	if ( m_Player )			delete m_Player;
	if ( m_NewSong )		delete m_NewSong;
	if ( m_LibPlaylist )	delete m_LibPlaylist;
	if ( m_DynPlaylist )	delete m_DynPlaylist;
	if ( m_StdPlaylist )	delete m_StdPlaylist;
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
	m_wndView = new CMusikPlaylistView( m_Library, m_Player, m_Prefs, m_LibPlaylist );
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

	// sources control
	m_wndSources = new CMusikSourcesBar( this, m_Library );
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

CStdString CMainFrame::ParseCmd( CStdString sStr )
{
	if ( sStr.IsEmpty() )
		sStr = GetCommandLine();

	int nLastQ = sStr.ReverseFind( "\"", sStr.GetLength() - 1 );
	int nFirstQ = sStr.ReverseFind( "\"", nLastQ - 1 );

	sStr = sStr.Left( nLastQ );
	sStr = sStr.Right( nLastQ - ( nFirstQ + 1 ) );

	if ( !sStr.IsEmpty() )
	{
		CMusikFilename MFN ( sStr );
		CStdString sExt = MFN.GetExtension();

		if ( sExt != "mp3" && sExt != "ogg" )
			return _T( "" );
	}

	return sStr;
}

///////////////////////////////////////////////////

bool CMainFrame::PlayCmd( const CStdString& cmd )
{
	CStdString fn = ParseCmd( cmd );

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
			if ( song.GetID() > 0 )
			{
				pPlaylist->Add( song );
				m_wndView->GetCtrl()->UpdateV();

				// set the player's playlist, and play
				if ( m_Player )
				{
					m_Player->SetPlaylist( pPlaylist );
					m_Player->Play( pPlaylist->GetCount() - 1, MUSIK_CROSSFADER_NEW_SONG );
				}

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

	m_Library->GetRelatedSongs( sSender, pSender->GetType(), *m_LibPlaylist );
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongChange( WPARAM wParam, LPARAM lParam )
{
	if ( m_Player->IsPlaying() )
	{
		CString s;
		s.Format( _T( "Musik [ %s - %s ]" ), 
			m_Player->GetCurrPlaying()->GetTitle().c_str(), 
			m_Player->GetCurrPlaying()->GetArtist().c_str() );

		SetWindowText( s );
		
		m_wndNowPlaying->GetCtrl()->UpdateInfo();
		m_wndView->GetCtrl()->RedrawWindow();
	}
	else
		SetWindowText( _T( "Musik" ) );	

	return 0L;
}	

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesLibrary( WPARAM wParam, LPARAM lParam )
{

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam )
{

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesDynPlaylist( WPARAM wParam, LPARAM lParam )
{

	return 0L;
}

///////////////////////////////////////////////////
