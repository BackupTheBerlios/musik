/*
 *  Tunage.cpp
 *
 *  Musik's system for updating websites, files, etc.
 *  with currently-playing info
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include "Tunage.h"

#include "../MusikGlobals.h"

#include "../MusikUtils.h"
#include "wx/url.h"
#include "wx/wfstream.h"
#include "wx/utils.h"

CTunage::CTunage()
{
}

CTunage::~CTunage()
{
}


///////////////////////////////////////////////////////////////////////////////

void CTunage::Execute( CMusikSong& song )
{
	// bail if nothing to do
	if ( !g_Prefs.nTunageWriteFile && !g_Prefs.nTunagePostURL && !g_Prefs.nTunageRunApp )
		return;

	m_Song = song;

	if ( g_Prefs.nTunageWriteFile )
		WriteFile();

	if ( g_Prefs.nTunagePostURL )
		PostURL();

	if ( g_Prefs.nTunageRunApp )
		RunApp();
}

void CTunage::Stopped()
{
	CMusikSong stopsong;
	stopsong.Filesize = -1;
	Execute( stopsong );
}

///////////////////////////////////////////////////////////////////////////////

void CTunage::PostURL()
{
	if ( g_Prefs.sTunageURL == wxT("") )
		return;

	// create a valid URL
	wxString urltext( g_Prefs.sTunageURL );
	ParseTags( urltext );
	urltext = wxURL::ConvertToValidURI( urltext );
	wxURL url( urltext );
	
	if ( url.GetError() != wxURL_NOERR )
	{
		wxMessageBox( wxT("Could not parse Tunage URL"), wxT("Tunage error"), wxOK | wxICON_WARNING );
		
		return;
	}

	// connect to URL
	wxYield();
	wxInputStream *data = url.GetInputStream();
	if ( !data )
	{
		wxMessageBox( wxT("Could not access URL"), wxT("Tunage error"), wxOK | wxICON_WARNING );
	}

	delete data;



}

///////////////////////////////////////////////////////////////////////////////

void CTunage::WriteFile()
{
	if ( ( g_Prefs.sTunageFilename == wxT("") ) || ( g_Prefs.sTunageFileLine == wxT("") ) )
		return;

	if ( !g_Prefs.nTunageAppendFile )
	{
		if ( wxFileExists( g_Prefs.sTunageFilename ) )
			wxRemoveFile( g_Prefs.sTunageFilename );
	}

	wxTextFile Out;	
	Out.Create( g_Prefs.sTunageFilename );
	Out.Open();

	if ( !Out.IsOpened() )
		return;

	wxString line( g_Prefs.sTunageFileLine );
	ParseTags( line );

	Out.AddLine( line );
	Out.Write( Out.GuessType() );
	Out.Close();
}

///////////////////////////////////////////////////////////////////////////////

void CTunage::RunApp()
{
	if ( g_Prefs.sTunageCmdLine == wxT("") )
		return;

	wxString cmd( g_Prefs.sTunageCmdLine );
	ParseTags( cmd );

	wxExecute( cmd );
}

///////////////////////////////////////////////////////////////////////////////

// replaces special tags such as %TITLE% with info from the song
void CTunage::ParseTags( wxString& str )
{
	CNiceFilesize filesize;
	filesize.AddB( m_Song.Filesize );
	wxString sFilesize = filesize.GetFormatted();

	str.Replace( wxT("$ARTIST"), m_Song.Artist );
	str.Replace( wxT("$ALBUM"), m_Song.Album );
	str.Replace( wxT("$TITLE"), m_Song.Title );

	if ( m_Song.Filesize == -1 )
		str.Replace( wxT("$NAME"), g_Prefs.sTunageStoppedText );
	else
		str.Replace( wxT("$NAME"), wxString::Format( wxT("%s - %s"), m_Song.Artist, m_Song.Title ) );

	str.Replace( wxT("$FILENAME"), m_Song.Filename );
	str.Replace( wxT("$FILESIZE"), sFilesize );
	str.Replace( wxT("$BITRATE"), wxString::Format( wxT("%d"), m_Song.Bitrate ) );

	str.Replace( wxT("$TIMESPLAYED"), wxString::Format( wxT("%d"), m_Song.TimesPlayed ) );
	str.Replace( wxT("$TRACKNUM"), wxString::Format( wxT("%d"), m_Song.TrackNum ) );
}

