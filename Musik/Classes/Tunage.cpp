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
		wxMessageBox( wxT("Could not parse Tunage URL"), wxT("Tunage error"), wxOK );
		
		return;
	}

	// connect to URL
	wxYield();
	wxInputStream *data = url.GetInputStream();
	if ( !data )
	{
		wxMessageBox( wxT("Could not access URL"), wxT("Tunage error"), wxOK );
	}

	delete data;



}

///////////////////////////////////////////////////////////////////////////////

void CTunage::WriteFile()
{
}

///////////////////////////////////////////////////////////////////////////////

void CTunage::RunApp()
{
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

	str.Replace( wxT("$NAME"), wxString::Format( wxT("%s - %s"), m_Song.Artist, m_Song.Title ) );
	str.Replace( wxT("$FILENAME"), m_Song.Filename );
	str.Replace( wxT("$FILESIZE"), sFilesize );
	str.Replace( wxT("$BITRATE"), wxString::Format( wxT("%d"), m_Song.Bitrate ) );

	str.Replace( wxT("$TIMESPLAYED"), wxString::Format( wxT("%d"), m_Song.TimesPlayed ) );
	str.Replace( wxT("$TRACKNUM"), wxString::Format( wxT("%d"), m_Song.TrackNum ) );

	
}