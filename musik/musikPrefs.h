///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "stdafx.h"

#include "3rdparty/iniFile.h"

#include "../musikCore/include/musikCrossfader.h"
#include "../musikCore/include/musikArrays.h"

///////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////

class CmusikPrefs
{
public:

	// construct and destruct
	CmusikPrefs( CString filename );
	~CmusikPrefs();

	void ParseColors();
	void UnparseColors();
	void ThemeChanged();

	//
	// basic stuff
	//

	void LoadPrefs();
	void SavePrefs();

	//
	// main dialog
	//

	CSize GetDlgSize(){ return m_Dlg_Size; }
	CPoint GetDlgPos(){ return m_Dlg_Pos; }
	bool IsMaximized(){ return m_Dlg_Maximized; }
	bool ResetUI(){ return m_Dlg_ResetUI; }
	int	GetStdPlaylistPrompt(){ return m_Dlg_StdPlaylist_Prompt; }
	int GetFileDropPrompt(){ return m_Dlg_FileDrop_Prompt; }
	bool PlaylistInfoVisible(){ return m_Dlg_PlaylistInfoVisible; }
	bool PurgeOnStartup(){ return m_Dlg_PurgeOnStartup; }
	bool WriteTagsToFile(){ return m_Dlg_WriteTagsToFile; }
	bool LibraryShowsAllSongs(){ return m_Dlg_LibraryShowsAllSongs; }
	bool MinimizeToTray(){ return m_Dlg_MinimizeToTray; }

	void SetDlgSize( const CSize sz ){ m_Dlg_Size = sz; }
	void SetDlgPos( const CPoint pt ){ m_Dlg_Pos = pt; }
	void SetMaximized( bool max ){ m_Dlg_Maximized = max; }
	void SetDlgResetUI( bool reset ){ m_Dlg_ResetUI = reset; }
	void SetStdPlaylistPrompt( int prompt ){ m_Dlg_StdPlaylist_Prompt = prompt; }
	void SetFileDropPrompt( int prompt ){ m_Dlg_FileDrop_Prompt = prompt; }
	void SetPlaylistInfoVisible( bool visible )	{ m_Dlg_PlaylistInfoVisible = visible; }
	void SetPurgeOnStartup( bool purge ){ m_Dlg_PurgeOnStartup = purge; }
	void SetWriteTagsToFile( bool write ){ m_Dlg_WriteTagsToFile = write; }
	void SetLibraryShowsAllSongs( bool showall ){ m_Dlg_LibraryShowsAllSongs = showall; }
	void SetMinimizeToTray( bool tray ){ m_Dlg_MinimizeToTray = tray; }

	//
	// selection box
	//

	size_t GetSelBoxCount(){ return m_SelectionBox_Types.size(); }
	int GetSelBoxType( int index ){ return m_SelectionBox_Types.at( index ); }

	void SetSelBoxType( int index, int type ){ m_SelectionBox_Types.at( index ) = type; }
	void SetNewSelBoxType( int type ){ m_SelectionBox_Types.push_back( type ); }
	void SetSelBoxTypes( const CIntArray& types ){ m_SelectionBox_Types = types; }
	
	//
	// playlist 
	//

	CIntArray GetPlaylistOrder(){ return m_Playlist_Order; }
	CIntArray GetPlaylistSizes(){ return m_Playlist_Sizes; }
	int GetPlaylistCol( int n ){ return m_Playlist_Order.at( n ); }
	int	GetPlaylistColWidth( int n ){ return m_Playlist_Sizes.at( n ); }
	size_t GetPlaylistColCount(){ return m_Playlist_Order.size(); }
	
	void SetPlaylistOrder( CIntArray n ){ m_Playlist_Order = n; }	
	void SetPlaylistSizes( CIntArray n ){ m_Playlist_Sizes = n; }
	
	//
	// now playing
	//

	int	GetNowPlayingCaptionFontSize(){ return m_NowPlaying_CaptionFont; }
	void SetNowPlayingCaptionFontSize( int y ){ m_NowPlaying_CaptionFont = y; }

	//
	// player
	//

	int	GetPlayerDriver(){ return m_Player_Driver; }					
	int	GetPlayerDevice(){ return m_Player_Device; }
	int	GetPlayerRate(){ return m_Player_Rate; }
	int	GetPlayerMaxChannels()	{ return m_Player_Max_Channels; }
	int	GetPlayerVolume(){ return m_Player_Volume; }
	unsigned long GetPlayerPlaymode(){ return m_Player_Playmode; }

	void SetPlayerDrive( int n ){ m_Player_Driver = n; }
	void SetPlayerDevice( int n ){ m_Player_Device = n; }
	void SetPlayerRate( int n ){ m_Player_Rate = n; }
	void SetPlayerMaxChannels( int n ){ m_Player_Max_Channels = n; }
	void SetPlayerVolume( int n ){ m_Player_Volume = n; }
	void SetPlayerPlaymode( unsigned long un ){ m_Player_Playmode = un; }

	//
	// crossfader
	//
	
	int  GetCrossfader(){ return m_Crossfader_Current; }
	bool IsCrossfaderEnabled(){ return m_Crossfader_Enabled; }

	void SetCrossfader( int n ){ m_Crossfader_Current = n; }
	void SetCrossfaderEnabled( bool b ){ m_Crossfader_Enabled = b; }

	//
	// equalizer
	//

	int GetEqualizer(){ return m_Equalizer_Current; }
	bool IsEqualizerEnabled(){ return m_Equalizer_Enabled; }
	int GetEqualizerBandState(){ return m_Equalizer_Band_State; }
	bool IsEqualizerChannelsLocked(){ return m_Equalizer_Locked; }
	bool IsEqualizerAutoManaged(){ return m_Equalizer_AutoManaged; }

	void SetEqualizer( int n ){ m_Equalizer_Current = n; }
	void SetEqualizerEnabled( bool b ){ m_Equalizer_Enabled = b; }
	void SetEqualizerBandState( int n ){ m_Equalizer_Band_State = n; }
	void SetEqualizerChannelsLocked( bool b ){ m_Equalizer_Locked = b; }
	void SetEqualizerAutoManaged( bool b ){ m_Equalizer_AutoManaged = b; }

	//
	// theme
	//

	COLORREF MUSIK_COLOR_ACTIVECAPTION;
	COLORREF MUSIK_COLOR_CAPTIONTEXT;
	COLORREF MUSIK_COLOR_INACTIVECAPTION;
	COLORREF MUSIK_COLOR_INACTIVECAPTIONTEXT;
	COLORREF MUSIK_COLOR_BTNFACE;
	COLORREF MUSIK_COLOR_BTNTEXT;
	COLORREF MUSIK_COLOR_BTNHILIGHT;
	COLORREF MUSIK_COLOR_BTNSHADOW;
	COLORREF MUSIK_COLOR_LISTCTRL;
	COLORREF MUSIK_COLOR_LISTCTRLTEXT;
	COLORREF MUSIK_COLOR_HIGHLIGHT;
	COLORREF MUSIK_COLOR_HIGHLIGHTTEXT;

protected:

	CIniFile* config;
	string GetDefPlaylistOrder();
	string GetDefPlaylistSizes();

private:

	// main dialog
	CSize	m_Dlg_Size;
	CPoint	m_Dlg_Pos;
	bool	m_Dlg_Maximized;
	bool	m_Dlg_ResetUI;
	int		m_Dlg_StdPlaylist_Prompt;
	int		m_Dlg_FileDrop_Prompt;
	bool	m_Dlg_PlaylistInfoVisible;
	bool	m_Dlg_PurgeOnStartup;
	bool	m_Dlg_WriteTagsToFile;
	bool	m_Dlg_LibraryShowsAllSongs;
	bool	m_Dlg_MinimizeToTray;

	// selection area
	CIntArray m_SelectionBox_Types;

	// playlist
	CIntArray m_Playlist_Order;
	CIntArray m_Playlist_Sizes;

	// now playing
	int m_NowPlaying_CaptionFont;

	// player
	int m_Player_Device;
	int m_Player_Driver;
	int m_Player_Rate;
	int m_Player_Max_Channels;
	int m_Player_Volume;
	unsigned long m_Player_Playmode;

	// crossfader
	int m_Crossfader_Current;
	bool m_Crossfader_Enabled;

	// equalizer
	int m_Equalizer_Current;
	bool m_Equalizer_Enabled;
	int m_Equalizer_Band_State;
	bool m_Equalizer_Locked;
	bool m_Equalizer_AutoManaged;

	// theme aware stuff
	bool m_ActiveCaptionDef;
	bool m_CaptionDef;
	bool m_InactiveCaptionDef;
	bool m_InactiveCaptionTextDef;
	bool m_BtnFaceDef;
	bool m_BtnTextDef;
	bool m_BtnHilightDef;
	bool m_BtnShadowDef;
	bool m_ListCtrlDef;
	bool m_ListCtrlTextDef;
	bool m_HilightDef;
	bool m_HilightTextDef;
};

///////////////////////////////////////////////////