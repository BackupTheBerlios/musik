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

CMusikPrefs::CMusikPrefs( CString filename )
{
	config = new CIniFile( filename.GetBuffer() );

	if ( !config->ReadFile() )
		config->WriteFile();

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
	m_Playlist_Stripe_Color	= StringToCOLORREF( config->GetValue( "Playlist", "Stripe Color", GetDefPlaylistStripeColor() ) );

	// now playing
	m_NowPlaying_CaptionFont = StringToInt( config->GetValue( "Now Playing", "Caption Font Size", "11" ) );
	
	// player
	m_Player_Driver			= StringToInt( config->GetValue( "Player", "Driver", "0" ) );
	m_Player_Device			= StringToInt( config->GetValue( "Player", "Device", "0" ) );
	m_Player_Rate			= StringToInt( config->GetValue( "Player", "Rate", "44100" ) );
	m_Player_Max_Channels	= StringToInt( config->GetValue( "Player", "Maximum Channels", "6" ) );
}

///////////////////////////////////////////////////

void CMusikPrefs::SavePrefs()
{
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
	config->SetValue( "Playlist", "Stripe Color", COLORREFToString( m_Playlist_Stripe_Color ) );

	// now playing
	config->SetValue( "Now Playing", "Caption Font Size", IntToString( m_NowPlaying_CaptionFont ) );

	// player
	config->SetValue( "Player", "Driver", IntToString( m_Player_Driver ) );
	config->SetValue( "Player", "Device", IntToString( m_Player_Device ) );
	config->SetValue( "Player", "Rate", IntToString( m_Player_Rate ) );
	config->SetValue( "Player", "Maximum Channels", IntToString( m_Player_Max_Channels ) );

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

string CMusikPrefs::GetDefPlaylistStripeColor()
{
	CString CRet;
	CRet.Format( _T( "244,244,244" ) );

	string sRet = CRet.GetBuffer();
	return sRet;
}

///////////////////////////////////////////////////