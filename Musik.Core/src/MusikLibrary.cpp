///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
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
//   CMusikLibrary
//
// Filename(s): 
//
//   MusikLibrary.h, MusikLibrary.cpp
//
// Information:
//
//   CMusikLibrary is a front end to read and write
//   a Musik library database.
// 
// Example: 
//
//   A UI can use this class to quickly query and store
//   information of media.
//
// Usage: 
//
//   Fairly straight forward, just create an instance
//   of the class, or even inherit a new one from it.
//   Use the Set() and Get() functions to store and retrieve
//   information.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "time.h"

#include "../include/MusikFilename.h"
#include "../include/MusikConfig.h"
#include "../include/MusikCrossfader.h"
#include "../include/MusikLibrary.h"
#include "../include/MusikMp3Info.h"
#include "../include/MusikOggInfo.h"

#include "ace/Thread.h"
#include "ace/Synch.h"

///////////////////////////////////////////////////

static int sqlite_AddSongToPlaylist(void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when
	// adding songs to a CMusikPlaylist

	CMusikPlaylist* p = (CMusikPlaylist*)args;

	CMusikSong *pLibItem = new CMusikSong();
	pLibItem->SetID( atoi( results[0] ) ); 
	
	p->Add( *pLibItem );

	delete pLibItem;

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetSongFieldFromID( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when
	// finding a song's field

	CStdString* p = (CStdString*)args;
	*p = CStdString( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetPlaylistID( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when
	// finding a playlist's internal ID

	int* n = (int*)args;
	*n = atoi( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetSongInfoFromID( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when
	// getting CMusikSongInfo info

	CMusikSongInfo* p = (CMusikSongInfo*)args;

	p->SetTrackNum		( results[0] );
	p->SetArtist		( results[1] );
	p->SetAlbum			( results[2] );
	p->SetGenre			( results[3] );
	p->SetTitle			( results[4] );
	p->SetDuration		( results[5] );
	p->SetFormat		( results[6] );
	p->SetVBR			( results[7] );
	p->SetYear			( results[8] );
	p->SetRating		( results[9] );
	p->SetBitrate		( results[10] );
	p->SetLastPlayed	( results[11] );
	p->SetNotes			( results[12] );
	p->SetTimesPlayed	( results[13] );
	p->SetTimeAdded		( results[14] );
	p->SetFilesize		( results[15] );
	p->SetFilename		( results[16] );

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddSongToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when 
	// adding items (fields) to a CStdStringArray	

	CStdStringArray* p = (CStdStringArray*)args;
	p->push_back( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetIDFromFilename( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when 
	// finding the song id from a filename	

	int* n = (int*)args;
	*n = atoi( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddRowToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when 
	// adding playlist names to a CStdStringArray	

	CStdStringArray* p = (CStdStringArray*)args;
	p->push_back( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

CMusikLibrary::CMusikLibrary( const CStdString& filename )
{
	m_pDB = NULL;

	m_ProtectingLibrary = new ACE_Thread_Mutex();
	CMusikSong::SetLibrary( this );
	m_Filename = filename;

	InitFields();

	Startup();
}

///////////////////////////////////////////////////

CMusikLibrary::~CMusikLibrary()
{
	Shutdown();
	delete m_ProtectingLibrary;
}

///////////////////////////////////////////////////

void CMusikLibrary::InitTimeAdded()
{
	time_t curr_time;
	time( &curr_time );

	tm *ptm = NULL;
	ptm = localtime( &curr_time );

	m_TimeAdded.Format( "%02d:%02d:%02d %02d/%02d/%d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_mday, ptm->tm_mon, ptm->tm_year + 1900);
}

///////////////////////////////////////////////////

void CMusikLibrary::InitFields()
{
	m_Fields.push_back( "Artist" );
	m_Fields.push_back( "Album" );
	m_Fields.push_back( "Year" );
	m_Fields.push_back( "Genre" );
	m_Fields.push_back( "Title" );
	m_Fields.push_back( "Track" );
	m_Fields.push_back( "Time Added" );
	m_Fields.push_back( "Last Played" );
	m_Fields.push_back( "Filesize" );
	m_Fields.push_back( "Format" );
	m_Fields.push_back( "Time" );
	m_Fields.push_back( "Rating" );
	m_Fields.push_back( "Times Played" );
	m_Fields.push_back( "Bitrate" );
	m_Fields.push_back( "Filename" );

	m_FieldsDB.push_back( "artist" );
	m_FieldsDB.push_back( "album" );
	m_FieldsDB.push_back( "year" );
	m_FieldsDB.push_back( "genre" );
	m_FieldsDB.push_back( "title" );
	m_FieldsDB.push_back( "tracknum" );
	m_FieldsDB.push_back( "timeadded" );
	m_FieldsDB.push_back( "lastplayed" );
	m_FieldsDB.push_back( "filesize" );
	m_FieldsDB.push_back( "format" );
	m_FieldsDB.push_back( "duration" );
	m_FieldsDB.push_back( "rating" );
	m_FieldsDB.push_back( "timesplayed" );
	m_FieldsDB.push_back( "bitrate" );
	m_FieldsDB.push_back( "filename" );
}

///////////////////////////////////////////////////

int CMusikLibrary::GetSongFieldID( CStdString field )
{
	for ( size_t i = 0; i < m_Fields.size(); i++ )
	{
		if ( field == m_Fields.at( i ) )
			return i;
	}
	return -1;
}

///////////////////////////////////////////////////

int CMusikLibrary::GetSongFieldDBID( CStdString field )
{
	for ( size_t i = 0; i < m_FieldsDB.size(); i++ )
	{
		if ( field == m_FieldsDB.at( i ) )
			return i;
	}
	return -1;
}

///////////////////////////////////////////////////

bool CMusikLibrary::InitStdTables()
{
	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery1  = 
		"CREATE TABLE " STD_PLAYLIST_TABLE_NAME " ( "	
		"std_playlist_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"std_playlist_name varchar(255) "
		" );";

	// construct the table that will store all the
	// songs that pertain to all the playlists
	static const char *szCreateDBQuery2  = 
		"CREATE TABLE " STD_PLAYLIST_SONGS " ( "	
		"std_playlist_songid INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"std_playlist_id INTEGER, "
		"songid INTEGER"
		" );";

	// put a lock on the library and open it up
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

	if ( m_pDB )
	{
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, NULL );
	}

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

	m_ProtectingLibrary->release();

	return error;
}

///////////////////////////////////////////////////

bool CMusikLibrary::InitCrossfaderTable()
{
	bool error = false;

	// construct the table that contains a list of
	// all the crossfader presets
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " CROSSFADER_PRESETS " ( "	
		"crossfader_name VARCHAR(255) PRIMARY KEY, "
		"newsong FLOAT, "
		"pause_resume FLOAT, "
		"stop FLOAT, "
		"exit FLOAT "
		" );";

	// put a lock on the library and open it up
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

	if ( m_pDB )
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, NULL );

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

	m_ProtectingLibrary->release();

	return error;
}

///////////////////////////////////////////////////

bool CMusikLibrary::InitLibTable()
{
	bool error = false;

	// construct the table
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " 
		SONG_TABLE_NAME 
		" ( "	
		"songid INTEGER PRIMARY KEY, "
		"format number(1), "		
		"vbr number(1), "			
		"filename varchar(255), "	
		"artist varchar(255), "	
		"title varchar(255), "	
		"album varchar(255), "	
		"tracknum number(3), "	
		"year varchar(255), "		
		"genre varchar(255), "	
		"rating number(1), "		
		"bitrate number(10), "	
		"lastplayed timestamp, "	
		"notes varchar(255), "	
		"timesplayed number(5), "	
		"duration number(10), "	
		"timeadded timestamp, "	
		"filesize number(10), "	
		"dirty number(10) "		
		" );";

	// construct the index
	const char* szCreateIdxQuery =
		"CREATE INDEX songs_title_idx on " SONG_TABLE_NAME " (title);"
		"CREATE UNIQUE INDEX songs_filename_idx on " SONG_TABLE_NAME " (filename);"
		"CREATE INDEX songs_artist_idx on " SONG_TABLE_NAME " (artist);"
		"CREATE INDEX songs_album_idx on " SONG_TABLE_NAME " (album);"
		"CREATE INDEX songs_genre_idx on " SONG_TABLE_NAME " (genre);"
		"CREATE INDEX songs_artist_album_tracknum_idx on " SONG_TABLE_NAME " (artist,album,tracknum);";


	// put a lock on the library and open it up
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

	if ( m_pDB )
	{
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, NULL );
	}

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

	m_ProtectingLibrary->release();

	return error;
}

///////////////////////////////////////////////////

bool CMusikLibrary::Startup()
{
   bool error = false;

   if ( m_pDB )
	   Shutdown();

   char* pErr = NULL;
   m_pDB = sqlite_open( m_Filename.c_str(), 0666, &pErr );

   if ( m_pDB && !pErr )
   {
		if ( !InitLibTable() )
			error = true;

		if ( !InitStdTables() )
			error = true;  
   }
   else
	   error = true;

   if ( pErr )
	   sqlite_freemem( pErr );

   return error;
}

///////////////////////////////////////////////////

void CMusikLibrary::Shutdown()
{
	// lock it up and close it down.
	m_ProtectingLibrary->acquire();
	if ( m_pDB )
	{
		sqlite_close( m_pDB );
		m_pDB = NULL;
	}	
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::BeginTransaction()
{
	m_ProtectingLibrary->acquire();
	sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::EndTransaction()
{
	m_ProtectingLibrary->acquire();
	sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::CreateCrossfader( CMusikCrossfader* fader )
{
	CStdString sQuery;

	sQuery.Format( "INSERT INTO %s VALUES ( %f,%f,%f,%f ); ",
		CROSSFADER_PRESETS,
		fader->GetDuration( MUSIK_CROSSFADER_NEW_SONG ),
		fader->GetDuration( MUSIK_CROSSFADER_PAUSE_RESUME ),
		fader->GetDuration( MUSIK_CROSSFADER_STOP ),
		fader->GetDuration( MUSIK_CROSSFADER_EXIT ) );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::DeleteCrossfader( const CStdString& name )
{
	CStdString sQuery;

	sQuery.Format( "DELETE FROM %s WHERE crossfader_name = %s",
		CROSSFADER_PRESETS,
		name.c_str() );

	m_ProtectingLibrary->acquire();
	sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::DeleteCrossfader( CMusikCrossfader* fader )
{
	CStdString sQuery;

	sQuery.Format( "DELETE FROM %s WHERE crossfader_name = %s",
		CROSSFADER_PRESETS,
		fader->GetName().c_str() );

	m_ProtectingLibrary->acquire();
	sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::CreateStdPlaylist( const CStdString& name, const CIntArray& songids )
{
	CStdString sQuery;
	int nID;

	// lock it up
	m_ProtectingLibrary->acquire();

	// create query to make new entry to 
	// std_playlist table
	sQuery.Format( "INSERT INTO %s VALUES ( %d,'%s' ); ",
		STD_PLAYLIST_TABLE_NAME,
		NULL,
		name.c_str() );

	// insert the new playlist name
	sqlite_exec( m_pDB, sQuery.c_str(), NULL, NULL, NULL );

	// get the ID of the newly created entry
	sQuery.Format( "SELECT %s FROM %s WHERE %s = '%s';", 
		"std_playlist_id", 
		STD_PLAYLIST_TABLE_NAME, 
		"std_playlist_name",
		name.c_str() );

	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_GetPlaylistID, &nID, NULL );
	
	// insert songs into playlist
	if ( nID >= 0 )
	{
		for ( size_t i = 0; i < songids.size(); i++ )
		{
			sQuery.Format( "INSERT INTO %s VALUES ( %d, %d ); ",
				STD_PLAYLIST_SONGS,
				nID,
				songids.at( i ) );

			sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );
		}
	}	

	// release the mutex lock
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::DeleteStdPlaylist( const CStdString& name )
{
	CStdString sQuery;
	int nID;

	// lock it up	
	m_ProtectingLibrary->acquire();

	// get ID of the currently named playlist
	sQuery.Format( "SELECT %s FROM %s WHERE std_playlist_name = '%s';", 
		"std_playlist_id", 
		STD_PLAYLIST_TABLE_NAME, 
		name.c_str() );

	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_GetPlaylistID, &nID, NULL );

	// remove entry from table containing
	// the list of standard playlists
	sQuery.Format( "DELETE FROM %s WHERE std_playlist_name = '%s';",
		STD_PLAYLIST_TABLE_NAME,
		name.c_str() );

	sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );

	// delete corresponding songs from the
	// other table
	sQuery.Format( "DELETE FROM %s WHERE std_playlist_id = %d",
		STD_PLAYLIST_SONGS,
		nID );

	sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );

	// release mutex lock
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::CreateDynPlaylist( CStdString name, CStdString query )
{
	// lock it up
	m_ProtectingLibrary->acquire();

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::DeleteDynPlaylist( CStdString name )
{
	// lock it up and close it down.
	m_ProtectingLibrary->acquire();

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

CStdString CMusikLibrary::GetOrder( int type, bool terminate )
{
	CStdString sTerminate = "";
	if ( terminate )
		sTerminate = ";";

	switch( type )
	{
	case MUSIK_LIBRARY_TYPE_ARTIST:
		sTerminate.Format( "ORDER BY artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_ALBUM:
		sTerminate.Format( "ORDER BY album,tracknum,artist,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_YEAR:
		sTerminate.Format( "ORDER BY year,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_GENRE:
		sTerminate.Format( "ORDER BY genre,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TITLE:
		sTerminate.Format( "ORDER BY title,artist,album,tracknum%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		sTerminate.Format( "ORDER BY tracknum,artist,album,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
		sTerminate.Format( "ORDER BY timeadded,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
		sTerminate.Format( "ORDER BY lastplayed,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FILESIZE:
		sTerminate.Format( "ORDER BY filesize,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FORMAT:
		sTerminate.Format( "ORDER BY format,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_DURATION:
		sTerminate.Format( "ORDER BY duration,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_RATING:
		sTerminate.Format( "ORDER BY rating,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		sTerminate.Format( "ORDER BY timesplayed,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_BITRATE:
		sTerminate.Format( "ORDER BY bitrate,artist,album,tracknum,title%s", sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FILENAME:
		break;
	}

	return sTerminate;
}

///////////////////////////////////////////////////

int CMusikLibrary::QueryCount( const char* pQueryResult )
{
	const char *pTail;
	sqlite_vm *pVM;

	m_ProtectingLibrary->acquire();
	
	sqlite_compile( m_pDB, pQueryResult, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	int result = 0;
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = atoi( coldata[0] );

	sqlite_finalize( pVM, &errmsg );

	m_ProtectingLibrary->release();

	return result;
}

///////////////////////////////////////////////////

void CMusikLibrary::VerifyYearList( CStdStringArray & list )
{
	list.clear();
	size_t count = list.size();

	for ( size_t i = 0; i < count ; i++ )
	{
		if ( list.at( i ).IsEmpty() )
		{
			list.erase( list.begin() + i );
			count--;
		}
	}
}

///////////////////////////////////////////////////

void CMusikLibrary::GetAllSongs( CMusikPlaylist& target )
{
	QuerySongs( "filename <> ''", target );
}

///////////////////////////////////////////////////

void CMusikLibrary::QuerySongs( const CStdString& query, CMusikPlaylist& target )
{
	target.Clear();

	CStdString queryWhere( "SELECT songid FROM " SONG_TABLE_NAME " where " );
	queryWhere += query;
	queryWhere += ";";

	// lock it up and query it
	m_ProtectingLibrary->acquire();

	sqlite_exec( m_pDB, queryWhere.c_str(), &sqlite_AddSongToPlaylist, &target, NULL );

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::GetRelatedItems( int source_type, const CStdStringArray& source_items, int target_type, CStdStringArray& target )
{
	target.clear();

	if ( source_type == -1 || target_type == -1 )
		return;

	const CStdString sInType = GetSongFieldDB( source_type );
	const CStdString sOutType = GetSongFieldDB( target_type );

	// construct the query
	CStdString query;
	query.Format( "SELECT DISTINCT %s,UPPER(%s) AS UP FROM %s where ", 
		sOutType.c_str(), 
		sOutType.c_str(), 
		SONG_TABLE_NAME );

	CStdString sCurrentItem;
	for ( size_t i = 0; i < source_items.size(); i++ )
	{
		sCurrentItem = source_items.at( i );
		sCurrentItem.Replace( "'", "''" );

		if ( i > 0 )
			query += "or ";

        query.Format( "%s%s = '%s' ", query.c_str(), sInType.c_str(), sCurrentItem.c_str() );
	}

	// get sorting order
	query += GetOrder( target_type );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);

	m_ProtectingLibrary->release();

	// if target is years, verify only years
	//get displayed.
	if ( target_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );
}

///////////////////////////////////////////////////

void CMusikLibrary::GetRelatedItems( CStdString sub_query, int dst_type, CStdStringArray& target )
{
	target.clear();
	CStdString sOutType = GetSongFieldDB( dst_type );

	CStdString query;
	query.Format( "SELECT DISTINCT %s, UPPER( %s ) AS UP FROM %s WHERE %s order by %s;", 
		sOutType.c_str(), 
		sOutType.c_str(), 
		SONG_TABLE_NAME,
		sub_query.c_str(),
		sOutType.c_str() );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);

	m_ProtectingLibrary->release();

	// if target is years, verify only years
	// get displayed.
	if ( dst_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );
}

///////////////////////////////////////////////////

void CMusikLibrary::GetRelatedSongs( CStdString sub_query, int source_type, CMusikPlaylist& target )
{
	target.Clear();

	CStdString order_by = GetOrder( source_type );

	CStdString query;
	query.Format( "SELECT DISTINCT songid FROM %s WHERE %s %s",
		SONG_TABLE_NAME,
		sub_query.c_str(),
		order_by.c_str() );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToPlaylist, &target, NULL);

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::GetAllDistinct( int source_type, CStdStringArray& target, bool clear_target )
{
	if ( clear_target )
		target.clear();

	CStdString query;
	CStdString sField = GetSongFieldDB( source_type );
	query.Format( "SELECT DISTINCT %s, UPPER( %s ) AS UP FROM %s ORDER BY UP;", 
		sField.c_str(), 
		sField.c_str(),
		SONG_TABLE_NAME );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	sqlite_exec( m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL );

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

int CMusikLibrary::GetSongCount()
{
	char *query = sqlite_mprintf( "SELECT COUNT(*) FROM " SONG_TABLE_NAME " ;" );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

///////////////////////////////////////////////////

void CMusikLibrary::GetFieldFromID( int id, int field, CStdString& string )
{
	CStdString query;
	CStdString type = GetSongFieldDB( field );

	query.Format( "SELECT %s FROM %s WHERE songid = %d;", 
		type.c_str(), 
		SONG_TABLE_NAME, 
		id );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	sqlite_exec( m_pDB, query.c_str(), &sqlite_GetSongFieldFromID, &string, NULL );

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::GetSongInfoFromID( int id, CMusikSongInfo* info )
{
	CStdString query;

	query.Format( "SELECT tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename FROM %s WHERE songid = %d;", 
		SONG_TABLE_NAME,
		id );
	
	info->SetID( id );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, query.c_str(), &sqlite_GetSongInfoFromID, info, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

bool CMusikLibrary::SetSongInfo( int songid, CMusikSongInfo* info )
{
	int result = 0;

	CStdString query;
	query.Format( "UPDATE %s SET format=%d, vbr=%d, filename='%s', artist='%s', title='%s', album='%s', tracknum=%d, year='%s', genre='%s', rating=%d,",
			SONG_TABLE_NAME,
			info->GetFormat(),
			info->GetVBR(),
			info->GetFilename().c_str(),
			info->GetArtist().c_str(),
			info->GetTitle().c_str(),
			info->GetAlbum().c_str(),
			info->GetTrackNum(),
			info->GetYear().c_str(),
			info->GetGenre().c_str(),
			info->GetRating() );
		
	query.Format( "%s bitrate=%d, lastplayed='%s', notes='%s', timesplayed=%d, duration=%d, timeadded='%s', filesize=%d, dirty=%d WHERE songid = %d;",
			query.c_str(),	
			info->GetBitrate(),
			info->GetLastPlayed().c_str(),
			info->GetNotes().c_str(),
			info->GetTimesPlayed(),
			info->GetDuration(),
			info->GetTimeAdded().c_str(),
			info->GetFilesize(),
			info->GetDirtyFlag(),
			songid );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();
	result = sqlite_exec( m_pDB, query.c_str(), NULL, NULL, NULL );
	m_ProtectingLibrary->release();

	if ( result != SQLITE_OK )
		return false;
	
	return true;    
}

///////////////////////////////////////////////////

bool CMusikLibrary::SetSongRating( int songid, int rating )
{
	int result = 0;

	CStdString query;
	query.Format( "UPDATE %s SET rating=%d WHERE songid=%d", 
		SONG_TABLE_NAME,
		rating, 
		songid );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();
    
	result = sqlite_exec( m_pDB, query.c_str(), NULL, NULL, NULL );
	
	m_ProtectingLibrary->release();

	if ( result != SQLITE_OK )
		return false;
	
	return true;    
}

///////////////////////////////////////////////////

void CMusikLibrary::GetAllStdPlaylists( CStdStringArray* target, bool clear_target )
{
	if ( clear_target )
		target->clear();

	CStdString sQuery( "SELECT std_playlist_name FROM " STD_PLAYLIST_TABLE_NAME " WHERE std_playlist_name <> ''" );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_AddRowToStringArray, target, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::GetAllCrossfaders( CStdStringArray* target, bool clear_target )
{
	if ( clear_target )
		target->clear();

	CStdString sQuery( "SELECT crossfader_name FROM " CROSSFADER_PRESETS " WHERE crossfader_name <> ''" );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_AddRowToStringArray, target, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

int CMusikLibrary::GetIDFromFilename( CStdString fn )
{
	int target;

	fn.Replace( "'", "''" );

	CStdString sQuery;
	sQuery.Format( "SELECT songid FROM " SONG_TABLE_NAME " WHERE filename = '%s';", fn.c_str() );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_GetIDFromFilename, &target, NULL );
	m_ProtectingLibrary->release();

	return target;
}

///////////////////////////////////////////////////

void CMusikLibrary::GetSongFromFilename( CStdString fn, CMusikSong& song )
{
	song.SetID( GetIDFromFilename( fn ) );	
}

///////////////////////////////////////////////////

int CMusikLibrary::GetFilesize( const CStdString& fn )
{
	FILE* pFile = fopen( fn.c_str(), "rb" );
	if ( pFile )
	{
		int fs = fseek( pFile, 0, SEEK_END );
		fclose( pFile );
		return fs;
	}

	return 0;
}

///////////////////////////////////////////////////

bool CMusikLibrary::RemoveSong( int songid )
{
	m_ProtectingLibrary->acquire();

	int result = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE songid=%d;", NULL, NULL, NULL,
		SONG_TABLE_NAME,
		songid );

	m_ProtectingLibrary->release();

	if ( result == SQLITE_OK )
		return true;

	return false;
}	

///////////////////////////////////////////////////

bool CMusikLibrary::RemoveSong( const CStdString& fn )
{
	m_ProtectingLibrary->acquire();

	int result = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE filename=%Q;", NULL, NULL, NULL,
		SONG_TABLE_NAME,
		fn.c_str() );

	m_ProtectingLibrary->release();

	if ( result == SQLITE_OK )
		return true;

	return false;
}	

///////////////////////////////////////////////////

bool CMusikLibrary::AddSong( const CStdString& fn )
{
	bool result = true;

	CMusikFilename MFN( fn );
	CString sExt = MFN.GetExtension();

	if ( sExt == "mp3" )
		result = AddMP3( fn );
	else if ( sExt == "ogg" )
		result = AddOGG( fn );

	return result;
}

///////////////////////////////////////////////////

bool CMusikLibrary::AddOGG( const CStdString& fn )
{
	CMusikOggInfo info;
	if ( info.LoadInfo( fn ) )
	{
		m_ProtectingLibrary->acquire();

		int result = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d );", NULL, NULL, NULL, 
			SONG_TABLE_NAME,								// song table 		
			NULL,											// id
			MUSIK_LIBRARY_FORMAT_OGG,						// format
			1,												// vbr
			fn.c_str(),										// filename
			info.Get()->GetArtist().c_str(),				// artist 
			info.Get()->GetTitle().c_str(),					// title
			info.Get()->GetAlbum().c_str(),					// album
			atoi( info.Get()->GetTrackNum().c_str() ),		// tracknum
			info.Get()->GetYear().c_str(),					// year
			info.Get()->GetGenre().c_str(),					// genre
			0,												// rating
			atoi( info.Get()->GetBitrate() ),				// bitrate
			"",												// last played
			"",												// notes
			0,												// times played
			atoi( info.Get()->GetDuration() ),				// duration
			m_TimeAdded.c_str(),							// time added
			GetFilesize( fn ),								// file size
			0 );											// dirty

		m_ProtectingLibrary->release();

		if ( result == SQLITE_OK )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool CMusikLibrary::AddMP3( const CStdString& fn )
{
	CMusikMp3Info info;
	if ( info.LoadInfo( fn ) )
	{
		m_ProtectingLibrary->acquire();

		int result = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d );", NULL, NULL, NULL, 
			SONG_TABLE_NAME,								// song table 		
			NULL,											// id
			MUSIK_LIBRARY_FORMAT_MP3,						// format
			atoi( info.Get()->GetVBR() ),					// vbr
			fn.c_str(),										// filename
			info.Get()->GetArtist().c_str(),				// artist 
			info.Get()->GetTitle().c_str(),					// title
			info.Get()->GetAlbum().c_str(),					// album
			atoi( info.Get()->GetTrackNum().c_str() ),		// tracknum
			info.Get()->GetYear().c_str(),					// year
			info.Get()->GetGenre().c_str(),					// genre
			0,												// rating
			atoi( info.Get()->GetBitrate() ),				// bitrate
			"",												// last played
			"",												// notes
			0,												// times played
			atoi( info.Get()->GetDuration() ),				// duration
			m_TimeAdded.c_str(),							// time added
			GetFilesize( fn ),								// file size
			0 );											// dirty

		m_ProtectingLibrary->release();

		if ( result == SQLITE_OK )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool CMusikLibrary::IsSongInLibrary( CStdString fn )
{
	bool result = false;

	fn.Replace( "'", "''" );

	char *query = sqlite_mprintf( "SELECT filename FROM %Q WHERE filename = %Q;", 
		SONG_TABLE_NAME,
		fn.c_str() );
	
	m_ProtectingLibrary->acquire();

	// run query
	const char *pTail;
	sqlite_vm *pVM;
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	// see if theres a row
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = true;

	// clean up
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );

	m_ProtectingLibrary->release();

	return result;
}

///////////////////////////////////////////////////