///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/MusikOggInfo.h"
#include "../include/MusikFilename.h"

#include "../include/vcedit.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

///////////////////////////////////////////////////

CMusikOggInfo::CMusikOggInfo()
{
}

///////////////////////////////////////////////////

CMusikOggInfo::~CMusikOggInfo()
{
}

///////////////////////////////////////////////////

bool CMusikOggInfo::LoadInfo( const CStdString& fn )
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
		int duration = ov_time_total ( &vorbisfile, -1 );
		temp.Format( "%d",  duration * 1000 );
		m_Info.SetDuration( temp );

		// cleanup, ov_clear closes audio file
		vorbis_comment_clear( pComment );
		vorbis_info_clear( pInfo );
		ov_clear( &vorbisfile );

		// if the title is empty, then use the
		// filename...
		if ( m_Info.GetTitle().IsEmpty() )
		{
			CMusikFilename MFN( fn );
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

CStdString CMusikOggInfo::GetField( int field )
{
	return m_Info.GetField( field );
}

///////////////////////////////////////////////////