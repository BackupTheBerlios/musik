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
//   CmusikPrefs
//
// Filename(s): 
//
//   musikPrefs.h, musikPrefs.cpp
//
// Information:
//
//   Preferences class for musikCube
// 
// Usage: 
//
//   LoadPrefs() to load, SavePrefs() to save...
//   GetXXX() and SetXXX() to store and retrieve
//   values.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikPrefs.h"

#include "../musikCore/include/musikLibrary.h"

///////////////////////////////////////////////////

CmusikPrefs::CmusikPrefs( CString filename )
{
	config = new CIniFile( filename.GetBuffer() );

	if ( !config->ReadFile() )
		config->WriteFile();

	m_ActiveCaptionDef			= false;
	m_CaptionDef				= false;
	m_InactiveCaptionDef		= false;
	m_InactiveCaptionTextDef	= false;
	m_BtnFaceDef				= false;
	m_BtnTextDef				= false;
	m_BtnHilightDef				= false;
	m_BtnShadowDef				= false;
	m_ListCtrlDef				= false;
	m_ListCtrlTextDef			= false;
	m_HilightDef				= false;
	m_HilightTextDef			= false;

	LoadPrefs();
}

///////////////////////////////////////////////////

CmusikPrefs::~CmusikPrefs()
{
	SavePrefs();
	delete config;
}

///////////////////////////////////////////////////

void CmusikPrefs::LoadPrefs()
{
	// main dialog
	m_Dlg_Size					= StringToCSize( config->GetValue( "Dialog", "Dialog Size", "800x600" ) );
	m_Dlg_Pos					= StringToCPoint( config->GetValue( "Dialog", "Dialog Position", "50,50" ) );
	m_Dlg_Maximized				= StringToBool( config->GetValue( "Dialog", "Maximized", "0" ) );
	m_Dlg_ResetUI				= StringToBool( config->GetValue( "Dialog", "Reset UI", "1" ) );
	m_Dlg_StdPlaylist_Prompt	= StringToInt( config->GetValue( "Dialog", "Prompt STD Playlist Save", "-1" ) );
	m_Dlg_FileDrop_Prompt		= StringToInt( config->GetValue( "Dialog", "Prompt File Drop Action", "-1" ) );
	m_Dlg_PlaylistInfoVisible	= StringToBool( config->GetValue( "Dialog", "Playlist Info Visible", "1" ) );
	m_Dlg_PlaylistInfoVizStyle	= StringToInt( config->GetValue( "Dialog", "Playlist Info Viz Style", "0" ) );
	m_Dlg_SynchronizeOnStartup	= StringToBool( config->GetValue( "Dialog", "Synchronize Old Files on Startup", "1" ) );
	m_Dlg_WriteTagsToFile		= StringToBool( config->GetValue( "Dialog", "Write Tags to File", "0" ) );
	m_Dlg_LibraryShowsAllSongs	= StringToBool( config->GetValue( "Dialog", "Library Shows All Songs", "1" ) );
	m_Dlg_MinimizeToTray		= StringToBool( config->GetValue( "Dialog", "Minimize to Notification Tray", "0" ) );
	m_Dlg_AddEntireToNP			= StringToBool( config->GetValue( "Dialog", "Add Entire Library Playlist to Now Playing", "1" ) );
	m_Dlg_AlwaysOnTop			= StringToBool( config->GetValue( "Dialog", "Always on Top", "0" ) );

	// winamp vis
	m_WinampVis_Name			= config->GetValue( "Winamp Visualizations", "Default", "" );
	m_WinampVis_Module			= StringToInt( config->GetValue( "Winamp Visualizations", "Module", "-1" ) );
	m_Winamp_PluginDir			= config->GetValue( "Winamp Visualizations", "PluginDir", "c:\\program files\\winamp\\plugins\\" );

	// transparency
	m_TransEnabled				= StringToBool( config->GetValue( "Transparency", "Enabled", "0" ) );
	m_TransAdaptive				= StringToBool( config->GetValue( "Transparency", "Adaptive", "0" ) );
	m_TransDur					= StringToFloat( config->GetValue( "Transparency", "Duration", "0.5" ) );
	m_TransFocus				= StringToInt( config->GetValue( "Transparency", "Focused", "225" ) );
	m_TransUnfocus				= StringToInt( config->GetValue( "Transparency", "Unfocused", "100" ) );

	// selection area
	m_SelectionBox_Types = StringToCIntArray( config->GetValue( "Selection Area", "Types", "0,1" ) );

	// sources
	m_Sources_SubLibsVisible		= StringToBool( config->GetValue( "Sources", "Show SubLibs", "0" ) );
	m_Sources_PlaylistsVisible		= StringToBool( config->GetValue( "Sources", "Show Playlists", "1" ) );
	m_Sources_QuickSearchVisible	= StringToBool( config->GetValue( "Sources", "Show Quick Search", "1" ) );

	// playlist
	m_Playlist_Order		= StringToCIntArray( config->GetValue( "Playlist", "Column Order", GetDefPlaylistOrder() ) );
	m_Playlist_Sizes		= StringToCIntArray( config->GetValue( "Playlist", "Column Sizes", GetDefPlaylistSizes() ) );
	m_PlaylistInfo_VizDur	= StringToInt( config->GetValue( "Playlist", "Viz Duration", "75" ) );

	// now playing
	m_NowPlaying_CaptionFont = StringToInt( config->GetValue( "Now Playing", "Caption Font Size", "12" ) );
	
	// player
	m_Player_Driver			= StringToInt( config->GetValue( "Player", "Driver", "0" ) );
	m_Player_Device			= StringToInt( config->GetValue( "Player", "Device", "0" ) );
	m_Player_Rate			= StringToInt( config->GetValue( "Player", "Rate", "44100" ) );
	m_Player_Max_Channels	= StringToInt( config->GetValue( "Player", "Maximum Channels", "6" ) );
	m_Player_Volume			= StringToInt( config->GetValue( "Player", "Volume", "200" ) );
	m_Player_Playmode		= StringToULong( config->GetValue( "Player", "Playmode", "0" ) );	

	// crossfader
	m_Crossfader_Enabled	= StringToBool( config->GetValue( "Crossfader", "Enabled", "1" ) );
	m_Crossfader_Current	= StringToInt( config->GetValue( "Crossfader", "Set Name", "-1" ) );

	// equalizer
	m_Equalizer_Enabled		= StringToBool( config->GetValue( "Equalizer", "Enabled", "0" ) );
	m_Equalizer_Current		= StringToInt( config->GetValue( "Equalizer", "Set Name", "-1" ) );
	m_Equalizer_Band_State	= StringToInt( config->GetValue( "Equalizer", "Band State", "6" ) );
	m_Equalizer_Locked		= StringToBool( config->GetValue( "Equalizer", "Channels Locked", "1" ) );
	m_Equalizer_AutoManaged	= StringToBool( config->GetValue( "Equalizer", "Automatically Managed", "1" ) );

	// dialog colors
	MUSIK_COLOR_ACTIVECAPTION		= StringToCOLORREF( config->GetValue( "Dialog Colors", "Active Caption", "255,0,255" ) );
	MUSIK_COLOR_CAPTIONTEXT			= StringToCOLORREF( config->GetValue( "Dialog Colors", "Active Caption Text", "255,0,255" ) );
	MUSIK_COLOR_INACTIVECAPTION		= StringToCOLORREF( config->GetValue( "Dialog Colors", "Inactive Caption", "255,0,255" ) );
	MUSIK_COLOR_INACTIVECAPTIONTEXT	= StringToCOLORREF( config->GetValue( "Dialog Colors", "Inactive Caption Text", "255,0,255" ) );
	MUSIK_COLOR_BTNFACE				= StringToCOLORREF( config->GetValue( "Dialog Colors", "Button Face", "255,0,255" ) );
	MUSIK_COLOR_BTNTEXT				= StringToCOLORREF( config->GetValue( "Dialog Colors", "Button Text", "255,0,255" ) );
	MUSIK_COLOR_BTNHILIGHT			= StringToCOLORREF( config->GetValue( "Dialog Colors", "Button Highlight", "255,0,255" ) );
	MUSIK_COLOR_BTNSHADOW			= StringToCOLORREF( config->GetValue( "Dialog Colors", "Button Shadow", "255,0,255" ) );
	MUSIK_COLOR_LISTCTRL			= StringToCOLORREF( config->GetValue( "Dialog Colors", "List Background", "255,0,255" ) );
	MUSIK_COLOR_LISTCTRLTEXT		= StringToCOLORREF( config->GetValue( "Dialog Colors", "List Text", "255,0,255" ) );
	MUSIK_COLOR_HIGHLIGHT			= StringToCOLORREF( config->GetValue( "Dialog Colors", "Color Highlight", "255,0,255" ) );
	MUSIK_COLOR_HIGHLIGHTTEXT		= StringToCOLORREF( config->GetValue( "Dialog Colors", "Color Highlight Text", "255,0,255" ) );

	// tunage
	m_Tunage_Enabled	= StringToBool( config->GetValue( "Tunage", "Enabled", "0" ) );
	m_Tunage_WriteFile	= StringToBool( config->GetValue( "Tunage", "WriteFile", "0" ) );	m_Tunage_Filename	= config->GetValue( "Tunage", "Filename", "" );	m_Tunage_FileLine	= config->GetValue( "Tunage", "FileLine", "" );	m_Tunage_AppendFile	= StringToBool( config->GetValue( "Tunage", "AppendFile", "0" ) );	m_Tunage_PostURL		= StringToBool( config->GetValue( "Tunage", "PostURL", "0" ) );	m_Tunage_URL			= config->GetValue( "Tunage", "URL", "" );	m_Tunage_RunApp		= StringToBool( config->GetValue( "Tunage", "RunApp", "0" ) );	m_Tunage_CmdLine		= config->GetValue( "Tunage", "CmdLine", "" );	m_Tunage_RunOnStop	= StringToBool( config->GetValue( "Tunage", "RunOnStop", "0" ) );	m_Tunage_StoppedText = config->GetValue( "Tunage", "StoppedText", "" );

	ParseColors();
}

///////////////////////////////////////////////////

void CmusikPrefs::SavePrefs()
{
	UnparseColors();

	// main dialog
	config->SetValue( "Dialog", "Dialog Size", CSizeToString( m_Dlg_Size ) );
	config->SetValue( "Dialog", "Dialog Position", CPointToString( m_Dlg_Pos ) );
	config->SetValue( "Dialog", "Maximized", BoolToString( m_Dlg_Maximized ) );
	config->SetValue( "Dialog", "Reset UI", BoolToString( m_Dlg_ResetUI ) );
	config->SetValue( "Dialog", "Prompt STD Playlist Save", IntToString( m_Dlg_StdPlaylist_Prompt ) );
	config->SetValue( "Dialog", "Prompt File Drop Action", IntToString( m_Dlg_FileDrop_Prompt ) );
	config->SetValue( "Dialog", "Playlist Info Visible", BoolToString( m_Dlg_PlaylistInfoVisible ) );
	config->SetValue( "Dialog", "Playlist Info Viz Style", IntToString( m_Dlg_PlaylistInfoVizStyle ) );
	config->SetValue( "Dialog", "Synchronize Old Files on Startup", BoolToString( m_Dlg_SynchronizeOnStartup ) );
	config->SetValue( "Dialog", "Write Tags to File", BoolToString( m_Dlg_WriteTagsToFile ) );
	config->SetValue( "Dialog", "Library Shows All Songs", BoolToString( m_Dlg_LibraryShowsAllSongs ) );
	config->SetValue( "Dialog", "Minimize to Notification Tray", BoolToString( m_Dlg_MinimizeToTray ) );
	config->SetValue( "Dialog", "Add Entire Library Playlist to Now Playing", BoolToString( m_Dlg_AddEntireToNP ) );
	config->SetValue( "Dialog", "Always on Top", BoolToString( m_Dlg_AlwaysOnTop ) );

	// winamp vis
	config->SetValue( "Winamp Visualizations", "Default", m_WinampVis_Name );
	config->SetValue( "Winamp Visualizations", "Module", IntToString( m_WinampVis_Module ) );
	config->SetValue( "Winamp Visualizations", "PluginDir", m_Winamp_PluginDir );
	
	// transparency
	config->SetValue( "Transparency", "Enabled", BoolToString( m_TransEnabled ) );
	config->SetValue( "Transparency", "Adaptive", BoolToString( m_TransAdaptive ) );
	config->SetValue( "Transparency", "Duration", FloatToString( m_TransDur ) );
	config->SetValue( "Transparency", "Focused", IntToString( m_TransFocus ) );
	config->SetValue( "Transparency", "Unfocused", IntToString( m_TransUnfocus ) );

	// selection area
	config->SetValue( "Selection Area", "Types", CIntArrayToString( m_SelectionBox_Types ) );

	// sources
	config->SetValue( "Sources", "Show SubLibs", BoolToString( m_Sources_SubLibsVisible ) );
	config->SetValue( "Sources", "Show Playlists", BoolToString( m_Sources_PlaylistsVisible ) );
	config->SetValue( "Sources", "Show Quick Search", BoolToString( m_Sources_QuickSearchVisible ) );

	// playlist
	config->SetValue( "Playlist", "Column Order", CIntArrayToString( m_Playlist_Order ) );
	config->SetValue( "Playlist", "Column Sizes", CIntArrayToString( m_Playlist_Sizes ) );
	config->SetValue( "Playlist", "Viz Duration", IntToString( m_PlaylistInfo_VizDur ) );

	// now playing
	config->SetValue( "Now Playing", "Caption Font Size", IntToString( m_NowPlaying_CaptionFont ) );

	// player
	config->SetValue( "Player", "Driver", IntToString( m_Player_Driver ) );
	config->SetValue( "Player", "Device", IntToString( m_Player_Device ) );
	config->SetValue( "Player", "Rate", IntToString( m_Player_Rate ) );
	config->SetValue( "Player", "Maximum Channels", IntToString( m_Player_Max_Channels ) );
	config->SetValue( "Player", "Volume", IntToString( m_Player_Volume ) );	
	config->SetValue( "Player", "Playmode", ULongToString( m_Player_Playmode ) );

	// crossfader
	config->SetValue( "Crossfader", "Enabled", BoolToString( m_Crossfader_Enabled ) );
	config->SetValue( "Crossfader", "Set Name", IntToString( m_Crossfader_Current ) );	

	// equalizer
	config->SetValue( "Equalizer", "Enabled", BoolToString( m_Equalizer_Enabled ) );
	config->SetValue( "Equalizer", "Set Name", IntToString( m_Equalizer_Current ) );
	config->SetValue( "Equalizer", "Band State", IntToString( m_Equalizer_Band_State ) );
	config->SetValue( "Equalizer", "Channels Locked", BoolToString( m_Equalizer_Locked ) );
	config->SetValue( "Equalizer", "Automatically Managed", BoolToString( m_Equalizer_AutoManaged ) );

	// dialog colors
	config->SetValue( "Dialog Colors", "Active Caption", COLORREFToString( MUSIK_COLOR_ACTIVECAPTION ) );
	config->SetValue( "Dialog Colors", "Active Caption Text", COLORREFToString( MUSIK_COLOR_CAPTIONTEXT ) );
	config->SetValue( "Dialog Colors", "Inactive Caption", COLORREFToString( MUSIK_COLOR_INACTIVECAPTION ) );
	config->SetValue( "Dialog Colors", "Inactive Caption Text", COLORREFToString( MUSIK_COLOR_INACTIVECAPTIONTEXT ) );
	config->SetValue( "Dialog Colors", "Button Face", COLORREFToString( MUSIK_COLOR_BTNFACE ) );
	config->SetValue( "Dialog Colors", "Button Text", COLORREFToString( MUSIK_COLOR_BTNTEXT ) );
	config->SetValue( "Dialog Colors", "Button Highlight", COLORREFToString( MUSIK_COLOR_BTNHILIGHT ) );
	config->SetValue( "Dialog Colors", "Button Shadow", COLORREFToString( MUSIK_COLOR_BTNSHADOW ) );
	config->SetValue( "Dialog Colors", "List Background", COLORREFToString( MUSIK_COLOR_LISTCTRL ) );
	config->SetValue( "Dialog Colors", "List Text", COLORREFToString( MUSIK_COLOR_LISTCTRLTEXT ) );
	config->SetValue( "Dialog Colors", "Color Highlight", COLORREFToString( MUSIK_COLOR_HIGHLIGHT ) );
	config->SetValue( "Dialog Colors", "Color Highlight Text", COLORREFToString( MUSIK_COLOR_HIGHLIGHTTEXT ) );

	// tunage
	config->SetValue( "Tunage", "Enabled", BoolToString( m_Tunage_Enabled ) ) ;
	config->SetValue( "Tunage", "WriteFile", BoolToString( m_Tunage_WriteFile ) ) ;	config->SetValue( "Tunage", "Filename", m_Tunage_Filename );	config->SetValue( "Tunage", "FileLine", m_Tunage_FileLine );	config->SetValue( "Tunage", "AppendFile", BoolToString(	m_Tunage_AppendFile ) );	config->SetValue( "Tunage", "PostURL", BoolToString( m_Tunage_PostURL ) );	config->SetValue( "Tunage", "URL", m_Tunage_URL ) ;	config->SetValue( "Tunage", "RunApp", BoolToString(	m_Tunage_RunApp ) );	config->SetValue( "Tunage", "CmdLine", m_Tunage_CmdLine );	config->SetValue( "Tunage", "RunOnStop", BoolToString( m_Tunage_RunOnStop ) );	config->SetValue( "Tunage", "StoppedText", m_Tunage_StoppedText );

	// write to ini file
	config->WriteFile();
}

///////////////////////////////////////////////////

string CmusikPrefs::GetDefPlaylistOrder()
{
	CString CRet;
	CRet.Format( _T( "%d,%d,%d,%d,%d,%d,%d" ), 
		MUSIK_LIBRARY_TYPE_TRACKNUM,
		MUSIK_LIBRARY_TYPE_TITLE,
		MUSIK_LIBRARY_TYPE_ARTIST,
		MUSIK_LIBRARY_TYPE_ALBUM,
		MUSIK_LIBRARY_TYPE_GENRE,
		MUSIK_LIBRARY_TYPE_DURATION,
		MUSIK_LIBRARY_TYPE_RATING );
	
	string sRet = CRet.GetBuffer();
	return sRet;
}

///////////////////////////////////////////////////

string CmusikPrefs::GetDefPlaylistSizes()
{
	CString CRet;
	CRet.Format( _T( "50,168,100,100,80,60,70" ) );
	
	string sRet = CRet.GetBuffer();
	return sRet;
}

///////////////////////////////////////////////////

void CmusikPrefs::ParseColors()
{
	const COLORREF transparent = RGB( 255, 0, 255 );

	if ( MUSIK_COLOR_ACTIVECAPTION == transparent )
	{
		MUSIK_COLOR_ACTIVECAPTION = GetSysColor( COLOR_ACTIVECAPTION );
		m_ActiveCaptionDef = true;
	}

	if ( MUSIK_COLOR_CAPTIONTEXT == transparent )
	{
		MUSIK_COLOR_CAPTIONTEXT = GetSysColor( COLOR_CAPTIONTEXT );
		m_CaptionDef = true;
	}

	if ( MUSIK_COLOR_INACTIVECAPTION == transparent )
	{
		MUSIK_COLOR_INACTIVECAPTION = GetSysColor( COLOR_INACTIVECAPTION );
		m_InactiveCaptionDef = true;
	}

	if ( MUSIK_COLOR_INACTIVECAPTIONTEXT == transparent )
	{	
		MUSIK_COLOR_INACTIVECAPTIONTEXT = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
		m_InactiveCaptionTextDef = true;
	}

	if ( MUSIK_COLOR_BTNFACE == transparent )
	{	
		MUSIK_COLOR_BTNFACE = GetSysColor( COLOR_BTNFACE );
		m_BtnFaceDef = true;
	}

	if ( MUSIK_COLOR_BTNTEXT == transparent )
	{	
		MUSIK_COLOR_BTNTEXT = GetSysColor( COLOR_BTNTEXT );
		m_BtnTextDef = true;
	}

	if ( MUSIK_COLOR_BTNHILIGHT == transparent )
	{	
		MUSIK_COLOR_BTNHILIGHT = GetSysColor( COLOR_BTNHILIGHT );
		m_BtnHilightDef = true;
	}
	
	if ( MUSIK_COLOR_BTNSHADOW == transparent )
	{	
		MUSIK_COLOR_BTNSHADOW = GetSysColor( COLOR_BTNSHADOW );
		m_BtnShadowDef = true;
	}

	if ( MUSIK_COLOR_LISTCTRL == transparent )
	{	
		MUSIK_COLOR_LISTCTRL = GetSysColor( COLOR_BTNHILIGHT );
		m_ListCtrlDef = true;
	}

	if ( MUSIK_COLOR_LISTCTRLTEXT == transparent )
	{	
		MUSIK_COLOR_LISTCTRLTEXT = GetSysColor( COLOR_BTNTEXT );
		m_ListCtrlTextDef = true;
	}

	if ( MUSIK_COLOR_HIGHLIGHT == transparent )
	{
		MUSIK_COLOR_HIGHLIGHT = GetSysColor( COLOR_HIGHLIGHT );
		m_HilightDef = true;
	}

	if ( MUSIK_COLOR_HIGHLIGHTTEXT == transparent )
	{
		MUSIK_COLOR_HIGHLIGHTTEXT = GetSysColor( COLOR_HIGHLIGHTTEXT );
		m_HilightTextDef = true;
	}
}

///////////////////////////////////////////////////

void CmusikPrefs::ThemeChanged()
{
	if ( m_ActiveCaptionDef )
		MUSIK_COLOR_ACTIVECAPTION = GetSysColor( COLOR_ACTIVECAPTION );

	if ( m_CaptionDef )
		MUSIK_COLOR_CAPTIONTEXT = GetSysColor( COLOR_CAPTIONTEXT );

	if ( m_InactiveCaptionDef )
		MUSIK_COLOR_INACTIVECAPTION = GetSysColor( COLOR_INACTIVECAPTION );

	if ( m_InactiveCaptionTextDef )
		MUSIK_COLOR_INACTIVECAPTIONTEXT = GetSysColor( COLOR_INACTIVECAPTIONTEXT );

	if ( m_BtnFaceDef )
		MUSIK_COLOR_BTNFACE = GetSysColor( COLOR_BTNFACE );

	if ( m_BtnTextDef )
		MUSIK_COLOR_BTNTEXT = GetSysColor( COLOR_BTNTEXT );

	if ( m_BtnHilightDef )
		MUSIK_COLOR_BTNHILIGHT = GetSysColor( COLOR_BTNHILIGHT );

	if ( m_BtnShadowDef )
		MUSIK_COLOR_BTNSHADOW = GetSysColor( COLOR_BTNSHADOW );

	if ( m_ListCtrlDef )
		MUSIK_COLOR_LISTCTRL = GetSysColor( COLOR_BTNHILIGHT );

	if ( m_ListCtrlTextDef )
		MUSIK_COLOR_LISTCTRLTEXT = GetSysColor( COLOR_BTNTEXT );

	if ( m_HilightDef )
		MUSIK_COLOR_HIGHLIGHT = GetSysColor( COLOR_HIGHLIGHT );

	if ( m_HilightTextDef )
		MUSIK_COLOR_HIGHLIGHTTEXT = GetSysColor( COLOR_HIGHLIGHTTEXT );

}

///////////////////////////////////////////////////

void CmusikPrefs::UnparseColors()
{
	const COLORREF transparent = RGB( 255, 0, 255 );

	if ( MUSIK_COLOR_ACTIVECAPTION == GetSysColor( COLOR_ACTIVECAPTION ) && m_ActiveCaptionDef )
		MUSIK_COLOR_ACTIVECAPTION = transparent;

	if ( MUSIK_COLOR_CAPTIONTEXT == GetSysColor( COLOR_CAPTIONTEXT ) && m_CaptionDef )
		MUSIK_COLOR_CAPTIONTEXT = transparent;

	if ( MUSIK_COLOR_INACTIVECAPTION == GetSysColor( COLOR_INACTIVECAPTION ) && m_InactiveCaptionDef )
		MUSIK_COLOR_INACTIVECAPTION = transparent;

	if ( MUSIK_COLOR_INACTIVECAPTIONTEXT == GetSysColor( COLOR_INACTIVECAPTIONTEXT ) && m_InactiveCaptionTextDef )
		MUSIK_COLOR_INACTIVECAPTIONTEXT = transparent;
	
	if ( MUSIK_COLOR_BTNFACE == GetSysColor( COLOR_BTNFACE ) && m_BtnFaceDef )
		MUSIK_COLOR_BTNFACE = transparent;

	if ( MUSIK_COLOR_BTNTEXT == GetSysColor( COLOR_BTNTEXT ) && m_BtnTextDef )
		MUSIK_COLOR_BTNTEXT = transparent;

	if ( MUSIK_COLOR_BTNHILIGHT == GetSysColor( COLOR_BTNHILIGHT ) && m_BtnHilightDef )
		MUSIK_COLOR_BTNHILIGHT = transparent;
	
	if ( MUSIK_COLOR_BTNSHADOW == GetSysColor( COLOR_BTNSHADOW ) && m_BtnShadowDef )
		MUSIK_COLOR_BTNSHADOW = transparent;
	
	if ( MUSIK_COLOR_LISTCTRL == GetSysColor( COLOR_BTNHILIGHT ) && m_ListCtrlDef )
		MUSIK_COLOR_LISTCTRL = transparent;

	if ( MUSIK_COLOR_LISTCTRLTEXT == GetSysColor( COLOR_BTNTEXT ) && m_ListCtrlTextDef )
		MUSIK_COLOR_LISTCTRLTEXT = transparent;

	if ( MUSIK_COLOR_HIGHLIGHT == GetSysColor( COLOR_HIGHLIGHT ) && m_HilightDef )
		MUSIK_COLOR_HIGHLIGHT = transparent;

	if ( MUSIK_COLOR_HIGHLIGHTTEXT == GetSysColor( COLOR_HIGHLIGHTTEXT ) && m_HilightTextDef )
		MUSIK_COLOR_HIGHLIGHTTEXT = transparent;
}

///////////////////////////////////////////////////