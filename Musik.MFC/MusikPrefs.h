///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "stdafx.h"

#include "3rdparty/iniFile.h"

#include "../Musik.Core/include/MusikCrossfader.h"
#include "../Musik.Core/include/MusikArrays.h"

///////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////

class CMusikPrefs
{
public:

	// construct and destruct
	CMusikPrefs( CString filename );
	~CMusikPrefs();

	void ParseColors();
	void UnparseColors();
	void ThemeChanged();

	// basic stuff
	void		LoadPrefs();
	void		SavePrefs();

	// main dialog gets
	CSize		GetDlgSize							( )					{ return m_Dlg_Size; }
	CPoint		GetDlgPos							( )					{ return m_Dlg_Pos; }
	bool		IsMaximized							( )					{ return m_Dlg_Maximized; }
	bool		ResetUI								( )					{ return m_Dlg_ResetUI; }

	// main dialog sets
	void		SetDlgSize							( const CSize sz )	{ m_Dlg_Size = sz; }
	void		SetDlgPos							( const CPoint pt )	{ m_Dlg_Pos = pt; }
	void		SetMaximized						( bool max )		{ m_Dlg_Maximized = max; }
	void		SetDlgResetUI						( bool reset )		{ m_Dlg_ResetUI = reset; }

	// selection box gets
	size_t		GetSelBoxCount						( )					{ return m_SelectionBox_Count; }

	// selection box sets
	void		SetSelBoxCount						( size_t n )		{ m_SelectionBox_Count = n; }

	// playlist gets
	CIntArray	GetPlaylistOrder					( )					{ return m_Playlist_Order; }
	CIntArray	GetPlaylistSizes					( )					{ return m_Playlist_Sizes; }
	int			GetPlaylistCol						( int n )			{ return m_Playlist_Order.at( n ); }
	int			GetPlaylistColWidth					( int n )			{ return m_Playlist_Sizes.at( n ); }
	size_t		GetPlaylistColCount					( )					{ return m_Playlist_Order.size(); }
	
	// playlist sets
	void		SetPlaylistOrder					( CIntArray n )		{ m_Playlist_Order = n; }	
	void		SetPlaylistSizes					( CIntArray n )		{ m_Playlist_Sizes = n; }
	
	// now playing gets
	int			GetNowPlayingCaptionFontSize		( )					{ return m_NowPlaying_CaptionFont; }

	// now playing sets
	void		SetNowPlayingCaptionFontSize		( int y )			{ m_NowPlaying_CaptionFont = y; }

	// player gets
	int			GetPlayerDriver						( )					{ return m_Player_Driver; }					
	int			GetPlayerDevice						( )					{ return m_Player_Device; }
	int			GetPlayerRate						( )					{ return m_Player_Rate; }
	int			GetPlayerMaxChannels				( )					{ return m_Player_Max_Channels; }

	// player sets
	void		SetPlayerDrive						( int n )			{ m_Player_Driver = n; }
	void		SetPlayerDevice						( int n )			{ m_Player_Device = n; }
	void		SetPlayerRate						( int n )			{ m_Player_Rate = n; }
	void		SetPlayerMaxChannels				( int n )			{ m_Player_Max_Channels = n; }

	// default crossfader get
	CMusikCrossfader GetDefaultCrossfader			()					{ return m_Crossfader_Default; }
	CStdString GetCurrCrossfader					()					{ return m_Crossfader_Current; }
	bool IsCrossfaderEnabled						()					{ return m_Crossfader_Enabled; }

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

	// selection area
	size_t	m_SelectionBox_Count;

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

	// crossfader
	CMusikCrossfader m_Crossfader_Default;
	CStdString m_Crossfader_Current;
	bool m_Crossfader_Enabled;

	// theme aware stuff
	bool m_ActiveCaptionDef;
	bool m_CaptionDef;
	bool m_InctiveCaptionDef;
	bool m_InctiveCaptionTextDef;
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