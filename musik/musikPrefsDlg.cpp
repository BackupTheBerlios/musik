///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikPrefsDlg.h"

#include "../musikCore/include/musikFilename.h"

///////////////////////////////////////////////////

// General::Interface

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceGeneral, CPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceGeneral::CmusikPrefsInterfaceGeneral( CmusikPrefs* prefs )
	: CmusikPropertyPage( CmusikPrefsInterfaceGeneral::IDD, prefs )
{
}

///////////////////////////////////////////////////

CmusikPrefsInterfaceGeneral::~CmusikPrefsInterfaceGeneral()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLETRAYICON, m_EnableTrayIcon);
	DDX_Control(pDX, IDC_CHECK1, m_PromptSavePlaylists);
	DDX_Control(pDX, IDC_CHECK2, m_PromptDropOnPlaylist);
	DDX_Control(pDX, IDC_CHECK_STARTWITHWINDOWS, m_StartWithWindows);
	DDX_Control(pDX, IDC_CHECK4, m_AutoSynchronize);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceGeneral, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceGeneral::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::LoadPrefs()
{
	m_EnableTrayIcon.SetCheck( m_Prefs->MinimizeToTray() );
	m_PromptSavePlaylists.SetCheck( m_Prefs->GetStdPlaylistPrompt() == -1 ? true : false );
	m_PromptDropOnPlaylist.SetCheck( m_Prefs->GetFileDropPrompt() == -1 ? true : false );
	m_StartWithWindows.SetCheck( GetStartup() );
	m_AutoSynchronize.SetCheck( m_Prefs->SynchronizeOnStartup() );
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::CommitChanges()
{
	m_Prefs->SetMinimizeToTray( m_EnableTrayIcon.GetCheck() );
	m_Prefs->SetSynchronizeOnStartup( m_AutoSynchronize.GetCheck() );

	if ( !m_PromptSavePlaylists.GetCheck() )
	{
		if ( m_Prefs->GetStdPlaylistPrompt() == -1 )
			m_Prefs->SetStdPlaylistPrompt( 1 );
	}
	else
		m_Prefs->SetStdPlaylistPrompt( -1 );

	if ( !m_PromptDropOnPlaylist.GetCheck() )
	{
		if ( m_Prefs->GetFileDropPrompt() == -1 )
			m_Prefs->SetFileDropPrompt( 1 );
	}
	else
		m_Prefs->SetFileDropPrompt( -1 );

	if ( m_StartWithWindows.GetCheck() )
		EnableStartup();
	else
		DisableStartup();
}

///////////////////////////////////////////////////

bool CmusikPrefsInterfaceGeneral::GetStartup()
{
	CmusikString userdir;

	char buffer[2000];
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );

	userdir = buffer;
	userdir += _T( "\\Start Menu\\Programs\\Startup\\musikCube.lnk" );
	return CmusikFilename::FileExists( userdir );
}

void CmusikPrefsInterfaceGeneral::EnableStartup()
{
	CmusikString shortcut_path, program_path;
	char buffer[2000];

	// where we want to store the startup shortcut
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );
	shortcut_path = buffer;
	shortcut_path += _T( "\\Start Menu\\Programs\\Startup\\musikCube.lnk" );
	
	if ( CmusikFilename::FileExists( shortcut_path ) )
		return;

	// path to musik's exe
	GetModuleFileName( NULL, buffer, sizeof( buffer ) );
	program_path = buffer;

	// start code 
	HRESULT hRes = E_FAIL;
    DWORD dwRet = 0;
    CComPtr<IShellLink> ipShellLink;

	// buffer that receives the null-terminated string 
	// for the drive and path
    TCHAR szPath[MAX_PATH];

	// buffer that receives the address of the final 
	// file name component in the path
    LPTSTR lpszFilePart;    
    WCHAR wszTemp[MAX_PATH];
        
    // Retrieve the full path and file name of a specified file
    dwRet = GetFullPathName( program_path.c_str(), sizeof(szPath) / sizeof(TCHAR), szPath, &lpszFilePart );

	if ( !dwRet )                                        
		return;

    // Get a pointer to the IShellLink interface
    hRes = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&ipShellLink );
                             
    if ( SUCCEEDED( hRes ) )
    {
        // Get a pointer to the IPersistFile interface
		CComQIPtr<IPersistFile> ipPersistFile( ipShellLink );

        // Set the path to the shortcut target and add the description
		hRes = ipShellLink->SetPath( szPath );
		if ( FAILED( hRes ) )
			return;

		hRes = ipShellLink->SetArguments( "--autostart" );
		if ( FAILED( hRes ) )
			return;		

		hRes = ipShellLink->SetDescription( "musikCube" );
		if ( FAILED( hRes ) )
			return;

		// IPersistFile is using LPCOLESTR, so make sure 
		// that the string is Unicode
		#if !defined _UNICODE
			MultiByteToWideChar( CP_ACP, 0, shortcut_path.c_str(), -1, wszTemp, MAX_PATH );
		#else
			wcsncpy( wszTemp, shortcut_path.c_str(), MAX_PATH );
		#endif

        // Write the shortcut to disk
		hRes = ipPersistFile->Save( wszTemp, TRUE );
    }
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::DisableStartup()
{
	CmusikString shortcut_path, program_path;
	char buffer[2000];

	// where we want to store the startup shortcut
	GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );
	shortcut_path = buffer;
	shortcut_path += _T( "\\Start Menu\\Programs\\Startup\\musikCube.lnk" );
	
	if ( CmusikFilename::FileExists( shortcut_path ) )
		remove( shortcut_path.c_str() );

}

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceGeneral::OnInitDialog()
{
	CmusikPropertyPage::OnInitDialog();
	LoadPrefs();
	return TRUE; 
}

///////////////////////////////////////////////////
