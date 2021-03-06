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
//   CmusikPrefsInterfaceWorkflow
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
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

// Interface::Workflow

///////////////////////////////////////////////////

class CmusikPrefsInterfaceWorkflow : public CmusikPropertyPage
{
public:

	// construct / destruct
	CmusikPrefsInterfaceWorkflow( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPrefsInterfaceWorkflow();

	// saving / loading
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// dialog data
	enum { IDD = IDD_PROPPAGE_INTERFACE_WORKFLOW };

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceWorkflow)
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
	CButton m_AddEntireToNP;
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
	
	// from CmusikPropertyPage
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// mfc vars and messages
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLbnSelchangePresetbox();

	CEdit m_NewSong;
	CEdit m_PauseResume;
	CEdit m_Seek;
	CEdit m_Stop;
	CEdit m_Exit;
	CListBox m_PresetBox;

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);
	enum { IDD = IDD_PROPPAGE_SOUND_CROSSFADER };
	void Populate( const CmusikCrossfader& fader );
	void RevPopulate( CmusikCrossfader& fader );
	int GetIndex();

	CIntArray m_IDs;

	// macros
	DECLARE_DYNAMIC(CmusikPrefsSoundCrossfader)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

// Interface::Transparency

///////////////////////////////////////////////////

class CmusikPrefsInterfaceTrans : public CmusikPropertyPage
{

public:

	// construct and destruct
	CmusikPrefsInterfaceTrans( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPrefsInterfaceTrans();

	// from CmusikPropertyPage
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// objects
	CButton m_TransEnabled;
	CButton m_TransAdaptive;
	CEdit m_TransFocused;
	CEdit m_TransUnfocused;
	CEdit m_TransFadeDuration;

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	enum { IDD = IDD_PROPPAGE_INTERFACE_TRANPARENCY };

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceTrans)
	DECLARE_MESSAGE_MAP()

};


///////////////////////////////////////////////////

class CmusikPrefsTunage : public CmusikPropertyPage
{

public:

	// construct and destruct
	CmusikPrefsTunage( CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPrefsTunage();

	// from CmusikPropertyPage
	virtual void CommitChanges();
	virtual void LoadPrefs();

	// objects
	CButton m_Enabled;
	CButton m_PostURLEnabled;
	CEdit m_URL;
	CButton m_WriteFileEnabled;
	CEdit m_Filename;
	CEdit m_Fileline;
	CButton m_Append;
	CButton m_RunAppEnabled;
	CEdit m_CommandLine;


	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	enum { IDD = IDD_PROPPAGE_TUNAGE };

	// macros
	DECLARE_DYNAMIC(CmusikPrefsTunage)
	DECLARE_MESSAGE_MAP()

};


///////////////////////////////////////////////////