#pragma once

#include "MusikSourcesCtrl.h"
#include "MusikPlaylistCtrl.h"

#include "MusikSourcesBar.h"
#include "MusikPlaylistView.h"
#include "MusikSelectionBar.h"
#include "MusikNowPlayingBar.h"

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
	DECLARE_DYNAMIC(CMainFrame)

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
public:

private:

protected:
	CMusikPlaylistView m_wndView;
	CMusikSourcesBar m_wndSourcesBar;
	CMusikNowPlayingBar m_wndNowPlaying;
	CMusikSelectionBar m_wndSelectionBars[4];

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//
public:
	void DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf );

private:

protected:

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
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};


