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
//   CmusikApp
//
// Filename(s): 
//
//   musik.h, musik.cpp
//
// Information:
//
//   musikCube Entry Point
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "MainFrm.h"

#include <OpenThreads/Thread>
using namespace OpenThreads;

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikApp::CmusikApp()
{
	Thread::Init();
}

///////////////////////////////////////////////////

CmusikApp::~CmusikApp()
{
}

///////////////////////////////////////////////////

CmusikApp theApp;

///////////////////////////////////////////////////

CString CmusikApp::GetUserDir()
{
	CString userdir;

	char buffer[2000];
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );

	userdir = buffer;
	return userdir;
}

///////////////////////////////////////////////////

CString CmusikApp::GetWorkingDir( bool include_filename )
{
	CString path;

	char buffer[2000];
	GetModuleFileName( NULL, buffer, sizeof( buffer ) );
	path = buffer;

	if ( !include_filename )
		path = path.Left( path.ReverseFind( '\\' ) );

	return path;
}

///////////////////////////////////////////////////

BOOL CmusikApp::InitInstance()
{
	// only allow a single instance to run
	if ( !CWinAppEx::InitInstance( _T( "{340277AE-C62B-41de-89DF-90191D8950CF}" ) ) )
		return FALSE;

	AfxOleInit();

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinAppEx::InitInstance();

	SetRegistryKey(_T("musikCube"));

	// see if we opened a file
	CString sCmdLine = m_lpCmdLine;
	bool autostart = false;
	bool play_song = false;
	if ( !sCmdLine.IsEmpty() )
	{
		if ( sCmdLine.Left( 1 ) == "\"" )
		{
			sCmdLine.Delete( 0 );
			sCmdLine.Delete( sCmdLine.GetLength() - 1 );
			play_song = true;
		}

		else if ( sCmdLine.Find( "--autostart" ) > -1 )
			autostart = true;
	}

	// such as the name of your company or organization
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame( autostart );
	if ( !pFrame )
		return FALSE;

	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame( IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->SetWindowText( MUSIK_VERSION_STR );

	if ( !autostart )
	{
		pFrame->ShowWindow( SW_SHOW );
		pFrame->UpdateWindow();
	}
	else
	{
		pFrame->ShowWindow( SW_MINIMIZE );
		pFrame->ShowWindow( SW_HIDE );
		pFrame->ShowTrayIcon();
	}

	if ( play_song )
		Play( sCmdLine );

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

///////////////////////////////////////////////////

BOOL CmusikApp::OnAnotherInstanceMessage( LPMSG pMsg )
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
		::GlobalDeleteAtom( (ATOM)pMsg->wParam );	
	}

	Play( m_lpCmdLine );

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikApp::Play( const CString& fn )
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

void CmusikApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

///////////////////////////////////////////////////
