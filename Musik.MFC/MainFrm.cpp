// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Musik.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_Split = false;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

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


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	CRect client_size;
	GetClientRect( &client_size );

	//-------------------------------------------------//
	//--- separates the top of the dialog with the	---//
	//--- now playing portion.						---//
	//--- +--------+								---//
	//--- |        |								---//
	//--- |        | 0,0							---//
	//--- +--------+								---//
	//--- +--------+ 1,0							---//
	//-------------------------------------------------//
	m_MainSplit.CreateStatic( this, 2, 1 );
	m_MainSplit.SetRowInfo( 0, client_size.Height() - 72, 72 );
	m_MainSplit.CreateView( 1, 0, RUNTIME_CLASS( CMusikSourcesView ), CSize( client_size.Width(), 100 ), pContext );

	//-------------------------------------------------//
	//--- separates the sources area and the right	---//
	//--- half of the top							---//
	//--- 0,0  0,1									---//
	//--- +-+------+								---//
	//--- | |      |								---//
	//--- | |      |								---//
	//--- +-+------+								---//
	//--- +--------+								---//
	//-------------------------------------------------//
	m_SourcesSplit.CreateStatic( &m_MainSplit, 1, 2, WS_CHILD | WS_VISIBLE, m_MainSplit.IdFromRowCol( 0, 0 ) );
	m_SourcesSplit.CreateView( 0, 0, RUNTIME_CLASS( CMusikSourcesView ), CSize( 100, client_size.Height() ), pContext );
	m_SourcesSplit.SetColumnInfo( 0, 128, 128 );

	//-------------------------------------------------//
	//--- separates the selection area and the		---//
	//--- playlist									---//
	//--- +-+------+								---//
	//--- | +------| 0,0							---//
	//--- | |      | 1,0							---//
	//--- +-+------+								---//
	//--- +--------+								---//
	//-------------------------------------------------//
	m_PlaylistSplit.CreateStatic( &m_SourcesSplit, 2, 1, WS_CHILD | WS_VISIBLE, m_SourcesSplit.IdFromRowCol( 0, 1 ) );
	m_PlaylistSplit.CreateView( 0, 0, RUNTIME_CLASS( CMusikSourcesView ), CSize( 100, 100 ), pContext );
	m_PlaylistSplit.CreateView( 1, 0, RUNTIME_CLASS( CMusikSourcesView ), CSize( 100, 100 ), pContext );

	m_Split = true;

	return TRUE;
}

void CMainFrame::OnSize( UINT nType, int cx, int cy )
{
	CFrameWnd::OnSize( nType, cx, cy );

}
