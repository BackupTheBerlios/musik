///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
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
// Class(s): 
//
//   CmusikOggInfo
//
// Filename(s): 
//
//   musikOggInfo.h, musikOggInfo.cpp
//
// Information:
//
//   CmusikOggInfo reads and writes ogg tags from a 
//   CmusikSongInfo
// 
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/musikOggInfo.h"
#include "../include/musikFilename.h"

#include "../include/vcedit.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

///////////////////////////////////////////////////

CmusikOggInfo::CmusikOggInfo()
{
}

///////////////////////////////////////////////////

CmusikOggInfo::~CmusikOggInfo()
{
}

///////////////////////////////////////////////////

bool CmusikOggInfo::LoadInfo( const CStdString& fn )
{
	FILE *pAudioFile = fopen( fn.c_str(), "rb" );
	OggVorbis_File vorbisfile;

	// file loaded ok
	if ( ov_open( pAudioFile, &vorbisfile, NULL, 0 ) >= 0 )
	{
		vorbis_comment *pComment = ov_comment( &vorbisfile, -1 );

		// tag
		m_Info.SetArtist	( vorbis_comment_query( pComment, "artist",			0 ) );
		m_Info.SetAlbum		( vorbis_comment_query( pComment, "album",			0 ) );
		m_Info.SetTitle		( vorbis_comment_query( pComment, "title",			0 ) );
		m_Info.SetGenre		( vorbis_comment_query( pComment, "genre",			0 ) );
		m_Info.SetYear		( vorbis_comment_query( pComment, "date",			0 ) );
		m_Info.SetTrackNum	( vorbis_comment_query( pComment, "tracknumber",	0 ) );

		CStdString temp;

		// bitrate
		vorbis_info *pInfo = ov_info( &vorbisfile, -1 );
		temp.Format( "%d", pInfo->bitrate_nominal / 1000 );
		m_Info.SetBitrate( temp );

		// duration
		float duration = (float)ov_time_total ( &vorbisfile, -1 );
		temp.Format( "%d",  (int)( duration * 1000.0f ) );
		m_Info.SetDuration( temp );

		// cleanup, ov_clear closes audio file
		vorbis_comment_clear( pComment );
		vorbis_info_clear( pInfo );
		ov_clear( &vorbisfile );

		// if the title is empty, then use the
		// filename...
		if ( m_Info.GetTitle().IsEmpty() )
		{
			CmusikFilename MFN( fn );
			m_Info.SetTitle( MFN.GetJustFilename() );
		}

		return true;
	}

	// file failed to load or was not
	// an ogg...
	else
	{
		fclose( pAudioFile );
		return false;
	}
}

///////////////////////////////////////////////////

bool CmusikOggInfo::WriteInfo( CmusikSongInfo info )
{
	FILE* pIn;
	FILE* pOut;

	CStdString sFilename = info.GetFilename();
	CStdString sTempFilename = CmusikFilename::GetTempFilename( info.GetFilename(), true );

	// make sure we can open the file in
	// write mode..
	pIn = fopen( sFilename.c_str(), "rb+" );
	if ( !pIn )
		return false;
	fclose( pIn );

	// now open it up in read only mode
	pIn = fopen( sFilename.c_str(), "rb" );
	if ( !pIn )
		return false;

	// open up the temp ogg file, this will
	// be where the new tag gets written
	pOut = fopen( sTempFilename.c_str(), "wb" );
	if ( !pOut )
		return false;

	// setup the vc state stuff and make
	// sure the file is ogg/vorbis
	vcedit_state *state;
	vorbis_comment *vc;
	state = vcedit_new_state();

	if ( vcedit_open( state, pIn ) < 0 )
	{
		fclose( pIn );
		return false;
	}

	// load the comments and clear them
	vc = vcedit_comments( state );
    vorbis_comment_clear( vc );
	vorbis_comment_init( vc );

	// add comments
	vorbis_comment_add_tag( vc, "ARTIST",		( char* )info.GetArtist().c_str()	);
	vorbis_comment_add_tag( vc, "ALBUM",		( char* )info.GetAlbum().c_str()	);
	vorbis_comment_add_tag( vc, "TITLE",		( char* )info.GetTitle().c_str()	);
	vorbis_comment_add_tag( vc, "GENRE",		( char* )info.GetGenre().c_str()	);
	vorbis_comment_add_tag( vc, "DATE",			( char* )info.GetYear().c_str()		);
	vorbis_comment_add_tag( vc, "TRACKNUMBER",	( char* )info.GetTrackNum().c_str()	);

	// write the tag
	vcedit_write( state, pOut );

	// clean up
	vcedit_clear( state );
	fclose( pIn );
	fclose( pOut );

    // remove old file, rename new file
	CStdString sTempFilename2 = CmusikFilename::GetTempFilename( sFilename, true );

	// attempt to rename the original file to 
	// a new temporary filename
	if ( !rename( sFilename.c_str(), sTempFilename2.c_str() ) ) 
		return false;

	// now rename the new file with the new
	// comments the original filename
	if ( !rename( sTempFilename.c_str(), sFilename.c_str() ) )
	{
		rename( sTempFilename2.c_str(), sFilename.c_str() );
		return false;
	}

	// if we get this far, remove the old file
	if ( !remove( sTempFilename2.c_str() ) )
		return false;

	return true;
}

///////////////////////////////////////////////////

CStdString CmusikOggInfo::GetField( int field )
{
	return m_Info.GetField( field );
}

///////////////////////////////////////////////////
