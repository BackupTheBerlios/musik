///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"

#include "../Musik.Core/include/MusikFilename.h"

#include "MainFrm.h"
#include ".\musik.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMusikApp::CMusikApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

///////////////////////////////////////////////////

CMusikApp theApp;

///////////////////////////////////////////////////

BOOL CMusikApp::InitInstance()
{
	// only allow a single instance to run
	if ( !CWinAppEx::InitInstance( _T( "{340277AE-C62B-41de-89DF-90191D8950CF}" ) ) )
		return FALSE;

	AfxEnableControlContainer();
	AfxOleInit();

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	SetRegistryKey(_T("Musik.MFC"));

	// such as the name of your company or organization
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;

	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame( IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	pFrame->m_uMsgCheckInst = m_uMsgCheckInst;

	// The one and only window has been initialized, so show and update it
	pFrame->SetWindowText( MUSIK_VERSION_STR );
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	// see if we opened a file
	CCommandLineInfo cmd;
	ParseCommandLine( cmd );
	if ( !cmd.m_strFileName.IsEmpty() )
		pFrame->PlayCmd( (CStdString)cmd.m_strFileName );

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

///////////////////////////////////////////////////

BOOL CMusikApp::OnAnotherInstanceMessage( LPMSG pMsg )
{
	// make sure the sender is the main frame...
	if ( m_pMainWnd )
	{
		if ( pMsg->hwnd != m_pMainWnd->GetSafeHwnd() )
			return false;
	}

	if( pMsg->wParam != NULL ) 
	{
		// get the contents of the atom, in
		// this case, the command line sent
		// to the last instance of the program
		::GlobalGetAtomName( (ATOM)pMsg->wParam, m_lpCmdLine, _MAX_FNAME );	

		// delete the atom
		int nChecker = 0;
		::SetLastError( ERROR_SUCCESS );
		while ( ::GetLastError() == ERROR_SUCCESS )
		{
			::GlobalDeleteAtom(  (ATOM)pMsg->wParam );	
			nChecker++;
			if ( nChecker > 100 )
				break;
		}
		if ( nChecker > 100 )
			TRACE0( "There appears to be a problem removing the global ATOM.\n" );
	}

	Play( m_lpCmdLine );

	return TRUE;
}

///////////////////////////////////////////////////

void CMusikApp::Play( const CStdString& fn )
{
	if ( m_pMainWnd )
	{
		CMainFrame* pMain = (CMainFrame*) m_pMainWnd;
		pMain->PlayCmd( fn );
	}
}

///////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:

	// construct and destruct
	CAboutDlg();

	// enumberations
	enum { IDD = IDD_ABOUTBOX };

protected:

	// overides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

///////////////////////////////////////////////////

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

///////////////////////////////////////////////////
