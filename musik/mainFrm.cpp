///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
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
#include "musikDirSync.h"
#include "musikPrefsDlg.h"
#include "musikPropertyPage.h"

#include "../musikCore/include/StdString.h"
#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"
#include "../musikCore/include/musikFilename.h"
#include "../musikCore/include/musikBatchAdd.h"
#include "../musikCore/include/musikRemoveOld.h"
#include "../musikCore/include/musikBatchRetag.h"
#include "../musikCore/include/musikCrossfader.h"
#include "../musikCore/include/musikEQSettings.h"
#include "../musikCore/include/musikEqualizer.h"

#include "memdc.h"

#include <io.h>
#include <Direct.h>
#include ".\mainfrm.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

int WM_SELBOXUPDATE			= RegisterWindowMessage( "SELBOXUPDATE" );
int WM_SELBOXEDITCOMMIT		= RegisterWindowMessage( "SELBOXEDITCOMMIT" );
int WM_SELBOXADDREMOVE		= RegisterWindowMessage( "SELBOXADDREMOVE" );
int WM_SELBOXREQUESTUPDATE	= RegisterWindowMessage( "SELBOXREQUESTUPDATE" );
int WM_SELBOXDELSEL			= RegisterWindowMessage( "SELBOXDELSEL" );

int WM_DRAGSTART			= RegisterWindowMessage( "DRAGSTART" );
int WM_DRAGEND				= RegisterWindowMessage( "DRAGEND" );

int WM_SOURCESLIBRARY		= RegisterWindowMessage( "SOURCESLIBRARY" );
int WM_SOURCESNOWPLAYING	= RegisterWindowMessage( "SOURCESNOWPLAYING" );
int WM_SOURCESSTDPLAYLIST	= RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
int WM_SOURCESSUBLIBRARY	= RegisterWindowMessage( "SOURCESSUBLIBRARY" );
int WM_SOURCESQUICKSEARCH	= RegisterWindowMessage( "SOURCESQUICKSEARCH" );
int WM_SOURCESITEMCLICKED	= RegisterWindowMessage( "SOURCESITEMCLICKED" );

int WM_CLOSEDIRSYNC			= RegisterWindowMessage( "CLOSEDIRSYNC" );

int WM_UPDATEPLAYLIST		= RegisterWindowMessage( "UPDATEPLAYLIST" );

int WM_RESTARTSOUNDSYSTEM	= RegisterWindowMessage( "RESTARTSOUNDSYSTEM" );

// we get these ones from the player
// via a CmusikFrmFunctor after a
// certain operation as completed
int WM_SONGCHANGE			= RegisterWindowMessage( "SONGCHANGE" );
int WM_SONGSTOP				= RegisterWindowMessage( "SONGSTOP" );
int WM_SONGPAUSE			= RegisterWindowMessage( "SONGPAUSE" );
int WM_SONGRESUME			= RegisterWindowMessage( "SONGRESUME" );
int WM_EQUALIZERCHANGE		= RegisterWindowMessage( "EQUALIZERCHANGE" );

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

// other dialogs, such as dockable windows, can
// call these to disable and enable transparency
// effects
int WM_INITTRANS	= RegisterWindowMessage( "INITTRANS" );
int WM_DEINITTRANS	= RegisterWindowMessage( "DEINITTRANS" );

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
	ON_WM_SYSCOMMAND()

	// menu 
	ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
	ON_COMMAND(ID_ADD_FILES, OnAddFiles)
	ON_COMMAND(ID_ADD_DIRECTORY, OnAddDirectory)
	ON_COMMAND(ID_FILE_SAVEPLAYLIST, OnFileSaveplaylist)
	ON_COMMAND(ID_VIEW_PLAYLISTINFORMATION, OnViewPlaylistinformation)
	ON_COMMAND(ID_VIEW_SOURCES, OnViewSources)
	ON_COMMAND(ID_VIEW_NOWPLAYING, OnViewNowplaying)
	ON_COMMAND(ID_AUDIO_EQUALIZER_ENABLED, OnAudioEqualizerEnabled)
	ON_COMMAND(ID_AUDIO_CROSSFADER_ENABLED, OnAudioCrossfaderEnabled)
	ON_COMMAND(ID_PLAYBACKMODE_REPEATSINGLE, OnPlaybackmodeRepeatsingle)
	ON_COMMAND(ID_PLAYBACKMODE_REPEATPLAYLIST, OnPlaybackmodeRepeatplaylist)
	ON_COMMAND(ID_PLAYBACKMODE_INTRO, OnPlaybackmodeIntro)
	ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_VIEW, OnUnsynchronizedtagsView)
	ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_WRITETOFILE, OnUnsynchronizedtagsWritetofile)
	ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_FINALIZEFORDATABASEONLY, OnUnsynchronizedtagsFinalizefordatabaseonly)
	ON_COMMAND(ID_VIEW_EQUALIZER, OnViewEqualizer)
	ON_COMMAND(ID_FILE_SYNCHRONIZEDDIRECTORIES, OnFileSynchronizeddirectories)
	ON_COMMAND(ID_VIEW_RESETINTERFACETODEFAULT, OnViewResetinterfacetodefault)
	ON_COMMAND(ID_NOTIFICATIONTRAY_EXIT, OnNotificationtrayExit)
	ON_COMMAND(ID_NOTIFICATIONTRAY_RESTORE, OnNotificationtrayRestore)
	ON_COMMAND(ID_NOTIFICATIONTRAY_PLAY, OnNotificationtrayPlay)
	ON_COMMAND(ID_NOTIFICATIONTRAY_STOP, OnNotificationtrayStop)
	ON_COMMAND(ID_NOTIFICATIONTRAY_PAUSE, OnNotificationtrayPause)
	ON_COMMAND(ID_NOTIFICATIONTRAY_NEXT, OnNotificationtrayNext)
	ON_COMMAND(ID_NOTIFICATIONTRAY_PREV, OnNotificationtrayPrev)
	ON_COMMAND(ID_FILE_CLEARLIBRARY, OnFileClearlibrary)
	ON_COMMAND(ID_LIBRARY_SYNCHRONIZEDIRECTORIESNOW, OnLibrarySynchronizedirectoriesnow)
	ON_COMMAND(ID_PLAYBACKMODE_SHUFFLECURRENTPLAYLIST, OnPlaybackmodeShufflecurrentplaylist)
	ON_COMMAND(ID_VIEW_VISUALIZATION, OnViewVisualization)

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
	ON_UPDATE_COMMAND_UI(ID_VIEW_EQUALIZER, OnUpdateViewEqualizer)
	ON_UPDATE_COMMAND_UI(ID_FILE_SYNCHRONIZEDDIRECTORIES, OnUpdateFileSynchronizeddirectories)
	ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PLAY, OnUpdateNotificationtrayPlay)
	ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PAUSE, OnUpdateNotificationtrayPause)
	ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_NEXT, OnUpdateNotificationtrayNext)
	ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PREV, OnUpdateNotificationtrayPrev)
	ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_STOP, OnUpdateNotificationtrayStop)
	ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_SHUFFLECURRENTPLAYLIST, OnUpdatePlaybackmodeShufflecurrentplaylist)

	// custom message maps
	ON_REGISTERED_MESSAGE( WM_SELBOXUPDATE, OnUpdateSel )
	ON_REGISTERED_MESSAGE( WM_SONGCHANGE, OnSongChange )
	ON_REGISTERED_MESSAGE( WM_SONGSTOP, OnSongStop )
	ON_REGISTERED_MESSAGE( WM_SONGPAUSE, OnSongPause )
	ON_REGISTERED_MESSAGE( WM_SONGRESUME, OnSongResume )
	ON_REGISTERED_MESSAGE( WM_SOURCESLIBRARY, OnSourcesLibrary )
	ON_REGISTERED_MESSAGE( WM_SOURCESNOWPLAYING, OnSourcesNowPlaying )
	ON_REGISTERED_MESSAGE( WM_SOURCESSUBLIBRARY, OnSourcesSubLib )
	ON_REGISTERED_MESSAGE( WM_SOURCESSTDPLAYLIST, OnSourcesStdPlaylist )
	ON_REGISTERED_MESSAGE( WM_SOURCESQUICKSEARCH, OnSourcesQuickSearch )
	ON_REGISTERED_MESSAGE( WM_DRAGSTART, OnDragStart )
	ON_REGISTERED_MESSAGE( WM_DRAGEND, OnDragEnd )
	ON_REGISTERED_MESSAGE( WM_SELBOXEDITCOMMIT, OnSelBoxEditCommit )
	ON_REGISTERED_MESSAGE( WM_SELBOXDELSEL, OnSelBoxDelSel )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_NEW, OnBatchAddNew )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_PROGRESS, OnBatchAddProgress )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_END, OnTaskEnd )
	ON_REGISTERED_MESSAGE( WM_REMOVEOLD_PROGRESS, OnRemoveOldProgress )
	ON_REGISTERED_MESSAGE( WM_REMOVEOLD_END, OnTaskEnd )
	ON_REGISTERED_MESSAGE( WM_PLAYER_PLAYSEL, OnPlayerPlaySel )
	ON_REGISTERED_MESSAGE( WM_BATCHADD_VERIFY_PLAYLIST, OnVerifyPlaylist )
	ON_REGISTERED_MESSAGE( WM_CLOSEDIRSYNC, OnCloseDirSync )
	ON_REGISTERED_MESSAGE( WM_SELBOXADDREMOVE, OnSelBoxAddRemove )
	ON_REGISTERED_MESSAGE( WM_SELBOXREQUESTUPDATE, OnSelBoxRequestUpdate )
	ON_REGISTERED_MESSAGE( WM_UPDATEPLAYLIST, OnUpdateCurrPlaylist )
	ON_REGISTERED_MESSAGE( WM_RESTARTSOUNDSYSTEM, OnRestartSoundSystem )
	ON_REGISTERED_MESSAGE( WM_EQUALIZERCHANGE, OnEqualizerChange )
	ON_REGISTERED_MESSAGE( WM_SOURCESITEMCLICKED, OnSourcesItemClicked )
	ON_REGISTERED_MESSAGE( WM_INITTRANS, OnInitTrans )
	ON_REGISTERED_MESSAGE( WM_DEINITTRANS, OnDeinitTrans )

	ON_UPDATE_COMMAND_UI(ID_VIEW_VISUALIZATION, OnUpdateViewVisualization)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CMainFrameWorker::open( void* parent )
{
	m_Parent = (CMainFrame*)parent;
	int ret_code = activate( THR_NEW_LWP | THR_JOINABLE | THR_USE_AFX );

	return ret_code;
}

///////////////////////////////////////////////////

int CMainFrameWorker::svc()
{
	m_Stop = false;
	m_Active = true;
	m_Finished = false;

	size_t pos = 0;
	size_t cnt = 0;
	CString turn;

	// sleep if we go idle
	ACE_Time_Value suspend, sleep;
	suspend.set( 0.1f );
	sleep.set( 1.0f );

	CString sCaption;
	while ( !m_Stop )
	{
		if ( m_Parent->GetTaskCount() )
		{
			switch ( pos )
			{
			case 0:
				turn = "/  ";
				++pos;
				break;
			case 1:
				turn = " | ";
				++pos;
				break;
			case 2:
				turn = "  \\";
				++pos;
				break;
			case 3:
				turn = " | ";
				pos = 0;
				break;			
			}

			sCaption = m_Parent->m_Caption;
			for ( size_t i = 0; i < m_Parent->GetTaskCount(); i++ )
			{
				sCaption += _T( "  " );
				sCaption += turn;
			}

			cnt++;
			if ( cnt == 12 )
			{
				m_Parent->RequerySelBoxes( NULL, true, true );
				cnt = 0;
			}

			m_Parent->SetWindowText( sCaption );
		}

		ACE_OS::sleep( sleep );

	}

	m_Finished = true;

	return 0;
}

///////////////////////////////////////////////////

CMainFrameFader::open( void* parent )
{
	m_Parent = (CMainFrame*)parent;
	int ret_code = activate( THR_NEW_LWP | THR_JOINABLE | THR_USE_AFX );

	return ret_code;
}

///////////////////////////////////////////////////

CMainFrameFader::svc()
{
	// we are a friend CMainFrame
	CmusikPrefs* m_Prefs = m_Parent->m_Prefs;

	// sleep if we go idle
	ACE_Time_Value suspend, sleep;
	sleep.set( 0.25f );

	m_Stop = false;
	m_Active = true;
	m_Finished = false;

	int fade_steps = 10;

	bool is_frame_focused = false;

	int trans = m_Prefs->GetTransUnFocus();
	while ( !m_Stop )
	{
		// transparency effects.
		if ( m_Prefs->IsTransEnabled() && m_Parent->IsTransEnb() )
		{
			// fade in
			if ( !is_frame_focused && GetForegroundWindow() == m_Parent->GetSafeHwnd() )
			{
				if ( trans < 1 )
					trans = 1;

				// adaptive filtering is only recommended
				// if the user has a good video card.
				if ( m_Prefs->IsTransAdaptive() )
				{
					suspend.set( m_Prefs->GetTransDur() / (float)( 1 + ( m_Prefs->GetTransFocus() - m_Prefs->GetTransUnFocus() ) ) );
					for ( int i = trans; i < m_Prefs->GetTransFocus(); i++ )
					{
						if ( m_Stop || !m_Parent->IsTransEnb() ) 
							break;

						trans++;

						if ( trans > m_Prefs->GetTransFocus() )
							trans = m_Prefs->GetTransFocus();

						m_Parent->SetTransparency( trans );
						ACE_OS::sleep( suspend );
					}
				}
				else	// non adaptive
				{
					int interval = ( m_Prefs->GetTransFocus() - m_Prefs->GetTransUnFocus() ) / fade_steps;
					suspend.set( m_Prefs->GetTransDur() / (float)( fade_steps ) );

					for ( int i = 0; i < fade_steps; i++ )
					{
						if ( m_Stop || !m_Parent->IsTransEnb() )
							break;

						trans += interval;

						if ( trans > m_Prefs->GetTransFocus() )
							trans = m_Prefs->GetTransFocus();

						m_Parent->SetTransparency( trans );
						ACE_OS::sleep( suspend );
					}
				}

				trans = m_Prefs->GetTransFocus();
				m_Parent->SetTransparency( trans );
				is_frame_focused = true;
			}

			// fade out
			else if ( is_frame_focused && GetForegroundWindow() != m_Parent->GetSafeHwnd() )
			{
				if ( trans < 1 )
					trans = 1;

				if ( m_Prefs->IsTransAdaptive() )
				{
					suspend.set( m_Prefs->GetTransDur() / ( 1 + ( m_Prefs->GetTransFocus() - m_Prefs->GetTransUnFocus() ) ) );
					for ( int i = 1; i < m_Prefs->GetTransFocus() - m_Prefs->GetTransUnFocus(); i++ )
					{
						if ( m_Stop || !m_Parent->IsTransEnb() )
							break;

						trans--;

						if ( trans < m_Prefs->GetTransUnFocus() )
							trans = m_Prefs->GetTransUnFocus();

						m_Parent->SetTransparency( trans );
						ACE_OS::sleep( suspend );
					}
				}
				else	// non adaptive
				{
					int interval = ( m_Prefs->GetTransFocus() - m_Prefs->GetTransUnFocus() ) / fade_steps;
					suspend.set( m_Prefs->GetTransDur() / (float)( fade_steps ) );

					for ( int i = 0; i < fade_steps; i++ )
					{
						if ( m_Stop || !m_Parent->IsTransEnb() )
							break;

						trans -= interval;

						if ( trans < m_Prefs->GetTransUnFocus() )
							trans = m_Prefs->GetTransUnFocus();

						m_Parent->SetTransparency( trans );
						ACE_OS::sleep( suspend );
					}
				}

				trans = m_Prefs->GetTransUnFocus();
				m_Parent->SetTransparency( trans );
				is_frame_focused = false;
			}
		}

		ACE_OS::sleep( sleep );
	}

	m_Finished = true;

	return 0;
}

///////////////////////////////////////////////////

CMainFrame::CMainFrame( bool autostart )
{
	m_AutoStart = autostart;
	m_SelBoxesVisible = false;
	m_TransEnb = false;

	m_hIcon16 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_MUSIK_16 ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	m_hIcon32 = ( HICON )LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_MUSIK_32 ), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );

	InitPaths();
	Initmusik();
	InitDragTypes();
}

///////////////////////////////////////////////////

CMainFrame::~CMainFrame()
{
	CIntArray sel_modes;
	for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
	{
		sel_modes.push_back( m_wndSelectionBars.at( i )->GetCtrl()->GetType() );
		delete m_wndSelectionBars.at( i );
	}
	m_Prefs->SetSelBoxTypes( sel_modes );

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
	m_UserDir += _T( "\\.musikCube\\" );

	RecurseMkDir( m_UserDir.GetBuffer() );

	m_Database = m_UserDir + _T( "musiklib.db" );
	m_PrefsIni = m_UserDir + _T( "musikprefs.ini" );
}

///////////////////////////////////////////////////

void CMainFrame::InitDragTypes()
{
	m_uPlaylistDrop_L	= RegisterClipboardFormat ( _T("musikPlaylist_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82") );
	m_uPlaylistDrop_R	= RegisterClipboardFormat ( _T("musikPlaylist_9543E137-7131-49fd-ADB5-208F0EF7A05C") );
	m_uSourcesDrop		= RegisterClipboardFormat ( _T("musikSources_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82") );
	m_uSelectionDrop_R	= RegisterClipboardFormat ( _T("musikSelection_070D6C9E-0E34-4b56-990D-5E2A2FA719A0") );
	m_uSelectionDrop_L	= RegisterClipboardFormat ( _T("musikSelection_374DA040-66D6-4b2d-9E8C-592421FF499D") );
}

///////////////////////////////////////////////////

void CMainFrame::Initmusik()
{
	m_NewSong		= new CmusikFrameFunctor( this );
	m_TaskCount		= 0;
	m_Caption		= CString( MUSIK_VERSION_STR ) + " ";
	m_BatchAddFnct	= new CmusikBatchAddFunctor( this );
	m_RemoveOldFnct	= new CmusikRemoveOldFunctor( this );
	m_Library		= new CmusikLibrary( ( CmusikString )m_Database );
	m_Prefs			= new CmusikPrefs( m_PrefsIni );
	m_DirSyncDlg	= NULL;
	m_PlaylistSel	= false;

	// show all songs, if we are supposed to
	m_Playlist = new CmusikPlaylist();
	
	if ( m_Prefs->LibraryShowsAllSongs() )
		m_Library->GetAllSongs( *m_Playlist, false );

	// setup the player...
	m_Player = new CmusikPlayer( m_NewSong, m_Library );
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
}

///////////////////////////////////////////////////

void CMainFrame::Cleanmusik()
{
	if ( m_DirSyncDlg )
	{
		m_DirSyncDlg->DestroyWindow();
		delete m_DirSyncDlg;
		m_DirSyncDlg = NULL;
	}

	if ( m_Prefs )		
	{
		delete m_Prefs;
		m_Prefs = NULL;
	}

	if ( m_Player )
	{
		delete m_Player;
		m_Player = NULL;
	}

	if ( m_Library )	
	{
		delete m_Library;
		m_Library = NULL;
	}

	if ( m_NewSong )	
	{
		delete m_NewSong;
		m_NewSong = NULL;
	}

	if ( m_Playlist )
	{
		delete m_Playlist;
		m_Playlist = NULL;
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

	if ( m_wndEqualizer )
	{
		delete m_wndEqualizer;
		m_wndEqualizer = NULL;
	}
}

///////////////////////////////////////////////////

void CMainFrame::LoadDlgSize()
{
	CRect rcNormal = CRect( m_Prefs->GetDlgPos(), m_Prefs->GetDlgSize() );
	MoveWindow( rcNormal );
	
	if ( m_Prefs->IsMaximized() )
	{
		WINDOWPLACEMENT max;
		max.showCmd = SW_MAXIMIZE;
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
	size.cx = 800;
	size.cy = 48;
	m_wndNowPlaying->ForceDockedSize( size, LM_HORZDOCK, true );
	m_wndNowPlaying->ForceFloatSize( size );

	// sources
	size.cx = 140;
	size.cy = 600;
	m_wndSources->ForceDockedSize( size, LM_VERTDOCK, true );
	m_wndSources->ForceFloatSize( size );

	// selection box
	size.cx = NULL;
	size.cy = 120;
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		m_wndSelectionBars.at( i )->ForceDockedSize( size, LM_HORZDOCK, true );

	// hide equalizer
	ShowControlBar( m_wndEqualizer, FALSE, TRUE );
}

///////////////////////////////////////////////////

void CMainFrame::ResetSelBoxes()
{
	CmusikSelectionCtrl::SetUpdating( true );
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		m_wndSelectionBars.at( i )->GetCtrl()->SetParent( false );
		m_wndSelectionBars.at( i )->GetCtrl()->UpdateV();
		m_wndSelectionBars.at( i )->GetCtrl()->SetItemState( -1, 0, LVIS_SELECTED );		
	}
	CmusikSelectionCtrl::SetUpdating( false );
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

	// create the view
	m_wndView = new CmusikPlaylistView( this, m_Library, m_Player, m_Prefs, m_uPlaylistDrop_L, m_uPlaylistDrop_R, m_uSourcesDrop, m_uSelectionDrop_L, m_uSelectionDrop_R );
	m_wndView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL );

	// set default playlist
	m_Playlist->m_Type = MUSIK_PLAYLIST_TYPE_LIBRARY;
	SetPlaylist();

	// now playing control
	m_wndNowPlaying = new CmusikNowPlayingBar( this, m_Player, m_Prefs );
	m_wndNowPlaying->Create( _T( "musik Now Playing" ), this, ID_NOWPLAYING );
	DockControlBar( m_wndNowPlaying, AFX_IDW_DOCKBAR_BOTTOM );

	// selection controls
	CmusikSelectionBar* pBar;
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		pBar = new CmusikSelectionBar( this, m_Library, m_Prefs, m_Prefs->GetSelBoxType( i ), i, m_uSelectionDrop_L, m_uSelectionDrop_R );
		pBar->Create( _T( "musik Selection Box" ), this, ID_SELECTIONBOX_START + i );
		if ( i == 0 )
			DockControlBar( pBar );
		else
			DockBarLeftOf( pBar, m_wndSelectionBars.at( i - 1 ) );

		m_wndSelectionBars.push_back( pBar );
	}

	// sources control
	m_wndSources = new CmusikSourcesBar( this, m_Library, m_Player, m_Prefs, m_uSourcesDrop, m_uPlaylistDrop_R, m_uSelectionDrop_R );
	m_wndSources->Create( _T( "Sources" ), this, ID_SOURCESBOX );
	DockControlBar( m_wndSources, AFX_IDW_DOCKBAR_LEFT );

	// equalizer control
	m_wndEqualizer = new CmusikEqualizerBar( m_Library, m_Player, m_Prefs );
	m_wndEqualizer->Create( _T( "Equalizer" ), this, ID_EQUALIZER );
	m_wndEqualizer->GetCtrl()->SetBandState( m_Prefs->GetEqualizerBandState() );
	m_wndEqualizer->GetCtrl()->SetChannelsLocked( m_Prefs->IsEqualizerChannelsLocked() );
	FloatControlBar( m_wndEqualizer, CPoint( 28, 28 ) );
	ShowControlBar( m_wndEqualizer, FALSE, FALSE );

	// append the system menu
	CMenu* pMenu = GetSystemMenu( false );
	if ( pMenu )
		pMenu->InsertMenu( 0, MF_STRING, SYSMENU_RESETDIALOG, _T( "Reset Window" ) );

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

		ShowSelectionBoxes( true, true );
	}

	// fire the updater task off
	m_ProtectingTasks.acquire();
		m_Updater = new CMainFrameWorker;
		m_Updater->open( this );
	m_ProtectingTasks.release();


	// tray icon stuff
	InitTrayIcon();

	// start all necessary synchronization tasks
	if ( !m_AutoStart && m_Prefs->SynchronizeOnStartup() )
		SynchronizeDirs();

	// fire off the fader task
	m_ProtectingTasks.acquire();
		m_Fader = new CMainFrameFader;
		m_Fader->open( this );
	m_ProtectingTasks.release();

	ImportTrans();
	if ( m_Prefs->IsTransEnabled() )
		InitTrans();

	return 0;
}

///////////////////////////////////////////////////

void CMainFrame::SetPlaylist( bool update, bool hide_sort_arrow, bool select_none )
{
	m_wndView->GetCtrl()->SetPlaylist( m_Playlist );
	
	if ( update )
		m_wndView->GetCtrl()->UpdateV( true, select_none );

	if ( hide_sort_arrow )
		m_wndView->GetCtrl()->HideSortArrow();

	// create a new one in its place... 
	// CmusikPlaylistCtrl::SetPlaylist will take ownership
	// of the old playlist to stay CPU and memory friendly
	m_Playlist = new CmusikPlaylist();
	m_Playlist->m_Type = m_wndView->GetCtrl()->GetPlaylist()->m_Type;
}

///////////////////////////////////////////////////

void CMainFrame::SetNowPlaying()
{
	m_wndView->GetCtrl()->SetPlaylist( m_Player->GetPlaylist() );

	ShowSelectionBoxes( false );
	m_wndView->GetCtrl()->UpdateV( true, true );
	m_wndView->GetCtrl()->HideSortArrow();
}

///////////////////////////////////////////////////

bool CMainFrame::PlayCmd( const CString& fn )
{
	if ( fn.IsEmpty() )
		return false;

	if ( m_Library )
	{
		// add song to library, if necessary
		m_Library->AddSong( (CmusikString)fn );

		// get player's playlist
		CmusikPlaylist* pPlaylist = m_Player->GetPlaylist();
		if ( pPlaylist )
		{
			// get the song we just added...
			CmusikSong song;
			m_Library->GetSongFromFilename( (CmusikString)fn, song );

			// add to the control's playlist
			if ( song.GetID() >= 0 )
			{
				pPlaylist->Add( song );

				// set the player's playlist, and play
				if ( m_Player )
				{
					if ( !m_Player->IsPlaying() )
						m_Player->Play( pPlaylist->GetCount() - 1, MUSIK_CROSSFADER_NEW_SONG );

					m_Playlist->m_Type = MUSIK_PLAYLIST_TYPE_NOWPLAYING;
					SetPlaylist();

					m_wndSources->GetCtrl()->FocusNowPlaying();
				}

				RequerySelBoxes();

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
	// make sure all the tasks are finished
	// before destroying the window...
	KillTasks( true, true, true, false );

	CString sProfile = _T( "musikProfile" );
	CSizingControlBar::GlobalSaveState( this, sProfile );
	SaveBarState( sProfile );

	HideTrayIcon();

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
	
	SaveWindowState();

	CFrameWnd::OnDestroy();
}

///////////////////////////////////////////////////

void CMainFrame::SaveWindowState()
{
	WINDOWPLACEMENT max;
	GetWindowPlacement( &max );


	if ( max.showCmd & SW_SHOWMAXIMIZED )
		m_Prefs->SetMaximized( true );
	else
		m_Prefs->SetMaximized( false );

	if ( !( max.showCmd & SW_SHOWMINIMIZED ) && !( max.showCmd & SW_HIDE ) )
	{
		CRect rc_dlg;
		GetWindowRect( &rc_dlg );

		m_Prefs->SetDlgSize( CSize( rc_dlg.right - rc_dlg.left, rc_dlg.bottom - rc_dlg.top ) );
		m_Prefs->SetDlgPos( CPoint( rc_dlg.left, rc_dlg.top ) );
		m_Prefs->SetMaximized( false );
	}
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSelBoxEditCommit( WPARAM wParam, LPARAM lParam )
{
	// setup vars
	CmusikSelectionCtrl* pSel = (CmusikSelectionCtrl*)wParam;
	int nLibType = (int)lParam;
	CmusikString sNew = pSel->GetEditCommitStr();

	// get selected items into a new
	// playlist
	CmusikPlaylist* playlist = new CmusikPlaylist();
	
	//CmusikString sub_query = pSel->GetSelQuery();
	CmusikString partial_query = GetSelQuery( pSel );
	bool sub_query = true;
	if ( partial_query.Left( 1 ) == _T( "W" ) )
		sub_query = false;

	m_Library->GetRelatedSongs( partial_query, pSel->GetType(), *playlist, sub_query, UseTempTable() );

	// create a new CmusikSongInfoArray and update
	// all the respective values...
	if ( playlist->GetCount() )
	{
		// get songs	
		CmusikSongInfoArray* pSongInfoArray = new CmusikSongInfoArray();
		m_Library->GetInfoArrayFromPlaylist( playlist, pSongInfoArray, nLibType, sNew, true );
		delete playlist;

		// setup params
		CmusikBatchRetag* params = new CmusikBatchRetag( m_Library, m_RemoveOldFnct, pSongInfoArray );
		params->m_WriteToFile = m_Prefs->WriteTagsToFile();

		// spawn the task
		m_ProtectingTasks.acquire();

			CmusikBatchRetagTask* task = new CmusikBatchRetagTask;
			
			m_Tasks.push_back( task );
			m_TaskCount++;

			task->open( params );

		m_ProtectingTasks.release();

		return 1L;
	}
	else
		delete playlist;

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnUpdateSel( WPARAM wParam, LPARAM lParam )
{
	size_t selbox_count = m_Prefs->GetSelBoxCount();
	if ( selbox_count < 1 )
		return 0L;

	CmusikSelectionCtrl* pSender	= NULL;
	CmusikSelectionCtrl* pCurr		= NULL;
	CmusikSelectionCtrl* pParent	= NULL;

	int nSender = (int)wParam;

	bool force_all_updatev;
	if ( lParam == NULL )
		force_all_updatev = false;
	else
		force_all_updatev = true;

	// find the sender and parent
	for ( size_t i = 0; i < selbox_count; i++ )
	{
		pCurr = m_wndSelectionBars.at( i )->GetCtrl();
		if ( pCurr->GetCtrlID() == nSender )
			pSender = pCurr;
		if ( pCurr->IsParent() )
			pParent = pCurr;
	}

	// sender not found
	if ( !pSender )
		return 0L;

	// if the first item was clicked, the user wants
	// to display all the info
	bool unsel_parent = false;
	if ( pSender->IsItemSelected( 0 ) || ( pParent && !pParent->GetSelectedCount() ) )
	{
		if ( pParent && !pParent->GetSelectedCount() && pParent->IsItemSelected( 0 ) )
			unsel_parent = true;

		CmusikSelectionCtrl::SetUpdating( true );

		if ( pParent )
			pParent->SetParent( false );

		pSender->SetParent( true );
		pParent = pSender;

		for( size_t i = 0; i < selbox_count; i++ )
		{
			pCurr = m_wndSelectionBars.at( i )->GetCtrl();

			if ( unsel_parent && pCurr == pParent )
				continue;

			pCurr->SetItemState( -1, 0, LVIS_SELECTED );
			pCurr->UpdateV();
		}

		CmusikSelectionCtrl::SetUpdating( false );

		// prompt to save old playlist...
		m_wndView->GetCtrl()->SavePlaylist();

		// update playlist
		if ( m_Prefs->LibraryShowsAllSongs() )
			m_Library->GetAllSongs( *m_Playlist, UseTempTable() );
		else
			m_Playlist->Clear();

		// assure the correct playlist is set and
		// then update the view
		m_Playlist->m_Type = m_wndView->GetCtrl()->GetPlaylist()->m_Type;

		SetPlaylist();

		if ( !unsel_parent )
			return 0L;
	}	
	else 
		m_wndView->GetCtrl()->SavePlaylist();

	// no parent found, so we have a new parent!
	if ( !pParent )
	{
		pParent = pSender;
		pParent->SetParent( true );
		pParent->RedrawWindow();
	}
	
	// update all the selection controls and
	// the playlist
	RequerySelBoxes( pSender );
	RequeryPlaylist( pSender, false );

	return 0L;
}

///////////////////////////////////////////////////

bool CMainFrame::UseTempTable()
{
	return m_PlaylistSel;
}

///////////////////////////////////////////////////

int CMainFrame::GetSelPlaylistType()
{
	CmusikPropTreeItem* pItem = m_wndSources->GetCtrl()->GetFocusedItem();

	if ( pItem )
		return pItem->GetPlaylistType();
	
	return -1;
}

///////////////////////////////////////////////////

// author: Per-Erik Nordlund 
// from: http://www.codeproject.com/w2k/win2k_transparent.asp
// modified by Casey Langen

void CMainFrame::SetTransparency( int trans )
{
	m_pSetLayeredWindowAttributes( GetSafeHwnd(), 0, trans, LWA_ALPHA );
	m_Trans = trans;
}

///////////////////////////////////////////////////

void CMainFrame::ImportTrans()
{
	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	m_pSetLayeredWindowAttributes = 
						(lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, 
						"SetLayeredWindowAttributes");
}

///////////////////////////////////////////////////

void CMainFrame::DeinitTrans()
{
	if ( m_TransEnb )
	{
		SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong( GetSafeHwnd() , GWL_EXSTYLE) & ~WS_EX_LAYERED );
		RedrawWindow();

		m_TransEnb = false;
	}
}

///////////////////////////////////////////////////

void CMainFrame::InitTrans( bool set_trans )
{
	if ( !m_TransEnb )
	{
		SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE ) 
				| WS_EX_LAYERED );

		if ( set_trans )
			SetTransparency( m_Trans );
		else
			RedrawWindow();

		m_TransEnb = true;
	}
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnInitTrans( WPARAM wParam, LPARAM lParam )
{
	InitTrans( true );
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnDeinitTrans( WPARAM wParam, LPARAM lParam )
{
	DeinitTrans();
	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::RequerySelBoxes( CmusikSelectionCtrl* sender, bool deselect_items, bool only_if_parent_exists )
{
	// first find if a parent exists
	CmusikSelectionCtrl* pParent = NULL;

	for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
	{
		if ( m_wndSelectionBars.at( i )->GetCtrl()->IsParent() )
		{
			pParent = m_wndSelectionBars.at( i )->GetCtrl();
			break;
		}
	}

	if ( only_if_parent_exists && pParent )
		return;

	CmusikSelectionCtrl::SetUpdating( true );

	// if there is no parent then do a full
	// reset of each box
	CmusikSelectionCtrl* pCurr = NULL;
	if ( !pParent ) 
	{
		for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
		{
			pCurr = m_wndSelectionBars.at( i )->GetCtrl();
			pCurr->UpdateV();
		}
	}

	// we have a parent box, so the children
	// must be updated in relation
	else
	{
		pParent->UpdateV();

		// parent sent the event
		if ( pParent == sender || sender == NULL )
		{
			// kill any selections, and reset the
			// child ordering. all children order's
			// are at -1 ...
			for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
			{
				pCurr = m_wndSelectionBars.at( i )->GetCtrl();
				if ( pCurr != pParent )
				{
					pCurr->SetItemState( -1, 0, LVIS_SELECTED );
					pCurr->ResetOrder();
				}
			}

			CmusikSelectionCtrl::ResetChildOrder();

			// step through all the children and requery
			// based on the parent...
			for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
			{
				pCurr = m_wndSelectionBars.at( i )->GetCtrl();

				if ( pCurr != pParent )
				{
					CmusikString query = GetSelQuery( NULL );
					pCurr->UpdateV( query );
				}
			}
		}

		// a child sent the event
		else
		{
			for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
			{
				pCurr = m_wndSelectionBars.at( i )->GetCtrl();
				
				if ( pCurr->IsParent() || sender == pCurr )
					continue;

				if ( pCurr->GetChildOrder() > sender->GetChildOrder() || pCurr->GetChildOrder() == -1 )
				{
					if ( deselect_items )
						pCurr->SetItemState(  -1, 0, LVIS_SELECTED );

					CmusikString query = GetSelQuery( pCurr );
					pCurr->UpdateV( query );
				}
			}
		}
	}

	CmusikSelectionCtrl::SetUpdating( false );
}

///////////////////////////////////////////////////

void CMainFrame::RequeryPlaylist( CmusikSelectionCtrl* sender, bool prompt_save )
{
	// if there is a sender then requery based on the
	// sender's information. otherwise just call
	// UpdateV() again to update the view.
	if ( sender )
	{
		// prompt to save old playlist...
		if ( prompt_save )
			m_wndView->GetCtrl()->SavePlaylist();

		// see what the order should be by...
		int order_by = -1;
		if ( sender )
			order_by = sender->GetType();

		// determine query type
		CmusikString sQuery = GetSelQuery( sender );
		bool sub_query = true;
		if ( sQuery.Left( 1 ) == _T( "W" ) )
			sub_query = false;

		// get the related songs...
		m_Library->GetRelatedSongs( sQuery, order_by, *m_Playlist, sub_query, UseTempTable() );
	} 

	// do it
	SetPlaylist( true, true, true );
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSelBoxRequestUpdate( WPARAM wParam, LPARAM lParam )
{
	RequerySelBoxes();
	return 0L;
}

///////////////////////////////////////////////////

CmusikString CMainFrame::GetSelQuery( CmusikSelectionCtrl* target )
{
	// get parent
	CmusikSelectionCtrl* pParent = NULL;
	for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
	{
		if ( m_wndSelectionBars.at( i )->GetCtrl()->IsParent() )
		{
			pParent = m_wndSelectionBars.at( i )->GetCtrl();
			break;
		}
	}

	if ( !pParent )
		return "";

	CmusikString sChildQuery, sParent;
	CmusikSelectionCtrl* pCurr = NULL;

	// get the child queries from the other
	// selection boxes... if the target is
	// NULL then we got an update from the parent
	// box, which means all the children need
	// to be reset...
	if ( target != NULL )
	{
		for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
		{
			pCurr = m_wndSelectionBars.at( i )->GetCtrl();

			if ( pCurr != pParent && pCurr->GetSelectedCount() )
			{
				if ( sChildQuery.IsEmpty() )
					sChildQuery += _T( " WHERE (" );
				else
					sChildQuery += _T( " AND ( " );

				sChildQuery += pCurr->GetSelQuery();
				sChildQuery += _T( ")" );
			}
		}
	}

	// parent portion of the query
	if ( pParent->GetSelectedCount() )
	{
		if ( sChildQuery.IsEmpty() )
		{
			sParent += _T( "WHERE (" );
			sParent += pParent->GetSelQuery();
			sParent += _T( ")" );
		}
		else
		{
			sParent += _T( "( SELECT %T FROM songs WHERE " );
			sParent += pParent->GetSelQuery();
			sParent += _T( ")" );

			sChildQuery.Trim();
		}
	}
	else if ( sParent.IsEmpty() )
	{
		sParent += _T( "WHERE (" );
		sParent += pParent->GetSelQuery();
		sParent += _T( ")" );
	}
	
	// combined portion
	CmusikString sQuery = sParent + sChildQuery;

	return sQuery;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnEqualizerChange( WPARAM wParam, LPARAM lParam )
{
	m_wndEqualizer->GetCtrl()->LoadCurrSong();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongChange( WPARAM wParam, LPARAM lParam )
{
	CString s;

	// tell the child windows to redraw their
	// state accordingly
	if ( m_wndView->GetCtrl()->GetPlaylist() == m_Player->GetPlaylist() )
		m_wndView->GetCtrl()->ScrollToCurr();

	m_wndView->GetCtrl()->ResyncItem( m_Player->GetCurrPlaying()->GetID() );

	m_wndNowPlaying->GetCtrl()->GetTimeCtrl()->OnNewSong();
	m_wndNowPlaying->GetCtrl()->UpdateInfo();
	m_wndNowPlaying->GetCtrl()->SetPlaying();
	
	// if the player is playing, then we need
	// to do our own updating
	if ( m_Player->IsPlaying() )
	{
		s.Format( _T( "[%s - %s] %s" ), 
			m_Player->GetCurrPlaying()->GetTitle().c_str(), 
			m_Player->GetCurrPlaying()->GetArtist().c_str(),
			MUSIK_VERSION_STR );

		SetWindowText( s );
	}
	else
	{
		s = MUSIK_VERSION_STR;
		SetWindowText( s );	
	}

	m_Caption = s;

	// update notification tooltip
	if ( m_Prefs->MinimizeToTray() )
	{
		strncpy ( m_TrayIcon.szTip, m_Caption, sizeof( m_TrayIcon.szTip ) );
		Shell_NotifyIcon( NIM_MODIFY, &m_TrayIcon );
	}

	return 0L;
}	

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongStop( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "Song stop signal caught\n" );

	m_wndNowPlaying->GetCtrl()->GetTimeCtrl()->OnNewSong();
	m_wndNowPlaying->GetCtrl()->UpdateInfo();
	m_wndNowPlaying->GetCtrl()->SetStopped();

	m_wndView->GetCtrl()->RedrawWindow();

	SetWindowText( MUSIK_VERSION_STR );
	
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongPause( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "Pause song signal caught\n" );
	m_wndNowPlaying->GetCtrl()->SetPaused();
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSongResume( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "Resume song signal caught\n" );
	m_wndNowPlaying->GetCtrl()->SetResumed();
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesItemClicked( WPARAM wParam, LPARAM lParam )
{
	m_wndView->GetCtrl()->SavePlaylist();
	return 0L;	
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesLibrary( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A musik Library was clicked\n" );

	m_PlaylistSel = false;

	if( m_Prefs->LibraryShowsAllSongs() )
		m_Library->GetAllSongs( *m_Playlist, false );
	else
	{
		m_Playlist->Clear();
		m_Playlist->m_Type = MUSIK_PLAYLIST_TYPE_LIBRARY;
	}

	ResetSelBoxes();
	ShowSelectionBoxes( true );
	
	SetPlaylist();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesQuickSearch( WPARAM wParam, LPARAM lParam )
{
	m_PlaylistSel = false;

	ShowSelectionBoxes( false );

	if ( !m_Playlist )
		m_Playlist = new CmusikPlaylist();

	CString* sPattern = (CString*)wParam;
	
	m_Library->QuickQuery( (CmusikString)*sPattern, *m_Playlist );

	if ( m_wndView->GetCtrl()->GetPlaylist() != m_Playlist )
		SetPlaylist();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesNowPlaying( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "'Now Playing' was clicked\n" );

	m_PlaylistSel = false;
	ShowSelectionBoxes( false );

	SetNowPlaying();
	m_wndView->GetCtrl()->ScrollToCurr();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesSubLib( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A sub library was clicked\n" );

	m_PlaylistSel = true;

	int nID = m_wndSources->GetCtrl()->GetFocusedItem()->GetPlaylistID();
	m_Library->GetStdPlaylist( nID, *m_Playlist, true );

	m_Library->PopulateTempSongTable( *m_Playlist );
	ResetSelBoxes();
	ShowSelectionBoxes( true );

	if ( !m_Prefs->LibraryShowsAllSongs() )
		m_Playlist->Clear();
		
	SetPlaylist();

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam )
{
	TRACE0( "A sub library was clicked\n" );

	m_PlaylistSel = true;

	int nID = m_wndSources->GetCtrl()->GetFocusedItem()->GetPlaylistID();
	m_Library->GetStdPlaylist( nID, *m_Playlist, true );

	m_Library->PopulateTempSongTable( *m_Playlist );
	ShowSelectionBoxes( false );
		
	SetPlaylist();

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
	CmusikPrefsInterfaceWorkflow wndPageInterfaceWorkflow( m_Prefs, m_Library, m_Player );
	CmusikPrefsInterfaceTrans wndPageInterfaceTrans(  m_Prefs, m_Library, m_Player );
	CmusikPrefsSoundCrossfader wndPageSoundCrossfader( m_Prefs, m_Library, m_Player );
	CmusikPrefsSoundDriver wndPageSoundDriver( m_Prefs, m_Library, m_Player );


	// remove help icon from gripper
	wndPageInterfaceWorkflow.m_psp.dwFlags&=	~PSP_HASHELP;
	wndPageInterfaceTrans.m_psp.dwFlags&=		~PSP_HASHELP;
	wndPageSoundCrossfader.m_psp.dwFlags&=		~PSP_HASHELP;
	wndPageSoundDriver.m_psp.dwFlags&=			~PSP_HASHELP;

	// initialize the CTreePropSheet class 
	// and remove help icon from gripper
	CmusikPropertySheet PrefSheet( _T( "musikCube Preferences" ) );
	PrefSheet.m_psh.dwFlags&= ~PSH_HASHELP;

	// physically add the preference sheets
	PrefSheet.AddPage( &wndPageInterfaceWorkflow );
	PrefSheet.AddPage( &wndPageInterfaceTrans );
	PrefSheet.AddPage( &wndPageSoundCrossfader );
	PrefSheet.AddPage( &wndPageSoundDriver );

	PrefSheet.SetEmptyPageText( _T( "Please select a child item of '%s'." ) );
	
	// set some miscellaneous tree view properties
	PrefSheet.SetTreeViewMode( true, true, false );

	PrefSheet.SetActivePage( &wndPageInterfaceWorkflow );

	PrefSheet.DoModal();

	if ( !m_Prefs->IsTransEnabled() && m_Trans != 255 )
		SetTransparency( 255 );
} 

///////////////////////////////////////////////////

void CMainFrame::OnAddFiles()
{
	// create the open dialog object
	CmusikFileDialog opendlg( TRUE, "mp3", NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
		"Supported Media Files ( *.mp3, *.ogg )|*.mp3;*.ogg|"
		"MP3 Files ( *.mp3 )|*.mp3|"
		"OGG Vorbis Files ( *.ogg )|*.ogg||" );

	if ( opendlg.DoModal() == IDOK )
	{
		CmusikStringArray* files = new CmusikStringArray();

		POSITION posCurr = opendlg.GetStartPosition();
		while ( posCurr )
			files->push_back( (CmusikString)opendlg.GetNextPathName( posCurr ) );

		if ( files->size() > 0 )
		{
			// setup params
			CmusikBatchAdd* params = new CmusikBatchAdd( files, NULL, m_Library, NULL, m_BatchAddFnct, 0, 0, 1 );

			// spawn task
			m_ProtectingTasks.acquire();

				CmusikBatchAddTask* task = new CmusikBatchAddTask;

				m_Tasks.push_back( task );
				m_TaskCount++;

				task->open( params );

			m_ProtectingTasks.release();

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
		m_ProtectingTasks.acquire();

			params->m_Functor = m_BatchAddFnct;
			CmusikBatchAddTask* task = new CmusikBatchAddTask;
			
			m_Tasks.push_back( task );
			m_TaskCount++;

			task->open( params );

		m_ProtectingTasks.release();
	}

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnBatchAddProgress( WPARAM wParam, LPARAM lParam )
{
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnTaskEnd( WPARAM wParam, LPARAM lParam )
{
	CmusikTask* ptr_task = (CmusikTask*)wParam;
	int type = FreeTask( ptr_task );

	if ( !m_TaskCount )
		SetWindowText( m_Caption );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnRemoveOldProgress( WPARAM wParam, LPARAM lParam )
{
	return 0L;
}

///////////////////////////////////////////////////

int CMainFrame::FreeTask( CmusikTask* pTask )
{
	int ret = -2;

	m_ProtectingTasks.acquire();

	for ( size_t i = 0; i < m_Tasks.size(); i++ )
	{
		if ( pTask == m_Tasks.at( i ) )
		{
			ret = m_Tasks.at( i )->GetType();

			// wait for the finish flag to be set, it has
			// a 20 second grace period.
			m_Tasks.at( i )->StopWait( 20 );

			// delete the completed task and erase from array
			delete m_Tasks.at( i );
            m_Tasks.erase( m_Tasks.begin() + i );

			// do any refreshing we need
			if ( ret > MUSIK_TASK_TYPE_UNKNOWN && ret < MUSIK_TASK_TYPE_PLAYER_WORKER )
			{
				if ( ret == MUSIK_TASK_TYPE_BATCHRETAG )
					RequerySelBoxes( NULL, true );
				else
					RequerySelBoxes( NULL, true, true );

				m_wndView->GetCtrl()->ResyncItem( -1, -1 );
			}

			break;
		}
	}

	if ( ret != -2 )
		m_TaskCount--;

	m_ProtectingTasks.release();

	return ret;
}

///////////////////////////////////////////////////

void CMainFrame::OnAddDirectory()
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

		CmusikStringArray* files = new CmusikStringArray();

        // get all the musik related files
		// in the directory we just found...
		CmusikString sPath = path;
		sPath += "\\*.*";
		CmusikDir dir( sPath, files );
		dir.Run();

        // now fire the task up...
		if ( files->size() > 0 )
		{
			// setup params
			CmusikBatchAdd* params = new CmusikBatchAdd( files, NULL, m_Library, NULL, m_BatchAddFnct, 0, 0, 1 );
			
			// start it up
			m_ProtectingTasks.acquire();

				CmusikBatchAddTask* task = new CmusikBatchAddTask;

				m_Tasks.push_back( task );
				m_TaskCount++;

				task->open( params );

			m_ProtectingTasks.release();
		}
		else 
			delete files;
	}
}

///////////////////////////////////////////////////

void CMainFrame::GetCrossfader( CmusikCrossfader* fader )
{
	if ( m_Library->GetDefaultCrossfader( fader ) != MUSIK_LIBRARY_OK )
	{
		fader->Set( 2.0f, 0.5f, 0.2f, 1.0f, 3.0f );
		m_Library->UpdateDefaultCrossfader( *fader );
	}
}

///////////////////////////////////////////////////

void CMainFrame::OnFileSaveplaylist()
{
	if ( m_wndView->GetCtrl()->PlaylistNeedsSave() )
		m_wndView->GetCtrl()->SavePlaylist( false );
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

void CMainFrame::ShowSelectionBoxes( bool show, bool force )
{
	if ( !force && m_SelBoxesVisible == show )
		return;

	if ( show && !m_SelBoxesVisible )
		CSizingControlBar::GlobalLoadState( this, _T( "musikProfile" ) );
	else
		CSizingControlBar::GlobalSaveState( this, _T( "musikProfile" ) );

	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		ShowControlBar( m_wndSelectionBars.at( i ), show, TRUE );

	m_SelBoxesVisible = show;
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewSelectionboxes(CCmdUI *pCmdUI)
{
	BOOL allvisible = TRUE;
	for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
	{
		if ( !m_wndSelectionBars.at( i )->IsVisible() )
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
		m_Player->UpdateEqualizer();
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
	{
		m_Prefs->SetPlaylistInfoVisible( false );
	
		if ( !m_Prefs->GetPlaylistInfoVizStyle() )
			m_wndView->DeinitPlaylistInfo();
	}
	else
		m_Prefs->SetPlaylistInfoVisible( true );

	// send a "dummy" size event to the
	// window, so it re-creates or removes
	// the now playing window.
	CRect lpRect;
	m_wndView->GetClientRect( &lpRect );
	m_wndView->OnSize( NULL, lpRect.Width(), lpRect.Height() );
	RedrawWindow();
	m_wndView->UpdatePlaylistInfo();
	
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

void CMainFrame::OnUnsynchronizedtagsView()
{
	m_wndView->GetCtrl()->SavePlaylist();
	m_Library->GetDirtySongs( m_Playlist, true );
	SetPlaylist();
}

///////////////////////////////////////////////////

void CMainFrame::OnUnsynchronizedtagsWritetofile()
{
	// get dirty items into a new playlist
	CmusikPlaylist* playlist = new CmusikPlaylist();
	m_Library->GetDirtySongs( playlist, false );

	// create a new CmusikSongInfoArray and update
	// all the respective values...
	if ( playlist->GetCount() )
	{
		CmusikSongInfoArray* pSongInfoArray = new CmusikSongInfoArray();
	
		m_Library->GetInfoArrayFromPlaylist( playlist, pSongInfoArray );

		delete playlist;

		// setup params
		CmusikBatchRetag* params = new CmusikBatchRetag( m_Library, m_RemoveOldFnct, pSongInfoArray );
		params->m_WriteToFile = true;

		// setup and fire off task
		m_ProtectingTasks.acquire();

			CmusikBatchRetagTask* task = new CmusikBatchRetagTask;

			m_Tasks.push_back( task );
			m_TaskCount++;

			task->open( params );

		m_ProtectingTasks.release();
	}
	else
		delete playlist;
}

///////////////////////////////////////////////////

void CMainFrame::OnUnsynchronizedtagsFinalizefordatabaseonly()
{
	m_Library->FinalizeDirtySongs();
}

///////////////////////////////////////////////////

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SYSMENU_RESETDIALOG )
	{
		this->ResetUI();
		return;
	}

	if ( nID == SC_MINIMIZE )
	{
		SaveWindowState();

		ShowWindow( SW_MINIMIZE );

		if ( m_Prefs->MinimizeToTray() )
		{
			ShowWindow( SW_HIDE );
			ShowTrayIcon();
		}

		return;
	}

	// dunno what it is, but 61730 gets
	// called when double clicking the title
	// bar to restore...
	else if ( nID == SC_RESTORE || nID == 61730 )
	{
		ShowWindow( SW_RESTORE );

		CRect rcNormal = CRect( m_Prefs->GetDlgPos(), m_Prefs->GetDlgSize() );

		WINDOWPLACEMENT max;
		GetWindowPlacement( &max );

		if ( max.showCmd &~ SW_MAXIMIZE )
			MoveWindow( rcNormal );

		if ( m_Prefs->MinimizeToTray() )
			HideTrayIcon();

		return;
	}

	else if ( nID == SC_MAXIMIZE || nID == 61458 )
		SaveWindowState();

	CFrameWnd::OnSysCommand(nID, lParam);
}

///////////////////////////////////////////////////

void CMainFrame::OnViewEqualizer()
{
	if ( m_wndEqualizer->IsVisible() )
		ShowControlBar( m_wndEqualizer, FALSE, FALSE );
	else
        ShowControlBar( m_wndEqualizer, TRUE, FALSE );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewEqualizer(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndEqualizer->IsVisible() );
}

///////////////////////////////////////////////////

size_t CMainFrame::GetTaskCount()
{
	size_t count;

	m_ProtectingTasks.acquire();
		count = m_Tasks.size();
	m_ProtectingTasks.release();

	return count;
}

///////////////////////////////////////////////////

void CMainFrame::OnFileSynchronizeddirectories()
{
	if ( !m_DirSyncDlg )
	{
		m_DirSyncDlg = new CmusikDirSync( this, m_Library );
		m_DirSyncDlg->Create( IDD_DIR_SYNC, this );
		m_DirSyncDlg->ShowWindow( SW_SHOW );
	}
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateFileSynchronizeddirectories(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_DirSyncDlg == NULL );
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnCloseDirSync( WPARAM wParam, LPARAM lParam )
{
	if ( m_DirSyncDlg )
	{
		m_DirSyncDlg->DestroyWindow();
		delete m_DirSyncDlg;
		m_DirSyncDlg = NULL;

		if ( wParam )
			SynchronizeDirs();
	}

	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::InitTrayIcon()
{
	m_TrayIcon.cbSize = sizeof( NOTIFYICONDATA ); 
	m_TrayIcon.hWnd   = GetSafeHwnd(); 
	m_TrayIcon.uID    = ID_NOTIFY_ICON;
	m_TrayIcon.hIcon  = m_hIcon16; 
	m_TrayIcon.uCallbackMessage = TRAY_NOTIFY_EVT; 
	m_TrayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
	strncpy ( m_TrayIcon.szTip, m_Caption, sizeof( m_TrayIcon.szTip ) );

	m_TrayIconVisible = false;
}

///////////////////////////////////////////////////

void CMainFrame::ShowTrayIcon()
{
	if ( !m_TrayIconVisible )
	{
		m_TrayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
		Shell_NotifyIcon( NIM_ADD, &m_TrayIcon );	
		m_TrayIconVisible = true;
	}
}

///////////////////////////////////////////////////

void CMainFrame::HideTrayIcon()
{
	if ( m_TrayIconVisible )
	{
		m_TrayIcon.uFlags = NIF_ICON;
		Shell_NotifyIcon( NIM_DELETE, &m_TrayIcon );
		m_TrayIconVisible = false;
	}
}

///////////////////////////////////////////////////

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// process messages from the tray
	if ( message == TRAY_NOTIFY_EVT )
	{
		switch( lParam )
		{
		case WM_LBUTTONDBLCLK:

			RestoreFromTray();

			return 1L;
			break;

		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:

			CPoint pos;
			::GetCursorPos( &pos );

			CMenu main_menu;
			CMenu* popup_menu;

			main_menu.LoadMenu( IDR_TRAY_MENU );
			popup_menu = main_menu.GetSubMenu( 0 );
			
			SetForegroundWindow();
			popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
			PostMessage( WM_NULL, 0, 0 );

			return 1L;
			break;
		}
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}

///////////////////////////////////////////////////

void CMainFrame::RestoreFromTray()
{
	ShowWindow( SW_NORMAL );
	SetForegroundWindow();
	SetFocus();
	HideTrayIcon();	

	if ( m_AutoStart )
	{
		if ( m_Prefs->SynchronizeOnStartup() )
			SynchronizeDirs();

		m_AutoStart = false;
	}
}

///////////////////////////////////////////////////

void CMainFrame::SynchronizeDirs()
{
	// tasks to add new files
	CmusikStringArray synchs;
	m_Library->GetAllPaths( &synchs, false );
	for ( size_t i = 0; i < synchs.size(); i++ )
	{
		CmusikStringArray* files = new CmusikStringArray();
		CmusikDir scan( synchs.at( i ) + _T( "*.*" ), files );
		scan.Run();

		// setup params
		CmusikBatchAdd* params = new CmusikBatchAdd( files, NULL, m_Library, NULL, m_BatchAddFnct, 0, 0, 1 );
		
		// setup and start task
		m_ProtectingTasks.acquire();

			CmusikBatchAddTask* task = new CmusikBatchAddTask;

			m_Tasks.push_back( task );
			m_TaskCount++;

			task->open( params );

		m_ProtectingTasks.release();
	}	

	// task to remove obselete entries
	CmusikRemoveOld* params = new CmusikRemoveOld( m_Library, m_RemoveOldFnct );
	
	m_ProtectingTasks.acquire();

		CmusikRemoveOldTask* task = new CmusikRemoveOldTask();
		
		m_Tasks.push_back( task );
		m_TaskCount++;

		task->open( params );

	m_ProtectingTasks.release();	
}

///////////////////////////////////////////////////

void CMainFrame::OnViewResetinterfacetodefault()
{
	ResetUI();
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayExit()
{
	RestoreFromTray();
	OnClose();
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayRestore()
{
	RestoreFromTray();
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateNotificationtrayPlay(CCmdUI *pCmdUI)
{
	if ( m_Player->IsPlaying() )
		pCmdUI->Enable( false );
	else
		pCmdUI->Enable( true );
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayPlay()
{
	if ( m_Player->GetPlaylist() )
		m_Player->Play( 0, MUSIK_CROSSFADER_NEW_SONG );
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateNotificationtrayPause(CCmdUI *pCmdUI)
{
	if ( m_Player->IsPlaying() && !m_Player->IsPaused() )
		pCmdUI->SetText( _T( "Pause" ) );
	else
		pCmdUI->SetText( _T( "Resume" ) );	

	if ( m_Player->IsPlaying() )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayPause()
{
	if ( m_Player->IsPlaying() && !m_Player->IsPaused() )
		m_Player->Pause();
	else
		m_Player->Resume();
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateNotificationtrayNext(CCmdUI *pCmdUI)
{
	if ( m_Player->IsPlaying() )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayNext()
{
	if ( m_Player->IsPlaying() )
		m_Player->Next();
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateNotificationtrayPrev(CCmdUI *pCmdUI)
{
	if ( m_Player->IsPlaying() )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayPrev()
{
	if ( m_Player->IsPlaying() )
		m_Player->Prev();
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateNotificationtrayStop(CCmdUI *pCmdUI)
{
	if ( m_Player->IsPlaying() )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}

///////////////////////////////////////////////////

void CMainFrame::OnNotificationtrayStop()
{
	if ( m_Player->IsPlaying() )
		m_Player->Stop();
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSelBoxAddRemove( WPARAM wParam, LPARAM lParam )
{
	BOOL add_new = (BOOL)wParam;
	bool save = false;

	CmusikSelectionBar* pBar;

	if ( add_new )
	{
		pBar = new CmusikSelectionBar( this, m_Library, m_Prefs, MUSIK_LIBRARY_TYPE_ARTIST, 123 + m_wndSelectionBars.size(), m_uSelectionDrop_L, m_uSelectionDrop_R );
		pBar->Create( _T( "musik Selection Box" ), this, ID_SELECTIONBOX_START + m_wndSelectionBars.size() );
		FloatControlBar( pBar, CPoint( 0, 0 ) );

		m_wndSelectionBars.push_back( pBar );
		save = true;

		RequerySelBoxes( NULL, false );
	}

	else
	{
		pBar = (CmusikSelectionBar*)lParam;

		// remove the bar
		for ( size_t i = 0; i < m_Prefs->GetSelBoxCount(); i++ )
		{
			if ( pBar == m_wndSelectionBars.at( i ) )
			{
				m_wndSelectionBars.at( i )->DestroyWindow();
				delete m_wndSelectionBars.at( i );
				m_wndSelectionBars.erase( m_wndSelectionBars.begin() + i );

				save = true;
				break;
			}
		}

		RecalcLayout();
	}

	// save new state to prefs
	if ( save )
	{
		CIntArray new_selboxes;
		for ( size_t i = 0; i < m_wndSelectionBars.size(); i++ )
			new_selboxes.push_back( m_wndSelectionBars.at( i )->GetCtrl()->GetType() );

		m_Prefs->SetSelBoxTypes( new_selboxes );
	}

	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::KillTasks( bool updater, bool fader, bool helpers, bool setwindowtext )
{
	m_ProtectingTasks.acquire();

		if ( updater )
		{
			if ( m_Updater )
			{
				m_Updater->StopWait( 4 );

				delete m_Updater;
				m_Updater = NULL;
			}
		}

		if ( fader )
		{
			if ( m_Fader )
			{
				m_Fader->StopWait( 4 );

				delete m_Fader;
				m_Fader = NULL;
			}
		}

		if ( helpers )
		{
			for ( size_t i = 0; i < m_Tasks.size(); i++ )
			{
				m_Tasks.at( i )->StopWait( 6 );
				delete m_Tasks.at( i );
			}

			m_Tasks.clear();
		}

	m_ProtectingTasks.release();

		if ( setwindowtext )
			SetWindowText( m_Caption );

	TRACE0( "All tasks killed." );
}

///////////////////////////////////////////////////

void CMainFrame::OnFileClearlibrary()
{
	int prompt = MessageBox( "Would you like to also clear synchronized directories, playlists, equalizer, and crossfade settings?", MUSIK_VERSION_STR, MB_ICONWARNING | MB_YESNOCANCEL );

	bool clear = false;
	switch ( prompt )
	{
	case IDYES:
		clear = true;
		break;
	case IDNO:
		clear = false;
		break;
	case IDCANCEL:
		return;
		break;
	}

	// kill all tasks that may be accessing the database
	KillTasks( true, false, true, true );

	// stop player
	m_Player->Stop();
	while ( m_Player->IsPlaying() || m_Player->IsPaused() )
		Sleep( 50 );
	m_Player->GetPlaylist()->Clear();

	// clear
	m_Library->ClearLibrary( clear );

	// send an event that the library
	// was selected...
	m_Playlist->Clear();

	m_wndSources->GetCtrl()->Reset();
	m_wndSources->GetCtrl()->FocusLibrary();
	m_Playlist->Clear();
	OnSourcesLibrary( NULL, NULL );
	ResetSelBoxes();

	// start the updater again
	m_ProtectingTasks.acquire();
		m_Updater = new CMainFrameWorker;
		m_Updater->open( this );
	m_ProtectingTasks.release();
}

///////////////////////////////////////////////////

void CMainFrame::OnLibrarySynchronizedirectoriesnow()
{
	SynchronizeDirs();
}

///////////////////////////////////////////////////

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && GetTaskCount() )
	{
		KillTasks( false, false, true, true );
		return true;
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnUpdateCurrPlaylist( WPARAM wParam, LPARAM lParam )
{
	m_wndView->GetCtrl()->UpdateV();
	m_wndView->GetCtrl()->ScrollToCurr();
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnRestartSoundSystem( WPARAM wParam, LPARAM lParam )
{
	m_Player->Stop();

	while ( m_Player->IsPlaying() )
		Sleep( 50 );

	m_Player->CleanSound();
	m_Player->InitSound( m_Prefs->GetPlayerDevice(), m_Prefs->GetPlayerDriver(), m_Prefs->GetPlayerRate(), m_Prefs->GetPlayerMaxChannels() );

	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::OnPlaybackmodeShufflecurrentplaylist()
{
	m_Player->Shuffle();

	if ( m_wndView->GetCtrl()->GetPlaylist() == m_Player->GetPlaylist() )
	{
		m_wndView->GetCtrl()->UpdateV();
		m_wndView->GetCtrl()->ScrollToCurr();
	}
}

///////////////////////////////////////////////////

LRESULT CMainFrame::OnSelBoxDelSel( WPARAM wParam, LPARAM lParam )
{
	// setup vars
	CmusikSelectionCtrl* pSel = (CmusikSelectionCtrl*)wParam;
	BOOL from_computer = (BOOL)lParam;

	// get selected items into a new
	// playlist
	CmusikPlaylist* playlist = new CmusikPlaylist();
	
	//CmusikString sub_query = pSel->GetSelQuery();
	CmusikString partial_query = GetSelQuery( pSel );
	bool sub_query = true;
	if ( partial_query.Left( 1 ) == _T( "W" ) )
		sub_query = false;

	m_Library->GetRelatedSongs( partial_query, pSel->GetType(), *playlist, sub_query, UseTempTable() );

	// send delete command to library and wait
	m_Library->DeleteSongs( *playlist, from_computer );	

	delete playlist;

	RequerySelBoxes();

	m_wndView->GetCtrl()->GetPlaylist()->Clear();
	m_wndView->GetCtrl()->UpdateV();

	return 0L;
}

///////////////////////////////////////////////////

void CMainFrame::OnUpdatePlaybackmodeShufflecurrentplaylist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_Player->GetPlaylist()->GetCount() );
}

///////////////////////////////////////////////////

void CMainFrame::OnViewVisualization()
{
	if ( m_Prefs->GetPlaylistInfoVizStyle() )
	{
		m_Prefs->SetPlaylistInfoVizStyle( NULL );

		if ( !m_Prefs->PlaylistInfoVisible() )
			m_wndView->DeinitPlaylistInfo();
	}
	else
	{
		m_Prefs->SetPlaylistInfoVizStyle( 1 );
		m_wndView->InitPlaylistInfo();
	}

	// send a "dummy" size event to the
	// window, so it re-creates or removes
	// the now playing window.
	CRect lpRect;
	m_wndView->GetClientRect( &lpRect );
	m_wndView->OnSize( NULL, lpRect.Width(), lpRect.Height() );
	RedrawWindow();
	m_wndView->UpdatePlaylistInfo();

}

///////////////////////////////////////////////////

void CMainFrame::OnUpdateViewVisualization(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_Prefs->GetPlaylistInfoVizStyle() );
}

///////////////////////////////////////////////////
