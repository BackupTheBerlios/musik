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

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
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
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	CRect client_size;
	GetClientRect( &client_size );

	if ( !m_MainSplit.CreateStatic( this, 1, 2 ) )
	{
		MessageBox( "Error setting up views. Please report this to the Musik development team.", "Musik", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	if ( !m_MainSplit.CreateView( 0, 0, RUNTIME_CLASS( CMusikSourcesCtrl ), CSize( client_size.Width()/2, client_size.Height() ), pContext ) )
	{
		MessageBox( "Error setting up the sources view. Please report this error to the Musik development team.", "Musik", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	if ( !m_MainSplit.CreateView( 0, 1, RUNTIME_CLASS( CMusikPlaylistCtrl ), CSize( client_size.Width()/2, client_size.Height() ), pContext ) )
	{
		MessageBox( "Error setting up the playlist view. Please report this error to the Musik development team.", "Musik", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	m_SourcesCtrl = m_MainSplit.GetPane( 0, 0 );
	m_PlaylistCtrl = m_MainSplit.GetPane( 0, 1 );
	m_Split = true;

	return TRUE;
}

void CMainFrame::OnSize( UINT nType, int cx, int cy )
{
	CFrameWnd::OnSize( nType, cx, cy );
	CRect client_size;
	GetWindowRect( &client_size );

	if ( m_Split && nType != SIZE_MINIMIZED )
	{
		m_MainSplit.SetRowInfo( 0, cy, 0 );
		m_MainSplit.SetColumnInfo( 0, client_size.Width() / 2, 50 );
		m_MainSplit.SetColumnInfo( 0, client_size.Width() / 2, 50 );

		m_MainSplit.RecalcLayout();
	}
}
