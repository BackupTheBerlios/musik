///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
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
//   musikCube's main SDI window.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include <shlobj.h>   

#include "musik.h"
#include "MainFrm.h"
#include "MainFrmFunctor.h"
#include "musikBatchAddFunctor.h"
#include "musikRemoveOldFunctor.h"
#include "musikFileDialog.h"
#include "musikTimeCtrl.h"

#include "../musikCore/include/StdString.h"
#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"
#include "../musikCore/include/musikFilename.h"
#include "../musikCore/include/musikBatchAdd.h"
#include "../musikCore/include/musikRemoveOld.h"
#include "../musikCore/include/musikCrossfader.h"
#include "../musikCore/include/musikEQSettings.h"

#include <io.h>
#include <Direct.h>

#include "3rdparty/TreePropSheet.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

int WM_SELBOXUPDATE			= RegisterWindowMessage( "SELBOXUPDATE" );
int WM_SELBOXRESET			= RegisterWindowMessage( "SELBOXRESET" );

int WM_PLAYERNEWPLAYLIST	= RegisterWindowMessage( "PLAYERNEWPLAYLIST" );

int WM_DRAGSTART			= RegisterWindowMessage( "DRAGSTART" );
int WM_DRAGEND				= RegisterWindowMessage( "DRAGEND" );

int WM_SOURCESLIBRARY		= RegisterWindowMessage( "SOURCESLIBRARY" );
int WM_SOURCESNOWPLAYING	= RegisterWindowMessage( "SOURCESNOWPLAYING" );
int WM_SOURCESSTDPLAYLIST	= RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
int WM_SOURCESDYNPLAYLIST	= RegisterWindowMessage( "SOURCESDYNDPLAYLIST" );

// we get these ones from the player
// via a CmusikFrmFunctor after a
// certain operation as completed
int WM_SONGCHANGE			= RegisterWindowMessage( "SONGCHANGE" );
int WM_SONGSTOP				= RegisterWindowMessage( "SONGSTOP" );
int WM_SONGPAUSE			= RegisterWindowMessage( "SONGPAUSERESUME" );

// these will come from all over the UI,
// such as the now playing control and 
// task tray button. they are commands to
// send the player.
int WM_PLAYER_PLAYSEL		= RegisterWindowMessage( "PLAYER_PLAYSEL" );
int WM_PLAYER_NEXT			= RegisterWindowMessage( "PLAYER_NEXT" );
int WM_PLAYER_PREV			= RegisterWindowMessage( "PLAYER_PREV" );
int WM_PLAYER_STOP			= RegisterWindowMessage( "PLAYER_STOP" );

// comes from other child controls when
// files have been dropped
int WM_BATCHADD_NEW			= RegisterWindowMessage( "BATCHADD_NEW" );

// come from the batch add functor
int WM_BATCHADD_PROGRESS	= RegisterWindowMessage( "BATCHADD_PROGRESS" );
int WM_BATCHADD_END			= RegisterWindowMessage( "BATCHADD_END" );

// come from the RemoveOld functor
int WM_BATCHADD_VERIFY_PLAYLIST	= RegisterWindowMessage( "BATCHADD_VERIFY_PLAYLIST" );
int WM_REMOVEOLD_PROGRESS		= RegisterWindowMessage( "REMOVEOLD_PROGRESS" );
int WM_REMOVEOLD_END			= RegisterWindowMessage( "REMOVEOLD_END" );

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()

	// menu 
	ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
	ON_COMMAND(ID_OPEN_FILES, OnOpenFiles)
	ON_COMMAND(ID_OPEN_DIRECTORY, OnOpenDirectory)
	ON_COMMAND(ID_FILE_SAVEPLAYLIST, OnFileSaveplaylist)
	ON_COMMAND(ID_VIEW_PLAYLISTINFORMATION, OnViewPlaylistinformation)
	ON_COMMAND(ID_VIEW_SOURCES, OnViewSources)
	ON_COMMAND(ID_VIEW_SELECTIONBOXES, OnViewSelectionboxes)
	ON_COMMAND(ID_VIEW_NOWPLAYING, OnViewNowplaying)
	ON_COMMAND(ID_AUDIO_EQUALIZER_ENABLED, OnAudioEqualizerEnabled)
	ON_COMMAND(ID_AUDIO_CROSSFADER_ENABLED, OnAudioCrossfaderEnabled)
	ON_COMMAND(ID_PLAYBACKMODE_REPEATSINGLE, OnPlaybackmodeRepeatsingle)
	ON_COMMAND(ID_PLAYBACKMODE_REPEATPLAYLIST, OnPlaybackmodeRepeatplaylist)
	ON_COMMAND(ID_PLAYBACKMODE_INTRO, OnPlaybackmodeIntro)

	// update ui
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOURCES, OnUpdateViewSources)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECTIONBOXES, OnUpdateViewSelectionboxes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NOWPLAYING, OnUpdateViewNowplaying)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_EQUALIZER_ENABLED, OnUpdateAudioEqualizerEnabled)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_CROSSFADER_ENABLED, OnUpdateAudioCrossfaderEnabled)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLAYLISTINFORMATION, OnUpdateViewPlaylistinformation)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEPLAYLIST, OnUpdateFileSaveplaylist)
	ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_REPEATSINGLE, OnUpdatePlaybackmodeRepeatsingle)
	ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_REPEATPLAYLIST, OnUpdatePlaybackmodeRepeatplaylist)
	ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_INTRO, OnUpdatePlaybackmodeIntro)

	// custom message maps
	ON_REGISTERED_MESSAGE( WM_SELBOXUPDATE, OnUpdateSel )
	ON_REGISTERED_MESSAGE( WM_SONGCHANGE, OnSongChange )
	ON_REGISTERED_MESSAGE( WM_SONGSTOP, OnSongStop )
	ON_REGISTERED_MESSAGE( WM_SONGPAUSE, OnSongPauseResume )
	ON_REGISTERED_MESSAGE( WM_SOURCESLIBRARY, OnSourcesLibrary )
	ON_REGISTERED_MESSAGE( WM_SOURCESNOWPLAYING, OnSourcesNowPlaying )
	ON_REGISTERED_MESSAGE( WM_SOURCESSTDPLAYLIST, OnSourcesStdPlaylist )
	ON_REGISTERED_MESSAGE( WM_SOURCESDYNPLAYLIST, OnSourcesDynPlaylist )
	ON_REGISTERED_MESSAGE( WM_DRAGSTART, OnDragStart )
	ON_REGISTERED_MESSAGE( WM_DRAGEND, OnDragEnd )
	ON_REGISTERED_MESSAGE( WM_PLAYERNEWPLAYLIST, OnPlayerNewPlaylist )
	ON_REGISTERED_MESSAGE( WM_SELBOXRESET, OnSelBoxesReset )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_NEW, OnBatchAddNew )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_PROGRESS, OnBatchAddProgress )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_END, OnThreadEnd )
	ON_REGISTERED_MESSAGE( WM_REMOVEOLD_PROGRESS, OnRemoveOldProgress )
	ON_REGISTERED_MESSAGE( WM_REMOVEOLD_END, OnThreadEnd )
	ON_REGISTERED_MESSAGE( WM_PLAYER_PLAYSEL, OnPlayerPlaySel )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_VERIFY_PLAYLIST, OnVerifyPlaylist )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMainFrame::CMainFrame()
{
	InitPaths();
	Initmusik();
	InitDragTypes();

	m_hIcon16 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_MUSIK_16 ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	m_hIcon32 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_MUSIK_32 ), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );
}

///////////////////////////////////////////////////

CMainFrame::~CMainFrame()
{
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		if ( m_wndSelectionBars[i] )
			delete m_wndSelectionBars[i];
	}

	if ( m_wndView )
		delete m_wndView;

	if ( m_wndSources )
		delete m_wndSources;

	if ( m_wndNowPlaying )
		delete m_wndNowPlaying;

	Cleanmusik();
}

///////////////////////////////////////////////////

void CMainFrame::InitPaths()
{
	char buffer[2000];
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );
	m_UserDir = buffer;
	m_UserDir += _T( "\\.musik\\" );

	RecurseMkDir( m_UserDir.GetBuffer() );

	m_Database = m_UserDir + _T( "musiklib.db" );
	m_PrefsIni = m_UserDir + _T( "musikprefs.ini" );
}

///////////////////////////////////////////////////

void CMainFrame::InitDragTypes()
{
	m_uPlaylistDrop = RegisterClipboardFormat ( _T("musikPlaylist_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82") );
	m_uSourcesDrop = RegisterClipboardFormat( _T("musikSources_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82") );
	m_uSelectionDrop = RegisterClipboardFormat( _T("musikSelection_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82") );
}

///////////////////////////////////////////////////

void CMainFrame::Initmusik()
{
	m_NewSong		= new CmusikFrameFunctor( this );
	m_LibPlaylist	= NULL;	
	m_StdPlaylist	= NULL;
	m_BatchAddFnct	= new CmusikBatchAddFunctor( this );
	m_RemoveOldFnct	= new CmusikRemoveOldFunctor( this );
	m_Library		= new CmusikLibrary( ( CStdString )m_Database );
	m_Prefs			= new CmusikPrefs( m_PrefsIni );

	// setup the player...
	m_Player		= new CmusikPlayer( m_NewSong, m_Library );
	m_Player->SetMaxVolume( m_Prefs->GetPlayerVolume() );
	m_Player->InitSound( m_Prefs->GetPlayerDevice(), m_Prefs->GetPlayerDriver(), m_Prefs->GetPlayerRate(), m_Prefs->GetPlayerMaxChannels() );
	m_Player->SetPlaymode( m_Prefs->GetPlayerPlaymode() );

	// give player a crossfader, it will take
	// care of loading equalizer settings itself...
	if ( m_Prefs->IsCrossfaderEnabled() )
	{
		CmusikCrossfader fade;
		GetCrossfader( &fade );

		m_Player->EnableCrossfader();
		m_Player->SetCrossfader( fade );
	}

	// enable the equalizer...
	if ( m_Prefs->IsEqualizerEnabled() )
		m_Player->EnableEqualizer( true );

	// startup a thread in the background
	// to remove old files...
	if ( m_Prefs->PurgeOnStartup() )
	{
		CmusikRemoveOld* params = new CmusikRemoveOld( m_Library, m_RemoveOldFnct );
		CmusikThread* thread = new CmusikThread();
	
		thread->Start( (ACE_THR_FUNC)musikRemoveOldWorker, params );
	
		m_Threads.push_back( thread );
	}
}

///////////////////////////////////////////////////

void CMainFrame::Cleanmusik()
{
	// the threads need to be cleaned first becuase
	// they are attached to nearly everything: the 
	// player, library, etc, etc... so we need to:
	// suspend, set abort switch, and then
	// resume any remaning threads...
	for ( size_t i = 0; i < m_Threads.size(); i++ )
	{
		m_Threads.at( i )->Suspend( true );
		m_Threads.at( i )->Abort();
		m_Threads.at( i )->Resume();

		while ( !m_Threads.at( i )->m_Finished )
			Sleep( 50 );

		delete m_Threads.at( i );
	}

	if ( m_Library )	
	{
		delete m_Library;
		m_Library = NULL;
	}

	if ( m_Prefs )		
	{
		delete m_Prefs;
		m_Prefs = NULL;
	}

	if ( m_StdPlaylist && m_StdPlaylist != m_Player->GetPlaylist() )
	{
		delete m_StdPlaylist;
		m_StdPlaylist = NULL;
	}

	if ( m_Player )
	{
		delete m_Player;
		m_Player = NULL;
	}

	if ( m_NewSong )	
	{
		delete m_NewSong;
		m_NewSong = NULL;
	}

	if ( m_LibPlaylist )
	{
		delete m_LibPlaylist;
		m_LibPlaylist = NULL;
	}

	if ( m_BatchAddFnct )
	{
		delete m_BatchAddFnct;
		m_BatchAddFnct = NULL;
	}

	if ( m_RemoveOldFnct )
	{
		delete m_RemoveOldFnct;
		m_RemoveOldFnct = NULL;
	}
}

///////////////////////////////////////////////////

void CMainFrame::LoadDlgSize()
{
	CSize dlg_size = m_Prefs->GetDlgSize();
	CPoint dlg_pos = m_Prefs->GetDlgPos();

	CRect rcNormal = CRect( dlg_pos, dlg_size );
	
	if ( m_Prefs->IsMaximized() )
	{
		WINDOWPLACEMENT max;
		max.showCmd = SW_SHOWMAXIMIZED;
		max.rcNormalPosition = rcNormal;
		SetWindowPlacement( &max );
		return;
	}
	else
		MoveWindow( rcNormal );

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

	// set default dock order
	EnableDocking( CBRS_ALIGN_BOTTOM );
	EnableDocking( CBRS_ALIGN_LEFT );
	EnableDocking( CBRS_ALIGN_RIGHT );
	EnableDocking( CBRS_ALIGN_TOP );

	#ifdef _SCB_REPLACE_MINIFRAME
		m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
	#endif

	// create the background window, which is the playlist
	m_wndView = new CmusikPlaylistView( this, m_Library, m_Player, m_Prefs, m_uPlaylistDrop );
	m_wndView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL );
	m_wndView->GetCtrl()->UpdateV();

	// now playing control
	m_wndNowPlaying = new CmusikNowPlayingBar( this, m_Player, m_Prefs );
	m_wndNowPlaying->Create( _T( "musik Now Playing" ), this, ID_NOWPLAYING );
	DockControlBar( m_wndNowPlaying, AFX_IDW_DOCKBAR_BOTTOM );

	// selection controls
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		m_wndSelectionBars[i] = new CmusikSelectionBar( this, m_Library, m_Prefs, i, i, m_uSelectionDrop );
		m_wndSelectionBars[i]->Create( _T( "musik Selection Box" ), this, ID_SELECTIONBOX_START + i );
		if ( i == 0 )
			DockControlBar( m_wndSelectionBars[i] );
		else
			DockBarLeftOf( m_wndSelectionBars[i], m_wndSelectionBars[i-1] );
	}

	// sources control
	m_wndSources = new CmusikSourcesBar( this, m_Library, m_Player, m_Prefs, m_uSourcesDrop );
	m_wndSources->Create( _T( "Sources" ), this, ID_SOURCESBOX );
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

		CString sProfile = _T( "musikProfile" );
		if ( VerifyBarState( sProfile ) )
		{
			CSizingControlBar::GlobalLoadState( this, sProfile );
			LoadBarState( sProfile );
		}
	}

	return 0;
}

///////////////////////////////////////////////////

bool CMainFrame::PlayCmd( const CString& fn )
{
	if ( fn.IsEmpty() )
		return false;

	if ( m_Library )
	{
		// add song to library, if necessary
		m_Library->AddSong( (CStdString)fn );

		// get player's playlist
		CmusikPlaylist* pPlaylist = m_Player->GetPlaylist();
		if ( pPlaylist )
		{
			// get the song we just added...
			CmusikSong song;
			m_Library->GetSongFromFilename( (CStdString)fn, song );

			// add to the control's playlist
			if ( song.GetID() >= 0 )
			{
				pPlaylist->Add( song );
				m_wndView->GetCtrl()->UpdateV();

				// set the player's playlist, and play
				if ( m_Player )
				{
					if ( !m_Player->IsPlaying() )
						m_Player->Play( pPlaylist->GetCount() - 1, MUSIK_CROSSFADER_NEW_SONG );

					m_wndView->GetCtrl()->SetPlaylist( m_Player->GetPlaylist(), MUSIK_SOURCES_TYPE_NOWPLAYING );
					m_wndView->GetCtrl()->UpdateV();

					m_wndSources->GetCtrl()->FocusNowPlaying();
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

BOOL CMainFrame::DestroyWindow()
{
	CString sProfile = _T( "musikProfile" );
	CSizingControlBar::GlobalSaveState( this, sProfile );
	SaveBarState( sProfile );

	return CFrameWnd::DestroyWindow();
}

///////////////////////////////////////////////////

// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
BOOL CMainFrame::VerifyBarState( LPCTSTR lpszProfileName )
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = (int)pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}

///////////////////////////////////////////////////

void CMainFrame::OnDestroy()
{
	CSizingControlBar::GlobalSaveState( this, "musikDockBars" );
	
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

	CmusikSelectionCtrl* pSender	= NULL;
	CmusikSelectionCtrl* pCurr		= NULL;
	CmusikSelectionCtrl* pParent	= NULL;

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
		CmusikSelectionCtrl::SetUpdating( true );

		for( size_t i = 0; i < selbox_count; i++ )
		{
			pCurr = m_wndSelectionBars[i]->GetCtrl();
			pCurr->SetItemState( -1, 0, LVIS_SELECTED );
		}

		pParent->SetParent( false );
		pSender->UpdateV( true );

		CmusikSelectionCtrl::SetUpdating( false );

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
	CmusikSelectionCtrl::SetUpdating( true );
	for( size_t i = 0; i < selbox_count; i++ )
	{
		pCurr = m_wndSelectionBars[i]->GetCtrl();

		if ( pCurr != pSender && pCurr != pParent )
			pCurr->UpdateV( sSender );
	}
	CmusikSelectionCtrl::SetUpdating( false );

	// get the songs
	if ( !m_LibPlaylist )
		m_LibPlaylist = new CmusikPlaylist();

	m_Library->GetRelatedSongs( sSender, pSender->GetType(), *m_LibPlaylist );

	// make sure the correct playlist is set
	if ( m_wndView->GetCtrl()->GetPlaylist() != m_LibPlaylist )
		m_wndView->GetCtrl()->SetPlaylist( m_LibPlaylist, MUSIK_SOURCES_TYPE_LIBRARY );

	// update the windows
	m_wndView->GetCtrl()->UpdateV();
	m_wndSources->GetCtrl()->FocusLibrary();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongChange( WPARAM wParam, LPARAM lParam )
{
	// tell the child windows to redraw their
	// state accordingly
	m_wndView->GetCtrl()->RedrawWindow();

	m_wndNowPlaying->GetCtrl()->GetTimeCtrl()->OnNewSong();
	m_wndNowPlaying->GetCtrl()->UpdateInfo();
	m_wndNowPlaying->GetCtrl()->UpdateButtonStates();

	// if the player is playing, then we need
	// to do our own updating
	if ( m_Player->IsPlaying() )
	{
		CString s;
		s.Format( _T( "%s [ %s - %s ]" ), 
			MUSIK_VERSION_STR,
			m_Player->GetCurrPlaying()->GetTitle().c_str(), 
			m_Player->GetCurrPlaying()->GetArtist().c_str() );

		SetWindowText( s );
	}
	else
		SetWindowText( MUSIK_VERSION_STR );	

	return 0L;
}	

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongStop( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "Song stop signal caught\n" );

	m_wndNowPlaying->GetCtrl()->GetTimeCtrl()->OnNewSong();
	m_wndNowPlaying->GetCtrl()->UpdateInfo();
	m_wndNowPlaying->GetCtrl()->UpdateButtonStates();

	m_wndView->GetCtrl()->RedrawWindow();

	SetWindowText( MUSIK_VERSION_STR );
	
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongPauseResume( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "Pause song signal caught\n" );

	m_wndNowPlaying->GetCtrl()->UpdateButtonStates();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesLibrary( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A musik Library was clicked\n" );

	if ( !m_LibPlaylist )
		m_LibPlaylist = new CmusikPlaylist();

	m_wndView->GetCtrl()->SetPlaylist( m_LibPlaylist, MUSIK_SOURCES_TYPE_LIBRARY );
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesNowPlaying( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "'Now Playing' was clicked\n" );

	if ( !m_Player->GetPlaylist() )
	{
		TRACE0( "Something strange happened, m_Player doesn't have a playlist...\n" );
		ASSERT( 1 );
	}

	m_wndView->GetCtrl()->SetPlaylist( m_Player->GetPlaylist(), MUSIK_SOURCES_TYPE_NOWPLAYING );
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}


///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A standard playlist was clicked\n" );

	if ( !m_StdPlaylist )
		m_StdPlaylist = new CmusikPlaylist();

	int nID = m_wndSources->GetCtrl()->GetFocusedItem()->GetPlaylistID();
	m_Library->GetStdPlaylist( nID, *m_StdPlaylist, true );
	m_StdPlaylist->SetPlaylistID( nID );

	m_wndView->GetCtrl()->SetPlaylist( m_StdPlaylist, MUSIK_PLAYLIST_TYPE_STANDARD );
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesDynPlaylist( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A dynamic playlist was clicked\n" );

	//ResetPlaylist();
	
	//m_CurPlaylist = m_DynPlaylist;
	// MUSIK_PLAYLIST_TYPE_DYNAMIC

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

// wParam here is whether the last known
// item was a library; 

LRESULT CMainFrame::OnPlayerNewPlaylist( WPARAM wParam, LPARAM lParam )
{
	// if its the library: player took control
	// of the playlist. get a copy of it back
	// so when user clicks "Library" again, he
	// is displayed his last known results...
	if ( (int)wParam == MUSIK_SOURCES_TYPE_LIBRARY )
	{
		m_LibPlaylist = new CmusikPlaylist();
		*m_LibPlaylist = *m_Player->GetPlaylist();
	}

	// player just took ownership of us! thats ok,
	// we were only temporary, anyway.
	if ( (int)wParam == MUSIK_PLAYLIST_TYPE_STANDARD )
		m_StdPlaylist = new CmusikPlaylist();

	m_wndSources->GetCtrl()->FocusNowPlaying();

	return 0L;
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

///////////////////////////////////////////////////

void CMainFrame::OnFilePreferences()
{
	// initialize the property pages
	CPropertyPage wndPageInterfaceColors(	IDD_PROPPAGE_INTERFACE_COLORS);
	CPropertyPage wndPageInterfaceGeneral(	IDD_PROPPAGE_INTERFACE_GENERAL);
	CPropertyPage wndPageSoundCrossfader(	IDD_PROPPAGE_SOUND_CROSSFADER);
	CPropertyPage wndPageSoundDriver(		IDD_PROPPAGE_SOUND_DRIVER);

	// remove help icon from gripper
	wndPageInterfaceColors.m_psp.dwFlags&=	~PSP_HASHELP;
	wndPageInterfaceGeneral.m_psp.dwFlags&= ~PSP_HASHELP;
	wndPageSoundCrossfader.m_psp.dwFlags&=	~PSP_HASHELP;
	wndPageSoundDriver.m_psp.dwFlags&=		~PSP_HASHELP;

	// initialize the CTreePropSheet class 
	// and remove help icon from gripper
	TreePropSheet::CTreePropSheet PrefSheet(_T("musik Preferences"));
	PrefSheet.m_psh.dwFlags&= ~PSH_HASHELP;

	// physically add the preference sheets
	PrefSheet.AddPage(&wndPageInterfaceGeneral);
	PrefSheet.AddPage(&wndPageInterfaceColors);
	PrefSheet.AddPage(&wndPageSoundDriver);
	PrefSheet.AddPage(&wndPageSoundCrossfader);

	PrefSheet.SetEmptyPageText(_T("Please select a child item of '%s'."));
	
	// set some miscellaneous tree view properties
	PrefSheet.SetTreeViewMode(true, true, false);

	PrefSheet.SetActivePage(&wndPageInterfaceGeneral);

	PrefSheet.DoModal();
}

///////////////////////////////////////////////////

void CMainFrame::OnOpenFiles()
{
	// create the open dialog object
	CmusikFileDialog opendlg( TRUE, "mp3", NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
		"Supported Media Files ( *.mp3, *.ogg )|*.mp3;*.ogg|"
		"MP3 Files ( *.mp3 )|*.mp3|"
		"OGG Vorbis Files ( *.ogg )|*.ogg||" );

	if ( opendlg.DoModal() == IDOK )
	{
		CStdStringArray* files = new CStdStringArray();

		POSITION posCurr = opendlg.GetStartPosition();
		while ( posCurr )
			files->push_back( (CStdString)opendlg.GetNextPathName( posCurr ) );

		if ( files->size() > 0 )
		{
			CmusikBatchAdd* params = new CmusikBatchAdd( files, NULL, m_Library, NULL, m_BatchAddFnct, 0, 0, 1 );
			CmusikThread* thread = new CmusikThread();
			thread->Start( (ACE_THR_FUNC)musikBatchAddWorker, (void*)params );

			m_Threads.push_back( thread );
		}
		else
			delete files;
	}
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnBatchAddNew( WPARAM wParam, LPARAM lParam )
{
	CmusikBatchAdd* params = (CmusikBatchAdd*)wParam;

	if ( params )
	{
		params->m_Functor = m_BatchAddFnct;

		CmusikThread* thread = new CmusikThread();
		thread->Start( (ACE_THR_FUNC)musikBatchAddWorker, (void*)params );

		m_Threads.push_back( thread );		
	}

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnBatchAddProgress( WPARAM wParam, LPARAM lParam )
{
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnThreadEnd( WPARAM wParam, LPARAM lParam )
{
	// if we get here, a batch add thread
	// has finished successfully, so go
	// ahead and clean it up
	CmusikThread* ptr_thr = (CmusikThread*)wParam;
	
	if ( FreeThread( ptr_thr ) )
	{
		ResetSelBoxes();
		m_wndView->GetCtrl()->UpdateV( true );
	}

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnRemoveOldProgress( WPARAM wParam, LPARAM lParam )
{
	return 0L;
}

///////////////////////////////////////////////////

bool CMainFrame::FreeThread( CmusikThread* pThread )
{
	for ( size_t i = 0; i < m_Threads.size(); i++ )
	{
		if ( pThread == m_Threads.at( i ) )
		{
			// delete the completed thread
			delete m_Threads.at( i );

			// erase from array
			m_Threads.erase( m_Threads.begin() + i );
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////

void CMainFrame::OnOpenDirectory()
{
	TCHAR path[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T( "Please select a directory containing music." );
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
		// get the name of the folder and put it in path
		SHGetPathFromIDList ( pidl, path );

        // free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}

		CStdStringArray* files = new CStdStringArray();

        // get all the musik related files
		// in the directory we just found...
		CStdString sPath = path;
		sPath += "\\*.*";
		CmusikDir dir( sPath, files, NULL );
		dir.Run();

        // now fire the thread up...
		if ( files->size() > 0 )
		{
			CmusikBatchAdd* params = new CmusikBatchAdd( files, NULL, m_Library, NULL, m_BatchAddFnct, 0, 0, 1 );
			CmusikThread* thread = new CmusikThread();
			thread->Start( (ACE_THR_FUNC)musikBatchAddWorker, (void*)params );

			m_Threads.push_back( thread );
		}
		else 
			delete files;
	}
}

///////////////////////////////////////////////////

void CMainFrame::GetCrossfader( CmusikCrossfader* fader )
{
	bool gotfader = true;
    
	int nFader = m_Prefs->GetCrossfader();

	if ( nFader != -1 )
	{
		if ( m_Library->GetCrossfader( nFader, fader ) != SQLITE_OK )
			gotfader = false;
	}

	if ( nFader == -1 || !gotfader )
		fader->Set( 2.0f, 0.5f, 0.2f, 1.0f, 3.0f );
}

///////////////////////////////////////////////////

void CMainFrame::OnFileSaveplaylist()
{
	if ( m_wndView->GetCtrl()->PlaylistNeedsSave() )
		m_wndView->GetCtrl()->SavePlaylist();
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnPlayerPlaySel( WPARAM wParam, LPARAM lParam )
{
	// player is paused, so resume...
	if ( m_Player->IsPlaying() && m_Player->IsPaused() )
		m_Player->Resume();
	
	else
	{
		// player is not paused, so trigger the playlist
		// to activate the current item, which will
		// start playback... if that doesn't work, tell the
		// player to start playing it's playlist from pos 0
		if ( !m_wndView->GetCtrl()->PlayItem() )
			m_Player->Play( 0, MUSIK_CROSSFADER_NEW_SONG );
	}

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnVerifyPlaylist( WPARAM wParam, LPARAM lParam )
{
	CmusikPlaylist* playlist = (CmusikPlaylist*)wParam;

	if ( playlist == NULL || m_wndView->GetCtrl()->GetPlaylist() == NULL )
		return 0L;

	if ( playlist == m_wndView->GetCtrl()->GetPlaylist() )
		return 1L;

	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::OnViewSources()
{
	if ( m_wndSources->IsVisible() )
		ShowControlBar( m_wndSources, FALSE, FALSE );
	else
        ShowControlBar( m_wndSources, TRUE, FALSE );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewSources(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndSources->IsVisible() );
}

///////////////////////////////////////////////////

void CMainFrame::OnViewSelectionboxes()
{
	if ( !m_Prefs->GetSelBoxCount() )
		return;

	BOOL allvisible = TRUE;
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		if ( !m_wndSelectionBars[i]->IsVisible() )
		{
			allvisible = FALSE;
			break;
		}
	}

	if ( allvisible )
	{
		for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
			ShowControlBar( m_wndSelectionBars[i], FALSE, TRUE );
	}
	else
	{
		for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
			ShowControlBar( m_wndSelectionBars[i], TRUE, TRUE );
	}

	RecalcLayout();
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewSelectionboxes(CCmdUI *pCmdUI)
{
	BOOL allvisible = TRUE;
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		if ( !m_wndSelectionBars[i]->IsVisible() )
		{
			allvisible = FALSE;
			break;
		}
	}

	pCmdUI->SetCheck( allvisible );
}

///////////////////////////////////////////////////

void CMainFrame::OnViewNowplaying()
{
	if ( m_wndNowPlaying->IsVisible() )
		ShowControlBar( m_wndNowPlaying, FALSE, FALSE );
	else
        ShowControlBar( m_wndNowPlaying, TRUE, FALSE );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewNowplaying(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndNowPlaying->IsVisible() );
}

///////////////////////////////////////////////////

void CMainFrame::OnAudioEqualizerEnabled()
{
	if ( m_Prefs->IsEqualizerEnabled() )
	{
		m_Prefs->SetEqualizerEnabled( false );
		m_Player->EnableEqualizer( false );
		return;
	}
	else
	{
		m_Prefs->SetEqualizerEnabled( true );
		m_Player->EnableEqualizer( true );
	}
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateAudioEqualizerEnabled(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_Prefs->IsEqualizerEnabled() );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateAudioCrossfaderEnabled(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_Prefs->IsCrossfaderEnabled() );
}

///////////////////////////////////////////////////

void CMainFrame::OnAudioCrossfaderEnabled()
{
	if ( m_Prefs->IsCrossfaderEnabled() )
	{
		m_Prefs->SetCrossfaderEnabled( false );
		m_Player->EnableCrossfader( false );
	}
	else
	{
		m_Prefs->SetCrossfaderEnabled( true );

		CmusikCrossfader fade;
		GetCrossfader( &fade );

		m_Player->EnableCrossfader( true );
		m_Player->SetCrossfader( fade );
	}
}

///////////////////////////////////////////////////

void CMainFrame::OnViewPlaylistinformation()
{
	if ( m_Prefs->PlaylistInfoVisible() )
		m_Prefs->SetPlaylistInfoVisible( false );
	else
		m_Prefs->SetPlaylistInfoVisible( true );

	// send a "dummy" size event to the
	// window, so it re-creates or removes
	// the now playing window.
	CRect lpRect;
	m_wndView->GetClientRect( &lpRect );
	m_wndView->OnSize( NULL, lpRect.Width(), lpRect.Height() );
	RedrawWindow();
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewPlaylistinformation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_Prefs->PlaylistInfoVisible() );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateFileSaveplaylist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_wndView->GetCtrl()->PlaylistNeedsSave() );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdatePlaybackmodeRepeatsingle(CCmdUI *pCmdUI)
{
	if ( m_Player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE )
		pCmdUI->SetCheck( true );
	else
		pCmdUI->SetCheck( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnPlaybackmodeRepeatsingle()
{
	if ( m_Player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE )
		m_Player->ModifyPlaymode( NULL, MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE );
	else
		m_Player->ModifyPlaymode( MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE, NULL );

	m_Prefs->SetPlayerPlaymode( m_Player->GetPlaymode() );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdatePlaybackmodeRepeatplaylist(CCmdUI *pCmdUI)
{
	if ( m_Player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
		pCmdUI->SetCheck( true );
	else
		pCmdUI->SetCheck( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnPlaybackmodeRepeatplaylist()
{
	if ( m_Player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
		m_Player->ModifyPlaymode( NULL, MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST );
	else
		m_Player->ModifyPlaymode( MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST, NULL );

	m_Prefs->SetPlayerPlaymode( m_Player->GetPlaymode() );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdatePlaybackmodeIntro(CCmdUI *pCmdUI)
{
	if ( m_Player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_INTRO )
		pCmdUI->SetCheck( true );
	else
		pCmdUI->SetCheck( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnPlaybackmodeIntro()
{
	if ( m_Player->GetPlaymode() & MUSIK_PLAYER_PLAYMODE_INTRO )
		m_Player->ModifyPlaymode( NULL, MUSIK_PLAYER_PLAYMODE_INTRO );
	else
		m_Player->ModifyPlaymode( MUSIK_PLAYER_PLAYMODE_INTRO, NULL );

	m_Prefs->SetPlayerPlaymode( m_Player->GetPlaymode() );
}

///////////////////////////////////////////////////
