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
// problems may arise.

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

///////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMainFrame();
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected: 
	void InitPaths();
	void InitMusik();

	void CleanMusik();

	DECLARE_DYNAMIC(CMainFrame)

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
public:

private:
	HICON m_hIcon16;
	HICON m_hIcon32;
	HICON m_Tray_Playing;
	HICON m_Tray_Stopped;

protected:
	CMusikFrameFunctor* m_NewSong;

	CMusikPlaylist* m_LibPlaylist;
	CMusikPlaylist* m_DynPlaylist;
	CMusikPlaylist* m_StdPlaylist;

	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	CString m_UserDir;
	CString m_Database;
	CString m_PrefsIni;

	CMusikPlaylistView*		m_wndView;
	CMusikSourcesBar*		m_wndSources;
	CMusikNowPlayingBar*	m_wndNowPlaying;
	CMusikSelectionBar*		m_wndSelectionBars[4];

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//
public:
	void ResetDialogRect();
	void ResetNowPlaying();

protected:
	void DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf );
	bool RecurseMkDir( char* pszDir );

private:

//-------------------------------------------------//
//--- overrides									---//
//-------------------------------------------------//
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );

//-------------------------------------------------//
//--- message maps								---//
//-------------------------------------------------//
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUpdateSel( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

///////////////////////////////////////////////////

