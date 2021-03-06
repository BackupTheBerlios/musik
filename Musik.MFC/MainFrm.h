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

#pragma once

///////////////////////////////////////////////////

#include "MusikSourcesCtrl.h"
#include "MusikPlaylistCtrl.h"
#include "MusikPrefs.h"

#include "MusikSourcesBar.h"
#include "MusikPlaylistView.h"
#include "MusikSelectionBar.h"
#include "MusikNowPlayingBar.h"

///////////////////////////////////////////////////

// these need to remain constant, or
// problems may arise. they are the
// ids for the dock bars

#define ID_NOWPLAYING 1344
#define ID_SOURCESBOX 1345
#define ID_SELECTIONBOX_START 1346

///////////////////////////////////////////////////

// forward declarations, the headers files
// are included in the implmentation	

class CMusikLibrary;
class CMusikPlaylist;
class CMusikPlayer;
class CMusikFrameFunctor;
class CMusikBatchAdd;
class CMusikBatchAddFunctor;

///////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
	friend void CMusikSourcesCtrl::DoDrag( CMusikPropTreeItem* pItem );

public:

	// construct and destruct
	CMainFrame();
	virtual ~CMainFrame();

	// ui related functions
	void ResetUI();
	void ResetSelBoxes();

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
	bool PlayCmd( const CStdString& fn );

	UINT m_uSelectionDrop;
	UINT m_uPlaylistDrop;
	UINT m_uSourcesDrop;

protected: 

	// init and clean musik core components
	void InitPaths();
	void InitMusik();
	void InitDragTypes();
	void CleanMusik();

	void ResetPlaylist();

	// resetting controls
	void LoadDlgSize();

	// icons
	HICON m_hIcon16;
	HICON m_hIcon32;
	HICON m_Tray_Playing;
	HICON m_Tray_Stopped;

	// functor that CMusikPlayer will use
	CMusikFrameFunctor* m_NewSong;

	// current playlist, only a pointer
	CMusikPlaylist* m_LibPlaylist;
	CMusikPlaylist* m_StdPlaylist;
	CMusikPlaylist* m_DynPlaylist;
	CMusikPlaylist* m_NowPlaylist;

	// library, player, and prefs
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// system paths and config files
	CString m_UserDir;
	CString m_Database;
	CString m_PrefsIni;

	// the actual controls
	CMusikPlaylistView*		m_wndView;
	CMusikSourcesBar*		m_wndSources;
	CMusikNowPlayingBar*	m_wndNowPlaying;
	CMusikSelectionBar*		m_wndSelectionBars[4];

	// utility functions
	void DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf );
	bool RecurseMkDir( char* pszDir );

	// custom message maps
	afx_msg LRESULT OnUpdateSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongStop( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesLibrary( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesNowPlaying( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesDynPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragStart(  WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragEnd(  WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxesReset( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPlayerNewPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddEnd( WPARAM wParam, LPARAM lParam );

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnFilePreferences();
	afx_msg void OnDestroy();
	afx_msg void OnOpenFiles();
	afx_msg void OnOpenDirectory();

	// batch add files thread
	CMusikBatchAdd* m_BatchAddThr;
	CMusikBatchAddFunctor* m_BatchAddFnct;

	// macros
	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

