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
//   CmusikMp3Info
//
// Filename(s): 
//
//   musikMp3Info.h, musikMp3Info.cpp
//
// Information:
//
//   CmusikMp3Info reads and writes mp3 tags from a 
//   CmusikSongInfo
// 
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/musikConfig.h"

#include "../include/musikMp3Info.h"
#include "../include/musikFilename.h"

#include <id3.h>
#include <id3/tag.h>
#include <id3/misc_support.h>

///////////////////////////////////////////////////

CmusikMp3Info::CmusikMp3Info()
{
}

///////////////////////////////////////////////////

CmusikMp3Info::~CmusikMp3Info()
{
}

///////////////////////////////////////////////////

bool CmusikMp3Info::LoadInfo( const CStdString& fn )
{
	ID3_Tag	id3Tag;
	id3Tag.Link( fn.c_str(), ID3TT_ALL );	

	// make sure the file was loaded
	if ( !id3Tag.GetFileSize() )
		return false;

	// get the info...
	char* pArtist	= ID3_GetArtist	( &id3Tag );
	char* pAlbum	= ID3_GetAlbum	( &id3Tag );
	char* pTitle	= ID3_GetTitle	( &id3Tag );
	char* pGenre	= ID3_GetGenre	( &id3Tag );
	char* pYear		= ID3_GetYear	( &id3Tag );
	char* pTrackNum	= ID3_GetTrack	( &id3Tag );

	// tag
	m_Info.SetArtist	( pArtist );
	m_Info.SetAlbum		( pAlbum );
	m_Info.SetTitle		( pTitle );
	m_Info.SetGenre		( GetGenre( pGenre ) );
	m_Info.SetYear		( pGenre );
	m_Info.SetTrackNum	( pTrackNum );

	// free the info
	ID3_FreeString( pArtist );
	ID3_FreeString( pAlbum );
	ID3_FreeString( pTitle );
	ID3_FreeString( pGenre );
	ID3_FreeString( pYear );
	ID3_FreeString( pTrackNum );

	// bitrate
	const Mp3_Headerinfo* mp3header = id3Tag.GetMp3HeaderInfo();
	if ( mp3header ) 
	{
		char buffer[32];

		m_Info.SetDuration( int2str( buffer, mp3header->time * 1000 ) );
		m_Info.SetVBR( mp3header->vbr_bitrate ? "1" : "0" );
		m_Info.SetBitrate( int2str( buffer, mp3header->bitrate / 1000 ) );
	}

	// if the title is empty, then use the
	// filename...
	if ( m_Info.GetTitle().IsEmpty() )
	{
		CmusikFilename MFN( fn );
		m_Info.SetTitle( MFN.GetJustFilename() );
	}

	return true;
}

///////////////////////////////////////////////////

bool CmusikMp3Info::WriteInfo( CmusikSongInfo info, bool clear_old )
{

	ID3_Tag	id3Tag;
	id3Tag.Link( info.GetFilename().c_str(), ID3TT_ALL );

	// make sure file was loaded
	if ( !id3Tag.GetFileSize() )
		return false;

	// itterate through all old values and
	// clear them...
	if ( clear_old )
	{
		ID3_Tag::Iterator* iter = id3Tag.CreateIterator();
		ID3_Frame* frame = NULL;
		while ( ( frame = iter->GetNext() )!= NULL )
		{
			frame = id3Tag.RemoveFrame( frame );
			delete frame;
		}
	}

	// ... or just clear the fields that
	// musik really cares about
	else
	{
		ID3_RemoveArtists	( &id3Tag );
		ID3_RemoveAlbums	( &id3Tag );
		ID3_RemoveTitles	( &id3Tag ); 
		ID3_RemoveGenres	( &id3Tag );
		ID3_RemoveYears		( &id3Tag );
		ID3_RemoveTracks	( &id3Tag );
	}

	// add the string info
	ID3_AddArtist	( &id3Tag, info.GetArtist().c_str(),	true );
	ID3_AddAlbum	( &id3Tag, info.GetAlbum().c_str(),		true );
	ID3_AddTitle	( &id3Tag, info.GetTitle().c_str(),		true ); 
	ID3_AddYear		( &id3Tag, info.GetYear().c_str(), 		true );
	
	// track
	int nTrackNum = atoi( info.GetTrackNum().c_str() );
	ID3_AddTrack	( &id3Tag, nTrackNum, true );
	
	// genre
	int nGenreID = GetGenreID( info.GetGenre() );
	if( nGenreID == -1 )
		ID3_AddGenre( &id3Tag, info.GetGenre(), true );
	else
		ID3_AddGenre( &id3Tag, nGenreID, true );

	// write to file
	id3Tag.Update();

	return true;
}

///////////////////////////////////////////////////

CStdString CmusikMp3Info::GetField( int field )
{
	return m_Info.GetField( field );
}

///////////////////////////////////////////////////

CStdString CmusikMp3Info::GetGenre( CStdString genre )
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
		sRet = "(unknown)";

	return sRet;
}

///////////////////////////////////////////////////

int CmusikMp3Info::GetGenreID( CStdString genre )
{
	for ( int i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
	{
		if ( genre.CompareNoCase( ID3_v1_genre_description[i] ) == 0 )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////
