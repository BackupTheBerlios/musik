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

#pragma once

///////////////////////////////////////////////////

#include "stdafx.h"
#include <sstream>

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
	bool SynchronizeOnStartup(){ return m_Dlg_SynchronizeOnStartup; }
	bool WriteTagsToFile(){ return m_Dlg_WriteTagsToFile; }
	bool LibraryShowsAllSongs(){ return m_Dlg_LibraryShowsAllSongs; }
	bool MinimizeToTray(){ return m_Dlg_MinimizeToTray; }
	bool AddEntireToNP(){ return m_Dlg_AddEntireToNP; }

	void SetDlgSize( const CSize sz ){ m_Dlg_Size = sz; }
	void SetDlgPos( const CPoint pt ){ m_Dlg_Pos = pt; }
	void SetMaximized( bool max ){ m_Dlg_Maximized = max; }
	void SetDlgResetUI( bool reset ){ m_Dlg_ResetUI = reset; }
	void SetStdPlaylistPrompt( int prompt ){ m_Dlg_StdPlaylist_Prompt = prompt; }
	void SetFileDropPrompt( int prompt ){ m_Dlg_FileDrop_Prompt = prompt; }
	void SetPlaylistInfoVisible( bool visible )	{ m_Dlg_PlaylistInfoVisible = visible; }
	void SetSynchronizeOnStartup( bool synchronize ){ m_Dlg_SynchronizeOnStartup = synchronize; }
	void SetWriteTagsToFile( bool write ){ m_Dlg_WriteTagsToFile = write; }
	void SetLibraryShowsAllSongs( bool showall ){ m_Dlg_LibraryShowsAllSongs = showall; }
	void SetMinimizeToTray( bool tray ){ m_Dlg_MinimizeToTray = tray; }
	void SetAddEntireToNP( bool entire ){ m_Dlg_AddEntireToNP = entire; }

	void AppendPlaylistColOrder( int col ){ m_Playlist_Order.push_back( col ); }
	void AppendPlaylistColSize( int size ){ m_Playlist_Sizes.push_back( size ); }
	void RemovePlaylistColOrder( int at ){ m_Playlist_Order.erase( m_Playlist_Order.begin() + at ); }
	void RemovePlaylistColSize( int at ){ m_Playlist_Sizes.erase( m_Playlist_Sizes.begin() + at ); }

	//
	// selection box
	//

	size_t GetSelBoxCount(){ return m_SelectionBox_Types.size(); }
	int GetSelBoxType( int index ){ return m_SelectionBox_Types.at( index ); }

	void SetSelBoxType( int index, int type ){ m_SelectionBox_Types.at( index ) = type; }
	void SetNewSelBoxType( int type ){ m_SelectionBox_Types.push_back( type ); }
	void SetSelBoxTypes( const CIntArray& types ){ m_SelectionBox_Types = types; }

	//
	// sources
	//
	bool IsSubLibsVisible(){ return m_Sources_SubLibsVisible; }
	bool IsPlaylistsVisible(){ return m_Sources_PlaylistsVisible; }
	bool IsQuickSearchVisible(){ return m_Sources_QuickSearchVisible; }

	void SetSubLibsVisible( bool visible ){ m_Sources_SubLibsVisible = visible; }
	void SetPlaylistsVisible( bool visible ){ m_Sources_PlaylistsVisible = visible; }
	void SetQuickSearchVisible( bool visible ){ m_Sources_QuickSearchVisible = visible; }	
	
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
	// transparency
	//
	bool IsTransEnabled(){ return m_TransEnabled; }
	bool IsTransAdaptive(){ return m_TransAdaptive; }
	float GetTransDur(){ return m_TransDur; }
	int GetTransFocus(){ return m_TransFocus; }
	int GetTransUnFocus(){ return m_TransUnfocus; }

	void SetTransEnabled( bool enabled ){ m_TransEnabled = enabled; }
	void SetTransAdaptive( bool adaptive ){ m_TransAdaptive = adaptive; }
	void SetTransDur( float secs ){ m_TransDur = secs; }
	void SetTransFocus( int trans ){ m_TransFocus = trans; }
	void SetTransUnFocus( int trans ){ m_TransUnfocus = trans; }

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

	void SetPlayerDriver( int n ){ m_Player_Driver = n; }
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
	bool	m_Dlg_SynchronizeOnStartup;
	bool	m_Dlg_WriteTagsToFile;
	bool	m_Dlg_LibraryShowsAllSongs;
	bool	m_Dlg_MinimizeToTray;
	bool	m_Dlg_AddEntireToNP;

	// sources
	bool	m_Sources_SubLibsVisible;
	bool	m_Sources_PlaylistsVisible;
	bool	m_Sources_QuickSearchVisible;

	// transparency
	bool m_TransEnabled;
	bool m_TransAdaptive;
	float m_TransDur;
	int m_TransFocus;
	int m_TransUnfocus;

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

// handy conversion functions

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

inline float StringToFloat( string s )
{
	float fRet;

	istringstream aStream;
	aStream.str( s );
	aStream >> (float)fRet;

	return fRet;
}

///////////////////////////////////////////////////

inline string FloatToString( float f )
{
	ostringstream aStream;
	aStream << f << "\0";

	string str = aStream.str();

	return str;
}

///////////////////////////////////////////////////

inline CString FloatToCString( float f )
{
	ostringstream aStream;
	aStream << f << "\0";

	CString str = aStream.str().c_str();

	return str;
}

///////////////////////////////////////////////////

inline CString IntToCString( int n )
{
	ostringstream aStream;
	aStream << n << "\0";

	CString str = aStream.str().c_str();

	return str;
}

///////////////////////////////////////////////////

inline unsigned long StringToULong( string s )
{
	unsigned long nRet;

	istringstream aStream;
	aStream.str( s );
	aStream >> (unsigned long)nRet;

	return nRet;
}

///////////////////////////////////////////////////

inline string ULongToString( unsigned long ul )
{
	ostringstream aStream;
	aStream << ul << "\0";

	string str = aStream.str();

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