///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
//
//   CmusikLibrary
//
// Filename(s): 
//
//   musikLibrary.h, musikLibrary.cpp
//
// Information:
//
//   CmusikLibrary is a front end to read and write
//   a musik library database.
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
// Notes:
//
//   FROM 		%Q
//   INTO 		%Q 
//   str =		%Q
//   DISTINCT 	%q
//   UPPER		%q
//   UPDATE		%Q
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikLibrary.h"

#include "../include/musikMp3Info.h"
#include "../include/musikOggInfo.h"
#include "../include/musikFilename.h"

#include "stdafx.h"
#include "time.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

static int sqlite_AddSongToPlaylist( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikSong *pLibItem = new CmusikSong();
	pLibItem->SetID( atoi( results[0] ) ); 
	pLibItem->SetDuration( atoi( results[1] ) / 1000 );
	pLibItem->SetFilesize( atoi( results[2] ) );
	
	CmusikPlaylist* p = (CmusikPlaylist*)args;
	p->Add( *pLibItem );

	delete pLibItem;

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetSongFromFilename( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikSong* p = (CmusikSong*)args;

	p->SetID( atoi( results[0] ) ); 
	p->SetDuration( atoi( results[1] ) / 1000 );
	p->SetFilesize( atoi( results[2] ) );

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetFieldFromID( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikString* p = (CmusikString*)args;
	*p = CmusikString( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetCrossfader( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikCrossfader* p = (CmusikCrossfader*)args;

	p->Set( 
		(float)atof( results[0] ), 
		(float)atof( results[1] ), 
		(float)atof( results[2] ), 
		(float)atof( results[3] ), 
		(float)atof( results[4] ) );

	if ( numCols == 6 )
        p->SetName( results[5] );
	else
		p->SetName( "Default" );

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetEqualizer( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikEQSettings* p = (CmusikEQSettings*)args;

	p->m_Left[0]		= (float)atof( results[0] );	
	p->m_Left[1]		= (float)atof( results[1] );
	p->m_Left[2]		= (float)atof( results[2] );
	p->m_Left[3]		= (float)atof( results[3] );
	p->m_Left[4]		= (float)atof( results[4] );
	p->m_Left[5]		= (float)atof( results[5] );
	p->m_Left[6]		= (float)atof( results[6] );	
	p->m_Left[7]		= (float)atof( results[7] );
	p->m_Left[8]		= (float)atof( results[8] );
	p->m_Left[9]		= (float)atof( results[9] );
	p->m_Left[10]		= (float)atof( results[10] );
	p->m_Left[11]		= (float)atof( results[11] );
	p->m_Left[12]		= (float)atof( results[12] );	
	p->m_Left[13]		= (float)atof( results[13] );
	p->m_Left[14]		= (float)atof( results[14] );
	p->m_Left[15]		= (float)atof( results[15] );
	p->m_Left[16]		= (float)atof( results[16] );
	p->m_Left[17]		= (float)atof( results[17] );
	p->m_Right[0]		= (float)atof( results[18] );	
	p->m_Right[1]		= (float)atof( results[19] );
	p->m_Right[2]		= (float)atof( results[20] );
	p->m_Right[3]		= (float)atof( results[21] );
	p->m_Right[4]		= (float)atof( results[22] );
	p->m_Right[5]		= (float)atof( results[23] );
	p->m_Right[6]		= (float)atof( results[24] );	
	p->m_Right[7]		= (float)atof( results[25] );
	p->m_Right[8]		= (float)atof( results[26] );
	p->m_Right[9]		= (float)atof( results[27] );
	p->m_Right[10]		= (float)atof( results[28] );
	p->m_Right[11]		= (float)atof( results[29] );
	p->m_Right[12]		= (float)atof( results[30] );	
	p->m_Right[13]		= (float)atof( results[31] );
	p->m_Right[14]		= (float)atof( results[32] );
	p->m_Right[15]		= (float)atof( results[33] );
	p->m_Right[16]		= (float)atof( results[34] );
	p->m_Right[17]		= (float)atof( results[35] );

	if ( numCols == 37 )
		p->m_Name = results[36];

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetSongInfoFromID( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikSongInfo* p = (CmusikSongInfo*)args;

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
	p->SetEqualizer		( results[17] );
	p->SetDirtyFlag		( results[18] );

    return 0;
}

///////////////////////////////////////////////////
 
static int sqlite_AddSongToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikStringArray* p = (CmusikStringArray*)args;
	p->push_back( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetIntFromRow( void *args, int numCols, char **results, char ** columnNames )
{
	int* n = (int*)args;
	*n = atoi( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddRowToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikStringArray* p = (CmusikStringArray*)args;
	p->push_back( results[0] ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddRowToIntArray( void *args, int numCols, char **results, char ** columnNames )
{
	CIntArray* p = (CIntArray*)args;
	p->push_back( atoi( results[0] ) ); 

    return 0;
}
///////////////////////////////////////////////////

static int sqlite_AddStdPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikPlaylistInfoArray* p = (CmusikPlaylistInfoArray*)args;
	
	CmusikPlaylistInfo info ( results[0], MUSIK_PLAYLIST_TYPE_STANDARD, atoi( results[1] ) );
	p->push_back( info ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddDynPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	CmusikPlaylistInfoArray* p = (CmusikPlaylistInfoArray*)args;
	
	CmusikPlaylistInfo info ( results[0], MUSIK_PLAYLIST_TYPE_DYNAMIC, atoi( results[1] ) );
	p->push_back( info ); 

    return 0;
}

///////////////////////////////////////////////////

CmusikLibrary::CmusikLibrary( const CmusikString& filename )
{
	m_pDB = NULL;
	m_Transactions = 0;
	m_DatabaseOpen = false;

	CmusikSong::SetLibrary( this );
	m_Filename = filename;

	InitFields();

	Startup();
}

///////////////////////////////////////////////////

CmusikLibrary::~CmusikLibrary()
{
	Shutdown();
}

///////////////////////////////////////////////////

void CmusikLibrary::InitTimeAdded()
{
	time_t curr_time;
	time( &curr_time );

	tm *ptm = NULL;
	ptm = localtime( &curr_time );

	m_TimeAdded.Format( "%02d/%02d/%d %02d:%02d:%02d ", ptm->tm_mday, ptm->tm_mon, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec );
}

///////////////////////////////////////////////////

void CmusikLibrary::InitFields()
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
	m_Fields.push_back( "Equalizer" );

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
	m_FieldsDB.push_back( "equalizer" );
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFieldID( CmusikString field )
{
	for ( size_t i = 0; i < m_Fields.size(); i++ )
	{
		if ( field == m_Fields.at( i ) )
			return i;
	}
	return -1;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFieldDBID( CmusikString field )
{
	for ( size_t i = 0; i < m_FieldsDB.size(); i++ )
	{
		if ( field == m_FieldsDB.at( i ) )
			return i;
	}
	return -1;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitStdTables()
{
	if ( !m_DatabaseOpen )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery1  = 
		"CREATE TABLE " STD_PLAYLIST_TABLE_NAME " ( "	
		"std_playlist_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"std_playlist_name varchar(255), "
		"std_playlist_type INTEGER "
		" );";

	// construct the table that will store all the
	// songs that pertain to all the playlists
	static const char *szCreateDBQuery2  = 
		"CREATE TABLE " STD_PLAYLIST_SONGS " ( "	
		"std_playlist_songid INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"std_playlist_id INTEGER, "
		"songfn varchar(1024)"
		" );";

	// put a lock on the library and open it up
	char *pErr1 = NULL;
	char *pErr2 = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	m_ProtectingLibrary.unlock();

	if ( pErr1 )
	{
		error = true;
		sqlite_freemem( pErr1 );
	}

	if ( pErr2 )
	{
		error = true;
		sqlite_freemem( pErr2 );
	}

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitEqTable()
{
	if ( !m_DatabaseOpen )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery1  = 
		"CREATE TABLE " EQUALIZER_PRESET " ( "	
		"equalizer_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"equalizer_name VARCHAR(255), "
		"equalizer_is_preset INTEGER, "
		"hz55_left INTEGER, "
		"hz77_left INTEGER, "
		"hz110_left INTEGER, "
		"hz156_left INTEGER, "
		"hz220_left INTEGER, "
		"hz311_left INTEGER, "
		"hz440_left INTEGER, "
		"hz622_left INTEGER, "
		"hz880_left INTEGER, "
		"hz1244_left INTEGER, "
		"hz1760_left INTEGER, "
		"hz2489_left INTEGER, "
		"hz3520_left INTEGER, "
		"hz4978_left INTEGER, "
		"hz7040_left INTEGER, "
		"hz9956_left INTEGER, "
		"hz14080_left INTEGER, "
		"hz19912_left INTEGER, "
		"hz55_right INTEGER, "
		"hz77_right INTEGER, "
		"hz110_right INTEGER, "
		"hz156_right INTEGER, "
		"hz220_right INTEGER, "
		"hz311_right INTEGER, "
		"hz440_right INTEGER, "
		"hz622_right INTEGER, "
		"hz880_right INTEGER, "
		"hz1244_right INTEGER, "
		"hz1760_right INTEGER, "
		"hz2489_right INTEGER, "
		"hz3520_right INTEGER, "
		"hz4978_right INTEGER, "
		"hz7040_right INTEGER, "
		"hz9956_right INTEGER, "
		"hz14080_right INTEGER, "
		"hz19912_right INTEGER "
		" ); ";

	// this is the default equalizer to use, there
	// should never be more than A SINGLE ROW in this
	// table...
	static const char *szCreateDBQuery2  = 
		"CREATE TABLE " EQUALIZER_DEFAULT " ( "	
		"equalizer_id INTEGER PRIMARY KEY, "
		"hz55_left INTEGER, "
		"hz77_left INTEGER, "
		"hz110_left INTEGER, "
		"hz156_left INTEGER, "
		"hz220_left INTEGER, "
		"hz311_left INTEGER, "
		"hz440_left INTEGER, "
		"hz622_left INTEGER, "
		"hz880_left INTEGER, "
		"hz1244_left INTEGER, "
		"hz1760_left INTEGER, "
		"hz2489_left INTEGER, "
		"hz3520_left INTEGER, "
		"hz4978_left INTEGER, "
		"hz7040_left INTEGER, "
		"hz9956_left INTEGER, "
		"hz14080_left INTEGER, "
		"hz19912_left INTEGER, "
		"hz55_right INTEGER, "
		"hz77_right INTEGER, "
		"hz110_right INTEGER, "
		"hz156_right INTEGER, "
		"hz220_right INTEGER, "
		"hz311_right INTEGER, "
		"hz440_right INTEGER, "
		"hz622_right INTEGER, "
		"hz880_right INTEGER, "
		"hz1244_right INTEGER, "
		"hz1760_right INTEGER, "
		"hz2489_right INTEGER, "
		"hz3520_right INTEGER, "
		"hz4978_right INTEGER, "
		"hz7040_right INTEGER, "
		"hz9956_right INTEGER, "
		"hz14080_right INTEGER, "
		"hz19912_right INTEGER "
		" ); ";

	// put a lock on the library and open it up
	char *pErr1 = NULL;
	char *pErr2 = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	m_ProtectingLibrary.unlock();

	if ( pErr1 )
	{
		error = true;
		sqlite_freemem( pErr1 );
	}

	if ( pErr2 )
	{
		error = true;
		sqlite_freemem( pErr2 );
	}

	ResetDefaultEqualizer( false );

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitPathTable()
{
	if ( !m_DatabaseOpen )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " MANAGED_PATH " ( "	
		"path_name varchar(1024) PRIMARY KEY );";

	// put a lock on the library and open it up
	char *pErr = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, &pErr );
	m_ProtectingLibrary.unlock();

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitDynTable()
{
	if ( !m_DatabaseOpen )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery1  = 
		"CREATE TABLE " DYN_PLAYLIST_TABLE_NAME " ( "	
		"dyn_playlist_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"dyn_playlist_name varchar(255) "
		" );";

	// construct the table that will store all the
	// songs that pertain to all the playlists
	static const char *szCreateDBQuery2  = 
		"CREATE TABLE " DYN_PLAYLIST_QUERY " ( "	
		"dyn_playlist_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"dyn_playlist_queryid INTEGER, "
		"dyn_query_str varchar(1024) INTEGER"
		" );";

	// put a lock on the library and open it up
	char *pErr1 = NULL;
	char *pErr2 = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	m_ProtectingLibrary.unlock();

	if ( pErr1 )
	{
		error = true;
		sqlite_freemem( pErr1 );
	}
	if ( pErr2 )
	{
		error = true;
		sqlite_freemem( pErr2 );
	}

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitCrossfaderTable()
{
	if ( !m_DatabaseOpen )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the crossfader presets
	static const char *szCreateDBQuery1  = 
		"CREATE TABLE " CROSSFADER_PRESET " ( "	
		"crossfader_id INTEGER PRIMARY KEY, "
		"crossfader_name VARCHAR(255), "
		"newsong FLOAT, "
		"pause_resume FLOAT, "
		"seek FLOAT, "
		"stop FLOAT, "
		"exit FLOAT "
		" );";

	static const char *szCreateDBQuery2  = 
		"CREATE TABLE " CROSSFADER_DEFAULT " ( "	
		"crossfader_id INTEGER PRIMARY KEY, "
		"newsong FLOAT, "
		"pause_resume FLOAT, "
		"seek FLOAT, "
		"stop FLOAT, "
		"exit FLOAT "
		" );";

	// put a lock on the library and open it up
	char *pErr1 = NULL;
	char *pErr2 = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	m_ProtectingLibrary.unlock();

	if ( pErr1 )
	{
		error = true;
		sqlite_freemem( pErr1 );
	}

	if ( pErr2 )
	{
		error = true;
		sqlite_freemem( pErr2 );
	}

	ResetDefaultCrossfader( false );

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitLibTable()
{
	if ( !m_DatabaseOpen )
		return false;

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
		"year number(4), "		
		"genre varchar(255), "	
		"rating number(1), "		
		"bitrate number(10), "	
		"lastplayed timestamp, "	
		"notes varchar(255), "	
		"timesplayed number(5), "	
		"duration number(10), "	
		"timeadded timestamp, "	
		"filesize number(10), "	
		"dirty number(10), "
		"equalizer number(10) "
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
	char *pErr1 = NULL;
	char* pErr2 = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, &pErr2 );
	m_ProtectingLibrary.unlock();

	if ( pErr1 )
	{
		error = true;
		sqlite_freemem( pErr1 );
	}

	if ( pErr2 )
	{
		error = true;
		sqlite_freemem( pErr2 );
	}

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitTempLibTable()
{
	if ( !m_DatabaseOpen )
		return false;

	bool error = false;

	// construct the table
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " 
		TEMP_SONG_TABLE_NAME 
		" ( "	
		"songid INTEGER PRIMARY KEY, "
		"format number(1), "		
		"vbr number(1), "			
		"filename varchar(255), "	
		"artist varchar(255), "	
		"title varchar(255), "	
		"album varchar(255), "	
		"tracknum number(3), "	
		"year number(4), "		
		"genre varchar(255), "	
		"rating number(1), "		
		"bitrate number(10), "	
		"lastplayed timestamp, "	
		"notes varchar(255), "	
		"timesplayed number(5), "	
		"duration number(10), "	
		"timeadded timestamp, "	
		"filesize number(10), "	
		"dirty number(10), "
		"equalizer number(10) "
		" );";

	// construct the index
	const char* szCreateIdxQuery =
		"CREATE INDEX songs_title_idx on " TEMP_SONG_TABLE_NAME " (title);"
		"CREATE UNIQUE INDEX songs_filename_idx on " TEMP_SONG_TABLE_NAME " (filename);"
		"CREATE INDEX songs_artist_idx on " TEMP_SONG_TABLE_NAME " (artist);"
		"CREATE INDEX songs_album_idx on " TEMP_SONG_TABLE_NAME " (album);"
		"CREATE INDEX songs_genre_idx on " TEMP_SONG_TABLE_NAME " (genre);"
		"CREATE INDEX songs_artist_album_tracknum_idx on " TEMP_SONG_TABLE_NAME " (artist,album,tracknum);";

	// put a lock on the library and open it up
	char *pErr1 = NULL;
	char* pErr2 = NULL;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, &pErr2 );
	m_ProtectingLibrary.unlock();

	if ( pErr1 )
	{
		error = true;
		sqlite_freemem( pErr1 );
	}

	if ( pErr2 )
	{
		error = true;
		sqlite_freemem( pErr2 );
	}

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::Startup()
{
	bool error = false;

	if ( m_DatabaseOpen )
		Shutdown();

	char* pErr = NULL;

	m_ProtectingLibrary.lock();

		m_pDB = sqlite_open( m_Filename.c_str(), 0666, &pErr );

		if ( m_pDB )
			m_DatabaseOpen = true;

	m_ProtectingLibrary.unlock();

	if ( m_DatabaseOpen && !pErr )	
	{
		InitLibTable();
		InitTempLibTable();
		InitStdTables();
		InitDynTable();
		InitEqTable();
		InitPathTable();
		InitTimeAdded();
		InitCrossfaderTable();
   }
   else
	   error = true;

   if ( pErr )
	   sqlite_freemem( pErr );

   return error;
}

///////////////////////////////////////////////////

void CmusikLibrary::Shutdown()
{
	// lock it up and close it down.
	m_ProtectingLibrary.lock();
		if ( m_DatabaseOpen )
		{
			sqlite_close( m_pDB );
			m_pDB = NULL;
			m_DatabaseOpen = false;
		}	
	m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

void CmusikLibrary::BeginTransaction()
{
	if ( !m_DatabaseOpen )
		return;

	if ( m_Transactions == 0 )
	{
		m_ProtectingLibrary.lock();
			sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
		m_ProtectingLibrary.unlock();

		TRACE0( "Begin transaction\n" );
	}

	++m_Transactions;
}

///////////////////////////////////////////////////

void CmusikLibrary::EndTransaction()
{
	if ( !m_DatabaseOpen )
		return;

	--m_Transactions;

	if ( m_Transactions == 0 )
	{
		m_ProtectingLibrary.lock();
			sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );
		m_ProtectingLibrary.unlock();

		TRACE0( "End transaction\n" );
	}
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteCrossfader( int id )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE crossfader_id = %d", 
			NULL, NULL, NULL,
			CROSSFADER_PRESET,
			id );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateStdPlaylist( const CmusikString& name, const CmusikStringArray& songids, int type, bool add_to_library )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( type != MUSIK_PLAYLIST_TYPE_STANDARD && type != MUSIK_PLAYLIST_TYPE_SUBLIBRARY )
		return MUSIK_LIBRARY_INVALID_PLAYLIST_TYPE;

	int nID, nRet;

	m_ProtectingLibrary.lock();
		// insert the new playlist name
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q, %d );", 
			NULL, NULL, NULL, 
			STD_PLAYLIST_TABLE_NAME,
			NULL,
			name.c_str(),
			type );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return -1;

	// get the ID of the newly created entry
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
			&sqlite_GetIntFromRow, &nID, NULL,
			STD_PLAYLIST_TABLE_NAME,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return -1;

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();
		for ( size_t i = 0; i < songids.size(); i++ )
		{
			if ( add_to_library )
				AddSong( songids.at( i ) );

			m_ProtectingLibrary.lock();
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
					NULL, NULL, NULL, 
					STD_PLAYLIST_SONGS,
					NULL,
					nID,
					songids.at( i ).c_str() );
			m_ProtectingLibrary.unlock();

			if ( nRet != MUSIK_LIBRARY_OK )
			{
				EndTransaction();
				return nRet;
			}
		}
		EndTransaction();
	}	

	return nID;
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateStdPlaylist( const CmusikString& name, CmusikPlaylist& playlist, int type )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( type != MUSIK_PLAYLIST_TYPE_STANDARD && type != MUSIK_PLAYLIST_TYPE_SUBLIBRARY )
		return MUSIK_LIBRARY_INVALID_PLAYLIST_TYPE;

	int nID, nRet;

	// lock it up
	m_ProtectingLibrary.lock();
		// insert the new playlist name
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q, %d );", 
			NULL, NULL, NULL, 
			STD_PLAYLIST_TABLE_NAME,
			NULL,
			name.c_str(),
			type );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return -1;

	// get the ID of the newly created entry
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
			&sqlite_GetIntFromRow, &nID, NULL,
			STD_PLAYLIST_TABLE_NAME,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return -1;

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();
		for ( size_t i = 0; i < playlist.GetCount(); i++ )
		{
			m_ProtectingLibrary.lock();
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
					NULL, NULL, NULL, 
					STD_PLAYLIST_SONGS,
					NULL,
					nID,
					playlist.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ).c_str() );
			m_ProtectingLibrary.unlock();

			if ( nRet != MUSIK_LIBRARY_OK )
			{
				EndTransaction();
				return nRet;
			}
		}
		EndTransaction();
	}	

	return nID;
}

///////////////////////////////////////////////////

int CmusikLibrary::AppendStdPlaylist( int id, const CmusikStringArray& files, bool add_to_library )
{
	if ( !m_DatabaseOpen ) 
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	if ( id >= 0 )
	{
		BeginTransaction();	
		for ( size_t i = 0; i < files.size(); i++ )
		{
			AddSong( files.at( i ) );

			if ( add_to_library )
				AddSong( files.at( i ) );

			m_ProtectingLibrary.lock();
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				id,
				files.at( i ).c_str() );
			m_ProtectingLibrary.unlock();

			if ( nRet != MUSIK_LIBRARY_OK )
			{
				EndTransaction();
				return nRet;
			}
		}
		EndTransaction();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::AppendStdPlaylist( int id, CmusikPlaylist& playlist )
{
	if ( !m_DatabaseOpen ) 
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	if ( id >= 0 )
	{
		BeginTransaction();	
		for ( size_t i = 0; i < playlist.GetCount(); i++ )
		{
			m_ProtectingLibrary.lock();
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				id,
				playlist.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ).c_str() );
			m_ProtectingLibrary.unlock();

			if ( nRet != MUSIK_LIBRARY_OK )
			{
				EndTransaction();
				return nRet;
			}
		}
		EndTransaction();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RewriteDynPlaylist( int id, const CmusikStringArray& query )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	if ( id >= 0 )
	{
		// remove old queries
		m_ProtectingLibrary.lock();
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_id = %d;",
			NULL, NULL, NULL, 
			DYN_PLAYLIST_QUERY,
			id );
		m_ProtectingLibrary.unlock();

		BeginTransaction();
		
		for ( size_t i = 0; i < query.size(); i++ )
		{
			m_ProtectingLibrary.lock();
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
				NULL, NULL, NULL, 
				DYN_PLAYLIST_QUERY,
				NULL,
				id,
				query.at( i ).c_str() );
			m_ProtectingLibrary.unlock();

			if ( nRet != MUSIK_LIBRARY_OK )
			{
				EndTransaction();
				return nRet;
			}
		}

		EndTransaction();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RewriteStdPlaylist( CmusikPlaylist* playlist )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	if ( playlist->m_ID >= 0 )
	{
		// remove all the old values in the playlist 
		// songs table
		m_ProtectingLibrary.lock();
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;",
			NULL, NULL, NULL, 
			STD_PLAYLIST_SONGS,
			playlist->m_ID );
		m_ProtectingLibrary.unlock();

		BeginTransaction();
		
		// add all the songs in the new order
		for ( size_t i = 0; i < playlist->GetCount(); i++ )
		{
			m_ProtectingLibrary.lock();
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				playlist->m_ID,
				playlist->GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ).c_str() );
			m_ProtectingLibrary.unlock();

			if ( nRet != MUSIK_LIBRARY_OK )
			{
				EndTransaction();
				return nRet;
			}
		}

		EndTransaction();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RenameStdPlaylist( int id, const CmusikString& str )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	if ( id >= 0 )
	{
		m_ProtectingLibrary.lock();
			nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET std_playlist_name = %Q WHERE std_playlist_id = %d;",
			NULL, NULL, NULL, 
			STD_PLAYLIST_TABLE_NAME,
			str.c_str(),
			id );
		m_ProtectingLibrary.unlock();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetStdPlaylist( int id, CmusikPlaylist& target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target.Clear();
	
	CmusikStringArray* items = new CmusikStringArray;
	int playlist_type = MUSIK_PLAYLIST_TYPE_UNKNOWN;

	int nRet1, nRet2;
	m_ProtectingLibrary.lock();

		// get the type
		nRet2 = sqlite_exec_printf( m_pDB, "SELECT std_playlist_type FROM %Q WHERE std_playlist_id = %d;", 
			&sqlite_GetIntFromRow, &playlist_type, NULL,
			STD_PLAYLIST_TABLE_NAME,
			id );

		// select only distinct songs from a sub library. this
		// may seem like kludge on the surface, but it is in
		// fact an optimization. even if multiple entries of
		// the same song do exist in the same table, sql can
		// parse it better than a loop with STL. so the user
		// will never see them, operation will be faster, and
		// next time the playlist is saved they will be auto-
		// matically removed.
		if ( playlist_type == MUSIK_PLAYLIST_TYPE_SUBLIBRARY )
			nRet1 = sqlite_exec_printf( m_pDB, "SELECT DISTINCT songfn FROM %Q WHERE std_playlist_id = %d;", 
				&sqlite_AddSongToStringArray, items, NULL,
				STD_PLAYLIST_SONGS,
				id );
		else
			nRet1 = sqlite_exec_printf( m_pDB, "SELECT songfn FROM %Q WHERE std_playlist_id = %d;", 
				&sqlite_AddSongToStringArray, items, NULL,
				STD_PLAYLIST_SONGS,
				id );


	//char* query = sqlite_mprintf( "select sum(a1.duration), sum(a1.filesize) SIZE from %Q A1, %Q A2 where A1.filename = A2.songfn and a2.std_playlist_id = %d;", SONG_TABLE_NAME, STD_PLAYLIST_SONGS, id );

	m_ProtectingLibrary.unlock();	

	//GetPlaylistTotals( target, query );

	// add all the items to the playlist
	CmusikSong song;
	for ( size_t i = 0; i < items->size(); i++ )
	{
		GetSongFromFilename( items->at( i ), song );
		target.Add( song );
	}

	// assure correct type and id are set
	target.m_Type = playlist_type;
	target.m_ID = id;

	delete items;

	if ( !nRet1 )
		return nRet2;

	return nRet1;
}

///////////////////////////////////////////////////

bool CmusikLibrary::GetStdPlaylistFns( int id, CmusikStringArray& target, bool clear_target )
{
	CmusikPlaylist ids;
	GetStdPlaylist( id, ids, false );

	if ( !ids.GetCount() )
		return false;

	if ( clear_target )
		target.clear();

	// do it
	BeginTransaction();

	for ( size_t i = 0; i < ids.GetCount(); i++ )
		target.push_back( ids.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ) );

	EndTransaction();

	return ( target.size() > 0 ? true : false );
}

///////////////////////////////////////////////////

bool CmusikLibrary::GetStdPlaylistFns( CmusikPlaylist& playlist, CmusikStringArray& target, bool clear_target )
{
	if ( !playlist.GetCount() )
		return false;

	if ( clear_target )
		target.clear();

	// do it
	BeginTransaction();

	for ( size_t i = 0; i < playlist.GetCount(); i++ )
		target.push_back( playlist.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ) );

	EndTransaction();

	return ( target.size() > 0 ? true : false );
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateDynPlaylist( const CmusikString& name, const CmusikStringArray& query )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nID, nRet;

	// lock it up
	{
		m_ProtectingLibrary.lock();
			// insert the new playlist name
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q );", 
				NULL, NULL, NULL, 
				DYN_PLAYLIST_TABLE_NAME,
				NULL,
				name.c_str() );
		m_ProtectingLibrary.unlock();
	}

	if ( nRet != MUSIK_LIBRARY_OK )
		return nRet;

	// get the ID of the newly created entry
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT dyn_playlist_id FROM %Q WHERE dyn_playlist_name = %Q;", 
			&sqlite_GetIntFromRow, &nID, NULL,
			DYN_PLAYLIST_TABLE_NAME,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return nRet;

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();

		for ( size_t i = 0; i < query.size(); i++ )
		{
			if ( !query.at( i ).IsEmpty() )
			{
				m_ProtectingLibrary.lock();
					nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
						NULL, NULL, NULL, 
						DYN_PLAYLIST_QUERY,
						NULL,
						nID,
						query.at( i ).c_str() );
				m_ProtectingLibrary.unlock();

				if ( nRet != MUSIK_LIBRARY_OK )
				{
					EndTransaction();
					return nRet;
				}
			}
		}

		EndTransaction();
	}	

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteStdPlaylist( const CmusikString& name )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nID, nRet;

	m_ProtectingLibrary.lock();
		// get ID of the currently named playlist
		nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
			&sqlite_GetIntFromRow, &nID, NULL,
			STD_PLAYLIST_TABLE_NAME, 
			name.c_str() );
	m_ProtectingLibrary.unlock();
	
	if ( nRet != MUSIK_LIBRARY_OK )
		return nRet;

	// remove entry from table containing
	// the list of standard playlists
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_name = %Q;", 
			NULL, NULL, NULL,
			STD_PLAYLIST_TABLE_NAME,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return nRet;

	// delete corresponding songs from the
	// other table
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d", 
			NULL, NULL, NULL,
			STD_PLAYLIST_SONGS,
			nID );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteStdPlaylist( int id )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	// remove entry from table containing
	// the list of standard playlists
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;", 
			NULL, NULL, NULL,
			STD_PLAYLIST_TABLE_NAME,
			id );
	m_ProtectingLibrary.unlock();

	if ( nRet != MUSIK_LIBRARY_OK )
		return nRet;

	// now remove corresponding entries in
	// the table that contains the songs
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;", 
			NULL, NULL, NULL,
			STD_PLAYLIST_SONGS,
			id );
	m_ProtectingLibrary.unlock();


	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteDynPlaylist( const CmusikString& name )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	// do it
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_name = %Q;", 
			NULL, NULL, NULL,
			DYN_PLAYLIST_TABLE_NAME,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteDynPlaylist( int id )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_id = %d;", 
			NULL, NULL, NULL,
			DYN_PLAYLIST_TABLE_NAME,
			id );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

CmusikString CmusikLibrary::GetOrder( int type, bool terminate, bool by_upper, bool descend )
{
	CmusikString sTerminate = "";
	if ( terminate )
		sTerminate = ";";

	CmusikString sDescend;
	if ( descend )
		sDescend = " desc";
	else
		sDescend = " asc";

	if ( !by_upper )
	{
		switch( type )
		{
		case MUSIK_LIBRARY_TYPE_ARTIST:
			sTerminate.Format( "ORDER BY artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_ALBUM:
			sTerminate.Format( "ORDER BY album%s,tracknum%s,artist%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_YEAR:
			sTerminate.Format( "ORDER BY year%s,artist%s,album%s,tracknum%s,title%s%s",
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_GENRE:
			sTerminate.Format( "ORDER BY genre%s,artist%s,album%s,tracknum%s,title%s%s",
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TITLE:
			sTerminate.Format( "ORDER BY title%s,artist%s,album%s,tracknum%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TRACKNUM:
			sTerminate.Format( "ORDER BY tracknum%s,artist%s,album%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TIMEADDED:
			sTerminate.Format( "ORDER BY timeadded%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_LASTPLAYED:
			sTerminate.Format( "ORDER BY lastplayed%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_FILESIZE:
			sTerminate.Format( "ORDER BY filesize%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_FORMAT:
			sTerminate.Format( "ORDER BY format%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_DURATION:
			sTerminate.Format( "ORDER BY duration%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_RATING:
			sTerminate.Format( "ORDER BY rating%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
			sTerminate.Format( "ORDER BY timesplayed%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_BITRATE:
			sTerminate.Format( "ORDER BY bitrate%s,artist%s,album%s,tracknum%s,title%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_FILENAME:
			break;
		}
	}

	else
	{
		switch( type )
		{
		case MUSIK_LIBRARY_TYPE_ARTIST:
			sTerminate.Format( "ORDER BY UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_ALBUM:
			sTerminate.Format( "ORDER BY UPPER(album)%s,tracknum%s,UPPER(artist)%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_YEAR:
			sTerminate.Format( "ORDER BY year%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s",
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_GENRE:
			sTerminate.Format( "ORDER BY UPPER(genre)%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s",
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TITLE:
			sTerminate.Format( "ORDER BY UPPER(title)%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TRACKNUM:
			sTerminate.Format( "ORDER BY tracknum%s,UPPER(artist)%s,UPPER(album)%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TIMEADDED:
			sTerminate.Format( "ORDER BY timeadded%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_LASTPLAYED:
			sTerminate.Format( "ORDER BY lastplayed%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_FILESIZE:
			sTerminate.Format( "ORDER BY filesize%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_FORMAT:
			sTerminate.Format( "ORDER BY format%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_DURATION:
			sTerminate.Format( "ORDER BY duration%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_RATING:
			sTerminate.Format( "ORDER BY rating%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
			sTerminate.Format( "ORDER BY timesplayed%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_BITRATE:
			sTerminate.Format( "ORDER BY bitrate%s,UPPER(artist)%s,UPPER(album)%s,tracknum%s,UPPER(title)%s%s", 
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sDescend.c_str(),
				sTerminate.c_str() );
			return sTerminate;
			break;

		case MUSIK_LIBRARY_TYPE_FILENAME:
			break;
		}
	}

	return sTerminate;
}

///////////////////////////////////////////////////

int CmusikLibrary::QueryCount( const char* pQueryResult )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	const char *pTail;
	sqlite_vm *pVM;
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;
	int result = 0;

	m_ProtectingLibrary.lock();
		sqlite_compile( m_pDB, pQueryResult, &pTail, &pVM, NULL );

		if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
			result = atoi( coldata[0] );

		sqlite_finalize( pVM, &errmsg );
	m_ProtectingLibrary.unlock();

	return result;
}

///////////////////////////////////////////////////

void CmusikLibrary::VerifyYearList( CmusikStringArray & list )
{
	size_t count = list.size();

	for ( size_t i = 0; i < count ; i++ )
	{
		if ( list.at( i ).IsEmpty() || list.at( i ) == "0" )
		{
			list.erase( list.begin() + i );
			count--;
		}
	}
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllSongs( CmusikPlaylist& target, bool use_temp_table )
{
	int nRet = QuerySongs( "filename <> ''", target, use_temp_table );

	if ( !nRet )
	{
		if ( use_temp_table )
			target.m_Type = MUSIK_PLAYLIST_TYPE_SUBLIBRARY;
		else
			target.m_Type = MUSIK_PLAYLIST_TYPE_LIBRARY;
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::QuickQuery( CmusikString str, CmusikPlaylist& target, bool use_temp_table )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	target.Clear();

	str = "%" + str + "%";

	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT songid,duration,filesize FROM %Q WHERE artist LIKE %Q OR album LIKE %Q OR title LIKE %Q OR filename LIKE %Q;", 
			&sqlite_AddSongToPlaylist, &target, NULL,
			use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME, 
			str.c_str(),
			str.c_str(),
			str.c_str(),
			str.c_str() );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::QuerySongs( const CmusikString& query, CmusikPlaylist& target, bool use_temp_table )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	target.Clear();

	// lock it up and query it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT DISTINCT songid,duration,filesize FROM %Q WHERE %q;", 
			&sqlite_AddSongToPlaylist, &target, NULL,
			use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME, 
			query.c_str() );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RawQuerySongs( const CmusikString& query, CmusikPlaylist& target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	target.Clear();

	// lock it up and query it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, query.c_str(), 
			&sqlite_AddSongToPlaylist, &target, NULL );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedItems( int source_type, const CmusikStringArray& source_items, int target_type, CmusikStringArray& target, bool use_temp_table )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	target.clear();

	if ( source_type == -1 || target_type == -1 )
		return -1;

	const CmusikString sInType = GetSongFieldDB( source_type );
	const CmusikString sOutType = GetSongFieldDB( target_type );

	// construct the query
	CmusikString query;
	query.Format( "SELECT DISTINCT %s, UPPER( %s ) AS UP FROM %s where ", 
		sOutType.c_str(), 
		sOutType.c_str(), 
		use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME );

	CmusikString sCurrentItem;
	for ( size_t i = 0; i < source_items.size(); i++ )
	{
		sCurrentItem = source_items.at( i );

		if ( i > 0 )
			query += "or ";

        query.Format( "%s%s = '%s' ", query.c_str(), sInType.c_str(), sCurrentItem.c_str() );
	}

	// get sorting order
	query += GetOrder( target_type );

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);
	m_ProtectingLibrary.unlock();

	// if target is years, verify only years
	//get displayed.
	if ( target_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedItems( CmusikString partial_query, int dst_type, CmusikStringArray& target, bool sub_query, bool use_temp_table )
{
	target.clear();

	CmusikString sOutType = GetSongFieldDB( dst_type );

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		if ( !sub_query )
		{
			nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %Q %s order by %q;", 
				&sqlite_AddSongToStringArray, &target, NULL,
				sOutType.c_str(), 
				sOutType.c_str(), 
				use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME,
				partial_query.c_str(),
				sOutType.c_str() );
		}
		else
		{
			partial_query.Replace( "%T", "songid,tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename,equalizer" );

			nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %s order by %q;", 
				&sqlite_AddSongToStringArray, &target, NULL,
				sOutType.c_str(), 
				sOutType.c_str(), 
				partial_query.c_str(),
				sOutType.c_str() );
		}
	m_ProtectingLibrary.unlock();

	// if target is years, verify only years
	// get displayed.
	if ( dst_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedSongs( CmusikString partial_query, int source_type, CmusikPlaylist& target, bool sub_query, bool use_temp_table )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	target.Clear();

	// do it
	int nRet;
	//char* query = NULL;
	m_ProtectingLibrary.lock();
		if ( !sub_query )
		{
			nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT songid,duration,filesize FROM %Q %s %q;", 
				&sqlite_AddSongToPlaylist, &target, NULL,
				use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME,
				partial_query.c_str(),
				GetOrder( source_type ).c_str() );
		}
		else
		{
			partial_query.Replace( "%T", "songid,tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename,equalizer" );

			nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT songid,duration,filesize FROM %s %q;", 
				&sqlite_AddSongToPlaylist, &target, NULL,
				partial_query.c_str(),
				GetOrder( source_type ).c_str() );		
		}

	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetOpenTransactions()
{
	return m_Transactions;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllDistinct( int source_type, CmusikStringArray& target, bool clear_target, bool use_temp_table )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target.clear();

	CmusikString sField = GetSongFieldDB( source_type );

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %Q ORDER BY UP;", 
			&sqlite_AddSongToStringArray, &target, NULL,
			sField.c_str(), 
			sField.c_str(),
			use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME );
	m_ProtectingLibrary.unlock();

	if ( source_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongCount( bool use_temp_table )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	CmusikString query;
	query.Format( "SELECT COUNT(*) FROM %s;", use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME );

	int result = QueryCount( query.c_str() );

	return result;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetFieldFromID( int id, int field, CmusikString& string )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	CmusikString type = GetSongFieldDB( field );

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT %q FROM %Q WHERE songid = %d;", 
			&sqlite_GetFieldFromID, &string, NULL,
			type.c_str(),
			SONG_TABLE_NAME, 
			id );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongInfoFromID( int id, CmusikSongInfo* info )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	info->SetID( id );

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename,equalizer,dirty FROM %Q WHERE songid = %d;", 
			&sqlite_GetSongInfoFromID, info, NULL,
			SONG_TABLE_NAME,
			id );
	m_ProtectingLibrary.unlock();

	info->SetID( id );

	return nRet;
}

///////////////////////////////////////////////////

void CmusikLibrary::GetSongInfoFromPlaylist( CmusikPlaylist* playlist, CmusikSongInfoArray& target )
{
	CmusikSongInfo info;
	if ( playlist )
	{
		target.clear();

		BeginTransaction();
		for ( size_t i = 0; i < playlist->GetCount(); i++ )
		{
			GetSongInfoFromID( playlist->GetSongID( i ), &info );
			target.push_back( info );
		}
		EndTransaction();
	}
}

///////////////////////////////////////////////////

bool CmusikLibrary::SetSongInfo( CmusikSongInfo* info, int songid )
{
	if ( !m_DatabaseOpen )
		return false;

	int result = 0;

	if ( songid == -1 )
		songid = info->GetID();

	// lock it up and run the query
	m_ProtectingLibrary.lock();
		result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET format = %d, vbr = %d, filename = %Q, artist = %Q, title = %Q, album = %Q, tracknum = %d, year = %d, genre = %Q, rating = %d, bitrate = %d, lastplayed = %Q, notes = %Q, timesplayed = %d, duration = %d, timeadded = %Q, filesize = %d, dirty = %d WHERE songid = %d;",
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			atoi( info->GetFormat().c_str() ),
			atoi( info->GetVBR().c_str() ),
			info->GetFilename().c_str(),
			info->GetArtist().c_str(),
			info->GetTitle().c_str(),
			info->GetAlbum().c_str(),
			atoi( info->GetTrackNum().c_str() ),
			atoi( info->GetYear().c_str() ),
			info->GetGenre().c_str(),
			atoi( info->GetRating().c_str() ),
			atoi( info->GetBitrate().c_str() ),
			info->GetLastPlayed().c_str(),
			info->GetNotes().c_str(),
			atoi( info->GetTimesPlayed().c_str() ),
			atoi( info->GetDuration().c_str() ),
			info->GetTimeAdded().c_str(),
			atoi( info->GetFilesize().c_str() ),
			atoi( info->GetDirtyFlag().c_str() ),
			songid );
	m_ProtectingLibrary.unlock();

	if ( result != MUSIK_LIBRARY_OK )
		return false;
	
	return true;    
}

///////////////////////////////////////////////////

bool CmusikLibrary::SetSongRating( int songid, int rating )
{
	if ( !m_DatabaseOpen )
		return false;

	int result = 0;

	// lock it up and run the query
	m_ProtectingLibrary.lock();
		result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET rating = %d WHERE songid = %d", 
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			rating,
			songid );
	m_ProtectingLibrary.unlock();

	if ( result != MUSIK_LIBRARY_OK )
		return false;
	
	return true;    
}

///////////////////////////////////////////////////

bool CmusikLibrary::SetSongEqualizer( int songid, int eq_id )
{
	if ( !m_DatabaseOpen )
		return false;

	int result = 0;

	// lock it up and run the query
	m_ProtectingLibrary.lock();
		result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET equalizer = %d WHERE songid = %d", 
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			eq_id,
			songid );
	m_ProtectingLibrary.unlock();

	if ( result != MUSIK_LIBRARY_OK )
		return false;
	
	return true;  
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllStdPlaylists( CmusikPlaylistInfoArray* target, int type, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( type != MUSIK_PLAYLIST_TYPE_STANDARD && type != MUSIK_PLAYLIST_TYPE_SUBLIBRARY )
		return MUSIK_LIBRARY_INVALID_PLAYLIST_TYPE;

	if ( clear_target )
		target->clear();

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_name,std_playlist_id FROM %Q WHERE std_playlist_type = %d;", 
			&sqlite_AddStdPlaylistInfoArray, target, NULL,
			STD_PLAYLIST_TABLE_NAME,
			type );
	m_ProtectingLibrary.unlock();

	// make sure the correct type is set
	for ( size_t i = 0; i < target->size(); i++ )
		target->at( i ).m_Type = type;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllDynPlaylists( CmusikPlaylistInfoArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target->clear();

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT dyn_playlist_name,dyn_playlist_id FROM %Q WHERE dyn_playlist_name <> '';",
			&sqlite_AddDynPlaylistInfoArray, target, NULL,
			DYN_PLAYLIST_TABLE_NAME );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllCrossfaderPresets( CmusikStringArray* target, CIntArray* target_ids, bool clear_targets )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_targets )
	{
		target->clear();

		if ( target_ids )
			target_ids->clear();
	}

	int nRet1, nRet2 = 0;
	BeginTransaction();

	m_ProtectingLibrary.lock();
		if ( target )
		{
			nRet1 = sqlite_exec_printf( m_pDB, "SELECT crossfader_name FROM %Q order by crossfader_id;",
				&sqlite_AddRowToStringArray, target, NULL,
				CROSSFADER_PRESET );
		}

		if ( target_ids )
		{
			nRet2 = sqlite_exec_printf( m_pDB, "SELECT crossfader_id FROM %Q order by crossfader_id;",
				&sqlite_AddRowToIntArray, target_ids, NULL,
				CROSSFADER_PRESET );	
		}
	m_ProtectingLibrary.unlock();

	EndTransaction();

	if ( nRet1 != 0 )
		return nRet1;

	return nRet2;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetCrossfader( int id, CmusikCrossfader* fader )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT newsong, pause_resume, seek, stop, exit, crossfader_name FROM %Q WHERE crossfader_id = %d;", 
			&sqlite_GetCrossfader, fader, NULL,
			CROSSFADER_PRESET,
			id );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFromFilename( CmusikString fn, CmusikSong& song )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	// do it
	int nRet;
	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT songid,duration,filesize FROM %Q WHERE filename = %Q;", 
			&sqlite_GetSongFromFilename, &song, NULL,
			SONG_TABLE_NAME,
			fn.c_str() );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetFilesize( const CmusikString& fn )
{
	FILE* pFile = fopen( fn.c_str(), "rb" );
	if ( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		int fs = ftell( pFile );
		fclose( pFile );
		return fs;
	}

	return 0;
}

///////////////////////////////////////////////////

bool CmusikLibrary::RemoveSong( int songid )
{
	if ( !m_DatabaseOpen )
		return false;

	int nRes;

	m_ProtectingLibrary.lock();
		nRes = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE songid=%d;", NULL, NULL, NULL,
			SONG_TABLE_NAME,
			songid );
	m_ProtectingLibrary.unlock();

	if ( nRes == MUSIK_LIBRARY_OK )
		return true;

	return false;
}	

///////////////////////////////////////////////////

bool CmusikLibrary::RemoveSong( const CmusikString& fn )
{
	if ( !m_DatabaseOpen )
		return false;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE filename = %Q;", NULL, NULL, NULL,
			SONG_TABLE_NAME,
			fn.c_str() );
	m_ProtectingLibrary.unlock();

	if ( nRet == MUSIK_LIBRARY_OK )
		return true;

	return false;
}	

///////////////////////////////////////////////////

bool CmusikLibrary::AddSong( const CmusikString& fn )
{
	bool result = true;

	if ( !IsSongInLibrary( fn ) )
	{
		CmusikFilename MFN( fn );
		CmusikString sExt = MFN.GetExtension();

		if ( sExt == "mp3" )
			result = AddMP3( fn );
		else if ( sExt == "ogg" )
			result = AddOGG( fn );
	}

	return result;
}

///////////////////////////////////////////////////

void CmusikLibrary::ClearTempSongTable()
{
	if ( !m_DatabaseOpen )
		return;

	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, "delete from temp_songs;", NULL, NULL, NULL );
	m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

int CmusikLibrary::PopulateTempSongTable( CmusikPlaylist& source )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int ret = MUSIK_LIBRARY_OK;

	ClearTempSongTable();

	BeginTransaction();
	for ( size_t i = 0; i < source.GetCount(); i++ )
	{
		ret = InsertTempSong( source.GetSongID( i ) );

		if ( ret != MUSIK_LIBRARY_OK )
			break;
	}
	EndTransaction();

	return ret;
}

///////////////////////////////////////////////////

int CmusikLibrary::InsertTempSong( int songid )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( songid == -1 )
		return MUSIK_LIBRARY_NO_ID_EXISTS;

	CmusikSongInfo info;
	GetSongInfoFromID( songid, &info );

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %d, %d, %d, %Q, %Q, %Q, %Q, %d, %d, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
			TEMP_SONG_TABLE_NAME, 		
			info.GetID(),
			atoi( info.GetFormat() ),
			atoi( info.GetVBR() ),
			info.GetFilename().c_str(),
			info.GetArtist().c_str(),
			info.GetTitle().c_str(),
			info.GetAlbum().c_str(),
			atoi( info.GetTrackNum() ),
			atoi( info.GetYear() ),
			info.GetGenre().c_str(),
			atoi( info.GetRating() ),
			atoi( info.GetBitrate() ),
			info.GetLastPlayed().c_str(),							
			info.GetNotes().c_str(),								
			atoi( info.GetDuration().c_str() ),							
			atoi( info.GetDuration().c_str() ),						
			info.GetTimeAdded().c_str(),							
			info.GetFilesize().c_str(),								
			atoi( info.GetDirtyFlag().c_str() ),				
			atoi( info.GetEqualizer().c_str() ) );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

bool CmusikLibrary::AddOGG( const CmusikString& fn )
{
	if ( !m_DatabaseOpen )
		return false;

	CmusikOggInfo info;
	if ( info.LoadInfo( fn ) )
	{
		int nRet;

		m_ProtectingLibrary.lock();
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %d, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
				SONG_TABLE_NAME,											// song table 		
				NULL,														// id
				MUSIK_LIBRARY_FORMAT_OGG,									// format
				1,															// vbr
				fn.c_str(),													// filename
				info.Get()->GetArtist().c_str(),							// artist 
				info.Get()->GetTitle().c_str(),								// title
				info.Get()->GetAlbum().c_str(),								// album
				atoi( info.Get()->GetTrackNum().c_str() ),					// tracknum
				atoi( info.Get()->GetYear().c_str() ),						// year
				info.Get()->GetGenre().c_str(),								// genre
				0,															// rating
				atoi( info.Get()->GetBitrate() ),							// bitrate
				"",															// last played
				"",															// notes
				0,															// times played
				atoi( info.Get()->GetDuration() ),							// duration
				m_TimeAdded.c_str(),										// time added
				GetFilesize( fn ),											// file size
				0,															// dirty
				-1 );														// default equalizer
		m_ProtectingLibrary.unlock();

		if ( nRet == MUSIK_LIBRARY_OK )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool CmusikLibrary::AddMP3( const CmusikString& fn )
{
	if ( !m_DatabaseOpen )
		return false;

	CmusikMp3Info info;
	if ( info.LoadInfo( fn ) )
	{
		int nRet;

		m_ProtectingLibrary.lock();
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
				SONG_TABLE_NAME,											// song table 		
				NULL,														// id
				MUSIK_LIBRARY_FORMAT_MP3,									// format
				atoi( info.Get()->GetVBR() ),								// vbr
				fn.c_str(),													// filename
				info.Get()->GetArtist().c_str(),							// artist 
				info.Get()->GetTitle().c_str(),								// title
				info.Get()->GetAlbum().c_str(),								// album
				atoi( info.Get()->GetTrackNum().c_str() ),					// tracknum
				info.Get()->GetYear().c_str(),								// year
				info.Get()->GetGenre().c_str(),								// genre
				0,															// rating
				atoi( info.Get()->GetBitrate() ),							// bitrate
				"",															// last played
				"",															// notes
				0,															// times played
				atoi( info.Get()->GetDuration() ),							// duration
				m_TimeAdded.c_str(),										// time added
				GetFilesize( fn ),											// file size
				0,															// dirty
				-1 );														// default equalizer
		m_ProtectingLibrary.unlock();

		if ( nRet == MUSIK_LIBRARY_OK )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool CmusikLibrary::IsSongInLibrary( CmusikString fn )
{
	if ( !m_DatabaseOpen )
		return false;


	bool result = false;

	char *query = sqlite_mprintf( "SELECT filename FROM %Q WHERE filename = %Q;", 
		SONG_TABLE_NAME,
		fn.c_str() );
	
	// run query
	const char *pTail;
	sqlite_vm *pVM;

	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	m_ProtectingLibrary.lock();
		sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );

		// see if theres a row
		if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
			result = true;

		// clean up
		sqlite_finalize( pVM, &errmsg );
	m_ProtectingLibrary.unlock();

	sqlite_freemem( query );
	if ( errmsg )
		sqlite_freemem( errmsg );

	return result;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizerIDFromSongID( int id )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int target;

	// do it
	m_ProtectingLibrary.lock();
		sqlite_exec_printf( m_pDB, "SELECT equalizer FROM %Q WHERE songid = %d;", 
			&sqlite_GetIntFromRow, &target, NULL,
			SONG_TABLE_NAME,
			id );
	m_ProtectingLibrary.unlock();

	return target;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetCrossfaderIDFromName( const CmusikString& name )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int target = -1;

	// do it
	m_ProtectingLibrary.lock();
		sqlite_exec_printf( m_pDB, "SELECT crossfader_id FROM %Q WHERE crossfader_name = %Q;", 
			&sqlite_GetIntFromRow, &target, NULL,
			CROSSFADER_PRESET,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	return target;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizerIDFromName( const CmusikString& name )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int target;

	// do it
	m_ProtectingLibrary.lock();
		sqlite_exec_printf( m_pDB, "SELECT equalizer_id FROM %Q WHERE equalizer_name = %Q;", 
			&sqlite_GetIntFromRow, &target, NULL,
			EQUALIZER_PRESET,
			name.c_str() );
	m_ProtectingLibrary.unlock();

	if ( target < 0 )
		return -1;

	return target;
}

///////////////////////////////////////////////////

// crossfader name must be unique

int CmusikLibrary::CreateCrossfader( CmusikCrossfader* fader )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( GetCrossfaderIDFromName( fader->m_Name ) != -1 )
		return MUSIK_LIBRARY_ID_EXISTS;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q,%Q,%f,%f,%f,%f,%f );", 
			NULL, NULL, NULL,
			CROSSFADER_PRESET,
			NULL,
			fader->m_Name.c_str(),
			fader->m_NewSong,
			fader->m_PauseResume,
			fader->m_Seek,
			fader->m_Stop,
			fader->m_Exit );
	m_ProtectingLibrary.unlock();

	fader->m_ID = GetCrossfaderIDFromName( fader->m_Name );

	return nRet;
}

///////////////////////////////////////////////////

// equalizer name must be unique

int CmusikLibrary::CreateEqualizer( CmusikEQSettings& eq, bool is_preset )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( GetEqualizerIDFromName( eq.m_Name ) != -1 )
		return MUSIK_LIBRARY_ID_EXISTS;

	int nRes;

	m_ProtectingLibrary.lock();
		nRes = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f );", NULL, NULL, NULL, 
			EQUALIZER_PRESET,								// eq table 		
			NULL,											// id
			eq.m_Name.c_str(),								// name
			(int)is_preset,									// preset?
			eq.m_Left[0],
			eq.m_Left[1],
			eq.m_Left[2],
			eq.m_Left[3],
			eq.m_Left[4],
			eq.m_Left[5],
			eq.m_Left[6],
			eq.m_Left[7],
			eq.m_Left[8],
			eq.m_Left[9],
			eq.m_Left[10],
			eq.m_Left[11],
			eq.m_Left[12],
			eq.m_Left[13],
			eq.m_Left[14],
			eq.m_Left[15],
			eq.m_Left[16],
			eq.m_Left[17],
			eq.m_Right[0],
			eq.m_Right[1],
			eq.m_Right[2],
			eq.m_Right[3],
			eq.m_Right[4],
			eq.m_Right[5],
			eq.m_Right[6],
			eq.m_Right[7],
			eq.m_Right[8],
			eq.m_Right[9],
			eq.m_Right[10],
			eq.m_Right[11],
			eq.m_Right[12],
			eq.m_Right[13],
			eq.m_Right[14],
			eq.m_Right[15],
			eq.m_Right[16],
			eq.m_Right[17] );
	m_ProtectingLibrary.unlock();

	eq.m_ID = GetEqualizerIDFromName( eq.m_Name );	

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteEqualizer( int id )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRes;
	m_ProtectingLibrary.lock();
		nRes = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE equalizer_id = %d;",
			NULL, NULL, NULL, 
			EQUALIZER_PRESET,	
			id );
	m_ProtectingLibrary.unlock();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::UpdateEqualizer( int id, const CmusikEQSettings& eq )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRes;

	m_ProtectingLibrary.lock();
		nRes = sqlite_exec_printf( m_pDB, "UPDATE %Q set equalizer_name = %Q, hz55_left = %f, hz77_left = %f, hz110_left = %f, hz156_left = %f, hz220_left = %f, hz311_left = %f, hz440_left = %f, hz622_left = %f, hz880_left = %f, hz1244_left = %f, hz1760_left = %f, hz2489_left = %f, hz3520_left = %f, hz4978_left = %f, hz7040_left = %f, hz9956_left = %f, hz14080_left = %f, hz19912_left = %f,"
							" hz55_right = %f, hz77_right = %f, hz110_right = %f, hz156_right = %f, hz220_right = %f, hz311_right = %f, hz440_right = %f, hz622_right = %f, hz880_right = %f, hz1244_right = %f, hz1760_right = %f, hz2489_right = %f, hz3520_right = %f, hz4978_right = %f, hz7040_right = %f, hz9956_right = %f, hz14080_right = %f, hz19912_right = %f WHERE equalizer_id = %d;",
				NULL, NULL, NULL,
				EQUALIZER_PRESET,
				eq.m_Name.c_str(),
				eq.m_Left[0],
				eq.m_Left[1],
				eq.m_Left[2],
				eq.m_Left[3],
				eq.m_Left[4],
				eq.m_Left[5],
				eq.m_Left[6],
				eq.m_Left[7],
				eq.m_Left[8],
				eq.m_Left[9],
				eq.m_Left[10],
				eq.m_Left[11],
				eq.m_Left[12],
				eq.m_Left[13],
				eq.m_Left[14],
				eq.m_Left[15],
				eq.m_Left[16],
				eq.m_Left[17],
				eq.m_Right[0],
				eq.m_Right[1],
				eq.m_Right[2],
				eq.m_Right[3],
				eq.m_Right[4],
				eq.m_Right[5],
				eq.m_Right[6],
				eq.m_Right[7],
				eq.m_Right[8],
				eq.m_Right[9],
				eq.m_Right[10],
				eq.m_Right[11],
				eq.m_Right[12],
				eq.m_Right[13],
				eq.m_Right[14],
				eq.m_Right[15],
				eq.m_Right[16],
				eq.m_Right[17],
				id );
	m_ProtectingLibrary.unlock();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::UpdateDefaultCrossfader( const CmusikCrossfader& fader )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRes;

	m_ProtectingLibrary.lock();
		nRes = sqlite_exec_printf( m_pDB, "UPDATE %Q SET newsong = %f, pause_resume = %f, seek = %f, stop = %f, exit = %f WHERE crossfader_id = -1;",
				NULL, NULL, NULL,
				CROSSFADER_DEFAULT,
				fader.m_NewSong,
				fader.m_PauseResume,
				fader.m_Seek,
				fader.m_Stop,
				fader.m_Exit );
	m_ProtectingLibrary.unlock();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::UpdateDefaultEqualizer( const CmusikEQSettings& eq )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRes;

	m_ProtectingLibrary.lock();
		nRes = sqlite_exec_printf( m_pDB, "UPDATE %Q SET hz55_left = %f, hz77_left = %f, hz110_left = %f, hz156_left = %f, hz220_left = %f, hz311_left = %f, hz440_left = %f, hz622_left = %f, hz880_left = %f, hz1244_left = %f, hz1760_left = %f, hz2489_left = %f, hz3520_left = %f, hz4978_left = %f, hz7040_left = %f, hz9956_left = %f, hz14080_left = %f, hz19912_left = %f,"
							" hz55_right = %f, hz77_right = %f, hz110_right = %f, hz156_right = %f, hz220_right = %f, hz311_right = %f, hz440_right = %f, hz622_right = %f, hz880_right = %f, hz1244_right = %f, hz1760_right = %f, hz2489_right = %f, hz3520_right = %f, hz4978_right = %f, hz7040_right = %f, hz9956_right = %f, hz14080_right = %f, hz19912_right = %f WHERE equalizer_id = -1;",
				NULL, NULL, NULL,
				EQUALIZER_DEFAULT,
				eq.m_Left[0],
				eq.m_Left[1],
				eq.m_Left[2],
				eq.m_Left[3],
				eq.m_Left[4],
				eq.m_Left[5],
				eq.m_Left[6],
				eq.m_Left[7],
				eq.m_Left[8],
				eq.m_Left[9],
				eq.m_Left[10],
				eq.m_Left[11],
				eq.m_Left[12],
				eq.m_Left[13],
				eq.m_Left[14],
				eq.m_Left[15],
				eq.m_Left[16],
				eq.m_Left[17],
				eq.m_Right[0],
				eq.m_Right[1],
				eq.m_Right[2],
				eq.m_Right[3],
				eq.m_Right[4],
				eq.m_Right[5],
				eq.m_Right[6],
				eq.m_Right[7],
				eq.m_Right[8],
				eq.m_Right[9],
				eq.m_Right[10],
				eq.m_Right[11],
				eq.m_Right[12],
				eq.m_Right[13],
				eq.m_Right[14],
				eq.m_Right[15],
				eq.m_Right[16],
				eq.m_Right[17] );
	m_ProtectingLibrary.unlock();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetDefaultCrossfader( CmusikCrossfader* target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT newsong, pause_resume, seek, stop, exit FROM %Q WHERE crossfader_id = -1;", 
									&sqlite_GetCrossfader, target, NULL, 
									CROSSFADER_DEFAULT );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetDefaultEqualizer( CmusikEQSettings* target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT hz55_left, hz77_left, hz110_left, hz156_left, hz220_left, hz311_left, hz440_left, hz622_left, hz880_left, hz1244_left, hz1760_left, hz2489_left, hz3520_left, hz4978_left, hz7040_left, hz9956_left, hz14080_left, hz19912_left,"
									" hz55_right, hz77_right, hz110_right, hz156_right, hz220_right, hz311_right, hz440_right, hz622_right, hz880_right, hz1244_right, hz1760_right, hz2489_right, hz3520_right, hz4978_right, hz7040_right, hz9956_right, hz14080_right, hz19912_right"
									" FROM %Q WHERE equalizer_id = -1;", 
									&sqlite_GetEqualizer, target, NULL, 
									EQUALIZER_DEFAULT );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::ResetDefaultEqualizer( bool clear_old_default )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRes;

	m_ProtectingLibrary.lock();
		if ( clear_old_default )
			sqlite_exec( m_pDB, "delete from equalizer_default", NULL, NULL, NULL );

		nRes = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( -1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,"
							" 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 );",
				NULL, NULL, NULL,
				EQUALIZER_DEFAULT );
	m_ProtectingLibrary.unlock();

	return nRes;	
}

///////////////////////////////////////////////////

int CmusikLibrary::ResetDefaultCrossfader( bool clear_old_default )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRes;

	m_ProtectingLibrary.lock();
		if ( clear_old_default )
			sqlite_exec( m_pDB, "delete from crossfader_default", NULL, NULL, NULL );

		nRes = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( -1, 2.0, 0.5, 0.2, 1.0, 3.0 );",
				NULL, NULL, NULL,
				CROSSFADER_DEFAULT );
	m_ProtectingLibrary.unlock();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizer( int eq_id, CmusikEQSettings* target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT"
									" hz55_left,  hz77_left,  hz110_left,  hz156_left,  hz220_left,  hz311_left,  hz440_left,  hz622_left,  hz880_left,  hz1244_left,  hz1760_left,  hz2489_left,  hz3520_left,  hz4978_left,  hz7040_left,  hz9956_left,  hz14080_left,  hz19912_left,"
									" hz55_right, hz77_right, hz110_right, hz156_right, hz220_right, hz311_right, hz440_right, hz622_right, hz880_right, hz1244_right, hz1760_right, hz2489_right, hz3520_right, hz4978_right, hz7040_right, hz9956_right, hz14080_right, hz19912_right, equalizer_name"
									" FROM %Q WHERE equalizer_id = %d;", 
									&sqlite_GetEqualizer, target, NULL, 
									EQUALIZER_PRESET,
									eq_id );
	m_ProtectingLibrary.unlock();
	
	if ( nRet == 0 )
		target->m_ID = eq_id;
	else
		target->m_ID = -2;

	return nRet;
}

///////////////////////////////////////////////////

// this could probably be a bit faster,
// but i seriously doubt anyone will
// ever notice unless they have something
// like 10,000 equalizer presets on a 486 

int  CmusikLibrary::GetAllEqualizerPresets( CmusikStringArray* target, CIntArray* target_ids, bool clear_targets )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_targets )
	{
		target->clear();

		if ( target_ids )
			target_ids->clear();
	}

	int nRet1, nRet2 = 0;
	BeginTransaction();

	m_ProtectingLibrary.lock();
		if ( target )
		{
			nRet1 = sqlite_exec_printf( m_pDB, "SELECT equalizer_name FROM %Q WHERE equalizer_is_preset = 1 order by equalizer_id;",
				&sqlite_AddRowToStringArray, target, NULL,
				EQUALIZER_PRESET );
		}

		if ( target_ids )
		{
			nRet2 = sqlite_exec_printf( m_pDB, "SELECT equalizer_id FROM %Q WHERE equalizer_is_preset = 1 order by equalizer_id;",
				&sqlite_AddRowToIntArray, target_ids, NULL,
				EQUALIZER_PRESET );	
		}
	m_ProtectingLibrary.unlock();

	EndTransaction();

	if ( nRet1 != 0 )
		return nRet1;

	return nRet2;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFormatFromID( int id, int* target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT format FROM %Q WHERE songid = %d;", 
			&sqlite_GetIntFromRow, target, NULL,
			SONG_TABLE_NAME,
			id );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

void CmusikLibrary::GetInfoArrayFromPlaylist(  CmusikPlaylist* playlist, CmusikSongInfoArray* info, int replace_field_type, CmusikString new_field, bool clear )
{
	if ( !m_DatabaseOpen )
		return;

	if ( clear )
		info->clear();

	CmusikSongInfo tmp_info;

	BeginTransaction();
	for ( size_t i = 0; i < playlist->GetCount(); i++ )
	{
		GetSongInfoFromID( playlist->GetSongID( i ), &tmp_info );

		if ( replace_field_type != -1 )
			tmp_info.SetField( replace_field_type, new_field );

		info->push_back( tmp_info );
	}
	EndTransaction();
}

///////////////////////////////////////////////////

int CmusikLibrary::GetDirtySongs( CmusikPlaylist* target, bool clear )
{
	if ( clear )
		target->Clear();

	int nRet = QuerySongs( "dirty = 1", *target, false );
	if ( !nRet )
		target->m_Type = MUSIK_PLAYLIST_TYPE_STANDARD;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::FinalizeDirtySongs( )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET dirty = 0 WHERE dirty = 1",
			NULL, NULL, NULL,
			SONG_TABLE_NAME );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::AddPath( const CmusikString& path )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q );",
			NULL, NULL, NULL,
			MANAGED_PATH,
			path.c_str() );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RemovePath( const CmusikString& path )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE path_name = %Q;",
			NULL, NULL, NULL,
			MANAGED_PATH,
			path.c_str() );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllPaths( CmusikStringArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target->clear();

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "SELECT path_name FROM %Q WHERE path_name <> '';",
			&sqlite_AddRowToStringArray, target, NULL,
			MANAGED_PATH );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

void CmusikLibrary::ClearLibrary( bool clear_all_tables )
{
	m_ProtectingLibrary.lock();
		sqlite_exec( m_pDB, "delete from songs;", NULL, NULL, NULL );

		if ( clear_all_tables )
		{
			sqlite_exec( m_pDB, "delete from std_playlist;", NULL, NULL, NULL );
			sqlite_exec( m_pDB, "delete from std_playlist_song;", NULL, NULL, NULL );
			sqlite_exec( m_pDB, "delete from dyn_playlist;", NULL, NULL, NULL );
			sqlite_exec( m_pDB, "delete from crossfader_preset;", NULL, NULL, NULL );
			sqlite_exec( m_pDB, "delete from equalizer_preset;", NULL, NULL, NULL );
			sqlite_exec( m_pDB, "delete from equalizer_default;", NULL, NULL, NULL );
			sqlite_exec( m_pDB, "delete from managed_path;", NULL, NULL, NULL );
		}
	m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

int CmusikLibrary::IncLastPlayed( int songid )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	InitTimeAdded();

	int nRet;

	m_ProtectingLibrary.lock();
		nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET timesplayed = (timesplayed + 1), lastplayed = %Q where songid = %d;",
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			m_TimeAdded.c_str(),
			songid );
	m_ProtectingLibrary.unlock();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::SortPlaylist( CmusikPlaylist* playlist, int field, bool descending )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	CmusikString sQuery;

	sQuery = "SELECT songid,duration,filesize FROM songs where ";

	CmusikString sCurr;
	for ( size_t i = 0; i < playlist->GetCount(); i++ )
	{
		if ( i < playlist->GetCount() - 1 )
			sCurr.Format( "songid = %d or ", playlist->GetSongID( i ) );
		else
            sCurr.Format( "songid = %d ", playlist->GetSongID( i ) );

		sQuery += sCurr;
	}

	if ( field == MUSIK_LIBRARY_TYPE_FORMAT || field == MUSIK_LIBRARY_TYPE_TRACKNUM ||
		field == MUSIK_LIBRARY_TYPE_YEAR || field == MUSIK_LIBRARY_TYPE_RATING || field == MUSIK_LIBRARY_TYPE_BITRATE ||
		field == MUSIK_LIBRARY_TYPE_FILESIZE )
		sCurr = GetOrder( field, false, true, descending );

	else
		sCurr = GetOrder( field, false, true, descending );

	sQuery += sCurr;
	sQuery += ";";

	playlist->Clear();

	return RawQuerySongs( sQuery, *playlist );
}

///////////////////////////////////////////////////

void CmusikLibrary::DeleteSongs( CmusikPlaylist& songs, bool delete_from_disk, bool use_temp_table )
{
	CmusikString filename;
	BeginTransaction();
	for ( size_t i = 0; i < songs.GetCount(); i++ )
	{
		if ( delete_from_disk )
			filename = songs.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME );

		m_ProtectingLibrary.lock();
			sqlite_exec_printf( m_pDB, "delete from %Q where songid=%d;", 
				NULL, NULL, NULL,
				use_temp_table ? TEMP_SONG_TABLE_NAME : SONG_TABLE_NAME,
				songs.GetSongID( i ) );
		m_ProtectingLibrary.unlock();

		if ( delete_from_disk && CmusikFilename::FileExists( filename ) )
			remove( filename.c_str() );
	}
	EndTransaction();
}

///////////////////////////////////////////////////
