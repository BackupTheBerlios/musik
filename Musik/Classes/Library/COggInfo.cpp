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

bool loadOGGInfo( CSongMetaData & MetaData )
{
	FILE *audiofile = wxFopen( MetaData.Filename.GetFullPath(), wxT("rb") );
	if( !audiofile )
		return false;
	// get filesize
	fseek(audiofile,0,SEEK_END);
	MetaData.nFilesize = ftell(audiofile);
	fseek(audiofile,0,SEEK_SET);

	OggVorbis_File vorbisfile;

	if ( ov_open( audiofile, &vorbisfile, NULL, 0 ) >= 0 )
	{
		vorbis_comment *pComment = ov_comment( &vorbisfile, -1 );

		MetaData.Artist = vorbis_comment_query( pComment, "artist",	0 );
		MetaData.Album	= vorbis_comment_query( pComment, "album",		0 );
		MetaData.Title	= vorbis_comment_query( pComment, "title",		0 );
		MetaData.Genre	= vorbis_comment_query( pComment, "genre",		0 );
		MetaData.Year	= vorbis_comment_query( pComment, "date", 		0 );
		char *szTracknum = vorbis_comment_query( pComment, "tracknumber", 0 );
		MetaData.nTracknum = atol( szTracknum ? szTracknum : "0" );

		//--- grab the bitrate ---//
		vorbis_info *pInfo = ov_info( &vorbisfile,-1 );
		MetaData.nBitrate = (int)pInfo->bitrate_nominal / 1000;
		MetaData.nDuration_ms = ov_time_total(&vorbisfile,-1) *1000;
		
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
