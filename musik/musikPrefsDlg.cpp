///////////////////////////////////////////////////

#include "stdafx.h"

#include "fmod.h"

#include "musik.h"
#include "musikPrefsDlg.h"

#include "../musikCore/include/musikFilename.h"
#include ".\musikprefsdlg.h"

///////////////////////////////////////////////////

#pragma warning (disable : 4800)	// forcing value bool 'true' to 'false'

///////////////////////////////////////////////////

// General::Interface

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPrefsInterfaceGeneral, CmusikPropertyPage )

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

void CmusikPrefsInterfaceGeneral::DoDataExchange( CDataExchange* pDX )
{
	CmusikPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLETRAYICON, m_EnableTrayIcon);
	DDX_Control(pDX, IDC_CHECK1, m_PromptSavePlaylists);
	DDX_Control(pDX, IDC_CHECK2, m_PromptDropOnPlaylist);
	DDX_Control(pDX, IDC_CHECK_STARTWITHWINDOWS, m_StartWithWindows);
	DDX_Control(pDX, IDC_CHECK4, m_AutoSynchronize);
	DDX_Control(pDX, IDC_CHECK3, m_LibraryShowAll);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CmusikPrefsInterfaceGeneral, CmusikPropertyPage )
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
	m_LibraryShowAll.SetCheck( m_Prefs->LibraryShowsAllSongs() );
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::CommitChanges()
{
	m_Prefs->SetMinimizeToTray( m_EnableTrayIcon.GetCheck() );

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

	m_Prefs->SetSynchronizeOnStartup( m_AutoSynchronize.GetCheck() );
	m_Prefs->SetLibraryShowsAllSongs( m_LibraryShowAll.GetCheck() );
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

// Sound::Driver

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPrefsSoundDriver, CmusikPropertyPage )

///////////////////////////////////////////////////

CmusikPrefsSoundDriver::CmusikPrefsSoundDriver( CmusikPrefs* prefs )
	: CmusikPropertyPage( CmusikPrefsSoundDriver::IDD, prefs )
{
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikPrefsSoundDriver::~CmusikPrefsSoundDriver()
{
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_SOUND_DRIVER, m_SoundDriver);
	DDX_Control(pDX, IDC_SOUND_DEVICE, m_SoundDevice);
	DDX_Control(pDX, IDC_SOUND_PLAYBACK_RATE, m_SoundPlaybackRate);
	DDX_Control(pDX, IDC_SOUND_MAX_CHANNELS, m_SoundMaxChannels);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CmusikPrefsSoundDriver, CmusikPropertyPage )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikPrefsSoundDriver::OnInitDialog()
{
	CmusikPropertyPage::OnInitDialog();

	LoadPrefs();

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::LoadPrefs()
{
	GetSoundDevices();
	GetSoundDrivers();
	GetSoundPlaybackRates();

	CString tmp;
	tmp.Format( _T( "%d" ), m_Prefs->GetPlayerMaxChannels() );
	m_SoundMaxChannels.SetWindowText( tmp );
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::CommitChanges()
{
	CString tmp;

	int orig_device	= m_Prefs->GetPlayerDevice();
	int orig_driver	= m_Prefs->GetPlayerDriver();
	int orig_rate	= m_Prefs->GetPlayerRate();
	int orig_chan	= m_Prefs->GetPlayerMaxChannels();

	int new_device	= m_SoundDevice.GetCurSel();
	int new_driver	= m_SoundDriver.GetCurSel();

	m_SoundPlaybackRate.GetWindowText( tmp );
    int new_rate	= atoi( tmp.GetBuffer() );

	m_SoundMaxChannels.GetWindowText( tmp );
	int new_chan	= atoi( tmp.GetBuffer() );

	if ( new_chan <= 0 )
		new_chan = orig_chan;

	bool fmod_needs_restart = false;
	if ( orig_device != new_device || orig_driver != new_driver || orig_chan != new_chan || orig_rate != new_rate )
		fmod_needs_restart = true;

	m_Prefs->SetPlayerDevice( new_device );
	m_Prefs->SetPlayerDriver( new_driver );
	m_Prefs->SetPlayerRate( new_rate );
	m_Prefs->SetPlayerMaxChannels( new_chan );

	if ( fmod_needs_restart )
	{
		int res = MessageBox( "The sound system must be stopped and restarted to apply the new changes. If you do not to restart now, the changes will not take effect until musikCube as been restarted. Restart sound system?", MUSIK_VERSION_STR, MB_ICONINFORMATION | MB_YESNO );
		if ( res == IDYES )
		{
			int WM_RESTARTSOUNDSYSTEM = RegisterWindowMessage( "RESTARTSOUNDSYSTEM" );
			AfxGetApp()->m_pMainWnd->SendMessage( WM_RESTARTSOUNDSYSTEM );
		}	
	}
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::GetSoundDevices( bool populate )
{
	m_DeviceInfo.clear();

	int num_drivers = FSOUND_GetNumDrivers();

	for ( int i = 0; i < num_drivers; i++ )
		m_DeviceInfo.push_back( FSOUND_GetDriverName( i ) );

	if ( !m_DeviceInfo.size() )
		m_DeviceInfo.push_back( _T( "(no devices found)" ) );

	if ( populate )
	{
		m_SoundDevice.Clear();
		for ( size_t i = 0; i < m_DeviceInfo.size(); i++ )
			m_SoundDevice.InsertString( i, m_DeviceInfo.at( i ) );

		if ( m_DeviceInfo.size() )
		{
			if ( m_Prefs->GetPlayerDevice() < (int)m_DeviceInfo.size() )
				m_SoundDevice.SelectString( -1, m_DeviceInfo.at( m_Prefs->GetPlayerDevice() ) );
			else
				m_SoundDevice.SelectString( -1, m_DeviceInfo.at( 0 ) );
		}
	}
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::GetSoundDrivers( bool populate )
{
	m_DriverInfo.clear();

	m_DriverInfo.push_back( _T( "Direct Sound" ) );
	m_DriverInfo.push_back( _T( "Windows Waveform" ) );
	m_DriverInfo.push_back( _T( "ASIO" ) );

	if ( populate )
	{
		m_SoundDriver.Clear();

		for ( size_t i = 0; i < m_DriverInfo.size(); i++ )
			m_SoundDriver.InsertString( i, m_DriverInfo.at( i ) );

		if ( m_DriverInfo.size() )
		{
			if ( m_Prefs->GetPlayerDriver() < (int)m_DriverInfo.size() )
				m_SoundDriver.SelectString( -1, m_DriverInfo.at( m_Prefs->GetPlayerDriver() ) );
			else
				m_SoundDriver.SelectString( -1, m_DriverInfo.at( 0 ) );
		}
	}
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::GetSoundPlaybackRates( bool populate )
{
	m_PlaybackRateInfo.clear();

	m_PlaybackRateInfo.push_back( _T( "8000" ) );
	m_PlaybackRateInfo.push_back( _T( "11025" ) );
	m_PlaybackRateInfo.push_back( _T( "22050" ) );
	m_PlaybackRateInfo.push_back( _T( "44100" ) );

	if ( populate )
	{
		m_SoundPlaybackRate.Clear();

		for ( size_t i = 0; i < m_PlaybackRateInfo.size(); i++ )
			m_SoundPlaybackRate.InsertString( i, m_PlaybackRateInfo.at( i ) );

		if ( m_PlaybackRateInfo.size() )
		{
			CString playback_mode_str;
			playback_mode_str.Format( _T( "%d" ), m_Prefs->GetPlayerRate() );

			m_SoundPlaybackRate.SelectString( -1, playback_mode_str );
		}
	}
}

///////////////////////////////////////////////////

BOOL CmusikPrefsSoundDriver::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );
	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////