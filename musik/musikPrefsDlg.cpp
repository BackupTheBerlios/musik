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
//   CmusikPrefsInterfaceWorkflow,
//   CmusikPrefsSoundCrossfader,
//   CmusikPrefsSoundDriver
//
// Filename(s): 
//
//   musikPrefsDlg.h, musikPrefsDlg.cpp
//
// Information:
//
//   All the stuff relating to preferences.
// 
// Usage: 
//
//   Void.
//
// Some code adapated / borrowed from...
//
//   http://www.codeproject.com/shell/create_shortcut.asp?target=shortcut (create shortcut)
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "fmod.h"

#include "musik.h"
#include "musikPrefsDlg.h"
#include "musikNameEntry.h"

#include "../musikCore/include/musikFilename.h"
#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"
#include ".\musikprefsdlg.h"

///////////////////////////////////////////////////

class CmusikPropertyPage;

///////////////////////////////////////////////////

#pragma warning (disable : 4800)	// forcing value bool 'true' to 'false'

///////////////////////////////////////////////////

// Interface::Workflow

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPrefsInterfaceWorkflow, CmusikPropertyPage )

///////////////////////////////////////////////////

CmusikPrefsInterfaceWorkflow::CmusikPrefsInterfaceWorkflow( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player )
	: CmusikPropertyPage( CmusikPrefsInterfaceWorkflow::IDD, prefs, library, player )
{
}

///////////////////////////////////////////////////

CmusikPrefsInterfaceWorkflow::~CmusikPrefsInterfaceWorkflow()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceWorkflow::DoDataExchange( CDataExchange* pDX )
{
	CmusikPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLETRAYICON, m_EnableTrayIcon);
	DDX_Control(pDX, IDC_CHECK1, m_PromptSavePlaylists);
	DDX_Control(pDX, IDC_CHECK2, m_PromptDropOnPlaylist);
	DDX_Control(pDX, IDC_CHECK_STARTWITHWINDOWS, m_StartWithWindows);
	DDX_Control(pDX, IDC_CHECK4, m_AutoSynchronize);
	DDX_Control(pDX, IDC_CHECK3, m_LibraryShowAll);
	DDX_Control(pDX, IDC_CHECK6, m_AddEntireToNP);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CmusikPrefsInterfaceWorkflow, CmusikPropertyPage )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceWorkflow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );
	m_Modified = true;

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceWorkflow::LoadPrefs()
{
	m_EnableTrayIcon.SetCheck( m_Prefs->MinimizeToTray() );
	m_PromptSavePlaylists.SetCheck( m_Prefs->GetStdPlaylistPrompt() == -1 ? true : false );
	m_PromptDropOnPlaylist.SetCheck( m_Prefs->GetFileDropPrompt() == -1 ? true : false );
	m_StartWithWindows.SetCheck( GetStartup() );
	m_AutoSynchronize.SetCheck( m_Prefs->SynchronizeOnStartup() );
	m_LibraryShowAll.SetCheck( m_Prefs->LibraryShowsAllSongs() );
	m_AddEntireToNP.SetCheck( m_Prefs->AddEntireToNP() );
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceWorkflow::CommitChanges()
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
	m_Prefs->SetAddEntireToNP( m_AddEntireToNP.GetCheck() );

	m_Modified = false;
	SetModified( FALSE );
}

///////////////////////////////////////////////////

bool CmusikPrefsInterfaceWorkflow::GetStartup()
{
	CmusikApp* app = (CmusikApp*)AfxGetApp();
	CmusikString userdir = app->GetUserDir();
	userdir += _T( "\\Start Menu\\Programs\\Startup\\musikCube.lnk" );

	return CmusikFilename::FileExists( userdir );
}

void CmusikPrefsInterfaceWorkflow::EnableStartup()
{
	CmusikApp* app = (CmusikApp*)AfxGetApp();

	// where we want to store the startup shortcut
	CmusikString shortcut_path;
	shortcut_path = app->GetUserDir();
	shortcut_path += _T( "\\Start Menu\\Programs\\Startup\\musikCube.lnk" );
	
	if ( CmusikFilename::FileExists( shortcut_path ) )
		return;

	// path to musik's exe
	CmusikString program_path;
	program_path = app->GetWorkingDir( true );

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

void CmusikPrefsInterfaceWorkflow::DisableStartup()
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

BOOL CmusikPrefsInterfaceWorkflow::OnInitDialog()
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

CmusikPrefsSoundDriver::CmusikPrefsSoundDriver( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player )
	: CmusikPropertyPage( CmusikPrefsSoundDriver::IDD, prefs, library, player )
{
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

	// note: we could restart using m_Player but
	// to be safe we'll post the event to the main
	// frame so it can update itself.

	if ( fmod_needs_restart )
	{
		int res = MessageBox( "The sound system must be stopped and restarted to apply the new changes. If you do not to restart now, the changes will not take effect until musikCube as been restarted. Restart sound system?", MUSIK_VERSION_STR, MB_ICONINFORMATION | MB_YESNO );
		if ( res == IDYES )
		{
			int WM_RESTARTSOUNDSYSTEM = RegisterWindowMessage( "RESTARTSOUNDSYSTEM" );
			AfxGetApp()->m_pMainWnd->SendMessage( WM_RESTARTSOUNDSYSTEM );
		}	
	}

	m_Modified = false;
	SetModified( FALSE );
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
	m_Modified = true;

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////

// Sound::Crossfader

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPrefsSoundCrossfader, CmusikPropertyPage )

///////////////////////////////////////////////////

CmusikPrefsSoundCrossfader::CmusikPrefsSoundCrossfader( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player )
	: CmusikPropertyPage( CmusikPrefsSoundCrossfader::IDD, prefs, library, player )
{
}

///////////////////////////////////////////////////

CmusikPrefsSoundCrossfader::~CmusikPrefsSoundCrossfader()
{
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEWSONG, m_NewSong);
	DDX_Control(pDX, IDC_PAUSERESUME, m_PauseResume);
	DDX_Control(pDX, IDC_SEEK, m_Seek);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_EXIT, m_Exit);
	DDX_Control(pDX, IDC_PRESETBOX, m_PresetBox);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsSoundCrossfader, CmusikPropertyPage)
	ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_LBN_SELCHANGE(IDC_PRESETBOX, OnLbnSelchangePresetbox)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikPrefsSoundCrossfader::OnInitDialog()
{
	CmusikPropertyPage::OnInitDialog();
	LoadPrefs();
	return TRUE;
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::LoadPrefs()
{
	// intervals
	CmusikCrossfader fader;
	m_Library->GetDefaultCrossfader( &fader );

	Populate( fader );

	// presets
	CmusikStringArray items;
	m_Library->GetAllCrossfaderPresets( &items, &m_IDs, true );

	m_PresetBox.ResetContent();
	for ( size_t i = 0; i < items.size(); i++ )
		m_PresetBox.AddString( items.at( i ) );
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::Populate( const CmusikCrossfader& fader )
{
	m_NewSong.SetWindowText( FloatToCString( fader.m_NewSong ) );
	m_PauseResume.SetWindowText( FloatToCString( fader.m_PauseResume ) );
	m_Seek.SetWindowText( FloatToCString( fader.m_Seek ) );
	m_Stop.SetWindowText( FloatToCString( fader.m_Stop ) );
	m_Exit.SetWindowText( FloatToCString( fader.m_Exit ) );
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::RevPopulate( CmusikCrossfader& fader )
{
	CString sWnd;

	m_NewSong.GetWindowText( sWnd );
	fader.m_NewSong = StringToFloat( sWnd.GetBuffer() );

	m_PauseResume.GetWindowText( sWnd );
	fader.m_PauseResume = StringToFloat( sWnd.GetBuffer() );

	m_Seek.GetWindowText( sWnd );
	fader.m_Seek = StringToFloat( sWnd.GetBuffer() );

	m_Stop.GetWindowText( sWnd );
	fader.m_Stop = StringToFloat( sWnd.GetBuffer() );

	m_Exit.GetWindowText( sWnd );
	fader.m_Exit = StringToFloat( sWnd.GetBuffer() );
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::CommitChanges()
{
	CmusikCrossfader fader_save;
	RevPopulate( fader_save );

	m_Library->UpdateDefaultCrossfader( fader_save );

	if ( m_Player->IsCrossfaderEnabled() )
		m_Player->SetCrossfader( fader_save );
}

///////////////////////////////////////////////////

BOOL CmusikPrefsSoundCrossfader::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );
	m_Modified = true;

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnBnClickedReset()
{
	m_Library->ResetDefaultCrossfader();
	
	CmusikCrossfader fader;
	m_Library->GetDefaultCrossfader( &fader );

	Populate( fader );
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnBnClickedAdd()
{
	CmusikCrossfader fader_new;
	RevPopulate( fader_new );

	CString name;
	CmusikNameEntry* pDlg = new CmusikNameEntry( this, &name );
	if ( pDlg->DoModal() == IDOK && !name.IsEmpty() )
	{
		fader_new.m_Name = name;
		int ret = m_Library->CreateCrossfader( &fader_new );
		if ( ret == MUSIK_LIBRARY_OK )
		{
			m_PresetBox.AddString( name );
			m_IDs.push_back( fader_new.m_ID );
	
			m_PresetBox.SetCurSel( m_PresetBox.GetCount() - 1 );
		}
		else if ( ret == MUSIK_LIBRARY_ID_EXISTS )
			MessageBox( _T( "Sorry, but a crossfader preset with this name already exists. Please enter a unique name." ), MUSIK_VERSION_STR, MB_OK | MB_ICONWARNING );
	}
	delete pDlg;
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnBnClickedDelete()
{
	int nSel = GetIndex();
	
	if ( nSel > -1 )
	{
		if ( m_Library->DeleteCrossfader( m_IDs.at( nSel ) ) == MUSIK_LIBRARY_OK )
		{
			m_PresetBox.DeleteString( nSel );
			m_IDs.erase( m_IDs.begin() + nSel );
	
			--nSel;
			if ( nSel == -1 )
				nSel = 0;

			m_PresetBox.SetCurSel( nSel );
		}
	}
}

///////////////////////////////////////////////////

int CmusikPrefsSoundCrossfader::GetIndex()
{
	for ( size_t i = 0; i < m_IDs.size(); i++ )
	{
		if ( m_PresetBox.GetSel( i ) )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnLbnSelchangePresetbox()
{
	int nSel = GetIndex();

	if ( nSel != -1 )
	{
		CmusikCrossfader fader;
		m_Library->GetCrossfader( m_IDs.at( nSel ), &fader );

		Populate( fader );
	}
}

///////////////////////////////////////////////////

BOOL CmusikPrefsSoundCrossfader::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE && GetFocus() == &m_PresetBox )
	{
		OnBnClickedDelete();
		return true;
	}

	return CmusikPropertyPage::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

// Interface::Transparency

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceTrans, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceTrans::CmusikPrefsInterfaceTrans( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player )
	: CmusikPropertyPage( CmusikPrefsInterfaceTrans::IDD, prefs, library, player )
{
}

///////////////////////////////////////////////////

CmusikPrefsInterfaceTrans::~CmusikPrefsInterfaceTrans()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceTrans::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_TransEnabled);
	DDX_Control(pDX, IDC_CHECK2, m_TransAdaptive);
	DDX_Control(pDX, IDC_EDIT1, m_TransFocused);
	DDX_Control(pDX, IDC_EDIT2, m_TransUnfocused);
	DDX_Control(pDX, IDC_EDIT5, m_TransFadeDuration);
}

///////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceTrans, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsInterfaceTrans::LoadPrefs()
{
	if ( m_Prefs->IsTransEnabled() )
		m_TransEnabled.SetCheck( TRUE );

	if ( m_Prefs->IsTransAdaptive() )
		m_TransAdaptive.SetCheck( TRUE );

	m_TransFocused.SetWindowText( IntToCString( m_Prefs->GetTransFocus() ) );
	m_TransUnfocused.SetWindowText( IntToCString( m_Prefs->GetTransUnFocus() ) );
	m_TransFadeDuration.SetWindowText( FloatToCString( m_Prefs->GetTransDur() ) );
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceTrans::CommitChanges()
{
	// toggles
	m_Prefs->SetTransEnabled( (bool)m_TransEnabled.GetCheck() );
	m_Prefs->SetTransAdaptive( (bool)m_TransAdaptive.GetCheck() );

	CString temp;
	int focus_per, unfocus_per;
	float duration;

	// focused
	m_TransFocused.GetWindowText( temp );
	focus_per = StringToInt( temp.GetBuffer() );

	if ( focus_per > 255 )
		focus_per = 255;
	if ( focus_per < 0 )
		focus_per = 0;
	m_Prefs->SetTransFocus( focus_per );

	// unfocused
	m_TransUnfocused.GetWindowText( temp );
	unfocus_per = StringToInt( temp.GetBuffer() );

	if ( unfocus_per > 255 )
		unfocus_per = 255;
	if ( unfocus_per < 0 )
		unfocus_per = 0;

	m_Prefs->SetTransUnFocus( unfocus_per );

	// duration
	m_TransFadeDuration.GetWindowText( temp );
	duration = StringToFloat( temp.GetBuffer() );

	m_Prefs->SetTransDur( duration );

	if ( m_Prefs->IsTransEnabled() )
	{
		int WM_INITTRANS = RegisterWindowMessage( "INITTRANS" );
		AfxGetApp()->m_pMainWnd ->SendMessage( WM_INITTRANS );
	}
	else
	{
		int WM_DEINITTRANS = RegisterWindowMessage( "DEINITTRANS" );
		AfxGetApp()->m_pMainWnd ->SendMessage( WM_DEINITTRANS );
	}
		

	// reload
	LoadPrefs();

	SetModified( FALSE );
	m_Modified = false;
}

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceTrans::OnInitDialog()
{
	CmusikPropertyPage::OnInitDialog();
	LoadPrefs();
	return TRUE;
}

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceTrans::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );
	m_Modified = true;

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////

///////////////////////////////////////////////////

// Tunage

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsTunage, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsTunage::CmusikPrefsTunage( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player )
	: CmusikPropertyPage( CmusikPrefsTunage::IDD, prefs, library, player )
{
}

///////////////////////////////////////////////////

CmusikPrefsTunage::~CmusikPrefsTunage()
{
}

///////////////////////////////////////////////////

void CmusikPrefsTunage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TUNAGEENABLED, m_Enabled);
	DDX_Control(pDX, IDC_TUNAGEURLENABLED, m_PostURLEnabled);
	DDX_Control(pDX, IDC_TUNAGEURL, m_URL);
	DDX_Control(pDX, IDC_TUNAGEFILEENABLED, m_WriteFileEnabled);
	DDX_Control(pDX, IDC_TUNAGEFILENAME, m_Filename);
	DDX_Control(pDX, IDC_TUNAGEFILELINE, m_Fileline);
	DDX_Control(pDX, IDC_TUNAGEAPPEND, m_Append);
	DDX_Control(pDX, IDC_TUNAGERUNAPPENABLED, m_RunAppEnabled);
	DDX_Control(pDX, IDC_TUNAGECOMMANDLINE, m_CommandLine);
}

///////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CmusikPrefsTunage, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsTunage::LoadPrefs()
{
	m_Enabled.SetCheck( m_Prefs->GetTunageEnabled() );
	m_PostURLEnabled.SetCheck( m_Prefs->GetTunagePostURL() );
	m_URL.SetWindowText( m_Prefs->GetTunageURL().c_str() );
	m_WriteFileEnabled.SetCheck( m_Prefs->GetTunageWriteFile() );
	m_Filename.SetWindowText( m_Prefs->GetTunageFilename().c_str() );
	m_Fileline.SetWindowText( m_Prefs->GetTunageFileLine().c_str() );
	m_Append.SetCheck( m_Prefs->GetTunageAppendFile() );
	m_RunAppEnabled.SetCheck( m_Prefs->GetTunageRunApp() );
	m_CommandLine.SetWindowText( m_Prefs->GetTunageCmdLine().c_str() );
}

///////////////////////////////////////////////////

void CmusikPrefsTunage::CommitChanges()
{
	CString sWnd;

	m_Prefs->SetTunageEnabled( m_Enabled.GetCheck()  );
	m_Prefs->SetTunagePostURL( m_PostURLEnabled.GetCheck()  );
	m_URL.GetWindowText( sWnd );
	m_Prefs->SetTunageURL( (LPCSTR)sWnd );
	m_Prefs->SetTunageWriteFile( m_WriteFileEnabled.GetCheck()  );
	m_Filename.GetWindowText( sWnd );
	m_Prefs->SetTunageFilename( (LPCSTR)sWnd );
	m_Fileline.GetWindowText( sWnd );
	m_Prefs->SetTunageFileLine( (LPCSTR)sWnd );
	m_Prefs->SetTunageAppendFile( m_Append.GetCheck()  );
	m_Prefs->SetTunageRunApp( m_RunAppEnabled.GetCheck()  );
	m_CommandLine.GetWindowText( sWnd);
	m_Prefs->SetTunageCmdLine( (LPCSTR)sWnd  );
		

	// reload
	LoadPrefs();

	SetModified( FALSE );
	m_Modified = false;
}

///////////////////////////////////////////////////

BOOL CmusikPrefsTunage::OnInitDialog()
{
	CmusikPropertyPage::OnInitDialog();
	LoadPrefs();
	return TRUE;
}

///////////////////////////////////////////////////

BOOL CmusikPrefsTunage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );
	m_Modified = true;

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////
