/*
 *  COggInfo.cpp
 *
 *  Helper functions to retrieve oggvorbis comments from a given file.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"
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

		m_Song.Artist		= ConvDBFieldToWX( vorbis_comment_query( pComment, "artist",	0 ) );
		m_Song.Album		= ConvDBFieldToWX( vorbis_comment_query( pComment, "album",		0 ) );
		m_Song.Title		= ConvDBFieldToWX( vorbis_comment_query( pComment, "title",		0 ) );
		m_Song.Genre		= ConvDBFieldToWX( vorbis_comment_query( pComment, "genre",		0 ) );
		m_Song.Year			= ConvDBFieldToWX( vorbis_comment_query( pComment, "date", 		0 ) );
		m_Song.TrackNum		= wxStringToInt( ConvDBFieldToWX( vorbis_comment_query( pComment, "tracknumber", 0 ) ) );

		//--- if the title is empty, make it the filenaem ---//
		if ( m_Song.Title.IsEmpty() )
		{
			wxFileName fn( filename );
			wxString fullname = fn.GetFullName();
			wxString justfilename = fullname.Left( fullname.Length() - fn.GetExt().Len()-1 );
			m_Song.Title = justfilename;
		}

		//--- grab the bitrate ---//
		vorbis_info *pInfo = ov_info( &vorbisfile,-1 );
		m_Song.Bitrate = (int)pInfo->bitrate_nominal / 1000;

		//--- cleanup ---//
		vorbis_comment_clear( pComment );
		vorbis_info_clear( pInfo );
		ov_clear( &vorbisfile );	//--- will also close the FILE (audiofile) ---//

		return true;
	}
	else
	{
		fclose( audiofile );
		return false;
	}

}
