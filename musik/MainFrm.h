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

#pragma once

///////////////////////////////////////////////////

#include "musikSourcesCtrl.h"
#include "musikNowPlayingCtrl.h"
#include "musikSelectionCtrl.h"
#include "musikCrossfaderCtrl.h"
#include "musikEqualizerCtrl.h"

#include "musikPlaylistCtrl.h"
#include "musikPlaylistView.h"

#include "musikPrefs.h"

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
class CmusikDirSync;
class ACE_Thread_Mutex;

///////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
	friend void CmusikSourcesCtrl::DoDrag( CmusikPropTreeItem* pItem );
	friend static void MainFrameWorker( CmusikThread* thread );

public:

	// construct and destruct
	CMainFrame();
	virtual ~CMainFrame();

	// ui related functions
	void ResetUI();
	void ResetSelBoxes( bool requery = true, bool resetparent = false );

	// overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );

	// debug stuffs
	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	// misc
	bool PlayCmd( const CString& fn );
	void SynchronizeDirs();
	void RestoreFromTray();

	// to save window state
	virtual BOOL DestroyWindow();
	BOOL VerifyBarState( LPCTSTR lpszProfileName );

	// unique drag identifiers
	UINT m_uSelectionDrop;
	UINT m_uPlaylistDrop;
	UINT m_uSourcesDrop;

	// current window text
	CString m_Caption;

	// thread stuff
	size_t GetThreadCount();

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
	CmusikPlaylist* m_LibPlaylist;
	CmusikPlaylist* m_StdPlaylist;

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
	CmusikSelectionBar*		m_wndSelectionBars[12];
	CmusikCrossfaderBar*	m_wndCrossfader;
	CmusikEqualizerBar*		m_wndEqualizer;

	// utility functions
	void DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf );
	bool RecurseMkDir( char* pszDir );
	void GetCrossfader( CmusikCrossfader* fader );

	// custom message maps
	afx_msg LRESULT OnUpdateSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongStop( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongPauseResume( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesLibrary( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesNowPlaying( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesDynPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesQuickSearch( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragStart(  WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragEnd(  WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxesReset( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxEditCommit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPlayerNewPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnRemoveOldProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThreadEnd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPlayerPlaySel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddNew( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnVerifyPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCloseDirSync( WPARAM wParam, LPARAM lParam );

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
	afx_msg void OnViewSelectionboxes();
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
	afx_msg void OnUpdateViewCrossfader(CCmdUI *pCmdUI);
	afx_msg void OnViewCrossfader();
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
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	// list of all threads running
	bool FreeThread( CmusikThread* pThread );
	CmusikThreadPtrArray m_Threads;

	// mutex to synchronize thread array
	ACE_Thread_Mutex* m_ProtectingThreads;
	int m_ThreadCount;

	// batch functors threads will use
	CmusikBatchAddFunctor* m_BatchAddFnct;
	CmusikRemoveOldFunctor* m_RemoveOldFnct;
	CmusikThread* m_Updater;

	// dir sync dialog
	CmusikDirSync* m_DirSyncDlg;

	// macros
	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_MESSAGE_MAP()

	// tray icon
	NOTIFYICONDATA m_TrayIcon;
	bool m_TrayIconVisible;
	void InitTrayIcon();
	void ShowTrayIcon();
	void HideTrayIcon();
};

///////////////////////////////////////////////////

