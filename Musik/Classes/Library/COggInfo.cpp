//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

/*
 *  COggInfo.cpp
 *
 *  Helper functions to retrieve oggvorbis comments from a given file.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include "COggInfo.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <wx/filename.h>

#include "../../MusikUtils.h"

bool COggInfo::loadInfo( const wxString &filename )
{
	if ( !wxFileExists( filename ) )
		return false;

	
	FILE *audiofile = wxFopen( filename, wxT("rb") );
	OggVorbis_File vorbisfile;

	if ( ov_open( audiofile, &vorbisfile, NULL, 0 ) >= 0 )
	{
		vorbis_comment *pComment = ov_comment( &vorbisfile, -1 );

		wxString artist		= ConvDBFieldToWX( vorbis_comment_query( pComment, "artist", 		0 ) );
		wxString album		= ConvDBFieldToWX( vorbis_comment_query( pComment, "album", 		0 ) );
		wxString title		= ConvDBFieldToWX( vorbis_comment_query( pComment, "title", 		0 ) );
		wxString tracknum	= ConvDBFieldToWX( vorbis_comment_query( pComment, "tracknumber",	0 ) );
		wxString genre		= ConvDBFieldToWX( vorbis_comment_query( pComment, "genre", 		0 ) );
		wxString year		= ConvDBFieldToWX( vorbis_comment_query( pComment, "date", 			0 ) );

		// remove any spaces that poor taggers might have left
		artist.Trim();		artist.Trim( false );
		album.Trim();		album.Trim( false );
		title.Trim();		title.Trim( false );
		tracknum.Trim();	tracknum.Trim( false );
		genre.Trim();		genre.Trim( false );
		year.Trim();		year.Trim( false );

		// only set the song info for non-empty strings
		if ( artist.Length() > 0 )
			m_Song.Artist = artist;
		if ( album.Length() > 0 )
			m_Song.Album = album;
		if ( title.Length() > 0 )
			m_Song.Title = title;
		else
		{
			wxFileName fn( filename );
			wxString fullname = fn.GetFullName();
			wxString justfilename = fullname.Left( fullname.Length() - fn.GetExt().Len()-1 );
			m_Song.Title = justfilename;
		}
		if ( tracknum.Length() > 0 )
			m_Song.TrackNum = StringToInt( tracknum );
		if ( genre.Length() > 0 )
			m_Song.Genre = genre;
		if ( year.Length() > 0 )
			m_Song.Year = year;

		// and now the other stuff
		vorbis_info *pInfo = ov_info( &vorbisfile,-1 );

		m_Song.Bitrate = (int)pInfo->bitrate_nominal / 1000;

		// cleanup
		vorbis_comment_clear( pComment );
		vorbis_info_clear( pInfo );

		// ov_clear also closes the file
		ov_clear( &vorbisfile );

		return true;
	}
	else
	{
		fclose( audiofile );
		return false;
	}

}
