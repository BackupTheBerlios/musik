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
#include "time.h"

#include "../include/musikFilename.h"
#include "../include/musikConfig.h"
#include "../include/musikCrossfader.h"
#include "../include/musikLibrary.h"
#include "../include/musikMp3Info.h"
#include "../include/musikOggInfo.h"

///////////////////////////////////////////////////

static int sqlite_AddSongToPlaylist(void *args, int numCols, char **results, char ** columnNames )
{
	CmusikPlaylist* p = (CmusikPlaylist*)args;

	CmusikSong *pLibItem = new CmusikSong();
	pLibItem->SetID( atoi( results[0] ) ); 
	
	p->Add( *pLibItem );

	delete pLibItem;

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

	double newsong		= atof( results[1] );
	double pauseresume	= atof( results[2] );
	double seek			= atof( results[3] );
	double stop			= atof( results[4] );
	double exit			= atof( results[5] );

	p->Set( (float)newsong, (float)pauseresume, (float)seek, (float)stop, (float)exit );
	p->SetName( results[0] );

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

static int sqlite_GetEqualizerIDFromID( void *args, int numCols, char **results, char **columnNames )
{
	int* p = (int*)args;

	*p = atoi( results[0] );

	return 0;
}

///////////////////////////////////////////////////

CmusikLibrary::CmusikLibrary( const CmusikString& filename )
{
	m_pDB = NULL;
	m_Transactions = NULL;
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

	m_TimeAdded.Format( "%02d:%02d:%02d %02d/%02d/%d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_mday, ptm->tm_mon, ptm->tm_year + 1900);
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
	char *pErr1 = NULL;
	char *pErr2 = NULL;

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	}

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

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	}

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

	// if there was no error that means that
	// the default table did not exist, so
	// we want to initialize some default values...
	InitDefaultEqualizer();

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

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, &pErr );
	}

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

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, &pErr2 );
	}

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
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " CROSSFADER_PRESET " ( "	
		"crossfader_id INTEGER PRIMARY KEY, "
		"crossfader_name VARCHAR(255), "
		"newsong FLOAT, "
		"pause_resume FLOAT, "
		"seek FLOAT, "
		"stop FLOAT, "
		"exit FLOAT "
		" );";

	// put a lock on the library and open it up
	char *pErr = NULL;

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, &pErr );
	}

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

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

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, &pErr1 );
		sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, &pErr2 );
	}

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

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		m_pDB = sqlite_open( m_Filename.c_str(), 0666, &pErr );

		if ( m_pDB )
			m_DatabaseOpen = true;
	}

	if ( m_DatabaseOpen && !pErr )	
	{
		InitLibTable();
		InitStdTables();
		InitDynTable();
		InitEqTable();
		InitPathTable();
		InitTimeAdded();
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
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		if ( m_DatabaseOpen )
		{
			sqlite_close( m_pDB );
			m_pDB = NULL;
			m_DatabaseOpen = false;
		}	
	}
}

///////////////////////////////////////////////////

void CmusikLibrary::BeginTransaction()
{
	if ( !m_DatabaseOpen )
		return;

	if ( m_Transactions == NULL )
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
		}

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

	if ( m_Transactions == NULL )
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );
		}

		TRACE0( "End transaction\n" );
	}
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateCrossfader( CmusikCrossfader* fader )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q,%f,%f,%f,%f );", 
			NULL, NULL, NULL,
			CROSSFADER_PRESET,
			NULL,
			fader->GetDuration( MUSIK_CROSSFADER_NEW_SONG ),
			fader->GetDuration( MUSIK_CROSSFADER_PAUSE_RESUME ),
			fader->GetDuration( MUSIK_CROSSFADER_STOP ),
			fader->GetDuration( MUSIK_CROSSFADER_EXIT ) );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteCrossfader( int id )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE crossfader_id = %d", 
			NULL, NULL, NULL,
			CROSSFADER_PRESET,
			id );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateStdPlaylist( const CmusikString& name, const CmusikStringArray& songids, bool add_to_library )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nID, nRet;

	// lock it up
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		// insert the new playlist name
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q );", 
			NULL, NULL, NULL, 
			STD_PLAYLIST_TABLE_NAME,
			NULL,
			name.c_str() );
	}

	if ( nRet != SQLITE_OK )
		return -1;

	// get the ID of the newly created entry
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
				&sqlite_GetIntFromRow, &nID, NULL,
				STD_PLAYLIST_TABLE_NAME,
				name.c_str() );
		}
	}

	if ( nRet != SQLITE_OK )
		return -1;

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();
		for ( size_t i = 0; i < songids.size(); i++ )
		{
			if ( add_to_library )
				AddSong( songids.at( i ) );

			ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
			{
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
					NULL, NULL, NULL, 
					STD_PLAYLIST_SONGS,
					NULL,
					nID,
					GetIDFromFilename( songids.at( i ) ) );
			}

			if ( nRet != SQLITE_OK )
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

int CmusikLibrary::CreateStdPlaylist( const CmusikString& name, CmusikPlaylist& playlist )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nID, nRet;

	// lock it up
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		// insert the new playlist name
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q );", 
			NULL, NULL, NULL, 
			STD_PLAYLIST_TABLE_NAME,
			NULL,
			name.c_str() );
	}

	if ( nRet != SQLITE_OK )
		return -1;

	// get the ID of the newly created entry
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
				&sqlite_GetIntFromRow, &nID, NULL,
				STD_PLAYLIST_TABLE_NAME,
				name.c_str() );
		}
	}

	if ( nRet != SQLITE_OK )
		return -1;

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();
		for ( size_t i = 0; i < playlist.GetCount(); i++ )
		{
			ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
			{
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
					NULL, NULL, NULL, 
					STD_PLAYLIST_SONGS,
					NULL,
					nID,
					playlist.GetSongID( i ) );
			}

			if ( nRet != SQLITE_OK )
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
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		BeginTransaction();	
		for ( size_t i = 0; i < files.size(); i++ )
		{
			AddSong( files.at( i ) );

			ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
			{
				if ( add_to_library )
					AddSong( files.at( i ) );

				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				id,
				GetIDFromFilename( files.at( i ) ) );
			}

			if ( nRet != SQLITE_OK )
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
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		BeginTransaction();	
		for ( size_t i = 0; i < playlist.GetCount(); i++ )
		{
			ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
			{
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				id,
				playlist.GetSongID( i ) );
			}

			if ( nRet != SQLITE_OK )
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
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		// remove old queries
		{
			ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
			{
				nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;",
				NULL, NULL, NULL, 
				DYN_PLAYLIST_QUERY,
				id );
			}
		}

		BeginTransaction();
		
		for ( size_t i = 0; i < query.size(); i++ )
		{
			{
				ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
				{
					nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %s );",
					NULL, NULL, NULL, 
					DYN_PLAYLIST_QUERY,
					NULL,
					id,
					query.at( i ).c_str() );
				}
			}

			if ( nRet != SQLITE_OK )
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

int CmusikLibrary::RewriteStdPlaylist( int id, CmusikPlaylist* playlist )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;",
			NULL, NULL, NULL, 
			STD_PLAYLIST_SONGS,
			id );
		}

		BeginTransaction();
		
		for ( size_t i = 0; i < playlist->GetCount(); i++ )
		{
			ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
			{
				nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				id,
				playlist->GetSongID( i ) );
			}

			if ( nRet != SQLITE_OK )
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
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET std_playlist_name = %Q WHERE std_playlist_id = %d;",
			NULL, NULL, NULL, 
			STD_PLAYLIST_TABLE_NAME,
			str.c_str(),
			id );
		}
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetStdPlaylist( int id, CmusikPlaylist& target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target.Clear();

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q WHERE std_playlist_id = %d;", 
			&sqlite_AddSongToPlaylist, &target, NULL,
			STD_PLAYLIST_SONGS,
			id );
	}	

	return nRet;
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
		return -1;

	int nID, nRet;

	// lock it up
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			// insert the new playlist name
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q );", 
				NULL, NULL, NULL, 
				DYN_PLAYLIST_TABLE_NAME,
				NULL,
				name.c_str() );
		}
	}

	if ( nRet != SQLITE_OK )
		return nRet;

	// get the ID of the newly created entry
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "SELECT dyn_playlist_id FROM %Q WHERE dyn_playlist_name = %Q;", 
				&sqlite_GetIntFromRow, &nID, NULL,
				DYN_PLAYLIST_TABLE_NAME,
				name.c_str() );
		}
	}

	if ( nRet != SQLITE_OK )
		return nRet;

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();

		for ( size_t i = 0; i < query.size(); i++ )
		{
			if ( !query.at( i ).IsEmpty() )
			{
				ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
				{
					nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %Q );",
						NULL, NULL, NULL, 
						DYN_PLAYLIST_QUERY,
						NULL,
						nID,
						query.at( i ) );
				}

				if ( nRet != SQLITE_OK )
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
		return -1;

	int nID, nRet;

	// lock it up	
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		// get ID of the currently named playlist
		nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
			&sqlite_GetIntFromRow, &nID, NULL,
			STD_PLAYLIST_TABLE_NAME, 
			name.c_str() );
	}
	
	if ( nRet != SQLITE_OK )
		return nRet;

	// remove entry from table containing
	// the list of standard playlists
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_name = %Q;", 
				NULL, NULL, NULL,
				STD_PLAYLIST_TABLE_NAME,
				name.c_str() );
		}
	}

	if ( nRet != SQLITE_OK )
		return nRet;

	// delete corresponding songs from the
	// other table
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d", 
				NULL, NULL, NULL,
				STD_PLAYLIST_SONGS,
				nID );

		}
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteStdPlaylist( int id )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;

	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			// remove entry from table containing
			// the list of standard playlists
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;", 
				NULL, NULL, NULL,
				STD_PLAYLIST_TABLE_NAME,
				id );
		}
	}

	if ( nRet != SQLITE_OK )
		return nRet;

	// now remove corresponding entries in
	// the table that contains the songs
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;", 
				NULL, NULL, NULL,
				STD_PLAYLIST_SONGS,
				id );
		}
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteDynPlaylist( const CmusikString& name )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;

	// do it
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_name = %Q;", 
			NULL, NULL, NULL,
			DYN_PLAYLIST_TABLE_NAME,
			name.c_str() );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteDynPlaylist( int id )
{
	if ( !m_DatabaseOpen )
		return -1;

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_id = %d;", 
			NULL, NULL, NULL,
			DYN_PLAYLIST_TABLE_NAME,
			id );
	}

	return nRet;
}

///////////////////////////////////////////////////

CmusikString CmusikLibrary::GetOrder( int type, bool terminate )
{
	CmusikString sTerminate = "";
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

int CmusikLibrary::QueryCount( const char* pQueryResult )
{
	if ( !m_DatabaseOpen )
		return -1;

	const char *pTail;
	sqlite_vm *pVM;
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;
	int result = 0;

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_compile( m_pDB, pQueryResult, &pTail, &pVM, NULL );

		if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
			result = atoi( coldata[0] );

		sqlite_finalize( pVM, &errmsg );
	}

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

int CmusikLibrary::GetAllSongs( CmusikPlaylist& target )
{
	return QuerySongs( "filename <> ''", target );
}

///////////////////////////////////////////////////

int CmusikLibrary::QuickQuery( CmusikString str, CmusikPlaylist& target )
{
	if ( !m_DatabaseOpen )
		return -1;

	target.Clear();

	str = "%" + str + "%";

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q WHERE artist LIKE %Q OR album LIKE %Q OR title LIKE %Q OR filename LIKE %Q;", 
			&sqlite_AddSongToPlaylist, &target, NULL,
			SONG_TABLE_NAME, 
			str.c_str(),
			str.c_str(),
			str.c_str(),
			str.c_str() );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::QuerySongs( const CmusikString& query, CmusikPlaylist& target )
{
	if ( !m_DatabaseOpen )
		return -1;

	target.Clear();

	// lock it up and query it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q WHERE %q;", 
			&sqlite_AddSongToPlaylist, &target, NULL,
			SONG_TABLE_NAME, 
			query.c_str() );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RawQuerySongs( const CmusikString& query, CmusikPlaylist& target )
{
	if ( !m_DatabaseOpen )
		return -1;

	target.Clear();

	// lock it up and query it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, query.c_str(), 
			&sqlite_AddSongToPlaylist, &target, NULL );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedItems( int source_type, const CmusikStringArray& source_items, int target_type, CmusikStringArray& target )
{
	if ( !m_DatabaseOpen )
		return -1;

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
		SONG_TABLE_NAME );

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
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);
	}

	// if target is years, verify only years
	//get displayed.
	if ( target_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedItems( CmusikString sub_query, int dst_type, CmusikStringArray& target )
{
	target.clear();

	CmusikString sOutType = GetSongFieldDB( dst_type );

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %Q WHERE %s order by %q;", 
			&sqlite_AddSongToStringArray, &target, NULL,
			sOutType.c_str(), 
			sOutType.c_str(), 
			SONG_TABLE_NAME,
			sub_query.c_str(),
			sOutType.c_str() );
	}

	// if target is years, verify only years
	// get displayed.
	if ( dst_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedSongs( CmusikString sub_query, int source_type, CmusikPlaylist& target )
{
	if ( !m_DatabaseOpen )
		return -1;

	target.Clear();

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT songid FROM %Q WHERE %s %q;", 
			&sqlite_AddSongToPlaylist, &target, NULL,
			SONG_TABLE_NAME,
			sub_query.c_str(),
			GetOrder( source_type ).c_str() );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllDistinct( int source_type, CmusikStringArray& target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target.clear();

	CmusikString sField = GetSongFieldDB( source_type );

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %Q ORDER BY UP;", 
			&sqlite_AddSongToStringArray, &target, NULL,
			sField.c_str(), 
			sField.c_str(),
			SONG_TABLE_NAME );
	}

	if ( source_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongCount()
{
	if ( !m_DatabaseOpen )
		return -1;

	char *query = sqlite_mprintf( "SELECT COUNT(*) FROM " SONG_TABLE_NAME " ;" );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetFieldFromID( int id, int field, CmusikString& string )
{
	if ( !m_DatabaseOpen )
		return -1;

	CmusikString type = GetSongFieldDB( field );

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT %q FROM %Q WHERE songid = %d;", 
			&sqlite_GetFieldFromID, &string, NULL,
			type.c_str(),
			SONG_TABLE_NAME, 
			id );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongInfoFromID( int id, CmusikSongInfo* info )
{
	if ( !m_DatabaseOpen )
		return -1;

	info->SetID( id );

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename,equalizer FROM %Q WHERE songid = %d;", 
			&sqlite_GetSongInfoFromID, info, NULL,
			SONG_TABLE_NAME,
			id );
	}

	return nRet;
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
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
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
	}

	if ( result != SQLITE_OK )
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
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET rating = %d WHERE songid = %d", 
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			rating,
			songid );
	}

	if ( result != SQLITE_OK )
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
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET equalizer = %d WHERE songid = %d", 
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			eq_id,
			songid );
	}

	if ( result != SQLITE_OK )
		return false;
	
	return true;  
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllStdPlaylists( CmusikPlaylistInfoArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_name,std_playlist_id FROM %Q WHERE std_playlist_name <> '';", 
			&sqlite_AddStdPlaylistInfoArray, target, NULL,
			STD_PLAYLIST_TABLE_NAME );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllDynPlaylists( CmusikPlaylistInfoArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT dyn_playlist_name,dyn_playlist_id FROM %Q WHERE dyn_playlist_name <> '';",
			&sqlite_AddDynPlaylistInfoArray, target, NULL,
			DYN_PLAYLIST_TABLE_NAME );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllCrossfaders( CmusikStringArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT crossfader_name FROM %Q WHERE crossfader_name <> '';", 
			&sqlite_AddRowToStringArray, target, NULL,
			CROSSFADER_PRESET );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllCrossfaders( CIntArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT crossfader_id FROM %Q WHERE crossfader_id > -1;", 
			&sqlite_GetIntFromRow, target, NULL,
			CROSSFADER_PRESET );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetCrossfader( int id, CmusikCrossfader* fader )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT crossfader_name, newsong, pause_resume, seek, stop, exit FROM %Q WHERE crossfader_id = %d;", 
			&sqlite_GetCrossfader, fader, NULL,
			CROSSFADER_PRESET,
			id );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetIDFromFilename( CmusikString fn )
{
	if ( !m_DatabaseOpen )
		return -1;

	int target;

	// do it
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q WHERE filename = %Q;", 
			&sqlite_GetIntFromRow, &target, NULL,
			SONG_TABLE_NAME,
			fn.c_str() );
	}

	return target;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFromFilename( CmusikString fn, CmusikSong& song )
{
	int nRet = GetIDFromFilename( fn );
	song.SetID( nRet );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetFilesize( const CmusikString& fn )
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

bool CmusikLibrary::RemoveSong( int songid )
{
	if ( !m_DatabaseOpen )
		return false;

	int nRes;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRes = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE songid=%d;", NULL, NULL, NULL,
			SONG_TABLE_NAME,
			songid );
	}

	if ( nRes == SQLITE_OK )
		return true;

	return false;
}	

///////////////////////////////////////////////////

bool CmusikLibrary::RemoveSong( const CmusikString& fn )
{
	if ( !m_DatabaseOpen )
		return false;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE filename = %Q;", NULL, NULL, NULL,
			SONG_TABLE_NAME,
			fn.c_str() );
	}

	if ( nRet == SQLITE_OK )
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

bool CmusikLibrary::AddOGG( const CmusikString& fn )
{
	if ( !m_DatabaseOpen )
		return false;

	CmusikOggInfo info;
	if ( info.LoadInfo( fn ) )
	{
		int nRet;
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %d, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
				SONG_TABLE_NAME,								// song table 		
				NULL,											// id
				MUSIK_LIBRARY_FORMAT_OGG,						// format
				1,												// vbr
				fn.c_str(),										// filename
				info.Get()->GetArtist().c_str(),				// artist 
				info.Get()->GetTitle().c_str(),					// title
				info.Get()->GetAlbum().c_str(),					// album
				atoi( info.Get()->GetTrackNum().c_str() ),		// tracknum
				atoi( info.Get()->GetYear().c_str() ),			// year
				info.Get()->GetGenre().c_str(),					// genre
				0,												// rating
				atoi( info.Get()->GetBitrate() ),				// bitrate
				"",												// last played
				"",												// notes
				0,												// times played
				atoi( info.Get()->GetDuration() ),				// duration
				m_TimeAdded.c_str(),							// time added
				GetFilesize( fn ),								// file size
				0,												// dirty
				-1 );											// default equalizer
		}

		if ( nRet == SQLITE_OK )
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
		ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
		{
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
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
				0,												// dirty
				-1 );											// default equalizer
		}

		if ( nRet == SQLITE_OK )
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

	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );

		// see if theres a row
		if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
			result = true;

		// clean up
		sqlite_finalize( pVM, &errmsg );
	}

	sqlite_freemem( query );
	if ( errmsg )
		sqlite_freemem( errmsg );

	return result;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizerIDFromSongID( int id )
{
	if ( !m_DatabaseOpen )
		return -1;

	int target;

	// do it
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec_printf( m_pDB, "SELECT equalizer FROM %Q WHERE songid = %d;", 
			&sqlite_GetEqualizerIDFromID, &target, NULL,
			SONG_TABLE_NAME,
			id );
	}

	return target;
}

///////////////////////////////////////////////////


int CmusikLibrary::GetEqualizerFromName( const CmusikString& name )
{
	if ( !m_DatabaseOpen )
		return -1;

	int target;

	// do it
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		sqlite_exec_printf( m_pDB, "SELECT equalizer_id FROM %Q WHERE equalizer_name = %Q;", 
			&sqlite_GetEqualizerIDFromID, &target, NULL,
			EQUALIZER_PRESET,
			name.c_str() );
	}

	return target;
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateEqualizer( CmusikEQSettings& eq, const CmusikString& name, bool is_preset )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRes;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRes = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f );", NULL, NULL, NULL, 
			EQUALIZER_PRESET,								// eq table 		
			NULL,											// id
			name.c_str(),									// name
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
	}

	eq.m_ID = GetEqualizerFromName( name.c_str() );	

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteEqualizer( int id )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRes;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRes = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE equalizer_id = %d;",
			NULL, NULL, NULL, 
			EQUALIZER_PRESET,	
			id );
	}

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::UpdateEqualizer( int id, const CmusikEQSettings& eq )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRes;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
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
	}

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::UpdateDefaultEqualizer( const CmusikEQSettings& eq )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRes;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{

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
	}

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetDefaultEqualizer( CmusikEQSettings* target )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT hz55_left, hz77_left, hz110_left, hz156_left, hz220_left, hz311_left, hz440_left, hz622_left, hz880_left, hz1244_left, hz1760_left, hz2489_left, hz3520_left, hz4978_left, hz7040_left, hz9956_left, hz14080_left, hz19912_left,"
									" hz55_right, hz77_right, hz110_right, hz156_right, hz220_right, hz311_right, hz440_right, hz622_right, hz880_right, hz1244_right, hz1760_right, hz2489_right, hz3520_right, hz4978_right, hz7040_right, hz9956_right, hz14080_right, hz19912_right"
									" FROM %Q WHERE equalizer_id = -1;", 
									&sqlite_GetEqualizer, target, NULL, 
									EQUALIZER_DEFAULT );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::InitDefaultEqualizer()
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRes;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRes = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( -1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,"
							" 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 );",
				NULL, NULL, NULL,
				EQUALIZER_DEFAULT );
	}

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizer( int eq_id, CmusikEQSettings* target )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT"
									" hz55_left,  hz77_left,  hz110_left,  hz156_left,  hz220_left,  hz311_left,  hz440_left,  hz622_left,  hz880_left,  hz1244_left,  hz1760_left,  hz2489_left,  hz3520_left,  hz4978_left,  hz7040_left,  hz9956_left,  hz14080_left,  hz19912_left,"
									" hz55_right, hz77_right, hz110_right, hz156_right, hz220_right, hz311_right, hz440_right, hz622_right, hz880_right, hz1244_right, hz1760_right, hz2489_right, hz3520_right, hz4978_right, hz7040_right, hz9956_right, hz14080_right, hz19912_right, equalizer_name"
									" FROM %Q WHERE equalizer_id = %d;", 
									&sqlite_GetEqualizer, target, NULL, 
									EQUALIZER_PRESET,
									eq_id );
	}
	
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
		return -1;

	if ( clear_targets )
	{
		target->clear();

		if ( target_ids )
			target_ids->clear();
	}

	int nRet1, nRet2 = 0;
	BeginTransaction();
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet1 = sqlite_exec_printf( m_pDB, "SELECT equalizer_name FROM %Q WHERE equalizer_is_preset = 1 order by equalizer_id;",
			&sqlite_AddRowToStringArray, target, NULL,
			EQUALIZER_PRESET );

		if ( target_ids )
		{
			nRet2 = sqlite_exec_printf( m_pDB, "SELECT equalizer_id FROM %Q WHERE equalizer_is_preset = 1 order by equalizer_id;",
				&sqlite_AddRowToIntArray, target_ids, NULL,
				EQUALIZER_PRESET );	
		}
	}
	EndTransaction();

	if ( nRet1 != 0 )
		return nRet1;

	return nRet2;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFormatFromID( int id, int* target )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT format FROM %Q WHERE songid = %d;", 
			&sqlite_GetIntFromRow, target, NULL,
			SONG_TABLE_NAME,
			id );
	}

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

	return QuerySongs( "dirty = 1", *target );
}

///////////////////////////////////////////////////

int CmusikLibrary::FinalizeDirtySongs()
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET dirty = 0 WHERE dirty = 1",
			NULL, NULL, NULL,
			SONG_TABLE_NAME );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::AddPath( const CmusikString& path )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q );",
			NULL, NULL, NULL,
			MANAGED_PATH,
			path.c_str() );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RemovePath( const CmusikString& path )
{
	if ( !m_DatabaseOpen )
		return -1;

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE path_name = %Q;",
			NULL, NULL, NULL,
			MANAGED_PATH,
			path.c_str() );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllPaths( CmusikStringArray* target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return -1;

	if ( clear_target )
		target->clear();

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "SELECT path_name FROM %Q WHERE path_name <> '';",
			&sqlite_AddRowToStringArray, target, NULL,
			MANAGED_PATH );
	}

	return nRet;
}

///////////////////////////////////////////////////

void CmusikLibrary::ClearLibrary( bool clear_all_tables )
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
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
	}
}

///////////////////////////////////////////////////

int CmusikLibrary::IncLastPlayed( int songid )
{
	if ( !m_DatabaseOpen )
		return -1;

	InitTimeAdded();

	int nRet;
	ACE_Guard<ACE_Thread_Mutex> guard( m_ProtectingLibrary );
	{
		nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET timesplayed = (timesplayed + 1), lastplayed = %Q where songid = %d;",
			NULL, NULL, NULL,
			SONG_TABLE_NAME,
			m_TimeAdded.c_str(),
			songid );
	}

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::SortPlaylist( CmusikPlaylist* playlist, int field, bool ascending )
{
	if ( !m_DatabaseOpen )
		return -1;

	CmusikString sQuery;

	sQuery = "SELECT songid FROM songs where ";

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
		sCurr.Format( "ORDER BY %s", GetSongFieldDB( field ) );
	else
		sCurr.Format( "ORDER BY UPPER(%s)", GetSongFieldDB( field ) );

	sQuery += sCurr;

	if ( !ascending )
		sQuery += " desc";

	sQuery += ";";

	playlist->Clear();

	return RawQuerySongs( sQuery, *playlist );
}

///////////////////////////////////////////////////

