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

#include <wx/ffile.h>
#include <wx/filename.h>
#include "vcedit.h"

#include "../../MusikUtils.h"




COggInfo::COggInfo()
{

}
bool COggInfo::ReadMetaData(CSongMetaData & MetaData) const
{
	FILE *audiofile = wxFopen( MetaData.Filename.GetFullPath(), wxT("rb") );
	if( !audiofile )
		return false;

	OggVorbis_File vorbisfile;

	if ( ov_open( audiofile, &vorbisfile, NULL, 0 ) >= 0 )
	{
		vorbis_comment *pComment = ov_comment( &vorbisfile, -1 );

		MetaData.Artist = vorbis_comment_query( pComment, "ARTIST",	0 );
		MetaData.Album	= vorbis_comment_query( pComment, "ALBUM",	0 );
		MetaData.Title	= vorbis_comment_query( pComment, "TITLE",	0 );
		MetaData.Genre	= vorbis_comment_query( pComment, "GENRE",	0 );
		MetaData.Notes	= vorbis_comment_query( pComment, "COMMENT",0 );

		MetaData.Year	= vorbis_comment_query( pComment, "DATA", 		0 );
		char *szTracknum = vorbis_comment_query( pComment, "TRACKNUMBER", 0 );
		MetaData.nTracknum = atol( szTracknum ? szTracknum : "0" );

		//--- grab the bitrate ---//
		vorbis_info *pInfo = ov_info( &vorbisfile,-1 );
		MetaData.nBitrate = (int)pInfo->bitrate_nominal / 1000;
		MetaData.nDuration_ms = ov_time_total(&vorbisfile,-1) *1000;
		// get file size
		fseek(audiofile,0,SEEK_END);
		MetaData.nFilesize = ftell(audiofile);

		//--- cleanup ---//
		vorbis_comment_clear( pComment );
		vorbis_info_clear( pInfo );
		ov_clear( &vorbisfile );	//--- will also close the FILE (audiofile) ---//
		MetaData.eFormat = MUSIK_FORMAT_OGG;
		MetaData.bVBR = true;

		return true;
	}
	else
	{
		fclose( audiofile );
		return false;
	}
}

bool  COggInfo::WriteMetaData(const CSongMetaData & MetaData,bool bClearAll)
{

	//--- file objects, etc ---//
	FILE *inFile= NULL, *outFile = NULL;
	vcedit_state *state = vcedit_new_state();

	//--- if file couldn't be openend, return ---//
	inFile = _tfopen( MetaData.Filename.GetFullPath(), wxT("r+b") );
	if (!inFile)
	{
		return false;
	}

	//--- if the file *DID* load, but is *NOT* and ogg, return ---//
	if( vcedit_open( state, inFile ) < 0 )
	{
		fclose(inFile);
		return false;
	}

	//--- load the comments//
	vorbis_comment *vc = vcedit_comments( state );

	CVCTagMap tagmap;
	if(!bClearAll)
		tagmap.AddTagsFromVC(vc); // fill map with all present tags, if not all tags should be cleared


	//--- add own comments to tagmap---//
	char bufNumber[20];
	tagmap.AddTag("TITLE", MetaData.Title );
	sprintf (bufNumber, "%d", MetaData.nTracknum);
	tagmap.AddTag("TRACKNUMBER",bufNumber);
	tagmap.AddTag("ARTIST", MetaData.Artist );
	tagmap.AddTag("ALBUM", MetaData.Album );
	tagmap.AddTag("GENRE", MetaData.Genre );
	tagmap.AddTag("DATE", MetaData.Year );
	tagmap.AddTag("COMMENT", MetaData.Notes );

	// clear the vc  out ---//
	vorbis_comment_clear( vc );
	// fill vc with tags from the map
	tagmap.CopyTagsToVC(vc);


	//--- write new file ---//
	wxString outfilename = wxFileName::CreateTempFileName(MUSIKAPPNAME);
	bool bTagWritingSucceded = false;
	outFile = _tfopen( outfilename, wxT("w+b") );
	if ( outFile )
	{
		bTagWritingSucceded = vcedit_write( state, outFile ) >= 0;
	}
	if( inFile )
		fclose(inFile);
	if( outFile )
		fclose(outFile);
	//--- clean up ---//
	vcedit_clear( state );
	// rename tempfile to origninal file
	return ( bTagWritingSucceded && wxRenameFile( outfilename, MetaData.Filename.GetFullPath() ) );
}

void COggInfo::CVCTagMap::AddTagsFromVC(vorbis_comment *pComment)
{
	CSongMetaData::StringData entry;
	CSongMetaData::StringData val;
	for (int i=0; i<pComment->comments; i++) 
	{
		entry.Attach (pComment->user_comments[i],false);
		off_t seppos = entry.Find('=');
		if(	seppos > -1 )
		{
			wxString key( ConvFromUTF8(entry),(size_t)seppos );
			key.MakeUpper();
			CSongMetaData::StringData val( entry.Substr(seppos + 1) );
			m_mapTags.insert (tagmap_t::value_type(key,val ));
		}
	}
}

void COggInfo::CVCTagMap::CopyTagsToVC(vorbis_comment *pComment) 
{

	CSongMetaData::StringData val;
	
	for (tagmap_t::iterator it = m_mapTags.begin(); it != m_mapTags.end(); it++) 
	{
		vorbis_comment_add_tag (pComment, (char*)((const char*)ConvW2A((*it).first)),(char*)((const char*)(*it).second));
	}
}

void COggInfo::CVCTagMap::AddTag(const char *tag,const char *val, bool bUnique)
{
	if (!val || strlen(val) == 0 ) return;
	// if the tag should be unique, erase all previous contained tags 
	wxString sTag = ConvFromUTF8(tag);
	if (bUnique) 
		m_mapTags.erase (sTag);

	m_mapTags.insert (tagmap_t::value_type(sTag,val));
	return;

}