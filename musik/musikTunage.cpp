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
//   CmusikTunage
//
// Filename(s): 
//
//   musikTunage.h, musikTunage.cpp
//
// Information:
//
//   Does something related to current song info, such as post to a website,
//   write to a text file, etc.
// 
// Usage: 
//
//	 Called when a song changes or is stopped
//
///////////////////////////////////////////////////

#include "StdAfx.h"

#include <afxinet.h>

#include "musik.h"
#include "musikTunage.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikFilename.h"

///////////////////////////////////////////////////

CmusikTunage::CmusikTunage( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikTunage::~CmusikTunage(void)
{
}

///////////////////////////////////////////////////

void CmusikTunage::Execute()
{
	if ( m_Prefs->GetTunagePostURL() )
		Execute_PostURL();

	if ( m_Prefs->GetTunageRunApp() )
		Execute_RunApp();

	if ( m_Prefs->GetTunageWriteFile() )
		Execute_WriteFile();
}

///////////////////////////////////////////////////

void CmusikTunage::Execute_PostURL()
{
	// bail if empty URL
	if ( m_Prefs->GetTunageURL() == "" ) 
		return;

	CmusikString url( m_Prefs->GetTunageURL() );
	ParseTags( url, true );

	CInternetSession session;
	CStdioFile* file = session.OpenURL( url );

	delete file;
	session.Close();
}

///////////////////////////////////////////////////

void CmusikTunage::Execute_RunApp()
{
	// bail if empty
	if ( m_Prefs->GetTunageCmdLine() == "" )
		return;

	CmusikString str( m_Prefs->GetTunageCmdLine() );
	ParseTags( str, false );

	WinExec( str, SW_NORMAL );
}

///////////////////////////////////////////////////

void CmusikTunage::Execute_WriteFile()
{
	// bail if empty
	if ( ( m_Prefs->GetTunageFilename() == "" ) || ( m_Prefs->GetTunageFileLine() == "" ) )
		return;

	if ( !m_Prefs->GetTunageAppendFile() )
	{
		if (  CmusikFilename::FileExists( m_Prefs->GetTunageFilename() ) )
			DeleteFile( m_Prefs->GetTunageFilename().c_str() );
	}

	CStdioFile out;
	if ( !out.Open( m_Prefs->GetTunageFilename().c_str(), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText ) )
		return;

	CmusikString str( m_Prefs->GetTunageFileLine() );
	ParseTags( str, false );

	out.WriteString( str );
	
	out.Close();
}

///////////////////////////////////////////////////

void CmusikTunage::ParseTags( CmusikString& str, bool encode )
{	
	CmusikString artist = m_Player->GetCurrPlaying()->GetArtist();
	if ( encode )
		URLEncode( artist );
	str.Replace( "$ARTIST", artist );

	CmusikString album = m_Player->GetCurrPlaying()->GetAlbum();
	if ( encode )
		URLEncode( album );
	str.Replace( "$ALBUM", album );
	
	CmusikString title = m_Player->GetCurrPlaying()->GetTitle();
	if ( encode )
		URLEncode( title );
	str.Replace( "$TITLE", title );

	CmusikString name;
	name.Format( "%s - %s", artist, title );
	str.Replace( "$NAME", name );

	CmusikString filename = m_Player->GetCurrPlaying()->GetFilename();
	if ( encode )
		URLEncode( filename );
	str.Replace( "$FILENAME", filename );

	double filesize = atof( m_Player->GetCurrPlaying()->GetFilesize().c_str() );
	CmusikString strsize( "0.0 mb" );
	if ( filesize < 1024.0 )
		strsize.Format( "%.2f b", filesize );
	else if ( filesize < ( 1024.0 * 1024.0 ) )
		strsize.Format( "%.2f kb", filesize / 1024.0 );
	else if ( filesize < ( 1024.0 * 1024.0 * 1024.0 ) )
		strsize.Format( "%.2f mb", filesize / 1024.0 / 1024.0 );
	else if ( filesize < ( 1024.0 * 1024.0 * 1024.0 * 1024.0 ) )
		strsize.Format( "%.2f gb", filesize / 1024.0 / 1024.0 / 1024.0 );

	str.Replace( "$FILESIZE", strsize );
	
	str.Replace( "$BITRATE", m_Player->GetCurrPlaying()->GetBitrate().c_str() );

	str.Replace( "$TIMESPLAYED", m_Player->GetCurrPlaying()->GetTimesPlayed().c_str() );
	str.Replace( "$TRACKNUM", m_Player->GetCurrPlaying()->GetTrackNum().c_str() );
}

///////////////////////////////////////////////////

// this is pretty much taken from Oddsock's Do Something plugin:
// http://www.oddsock.org/tools/dosomething/
void CmusikTunage::URLEncode( CmusikString& str )
{
	str.Replace( "%", "%25" );
	str.Replace( ";", "%3B" );
	str.Replace( "/", "%2F" );
	str.Replace( "?", "%3F" );
	str.Replace( ":", "%3A" );
	str.Replace( "@", "%40" );
	str.Replace( "&", "%26" );
	str.Replace( "=", "%3D" );
	str.Replace( "+", "%2B" );
	str.Replace( " ", "%20" );
	str.Replace( "\"", "%22" );
	str.Replace( "#", "%23" );
	
	str.Replace( "<", "%3C" );
	str.Replace( ">", "%3E" );
	str.Replace( "!", "%21" );
	str.Replace( "*", "%2A" );
	str.Replace( "'", "%27" );
	str.Replace( "(", "%28" );
	str.Replace( ")", "%29" );
	str.Replace( ",", "%2C" );
}