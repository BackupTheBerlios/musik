#include "stdafx.h"

#include "MusikPrefs.h"

inline int StringToInt( const string str )
{
	return atoi( str.c_str() );	
}

inline string IntToString( int n )
{
	char buffer[20];
	itoa( n, buffer, 10 );
	string str = buffer;
    return str;	
}

inline CSize StringToCSize( string str )
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

inline string CSizeToString( CSize size )
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

inline CPoint StringToCPoint( string str )
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

inline string CPointToString( CPoint pt )
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

CMusikPrefs::CMusikPrefs( CString filename )
{
	config = new CIniFile( filename.GetBuffer() );

	if ( !config->ReadFile() )
		config->WriteFile();

	LoadPrefs();
}

CMusikPrefs::~CMusikPrefs()
{
	SavePrefs();
	delete config;
}

void CMusikPrefs::LoadPrefs()
{
	//-----------------------------------------------------//
	//--- whole dialog									---//
	//-----------------------------------------------------//
	m_Dlg_Size	= StringToCSize( config->GetValue( "Dialog", "Dialog Size", "800x600" ) );
	m_Dlg_Pos	= StringToCPoint( config->GetValue( "Dialog", "Dialog Position", "50,50" ) );

	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	m_SelectionBox_Count = StringToInt( config->GetValue( "Selection Area", "Count", "2" ) );

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- now playing									---//
	//-----------------------------------------------------//
	m_NowPlaying_Height = StringToInt( config->GetValue( "Now Playing", "Height", "72" ) );
}

void CMusikPrefs::SavePrefs()
{
	//-----------------------------------------------------//
	//--- whole dialog									---//
	//-----------------------------------------------------//
	config->SetValue( "Dialog", "Dialog Size", CSizeToString( m_Dlg_Size ) );
	config->SetValue( "Dialog", "Dialog Position", CPointToString( m_Dlg_Pos ) );

	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	config->SetValue( "Selection Area", "Count", IntToString( m_SelectionBox_Count ) );

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- now playing									---//
	//-----------------------------------------------------//
	config->SetValue( "Now Playing", "Height", IntToString( m_NowPlaying_Height ) );

	config->WriteFile();
}
