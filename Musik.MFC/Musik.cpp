///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"

#include "../Musik.Core/include/MusikFilename.h"

#include "MainFrm.h"

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
	CString sCmd = ParseCmd();
	
	// only allow a single instance of Musik to run
	if ( !CSingleInstance::Create( _T( "7f1832d0-f3be-4a1c-be03-a4b0f70998e2" ) ) )
		return FALSE;	

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

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	// see if we opened a file
	if ( !sCmd.IsEmpty() )
		Play( sCmd );

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

///////////////////////////////////////////////////

void CMusikApp::WakeUp( LPCSTR aCommandLine )
{
	CString sCmd = ParseCmd();
	if ( !sCmd.IsEmpty() )
		Play( sCmd );
}

///////////////////////////////////////////////////

void CMusikApp::Play( const CString& fn )
{
	if ( m_pMainWnd )
	{
		// see if file is in library
		// add song to playlist
		// set player's playlist
		// startup playback
	}
}

///////////////////////////////////////////////////

CString CMusikApp::ParseCmd( CString sCmd )
{
	if ( sCmd.IsEmpty() )
		sCmd = GetCommandLine();

	CStdString sStr( sCmd );
	
	int nLastQ = sStr.ReverseFind( "\"", sStr.GetLength() - 1 );
	int nFirstQ = sStr.ReverseFind( "\"", nLastQ - 1 );

	sStr = sStr.Left( nLastQ );
	sStr = sStr.Right( nLastQ - ( nFirstQ + 1 ) );

	if ( !sStr.IsEmpty() )
	{
		CMusikFilename MFN ( sStr );
		CStdString sExt = MFN.GetExtension();

		if ( sExt != "mp3" && sExt != "ogg" )
			return _T( "" );
	}

	return (CString)sStr;
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