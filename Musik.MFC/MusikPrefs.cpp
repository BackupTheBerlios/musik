///////////////////////////////////////////////////

#include "stdafx.h"

#include "MusikPrefs.h"

#include "../Musik.Core/include/MusikLibrary.h"

///////////////////////////////////////////////////

inline int StringToInt( const string str )
{
	return atoi( str.c_str() );	
}

///////////////////////////////////////////////////

inline string IntToString( const int &n )
{
	char buffer[20];
	itoa( n, buffer, 10 );
	string str = buffer;
    return str;	
}

///////////////////////////////////////////////////

inline CSize StringToCSize( const string &str )
{
	CSize ret;
	CString cstr = str.c_str();
	if ( cstr.Find( "x", 0 ) )
	{
		int currpos = 0;
		CString strx = cstr.Tokenize( "x", currpos );
		CString stry = cstr.Tokenize( "x", currpos );
		ret.cx = atoi( strx );
		ret.cy = atoi( stry );
	}
	else
		ret.SetSize( 0, 0 );

	return ret;
}

///////////////////////////////////////////////////

inline string CSizeToString( const CSize &size )
{
	string ret;
	char buffer_w[10];
	char buffer_h[10];

	itoa( size.cx, buffer_w, 10 );
	itoa( size.cy, buffer_h, 10 );

	string str;
	str += buffer_w;
	str += "x";
	str += buffer_h;

	return str;
}

///////////////////////////////////////////////////

inline CPoint StringToCPoint( const string &str )
{
	CPoint ret;
	CString cstr = str.c_str();
	if ( cstr.Find( ",", 0 ) )
	{
		int currpos = 0;
		CString strx = cstr.Tokenize( ",", currpos );
		CString stry = cstr.Tokenize( ",", currpos );
		ret.x = atoi( strx );
		ret.y = atoi( stry );
	}
	else
	{
		ret.x = 0;
		ret.y = 0;
	}

	return ret;
}

///////////////////////////////////////////////////

inline string CPointToString( const CPoint &pt )
{
	string ret;
	char buffer_x[10];
	char buffer_y[10];

	itoa( pt.x, buffer_x, 10 );
	itoa( pt.y, buffer_y, 10 );

	string str;
	str += buffer_x;
	str += ",";
	str += buffer_y;

	return str;
}

///////////////////////////////////////////////////

inline CIntArray StringToCIntArray( const string &str )
{
	CIntArray ret;
	CString cstr = str.c_str();
	int pos = 0;

	CString resToken;
	resToken = cstr.Tokenize( ",", pos);
	while ( resToken != "" )
	{
		ret.push_back( atoi( resToken.GetBuffer() ) );
		resToken = cstr.Tokenize( ",", pos );
	};

	return ret;
}

///////////////////////////////////////////////////

inline string CIntArrayToString( const CIntArray &array )
{
	CString CRet;
	char buffer[10];
	for ( size_t i = 0; i < array.size(); i++ )
	{
		CRet += itoa( array.at( i ), buffer, 10 );
		if ( i != ( array.size() - 1 ) )
			CRet += ",";
	}
	string sRet = CRet.GetBuffer();
	return sRet;
}

///////////////////////////////////////////////////

inline COLORREF StringToCOLORREF( const string str )
{
	int r = 0, g = 0, b = 0;
	CString CStr = str.c_str();
	int pos = 0;
	CString resToken;
	r = atoi( CStr.Tokenize( ",", pos ) );
	g = atoi( CStr.Tokenize( ",", pos ) );
	b = atoi( CStr.Tokenize( ",", pos ) );

	COLORREF ret = RGB( r, g, b );
	return ret;
}

///////////////////////////////////////////////////

inline string COLORREFToString( const COLORREF color )
{
	CString CRet;
	CRet.Format( _T( "%d,%d,%d" ), 
		GetRValue( color ), 
		GetGValue( color ), 
		GetBValue( color ) );

	string sRet = CRet.GetBuffer();
	return sRet;
}

///////////////////////////////////////////////////

inline bool StringToBool( const string str )
{
	if ( str.empty() )
		return false;

	if ( str == _T( "1" ) )
		return true;

	return false;
}

///////////////////////////////////////////////////

inline string BoolToString( const bool val )
{
	if ( val )
		return _T( "1" );

	return _T( "0" );
}

///////////////////////////////////////////////////

inline CMusikCrossfader StringToCrossfader( const string& str )
{
	CMusikCrossfader ret;
	CString CStr = str.c_str();

	int pos = 0;

	double new_song		= atof( CStr.Tokenize( ",", pos ) );
	double pause_resume	= atof( CStr.Tokenize( ",", pos ) );
	double seek			= atof( CStr.Tokenize( ",", pos ) );
	double stop			= atof( CStr.Tokenize( ",", pos ) );
	double exit			= atof( CStr.Tokenize( ",", pos ) );

	ret.Set( (float)new_song, (float)pause_resume, (float)seek, (float)stop, (float)exit );

	return ret;
}

///////////////////////////////////////////////////

inline string CrossfaderToString( CMusikCrossfader& fader )
{
	CStdString sRet;
	sRet.Format( _T( "%f, %f, %f, %f, %f" ), 
		fader.GetDuration( MUSIK_CROSSFADER_NEW_SONG ),
		fader.GetDuration( MUSIK_CROSSFADER_PAUSE_RESUME ),
		fader.GetDuration( MUSIK_CROSSFADER_SEEK ),
		fader.GetDuration( MUSIK_CROSSFADER_STOP ),
		fader.GetDuration( MUSIK_CROSSFADER_EXIT ) );

	return sRet.c_str();
}

///////////////////////////////////////////////////

CMusikPrefs::CMusikPrefs( CString filename )
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

CMusikPrefs::~CMusikPrefs()
{
	SavePrefs();
	delete config;
}

///////////////////////////////////////////////////

void CMusikPrefs::LoadPrefs()
{
	// main dialog
	m_Dlg_Size		= StringToCSize( config->GetValue( "Dialog", "Dialog Size", "800x600" ) );
	m_Dlg_Pos		= StringToCPoint( config->GetValue( "Dialog", "Dialog Position", "50,50" ) );
	m_Dlg_Maximized	= StringToBool( config->GetValue( "Dialog", "Maximized", "0" ) );
	m_Dlg_ResetUI	= StringToBool( config->GetValue( "Dialog", "Reset UI", "1" ) );

	// selection area
	m_SelectionBox_Count = StringToInt( config->GetValue( "Selection Area", "Count", "2" ) );

	// playlist
	m_Playlist_Order		= StringToCIntArray( config->GetValue( "Playlist", "Column Order", GetDefPlaylistOrder() ) );
	m_Playlist_Sizes		= StringToCIntArray( config->GetValue( "Playlist", "Column Sizes", GetDefPlaylistSizes() ) );

	// now playing
	m_NowPlaying_CaptionFont = StringToInt( config->GetValue( "Now Playing", "Caption Font Size", "11" ) );
	
	// player
	m_Player_Driver			= StringToInt( config->GetValue( "Player", "Driver", "0" ) );
	m_Player_Device			= StringToInt( config->GetValue( "Player", "Device", "0" ) );
	m_Player_Rate			= StringToInt( config->GetValue( "Player", "Rate", "44100" ) );
	m_Player_Max_Channels	= StringToInt( config->GetValue( "Player", "Maximum Channels", "6" ) );

	// crossfader
	m_Crossfader_Enabled	= StringToBool( config->GetValue( "Crossfader", "Enabled", "1" ) );
	m_Crossfader_Current	= config->GetValue( "Crossfader", "Set Name", "Default" );
	m_Crossfader_Default	= StringToCrossfader( config->GetValue( "Crossfader", "Values", "2.0,0.5,1.0,3.0" ) );

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

	ParseColors();
}

///////////////////////////////////////////////////

void CMusikPrefs::SavePrefs()
{
	UnparseColors();

	// main dialog
	config->SetValue( "Dialog", "Dialog Size", CSizeToString( m_Dlg_Size ) );
	config->SetValue( "Dialog", "Dialog Position", CPointToString( m_Dlg_Pos ) );
	config->SetValue( "Dialog", "Maximized", BoolToString( m_Dlg_Maximized ) );
	config->SetValue( "Dialog", "Reset UI", BoolToString( m_Dlg_ResetUI ) );

	// selection area
	config->SetValue( "Selection Area", "Count", IntToString( (int)m_SelectionBox_Count ) );

	// playlist
	config->SetValue( "Playlist", "Column Order", CIntArrayToString( m_Playlist_Order ) );
	config->SetValue( "Playlist", "Column Sizes", CIntArrayToString( m_Playlist_Sizes ) );

	// now playing
	config->SetValue( "Now Playing", "Caption Font Size", IntToString( m_NowPlaying_CaptionFont ) );

	// player
	config->SetValue( "Player", "Driver", IntToString( m_Player_Driver ) );
	config->SetValue( "Player", "Device", IntToString( m_Player_Device ) );
	config->SetValue( "Player", "Rate", IntToString( m_Player_Rate ) );
	config->SetValue( "Player", "Maximum Channels", IntToString( m_Player_Max_Channels ) );

	// crossfader
	config->SetValue( "Crossfader", "Enabled", BoolToString( m_Crossfader_Enabled ) );
	config->SetValue( "Crossfader", "Set Name", m_Crossfader_Current );	
	config->SetValue( "Crossfader", "Values", CrossfaderToString( m_Crossfader_Default ) );

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

	// write to ini file
	config->WriteFile();
}

///////////////////////////////////////////////////

string CMusikPrefs::GetDefPlaylistOrder()
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

string CMusikPrefs::GetDefPlaylistSizes()
{
	CString CRet;
	CRet.Format( _T( "50,168,100,100,80,60,70" ) );
	
	string sRet = CRet.GetBuffer();
	return sRet;
}

///////////////////////////////////////////////////

void CMusikPrefs::ParseColors()
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

void CMusikPrefs::ThemeChanged()
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

void CMusikPrefs::UnparseColors()
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