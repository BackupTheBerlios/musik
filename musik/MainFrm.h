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

#pragma once

///////////////////////////////////////////////////

#include "../musikCore/include/musikTask.h"

#include "musikSourcesCtrl.h"
#include "musikNowPlayingCtrl.h"
#include "musikSelectionCtrl.h"
#include "musikEqualizerCtrl.h"

#include "musikPlaylistCtrl.h"
#include "musikPlaylistView.h"

#include "musikPrefs.h"

#include "ace/Task.h"

#include <vector>

///////////////////////////////////////////////////

// these need to remain constant, or
// problems may arise. they are the
// ids for the dock bars

#define ID_CROSSFADER 1342
#define ID_EQUALIZER 1343
#define ID_NOWPLAYING 1344
#define ID_SOURCESBOX 1345
#define ID_SELECTIONBOX_START 1346

///////////////////////////////////////////////////

// task tray notification stuff

#define ID_NOTIFY_ICON 125
#define TRAY_NOTIFY_EVT (WM_USER+1245)

///////////////////////////////////////////////////

enum 
{
	SYSMENU_RESETDIALOG = 85904,
	SYSMENU_PLAYSEL,
	SYSMENU_PAUSERESUME,
	SYSMENU_NEXT,
	SYSMENU_PREV,
	SYSMENU_STOP
};

///////////////////////////////////////////////////

// forward declarations, the headers files
// are included in the implmentation	

class CmusikLibrary;
class CmusikPlaylist;
class CmusikPlayer;
class CmusikFrameFunctor;
class CmusikBatchAddFunctor;
class CmusikRemoveOldFunctor;
class CmusikWinampVisDlg;
class CmusikDirSync;
class CMainFrame;
class ACE_Mutex;

///////////////////////////////////////////////////

// transparency

#ifndef WS_EX_LAYERED
	#define WS_EX_LAYERED           0x00080000
	#define LWA_COLORKEY            0x00000001
	#define LWA_ALPHA               0x00000002
#endif

///////////////////////////////////////////////////

// these are our library imports

///////////////////////////////////////////////////

// transparency

typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, 
								COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

// winamp vis

typedef bool	( WINAPI* lpfnLoadVisPlugins )( LPCTSTR path );
typedef LPSTR	( WINAPI* lpfnGetVisInfo )( int );
typedef UINT	( WINAPI* lpfnGetVisCount )();
typedef void	( WINAPI* lpfnFreeVisInfo )();
typedef void	( WINAPI* lpfnLoadVis )( int );
typedef void	( WINAPI* lpfnFreeVis )( int );
typedef void	( WINAPI* lpfnStopVis )( int );
typedef void	( WINAPI* lpfnStartVis )( int );
typedef void	( WINAPI* lpfnSetVisModule )( int );
typedef UINT	( WINAPI* lpfnGetVisModuleCount )( int );
typedef LPSTR	( WINAPI* lpfnGetVisModuleInfo )( int, int );
typedef HWND	( WINAPI* lpfnGetVisHwnd )();
typedef void	( WINAPI* lpfnSetVisHwnd )( HWND hwnd );
typedef void	( WINAPI* lpfnConfigVis )( int, int );
typedef void	( WINAPI* lpfnSetVisPlaying ) ( int );
typedef void	( WINAPI* lpfnVisSetSongTitle ) ( LPCSTR title );

///////////////////////////////////////////////////

typedef std::vector<CmusikSelectionBar*> CmusikSelBarArray;

///////////////////////////////////////////////////

class CMainFrameWorker : public CmusikTask
{

public:

	int open( void* parent );
	int svc();

private:

	CMainFrame* m_Parent;

};

///////////////////////////////////////////////////

class CMainFrameFader : public CmusikTask
{

public:

	int open( void* parent );
	int svc();

private:

	CMainFrame* m_Parent;

};

///////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{

	// the threads and certain drag and drop operations
	// are allowed access to our private members.
	friend void CmusikSourcesCtrl::DoDrag( CmusikPropTreeItem* pItem );
	friend class CMainFrameFader;

public:

	// construct and destruct
	CMainFrame( bool autostart = false );
	virtual ~CMainFrame();

	// ui related functions
	void ResetUI( bool coordinates_only = false );
	void ResetSelBoxes();

	// overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// debug stuffs
	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	// misc
	bool PlayCmd( const CString& fn );
	void SynchronizeDirs();
	void RestoreFromTray();
	void SaveWindowState();
	void RequeryPlaylist( CmusikSelectionCtrl* sender = NULL, bool prompt_save = true );
	void RequerySelBoxes( CmusikSelectionCtrl* sender = NULL, bool deselect_items = true, bool only_if_parent_exists = false );
	CmusikString GetSelQuery( CmusikSelectionCtrl* target = NULL );
	bool UseTempTable();
	int  GetSelPlaylistType();

	// tray icon
	void ShowTrayIcon();
	void HideTrayIcon();

	// to save window state
	virtual BOOL DestroyWindow();
	BOOL VerifyBarState( LPCTSTR lpszProfileName );

	// unique drag identifiers
	UINT m_uSourcesDrop;
	UINT m_uPlaylistDrop_L;
	UINT m_uPlaylistDrop_R;
	UINT m_uSelectionDrop_L;
	UINT m_uSelectionDrop_R;

	// current window text
	CString m_Caption;

	// task stuff
	size_t GetTaskCount();

	// transparency
	lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;
	void SetTransparency( int trans );
	void InitTrans( bool set_trans = false );
	void DeinitTrans();
	bool IsTransEnb(){ return m_TransEnb; }

protected: 

	// init and clean musik core components
	void InitPaths();
	void Initmusik();
	void InitDragTypes();
	void Cleanmusik();

	// resetting controls
	void LoadDlgSize();

	// icons
	HICON m_hIcon16;
	HICON m_hIcon32;
	HICON m_Tray_Playing;
	HICON m_Tray_Stopped;

	// functor that CmusikPlayer will use
	CmusikFrameFunctor* m_NewSong;

	// current playlist, only a pointer
	void SetPlaylist( bool update_view = true, bool hide_sort_arrow = true, bool select_none = false );
	void SetNowPlaying();
	CmusikPlaylist* m_Playlist;

	// library, player, and prefs
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// system paths and config files
	CString m_UserDir;
	CString m_Database;
	CString m_PrefsIni;

	// the actual controls
	CmusikPlaylistView*		m_wndView;
	CmusikSourcesBar*		m_wndSources;
	CmusikNowPlayingBar*	m_wndNowPlaying;
	CmusikSelBarArray		m_wndSelectionBars;
	CmusikEqualizerBar*		m_wndEqualizer;

	void ShowSelectionBoxes( bool show = true, bool force = false );

	// utility functions
	void DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf );
	bool RecurseMkDir( char* pszDir );
	void GetCrossfader( CmusikCrossfader* fader );

	// custom message maps
	afx_msg LRESULT OnUpdateSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEqualizerChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongStop( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongPause( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongResume( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesLibrary( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesNowPlaying( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesSubLib( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesQuickSearch( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragStart( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragEnd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxEditCommit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnRemoveOldProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTaskEnd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPlayerPlaySel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddNew( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnVerifyPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCloseDirSync( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxAddRemove( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxRequestUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateCurrPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnRestartSoundSystem( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxDelSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesItemClicked( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnInitTrans( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDeinitTrans( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCloseWinampVisConfig( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetWinampVisModules( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnConfigWinampVis( WPARAM wParam, LPARAM lParam );

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnFilePreferences();
	afx_msg void OnDestroy();
	afx_msg void OnAddFiles();
	afx_msg void OnAddDirectory();
	afx_msg void OnFileSaveplaylist();
	afx_msg void OnViewPlaylistinformation();
	afx_msg void OnUpdateViewSources(CCmdUI *pCmdUI);
	afx_msg void OnViewSources();
	afx_msg void OnUpdateViewSelectionboxes(CCmdUI *pCmdUI);
	afx_msg void OnViewNowplaying();
	afx_msg void OnUpdateViewNowplaying(CCmdUI *pCmdUI);
	afx_msg void OnAudioEqualizerEnabled();
	afx_msg void OnUpdateAudioEqualizerEnabled(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAudioCrossfaderEnabled(CCmdUI *pCmdUI);
	afx_msg void OnAudioCrossfaderEnabled();
	afx_msg void OnUpdateViewPlaylistinformation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveplaylist(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlaybackmodeRepeatsingle(CCmdUI *pCmdUI);
	afx_msg void OnPlaybackmodeRepeatsingle();
	afx_msg void OnUpdatePlaybackmodeRepeatplaylist(CCmdUI *pCmdUI);
	afx_msg void OnPlaybackmodeRepeatplaylist();
	afx_msg void OnUpdatePlaybackmodeIntro(CCmdUI *pCmdUI);
	afx_msg void OnPlaybackmodeIntro();
	afx_msg void OnUnsynchronizedtagsView();
	afx_msg void OnUnsynchronizedtagsWritetofile();
	afx_msg void OnUnsynchronizedtagsFinalizefordatabaseonly();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnViewEqualizer();
	afx_msg void OnUpdateViewEqualizer(CCmdUI *pCmdUI);
	afx_msg void OnFileSynchronizeddirectories();
	afx_msg void OnUpdateFileSynchronizeddirectories(CCmdUI *pCmdUI);
	afx_msg void OnViewResetinterfacetodefault();
	afx_msg void OnNotificationtrayExit();
	afx_msg void OnNotificationtrayRestore();
	afx_msg void OnUpdateNotificationtrayPlay(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayPlay();
	afx_msg void OnUpdateNotificationtrayPause(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayPause();
	afx_msg void OnUpdateNotificationtrayNext(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayNext();
	afx_msg void OnUpdateNotificationtrayPrev(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayPrev();
	afx_msg void OnUpdateNotificationtrayStop(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayStop();
	afx_msg void OnFileClearlibrary();
	afx_msg void OnLibrarySynchronizedirectoriesnow();
	afx_msg void OnPlaybackmodeShufflecurrentplaylist();
	afx_msg void OnUpdatePlaybackmodeShufflecurrentplaylist(CCmdUI *pCmdUI);
	afx_msg void OnViewVisualization();
	afx_msg void OnUpdateViewVisualization(CCmdUI *pCmdUI);
	afx_msg void OnViewAlwaysontop();
	afx_msg void OnUpdateViewAlwaysontop(CCmdUI *pCmdUI);
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI *pCmdUI);
	afx_msg void OnWinampvisualizationsConfigure();
	afx_msg void OnUpdateWinampvisualizationsConfigure(CCmdUI *pCmdUI);
	afx_msg void OnWinampvisualizationsEnabled();
	afx_msg void OnUpdateWinampvisualizationsEnabled(CCmdUI *pCmdUI);

	// list of all taskss running
	int FreeTask( CmusikTask* pTask );
	CmusikTaskPtrArray m_Tasks;

	// mutex to synchronize task array
	ACE_Mutex m_ProtectingTasks;
	int m_TaskCount;
	void KillTasks( bool updater = true, bool fader = true, bool childtasks = true, bool setwindowtext = true );

	// batch functors tasks will use
	CmusikBatchAddFunctor* m_BatchAddFnct;
	CmusikRemoveOldFunctor* m_RemoveOldFnct;
	CMainFrameWorker* m_Updater;
	CMainFrameFader* m_Fader;

	// dir sync dialog
	CmusikDirSync* m_DirSyncDlg;

	// macros
	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_MESSAGE_MAP()

	// tray icon
	NOTIFYICONDATA m_TrayIcon;
	bool m_TrayIconVisible;
	bool m_SelBoxesVisible;
	void InitTrayIcon();

	// full screen
	bool m_FullScreen;
	bool m_GoingFullScreen;
	void GoFullScreen();
	void RetFullScreen(); 

	// various
	bool m_PlaylistSel;
	bool m_AutoStart;
	bool m_TransEnb;
	int m_Trans;
	void ImportTrans();

	// winamp vis emulation
	bool m_WinampVisActive;
	void ImportWinamp();
	void DeinitWinamp();
	void GetVisList();
	HMODULE m_WinampVis;

	CmusikStringArray m_VisList;
	lpfnLoadVisPlugins visLoadVisPlugins;
	lpfnGetVisInfo visGetVisInfo;
	lpfnGetVisCount visGetVisCount;
	lpfnFreeVisInfo visFreeVisInfo;
	lpfnStartVis visStartVis;
	lpfnSetVisModule visSetVisModule;
	lpfnGetVisModuleCount visGetVisModuleCount;
	lpfnGetVisModuleInfo visGetVisModuleInfo;
	lpfnLoadVis visLoadVis;
	lpfnFreeVis visFreeVis;
	lpfnStopVis visStopVis;
	lpfnGetVisHwnd visGetVisHwnd;
	lpfnSetVisHwnd visSetVisHwnd;
	lpfnConfigVis visConfigVis;
	lpfnSetVisPlaying visSetVisPlaying;
	lpfnVisSetSongTitle visSetSongTitle;

	CmusikWinampVisDlg* m_VisDlg;

public:
	afx_msg void OnWinampvisualizationsActivate();
};

///////////////////////////////////////////////////

