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
//   CmusikPrefsInterfaceGeneral
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

#pragma once

///////////////////////////////////////////////////

#include "afxwin.h"
#include "musikPropertyPage.h"
#include "../musikCore/include/musikArrays.h"

///////////////////////////////////////////////////

class CmusikPrefs;
class CmusikLibrary;
class CmusikPlayer;

///////////////////////////////////////////////////

// General::Interface

///////////////////////////////////////////////////

class CmusikPrefsInterfaceGeneral : public CmusikPropertyPage
{
public:

	// construct / destruct
	CmusikPrefsInterfaceGeneral( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPrefsInterfaceGeneral();

	// saving / loading
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// dialog data
	enum { IDD = IDD_PROPPAGE_INTERFACE_GENERAL };

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceGeneral)
	DECLARE_MESSAGE_MAP()
protected:

	// misc
	virtual void DoDataExchange( CDataExchange* pDX ); 

	// startup
	void EnableStartup();
	void DisableStartup();
	bool GetStartup();

	// controls
	CButton m_EnableTrayIcon;
	CButton m_PromptSavePlaylists;
	CButton m_PromptDropOnPlaylist;
	CButton m_StartWithWindows;
	CButton m_AutoSynchronize;
	CButton m_LibraryShowAll;
};

///////////////////////////////////////////////////

// Sound::Driver

///////////////////////////////////////////////////

class CmusikPrefsSoundDriver : public CmusikPropertyPage
{
public:

	// construct / destruct
	CmusikPrefsSoundDriver( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPrefsSoundDriver();

	// dialog data
	enum { IDD = IDD_PROPPAGE_SOUND_DRIVER };

	// saving / loading
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// macros
	DECLARE_DYNAMIC( CmusikPrefsSoundDriver )
	DECLARE_MESSAGE_MAP()

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// for getting sound devices
	void GetSoundDevices( bool populate = true );
	void GetSoundDrivers( bool populate = true );
	void GetSoundPlaybackRates( bool populate = true );

	// controls
	CComboBox m_SoundDriver;
	CComboBox m_SoundDevice;
	CComboBox m_SoundPlaybackRate;
	CEdit m_SoundMaxChannels;

	// arrays that hold sound driver / device info
	CmusikStringArray m_DeviceInfo;
	CmusikStringArray m_DriverInfo;
	CmusikStringArray m_PlaybackRateInfo;
};

///////////////////////////////////////////////////

// Sound::Crossfader

///////////////////////////////////////////////////

class CmusikPrefsSoundCrossfader : public CmusikPropertyPage
{

public:

	// construct and destruct
	CmusikPrefsSoundCrossfader( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPrefsSoundCrossfader();
	
	virtual void CommitChanges();
	
	// mfc vars and messages
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	CEdit m_NewSong;
	CEdit m_PauseResume;
	CEdit m_Seek;
	CEdit m_Stop;
	CEdit m_Exit;

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);
	enum { IDD = IDD_PROPPAGE_SOUND_CROSSFADER };
	void Populate( const CmusikCrossfader& fader );

	// macros
	DECLARE_DYNAMIC(CmusikPrefsSoundCrossfader)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
