///////////////////////////////////////////////////

#include "stdafx.h"
#include "time.h"

#include "../include/MusikCrossfader.h"
#include "../include/MusikLibrary.h"

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

	p->m_TrackNum		= CStdString	( results[0] );
	p->m_Artist			= CStdString	( results[1] );
	p->m_Album			= CStdString	( results[2] );
	p->m_Genre			= CStdString	( results[3] );
	p->m_Title			= CStdString	( results[4] );
	p->m_Duration		= CStdString	( results[5] );
	p->m_Format			= CStdString	( results[6] );
	p->m_VBR			= CStdString	( results[7] );
	p->m_Year			= CStdString	( results[8] );
	p->m_Rating			= CStdString	( results[9] );
	p->m_Bitrate		= CStdString	( results[10] );
	p->m_LastPlayed		= CStdString	( results[11] );
	p->m_Notes			= CStdString	( results[12] );
	p->m_TimesPlayed	= CStdString	( results[13] );
	p->m_TimeAdded		= CStdString	( results[14] );
	p->m_Filesize		= CStdString	( results[15] );
	p->m_Filename		= CStdString	( results[16] );

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
	m_Fields.push_back( _T( "Artist" ) );
	m_Fields.push_back( _T( "Album" ) );
	m_Fields.push_back( _T( "Year" ) );
	m_Fields.push_back( _T( "Genre" ) );
	m_Fields.push_back( _T( "Title" ) );
	m_Fields.push_back( _T( "Track" ) );
	m_Fields.push_back( _T( "Time Added" ) );
	m_Fields.push_back( _T( "Last Played" ) );
	m_Fields.push_back( _T( "Filesize" ) );
	m_Fields.push_back( _T( "Format" ) );
	m_Fields.push_back( _T( "Time" ) );
	m_Fields.push_back( _T( "Rating" ) );
	m_Fields.push_back( _T( "Times Played" ) );
	m_Fields.push_back( _T( "Bitrate" ) );
	m_Fields.push_back( _T( "Filename" ) );

	m_FieldsDB.push_back( _T( "artist" ) );
	m_FieldsDB.push_back( _T( "album" ) );
	m_FieldsDB.push_back( _T( "year" ) );
	m_FieldsDB.push_back( _T( "genre" ) );
	m_FieldsDB.push_back( _T( "title" ) );
	m_FieldsDB.push_back( _T( "tracknum" ) );
	m_FieldsDB.push_back( _T( "timeadded" ) );
	m_FieldsDB.push_back( _T( "lastplayed" ) );
	m_FieldsDB.push_back( _T( "filesize" ) );
	m_FieldsDB.push_back( _T( "format" ) );
	m_FieldsDB.push_back( _T( "duration" ) );
	m_FieldsDB.push_back( _T( "rating" ) );
	m_FieldsDB.push_back( _T( "timesplayed" ) );
	m_FieldsDB.push_back( _T( "bitrate" ) );
	m_FieldsDB.push_back( _T( "filename" ) );
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
		"std_playlist_id INTEGER PRIMARY KEY, "
		"std_playlist_name varchar(255) "
		" );";

	// construct the table that will store all the
	// songs that pertain to all the playlists
	static const char *szCreateDBQuery2  = 
		"CREATE TABLE " STD_PLAYLIST_SONGS " ( "	
		"std_playlist_songid INTEGER PRIMARY KEY, "
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
	sqlite_exec_printf( m_pDB, _T( "begin transaction;" ), NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::EndTransaction()
{
	m_ProtectingLibrary->acquire();
	sqlite_exec_printf( m_pDB, _T( "end transaction;" ), NULL, NULL, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::CreateCrossfader( CMusikCrossfader* fader )
{
	CStdString sQuery;

	sQuery.Format( _T( "INSERT INTO %s VALUES ( %f,%f,%f,%f ); " ),
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

	sQuery.Format( _T( "DELETE FROM %s WHERE crossfader_name = %s" ),
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

	sQuery.Format( _T( "DELETE FROM %s WHERE crossfader_name = %s" ),
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
	sQuery.Format( _T( "INSERT INTO %s VALUES ( '%s' ); " ),
		STD_PLAYLIST_TABLE_NAME,
		name.c_str() );

	// insert the new playlist name
	sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );

	// get the ID of the newly created entry
	sQuery.Format( _T( "SELECT %s FROM %s WHERE %s = '%s';" ), 
		"std_playlist_id", 
		STD_PLAYLIST_TABLE_NAME, 
		"std_playlist_name",
		name.c_str() );

	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_GetPlaylistID, &nID, NULL );
	
	// insert songs into playlist
	for ( size_t i = 0; i < songids.size(); i++ )
	{
		sQuery.Format( _T( "INSERT INTO %s VALUES ( %n, %n ); " ),
			STD_PLAYLIST_SONGS,
			nID,
			songids.at( i ) );

		sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );
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
	sQuery.Format( _T( "SELECT %s FROM %s WHERE std_playlist_name = '%s';" ), 
		"std_playlist_id", 
		STD_PLAYLIST_TABLE_NAME, 
		name.c_str() );

	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_GetPlaylistID, &nID, NULL );

	// remove entry from table containing
	// the list of standard playlists
	sQuery.Format( _T( "DELETE FROM %s WHERE std_playlist_name = '%s';" ),
		STD_PLAYLIST_TABLE_NAME,
		name.c_str() );

	sqlite_exec_printf( m_pDB, sQuery.c_str(), NULL, NULL, NULL );

	// delete corresponding songs from the
	// other table
	sQuery.Format( _T( "DELETE FROM %s WHERE std_playlist_id = %d" ),
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
	CStdString sTerminate = _T( "" );
	if ( terminate )
		sTerminate = _T( ";" );

	switch( type )
	{
	case MUSIK_LIBRARY_TYPE_ARTIST:
		sTerminate.Format( _T( "ORDER BY artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_ALBUM:
		sTerminate.Format( _T( "ORDER BY album,tracknum,artist,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_YEAR:
		sTerminate.Format( _T( "ORDER BY year,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_GENRE:
		sTerminate.Format( _T( "ORDER BY genre,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TITLE:
		sTerminate.Format( _T( "ORDER BY title,artist,album,tracknum%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		sTerminate.Format( _T( "ORDER BY tracknum,artist,album,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
		sTerminate.Format( _T( "ORDER BY timeadded,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
		sTerminate.Format( _T( "ORDER BY lastplayed,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FILESIZE:
		sTerminate.Format( _T( "ORDER BY filesize,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FORMAT:
		sTerminate.Format( _T( "ORDER BY format,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_DURATION:
		sTerminate.Format( _T( "ORDER BY duration,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_RATING:
		sTerminate.Format( _T( "ORDER BY rating,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		sTerminate.Format( _T( "ORDER BY timesplayed,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_BITRATE:
		sTerminate.Format( _T( "ORDER BY bitrate,artist,album,tracknum,title%s" ), sTerminate.c_str() );
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
	QuerySongs( _T( "filename <> ''" ), target );
}

///////////////////////////////////////////////////

void CMusikLibrary::QuerySongs( const CStdString& query, CMusikPlaylist& target )
{
	target.Clear();

	CStdString queryWhere( _T( "SELECT songid FROM " ) SONG_TABLE_NAME _T( " where " ) );
	queryWhere += query;
	queryWhere += _T( ";" );

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
	query.Format( _T( "SELECT DISTINCT %s,UPPER(%s) AS UP FROM %s where " ), 
		sOutType.c_str(), 
		sOutType.c_str(), 
		SONG_TABLE_NAME );

	CStdString sCurrentItem;
	for ( size_t i = 0; i < source_items.size(); i++ )
	{
		sCurrentItem = source_items.at( i );
		sCurrentItem.Replace( _T("'"), _T("''") );

		if ( i > 0 )
			query += _T("or ");

        query.Format( _T( "%s%s = '%s' " ), query.c_str(), sInType.c_str(), sCurrentItem.c_str() );
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
	query.Format( _T( "SELECT DISTINCT %s, UPPER( %s ) AS UP FROM %s WHERE %s order by %s;" ), 
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
	query.Format( _T( "SELECT DISTINCT songid FROM %s WHERE %s %s" ),
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
	query.Format( _T( "SELECT DISTINCT %s, UPPER( %s ) AS UP FROM %s ORDER BY UP;" ), 
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

	query.Format( _T( "SELECT %s FROM %s WHERE songid = %d;" ), 
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

	query.Format( _T( "SELECT tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename FROM %s WHERE songid = %d;" ), 
		SONG_TABLE_NAME,
		id );
	
	info->SetID( id );

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	int result = sqlite_exec( m_pDB, query.c_str(), &sqlite_GetSongInfoFromID, info, NULL );

	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

bool CMusikLibrary::SetSongInfo( int songid, CMusikSongInfo* info )
{
	int result = 0;

	CStdString query;
	query.Format( _T( "UPDATE %s SET format=%d, vbr=%d, filename='%s', artist='%s', title='%s', album='%s', tracknum=%d, year='%s', genre='%s', rating=%d," ),
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
		
	query.Format( _T( "%s bitrate=%d, lastplayed='%s', notes='%s', timesplayed=%d, duration=%d, timeadded='%s', filesize=%d, dirty=%d WHERE songid = %d;" ),
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

	MessageBox( NULL, query.c_str(), NULL, NULL );

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
	query.Format( _T( "UPDATE %s SET rating=%d WHERE songid=%d" ), 
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

	CStdString sQuery( _T( "SELECT std_playlist_name  FROM " ) STD_PLAYLIST_TABLE_NAME _T( " WHERE std_playlist_name <> ''" ) );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_AddRowToStringArray, target, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CMusikLibrary::GetAllCrossfaders( CStdStringArray* target, bool clear_target )
{
	if ( clear_target )
		target->clear();

	CStdString sQuery( _T( "SELECT crossfader_name  FROM " ) CROSSFADER_PRESETS _T( " WHERE crossfader_name <> ''" ) );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_AddRowToStringArray, target, NULL );
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

int CMusikLibrary::GetIDFromFilename( const CStdString& fn )
{
	int target;

	CStdString sQuery;
	sQuery.Format( ( _T( "SELECT songid  FROM " ) SONG_TABLE_NAME _T( " WHERE filename = '%s'" ) ), fn.c_str() );

	m_ProtectingLibrary->acquire();
	sqlite_exec( m_pDB, sQuery.c_str(), &sqlite_GetIDFromFilename, &target, NULL );
	m_ProtectingLibrary->release();

	return target;
}

///////////////////////////////////////////////////