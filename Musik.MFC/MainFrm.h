#pragma once

#include "ChildView.h"
#include "MusikSourcesCtrl.h"
#include "MusikPlaylistCtrl.h"

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
	CChildView    m_wndView;	

protected: 
	DECLARE_DYNAMIC(CMainFrame)

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
public:

private:
	CSplitterWnd m_MainSplit;
	CWnd* m_SourcesCtrl;
	CWnd* m_PlaylistCtrl;
	bool m_Split;

protected:

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//
public:

private:

protected:

//-------------------------------------------------//
//--- overrides									---//
//-------------------------------------------------//
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );
	virtual void OnSize( UINT nType, int cx, int cy );

//-------------------------------------------------//
//--- message maps								---//
//-------------------------------------------------//
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
};


