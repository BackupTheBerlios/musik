///////////////////////////////////////////////////

#include "stdafx.h"

#include "MusikMp3Info.h"
#include "MusikFilename.h"

#include <id3.h>
#include <id3/tag.h>
#include <id3/misc_support.h>

///////////////////////////////////////////////////

CMusikMp3Info::CMusikMp3Info()
{
}

///////////////////////////////////////////////////

CMusikMp3Info::~CMusikMp3Info()
{
}

///////////////////////////////////////////////////

bool CMusikMp3Info::LoadInfo( const CStdString& fn )
{
	ID3_Tag	id3Tag;
	id3Tag.Link( fn.c_str(), ID3TT_ALL );	

	// tag
	m_Info.SetArtist	( ID3_GetArtist	( &id3Tag ) );
	m_Info.SetAlbum		( ID3_GetAlbum	( &id3Tag ) );
	m_Info.SetTitle		( ID3_GetTitle	( &id3Tag ) );
	m_Info.SetGenre		( ID3_GetGenre	( &id3Tag ) );
	m_Info.SetYear		( ID3_GetYear	( &id3Tag ) );
	m_Info.SetTrackNum	( ID3_GetTrack	( &id3Tag ) );

	// get the *real* genre of the
	// just added value...
	m_Info.SetGenre( GetGenre( m_Info.GetGenre() ) );

	// bitrate
	const Mp3_Headerinfo* mp3header = id3Tag.GetMp3HeaderInfo();
	if ( mp3header ) 
	{
		char buffer[32];

		m_Info.SetDuration( itoa( mp3header->time * 1000, buffer, 10 ) );
		m_Info.SetVBR( mp3header->vbr_bitrate ? "1" : "0" );
		m_Info.SetBitrate( itoa( mp3header->bitrate / 1000, buffer, 10 ) );
	}

	// if the title is empty, then use the
	// filename...
	if ( m_Info.GetTitle().IsEmpty() )
	{
		CMusikFilename MFN( fn );
		m_Info.SetTitle( MFN.GetJustFilename() );
	}

	return true;
}

///////////////////////////////////////////////////

CStdString CMusikMp3Info::GetField( int field )
{
	return m_Info.GetField( field );
}

///////////////////////////////////////////////////

CStdString CMusikMp3Info::GetGenre( CStdString genre )
{
	CStdString sRet;

	if ( genre.size() > 0 )
	{
		int nGenreID = -1;

		// "x"
		if ( isdigit( genre[0] ) )
			nGenreID = atoi( genre.c_str() );

		// "(x)" -or- "(xx)" -or- "(xxx)"
		else if ( genre[0] == '(' && ( genre[2] == ')' || genre[3] == ')' || genre[4] == ')' ) )
		{
			genre.Replace( "(", "" );
			genre.Replace( ")", "" );
			nGenreID = atoi( genre );
		}

		// convert id -> str
		if ( nGenreID >=0 && nGenreID < ID3_NR_OF_V1_GENRES )
			sRet = ID3_v1_genre_description[nGenreID];
	}
	else
		sRet = "<unknown>";

	return sRet;
}

///////////////////////////////////////////////////
