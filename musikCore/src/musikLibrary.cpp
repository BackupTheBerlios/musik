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
// Class(s): 
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

#include "ace/Thread.h"
#include "ace/Synch.h"

///////////////////////////////////////////////////

static int sqlite_AddSongToPlaylist(void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when
	// adding songs to a CmusikPlaylist

	CmusikPlaylist* p = (CmusikPlaylist*)args;

	CmusikSong *pLibItem = new CmusikSong();
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

    p->m_Name = results[36];

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
	// getting CmusikSongInfo info

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

static int sqlite_AddRowToIntArray( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when 
	// adding playlist names to a CIntArray	

	CIntArray* p = (CIntArray*)args;
	p->push_back( atoi( results[0] ) ); 

    return 0;
}
///////////////////////////////////////////////////

static int sqlite_AddStdPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when 
	// adding std playlist info to an array	

	CmusikPlaylistInfoArray* p = (CmusikPlaylistInfoArray*)args;
	
	CmusikPlaylistInfo info ( results[0], MUSIK_PLAYLIST_TYPE_STANDARD, atoi( results[1] ) );
	p->push_back( info ); 

    return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddDynPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	// this is a callback for sqlite to use when 
	// adding std playlist info to an array	

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

CmusikLibrary::CmusikLibrary( const CStdString& filename )
{
	m_pDB = NULL;
	m_Transactions = NULL;

	m_ProtectingLibrary = new ACE_Thread_Mutex();
	CmusikSong::SetLibrary( this );
	m_Filename = filename;

	InitFields();

	Startup();
}

///////////////////////////////////////////////////

CmusikLibrary::~CmusikLibrary()
{
	Shutdown();
	delete m_ProtectingLibrary;
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

int CmusikLibrary::GetSongFieldID( CStdString field )
{
	for ( size_t i = 0; i < m_Fields.size(); i++ )
	{
		if ( field == m_Fields.at( i ) )
			return i;
	}
	return -1;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFieldDBID( CStdString field )
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
	if ( !m_pDB )
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
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

	sqlite_exec( m_pDB, szCreateDBQuery1, NULL, NULL, NULL );
	sqlite_exec( m_pDB, szCreateDBQuery2, NULL, NULL, NULL );

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

	m_ProtectingLibrary->release();

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::InitEqTable()
{
	if ( !m_pDB )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " EQUALIZER_PRESET " ( "	
		"equalizer_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"equalizer_name VARCHAR(255), "
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
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

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

bool CmusikLibrary::InitPathTable()
{
	if ( !m_pDB )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " MANAGED_PATH " ( "	
		"path_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"path_name varchar(1024) );";

	// put a lock on the library and open it up
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

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

bool CmusikLibrary::InitDynTable()
{
	if ( !m_pDB )
		return false;

	bool error = false;

	// construct the table that contains a list of
	// all the standard playlist names
	static const char *szCreateDBQuery  = 
		"CREATE TABLE " DYN_PLAYLIST_TABLE_NAME " ( "	
		"dyn_playlist_id INTEGER AUTO_INCREMENT PRIMARY KEY, "
		"dyn_playlist_name varchar(255), "
		"dyn_playlist_query varchar(1024) "
		" );";

	// put a lock on the library and open it up
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

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

bool CmusikLibrary::InitCrossfaderTable()
{
	if ( !m_pDB )
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
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

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

bool CmusikLibrary::InitLibTable()
{
	if ( !m_pDB )
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
	m_ProtectingLibrary->acquire();

	char *pErr = NULL;

	sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, NULL );
	sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, NULL );

	if ( pErr )
	{
		error = true;
		sqlite_freemem( pErr );
	}

	m_ProtectingLibrary->release();

	return error;
}

///////////////////////////////////////////////////

bool CmusikLibrary::Startup()
{
   bool error = false;

   if ( m_pDB )
	   Shutdown();

   char* pErr = NULL;
   m_pDB = sqlite_open( m_Filename.c_str(), 0666, &pErr );

   if ( m_pDB && !pErr )
   {
		if ( !InitLibTable() )
		{
			error = true;
			TRACE0( "Library table creation failed.\n" );
		}

		if ( !InitStdTables() )
		{
			TRACE0( "Standard playlist table creation failed.\n" );		
			error = true;  
		}

		if ( !InitDynTable() )
		{
			error = true;
			TRACE0( "Dynamic playlist table creation failed.\n" );
		}

		if ( !InitEqTable() )
		{
			error = true;
			TRACE0( "Equalizer preset table creation failed.\n" );
		}

		if ( !InitPathTable() )
		{
			TRACE0( "Managed paths table creation failed.\n" );		
			error = true;
		}
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
	m_ProtectingLibrary->acquire();
	if ( m_pDB )
	{
		sqlite_close( m_pDB );
		m_pDB = NULL;
	}	
	m_ProtectingLibrary->release();
}

///////////////////////////////////////////////////

void CmusikLibrary::BeginTransaction()
{
	if ( !m_pDB )
		return;

	m_ProtectingLibrary->acquire();

	if ( m_Transactions == NULL )
		sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
	++m_Transactions;
	
	m_ProtectingLibrary->release();

	TRACE0( "Begin transaction\n" );
}

///////////////////////////////////////////////////

void CmusikLibrary::EndTransaction()
{
	if ( !m_pDB )
		return;

	m_ProtectingLibrary->acquire();

	--m_Transactions;
	if ( m_Transactions == NULL )
		sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );

	m_ProtectingLibrary->release();

	TRACE0( "End transaction\n" );
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateCrossfader( CmusikCrossfader* fader )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q,%f,%f,%f,%f );", 
		NULL, NULL, NULL,
		CROSSFADER_PRESET,
		NULL,
		fader->GetDuration( MUSIK_CROSSFADER_NEW_SONG ),
		fader->GetDuration( MUSIK_CROSSFADER_PAUSE_RESUME ),
		fader->GetDuration( MUSIK_CROSSFADER_STOP ),
		fader->GetDuration( MUSIK_CROSSFADER_EXIT ) );
	
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteCrossfader( int id )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE crossfader_id = %d", 
		NULL, NULL, NULL,
		CROSSFADER_PRESET,
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateStdPlaylist( const CStdString& name, const CStdStringArray& songids )
{
	if ( !m_pDB )
		return -1;

	int nID, nRet;

	// lock it up
	m_ProtectingLibrary->acquire();

	// insert the new playlist name
	nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q );", 
		NULL, NULL, NULL, 
		STD_PLAYLIST_TABLE_NAME,
		NULL,
		name.c_str() );

	if ( nRet != SQLITE_OK )
	{
		m_ProtectingLibrary->release();
		return nRet;
	}

	// get the ID of the newly created entry
	nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
		&sqlite_GetPlaylistID, &nID, NULL,
		STD_PLAYLIST_TABLE_NAME,
		name.c_str() );
	
	if ( nRet != SQLITE_OK )
	{
		m_ProtectingLibrary->release();
		return nRet;
	}

	// insert songs into playlist
	if ( nID >= 0 )
	{
		BeginTransaction();
		for ( size_t i = 0; i < songids.size(); i++ )
		{
			AddSong( songids.at( i ) );

			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
				NULL, NULL, NULL, 
				STD_PLAYLIST_SONGS,
				NULL,
				nID,
				GetIDFromFilename( songids.at( i ) ) );

			if ( nRet != SQLITE_OK )
			{
				EndTransaction();
				m_ProtectingLibrary->release();
				return nRet;
			}
		}
		EndTransaction();
	}	

	// release the mutex lock
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::AppendStdPlaylist( int id, const CStdStringArray& files )
{
	if ( !m_pDB ) 
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		m_ProtectingLibrary->acquire();

		BeginTransaction();	
		for ( size_t i = 0; i < files.size(); i++ )
		{
			AddSong( files.at( i ) );

			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
			NULL, NULL, NULL, 
			STD_PLAYLIST_SONGS,
			NULL,
			id,
			GetIDFromFilename( files.at( i ) ) );

			if ( nRet != SQLITE_OK )
			{
				EndTransaction();
				m_ProtectingLibrary->release();
				return nRet;
			}
		}
		EndTransaction();

		m_ProtectingLibrary->release();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RewriteStdPlaylist( int id, CmusikPlaylist* playlist )
{
	if ( !m_pDB )
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		m_ProtectingLibrary->acquire();

		nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;",
		NULL, NULL, NULL, 
		STD_PLAYLIST_SONGS,
		id );

		BeginTransaction();
		
		for ( size_t i = 0; i < playlist->GetCount(); i++ )
		{
			nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d );",
			NULL, NULL, NULL, 
			STD_PLAYLIST_SONGS,
			NULL,
			id,
			playlist->GetSongID( i ) );

			if ( nRet != SQLITE_OK )
			{
				EndTransaction();
				m_ProtectingLibrary->release();
				return nRet;
			}
		}

		EndTransaction();

		m_ProtectingLibrary->release();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::RenameStdPlaylist( int id, const CStdString& str )
{
	if ( !m_pDB )
		return -1;

	int nRet;

	if ( id >= 0 )
	{
		m_ProtectingLibrary->acquire();

		nRet = sqlite_exec_printf( m_pDB, "UPDATE %Q SET std_playlist_name = %Q WHERE std_playlist_id = %d;",
		NULL, NULL, NULL, 
		STD_PLAYLIST_TABLE_NAME,
		str.c_str(),
		id );

		m_ProtectingLibrary->release();
	}
	else
		return SQLITE_ERROR;

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetStdPlaylist( int id, CmusikPlaylist& target, bool clear_target )
{
	if ( !m_pDB )
		return -1;

	if ( clear_target )
		target.Clear();

	// do it
	m_ProtectingLibrary->acquire();
	int nRet = sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q WHERE std_playlist_id = %d;", 
		&sqlite_AddSongToPlaylist, &target, NULL,
		STD_PLAYLIST_SONGS,
		id );
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

bool CmusikLibrary::GetStdPlaylistFns( int id, CStdStringArray& target, bool clear_target )
{
	CmusikPlaylist ids;
	GetStdPlaylist( id, ids, false );

	if ( !ids.GetCount() )
		return false;

	if ( clear_target )
		target.clear();

	// do it
	m_ProtectingLibrary->acquire();
	BeginTransaction();

	for ( size_t i = 0; i < ids.GetCount(); i++ )
		target.push_back( ids.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ) );

	EndTransaction();
	m_ProtectingLibrary->release();

	return ( target.size() > 0 ? true : false );
}

///////////////////////////////////////////////////

bool CmusikLibrary::GetStdPlaylistFns( CmusikPlaylist& playlist, CStdStringArray& target, bool clear_target )
{
	if ( !playlist.GetCount() )
		return false;

	if ( clear_target )
		target.clear();

	// do it
	m_ProtectingLibrary->acquire();
	BeginTransaction();

	for ( size_t i = 0; i < playlist.GetCount(); i++ )
		target.push_back( playlist.GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ) );

	EndTransaction();
	m_ProtectingLibrary->release();

	return ( target.size() > 0 ? true : false );
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateDynPlaylist( const CStdString& name, const CStdString& query )
{
	if ( !m_pDB )
		return -1;

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %q, %q ); ", 
		NULL, NULL, NULL,
		DYN_PLAYLIST_TABLE_NAME,
		NULL,
		name.c_str(),
		query.c_str() );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteStdPlaylist( const CStdString& name )
{
	if ( !m_pDB )
		return -1;

	int nID, nRet;

	// lock it up	
	m_ProtectingLibrary->acquire();

	// get ID of the currently named playlist
	nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_id FROM %Q WHERE std_playlist_name = %Q;", 
		&sqlite_GetPlaylistID, &nID, NULL,
		STD_PLAYLIST_TABLE_NAME, 
		name.c_str() );
	
	if ( nRet != SQLITE_OK )
	{
		m_ProtectingLibrary->release();
		return nRet;
	}

	// remove entry from table containing
	// the list of standard playlists
	nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_name = %Q;", 
		NULL, NULL, NULL,
		STD_PLAYLIST_TABLE_NAME,
		name.c_str() );

	if ( nRet != SQLITE_OK )
	{
		m_ProtectingLibrary->release();
		return nRet;
	}

	// delete corresponding songs from the
	// other table
	nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d", 
		NULL, NULL, NULL,
		STD_PLAYLIST_SONGS,
		nID );

	// release mutex lock
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteStdPlaylist( int id )
{
	if ( !m_pDB )
		return -1;

	int nRet;

	m_ProtectingLibrary->acquire();

	// remove entry from table containing
	// the list of standard playlists
	nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;", 
		NULL, NULL, NULL,
		STD_PLAYLIST_TABLE_NAME,
		id );

	if ( nRet != SQLITE_OK )
	{
		m_ProtectingLibrary->release();
		return nRet;
	}

	// now remove corresponding entries in
	// the table that contains the songs
	nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE std_playlist_id = %d;", 
		NULL, NULL, NULL,
		STD_PLAYLIST_SONGS,
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteDynPlaylist( const CStdString& name )
{
	if ( !m_pDB )
		return -1;

	int nRet;

	// do it
	m_ProtectingLibrary->acquire();

	nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_name = %Q;", 
		NULL, NULL, NULL,
		DYN_PLAYLIST_TABLE_NAME,
		name.c_str() );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteDynPlaylist( int id )
{
	if ( !m_pDB )
		return -1;

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE dyn_playlist_id = %d;", 
		NULL, NULL, NULL,
		DYN_PLAYLIST_TABLE_NAME,
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

CStdString CmusikLibrary::GetOrder( int type, bool terminate )
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

int CmusikLibrary::QueryCount( const char* pQueryResult )
{
	if ( !m_pDB )
		return -1;

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

void CmusikLibrary::VerifyYearList( CStdStringArray & list )
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

int CmusikLibrary::GetAllSongs( CmusikPlaylist& target )
{
	return QuerySongs( "filename <> ''", target );
}

///////////////////////////////////////////////////

int CmusikLibrary::QuerySongs( const CStdString& query, CmusikPlaylist& target )
{
	if ( !m_pDB )
		return -1;

	target.Clear();

	// lock it up and query it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q where %q;", 
		&sqlite_AddSongToPlaylist, &target, NULL,
		SONG_TABLE_NAME, 
		query.c_str() );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedItems( int source_type, const CStdStringArray& source_items, int target_type, CStdStringArray& target )
{
	if ( !m_pDB )
		return -1;

	target.clear();

	if ( source_type == -1 || target_type == -1 )
		return -1;

	const CStdString sInType = GetSongFieldDB( source_type );
	const CStdString sOutType = GetSongFieldDB( target_type );

	// construct the query
	CStdString query;
	query.Format( "SELECT DISTINCT %s, UPPER( %s ) AS UP FROM %s where ", 
		sOutType.c_str(), 
		sOutType.c_str(), 
		SONG_TABLE_NAME );

	CStdString sCurrentItem;
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
	m_ProtectingLibrary->acquire();
	int nRet = sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);
	m_ProtectingLibrary->release();

	// if target is years, verify only years
	//get displayed.
	if ( target_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedItems( CStdString sub_query, int dst_type, CStdStringArray& target )
{
	target.clear();

	CStdString sOutType = GetSongFieldDB( dst_type );

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %Q WHERE %s order by %q;", 
		&sqlite_AddSongToStringArray, &target, NULL,
		sOutType.c_str(), 
		sOutType.c_str(), 
		SONG_TABLE_NAME,
		sub_query.c_str(),
		sOutType.c_str() );

	m_ProtectingLibrary->release();

	// if target is years, verify only years
	// get displayed.
	if ( dst_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetRelatedSongs( CStdString sub_query, int source_type, CmusikPlaylist& target )
{
	if ( !m_pDB )
		return -1;

	target.Clear();

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf(m_pDB, "SELECT DISTINCT songid FROM %Q WHERE %s %q", 
		&sqlite_AddSongToPlaylist, &target, NULL,
		SONG_TABLE_NAME,
		sub_query.c_str(),
		GetOrder( source_type ).c_str() );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllDistinct( int source_type, CStdStringArray& target, bool clear_target )
{
	if ( !m_pDB )
		return -1;

	if ( clear_target )
		target.clear();

	CStdString sField = GetSongFieldDB( source_type );

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT DISTINCT %q, UPPER( %q ) AS UP FROM %Q ORDER BY UP;", 
		&sqlite_AddSongToStringArray, &target, NULL,
		sField.c_str(), 
		sField.c_str(),
		SONG_TABLE_NAME );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongCount()
{
	if ( !m_pDB )
		return -1;

	char *query = sqlite_mprintf( "SELECT COUNT(*) FROM " SONG_TABLE_NAME " ;" );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetFieldFromID( int id, int field, CStdString& string )
{
	if ( !m_pDB )
		return -1;

	CStdString type = GetSongFieldDB( field );

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT %q FROM %Q WHERE songid = %d;", 
		&sqlite_GetSongFieldFromID, &string, NULL,
		type.c_str(),
		SONG_TABLE_NAME, 
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongInfoFromID( int id, CmusikSongInfo* info )
{
	if ( !m_pDB )
		return -1;

	info->SetID( id );

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize,filename,equalizer FROM %Q WHERE songid = %d;", 
		&sqlite_GetSongInfoFromID, info, NULL,
		SONG_TABLE_NAME,
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

bool CmusikLibrary::SetSongInfo( CmusikSongInfo* info, int songid )
{
	if ( !m_pDB )
		return false;

	int result = 0;

	if ( songid == -1 )
		songid = info->GetID();

	// lock it up and run the query
	m_ProtectingLibrary->acquire();

	result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET format = %d, vbr = %d, filename = %Q, artist = %Q, title = %Q, album = %Q, tracknum = %d, year = %Q, genre = %Q, rating = %d, bitrate = %d, lastplayed = %Q, notes = %Q, timesplayed = %d, duration = %d, timeadded = %Q, filesize = %d, dirty = %d WHERE songid = %d;",
		NULL, NULL, NULL,
		SONG_TABLE_NAME,
		atoi( info->GetFormat().c_str() ),
		atoi( info->GetVBR().c_str() ),
		info->GetFilename().c_str(),
		info->GetArtist().c_str(),
		info->GetTitle().c_str(),
		info->GetAlbum().c_str(),
		atoi( info->GetTrackNum().c_str() ),
		info->GetYear().c_str(),
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

	m_ProtectingLibrary->release();

	if ( result != SQLITE_OK )
		return false;
	
	return true;    
}

///////////////////////////////////////////////////

bool CmusikLibrary::SetSongRating( int songid, int rating )
{
	if ( !m_pDB )
		return false;

	int result = 0;

	// lock it up and run the query
	m_ProtectingLibrary->acquire();
    
	result = sqlite_exec_printf( m_pDB, "UPDATE %Q SET rating = %d WHERE songid = %d", 
		NULL, NULL, NULL,
		SONG_TABLE_NAME,
		rating,
		songid );
	
	m_ProtectingLibrary->release();

	if ( result != SQLITE_OK )
		return false;
	
	return true;    
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllStdPlaylists( CmusikPlaylistInfoArray* target, bool clear_target )
{
	if ( !m_pDB )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT std_playlist_name,std_playlist_id FROM %Q WHERE std_playlist_name <> '';", 
		&sqlite_AddStdPlaylistInfoArray, target, NULL,
		STD_PLAYLIST_TABLE_NAME );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllDynPlaylists( CmusikPlaylistInfoArray* target, bool clear_target )
{
	if ( !m_pDB )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT dyn_playlist_name,dyn_playlist_id FROM %Q WHERE dyn_playlist_name <> '';",
		&sqlite_AddDynPlaylistInfoArray, target, NULL,
		DYN_PLAYLIST_TABLE_NAME );
	
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllCrossfaders( CStdStringArray* target, bool clear_target )
{
	if ( !m_pDB )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	m_ProtectingLibrary->acquire();
	
	int nRet = sqlite_exec_printf( m_pDB, "SELECT crossfader_name FROM %Q WHERE crossfader_name <> '';", 
		&sqlite_AddRowToStringArray, target, NULL,
		CROSSFADER_PRESET );
	
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetAllCrossfaders( CIntArray* target, bool clear_target )
{
	if ( !m_pDB )
		return -1;

	if ( clear_target )
		target->clear();

	// do it
	m_ProtectingLibrary->acquire();
	
	int nRet = sqlite_exec_printf( m_pDB, "SELECT crossfader_id FROM %Q WHERE crossfader_id > -1;", 
		&sqlite_AddRowToIntArray, target, NULL,
		CROSSFADER_PRESET );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetCrossfader( int id, CmusikCrossfader* fader )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT crossfader_name, newsong, pause_resume, seek, stop, exit FROM %Q WHERE crossfader_id = %d;", 
		&sqlite_GetCrossfader, fader, NULL,
		CROSSFADER_PRESET,
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetIDFromFilename( CStdString fn )
{
	if ( !m_pDB )
		return -1;

	int target;

	// do it
	m_ProtectingLibrary->acquire();

	sqlite_exec_printf( m_pDB, "SELECT songid FROM %Q WHERE filename = %Q;", 
		&sqlite_GetIDFromFilename, &target, NULL,
		SONG_TABLE_NAME,
		fn.c_str() );

	m_ProtectingLibrary->release();

	return target;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFromFilename( CStdString fn, CmusikSong& song )
{
	int nRet = GetIDFromFilename( fn );
	song.SetID( nRet );

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetFilesize( const CStdString& fn )
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
	if ( !m_pDB )
		return false;

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

bool CmusikLibrary::RemoveSong( const CStdString& fn )
{
	if ( !m_pDB )
		return false;

	m_ProtectingLibrary->acquire();

	int result = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE filename = %Q;", NULL, NULL, NULL,
		SONG_TABLE_NAME,
		fn.c_str() );

	m_ProtectingLibrary->release();

	if ( result == SQLITE_OK )
		return true;

	return false;
}	

///////////////////////////////////////////////////

bool CmusikLibrary::AddSong( const CStdString& fn )
{
	bool result = true;

	if ( !IsSongInLibrary( fn ) )
	{
		CmusikFilename MFN( fn );
		CStdString sExt = MFN.GetExtension();

		if ( sExt == "mp3" )
			result = AddMP3( fn );
		else if ( sExt == "ogg" )
			result = AddOGG( fn );
	}

	return result;
}

///////////////////////////////////////////////////

bool CmusikLibrary::AddOGG( const CStdString& fn )
{
	if ( !m_pDB )
		return false;

	CmusikOggInfo info;
	if ( info.LoadInfo( fn ) )
	{
		m_ProtectingLibrary->acquire();

		int result = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
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
			0,												// dirty
			-1 );											// default equalizer

		m_ProtectingLibrary->release();

		if ( result == SQLITE_OK )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool CmusikLibrary::AddMP3( const CStdString& fn )
{
	if ( !m_pDB )
		return false;

	CmusikMp3Info info;
	if ( info.LoadInfo( fn ) )
	{
		m_ProtectingLibrary->acquire();

		int result = sqlite_exec_printf( m_pDB, "INSERT INTO %q VALUES ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d, %d );", NULL, NULL, NULL, 
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

		m_ProtectingLibrary->release();

		if ( result == SQLITE_OK )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool CmusikLibrary::IsSongInLibrary( CStdString fn )
{
	if ( !m_pDB )
		return false;


	bool result = false;

	char *query = sqlite_mprintf( "SELECT filename FROM %Q WHERE filename = %Q;", 
		SONG_TABLE_NAME,
		fn.c_str() );
	
	m_ProtectingLibrary->acquire();

	// run query
	const char *pTail;
	sqlite_vm *pVM;
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	// see if theres a row
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = true;

	// clean up
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );
	if ( errmsg )
		sqlite_freemem( errmsg );

	m_ProtectingLibrary->release();

	return result;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizerIDFromSongID( int id )
{
	if ( !m_pDB )
		return -1;

	int target;

	// do it
	m_ProtectingLibrary->acquire();

	sqlite_exec_printf( m_pDB, "SELECT equalizer FROM %Q WHERE songid = %d;", 
		&sqlite_GetEqualizerIDFromID, &target, NULL,
		SONG_TABLE_NAME,
		id );

	m_ProtectingLibrary->release();

	return target;
}

///////////////////////////////////////////////////

int CmusikLibrary::CreateEqualizer( const CmusikEQSettings& eq, const CStdString& name )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRes = sqlite_exec_printf( m_pDB, "INSERT INTO %Q VALUES ( %Q, %Q, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, );", NULL, NULL, NULL, 
		CROSSFADER_PRESET,								// song table 		
		NULL,											// id
		name.c_str(),									// name
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

	m_ProtectingLibrary->release();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::DeleteEqualizer( int id )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRes = sqlite_exec_printf( m_pDB, "DELETE FROM %Q WHERE equalizer_id = %d;",
		NULL, NULL, NULL, 
		CROSSFADER_PRESET,	
		id );

	m_ProtectingLibrary->release();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::UpdateEqualizer( int id, const CmusikEQSettings& eq )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRes = sqlite_exec_printf( m_pDB, "UPDATE %Q set equalizer_name = %Q, hz55_left = %d, hz77_left = %d, hz110_left = %d, hz156_left = %d, hz220_left = %d, hz311_left = %d, hz440_left = %d, hz622_left = %d, hz880_left = %d, hz1244_left = %d, hz1760_left = %d, hz2489_left = %d, hz3520_left = %d, hz4978_left = %d, hz7040_left = %d, hz9956_left = %d, hz14080_left = %d, hz19912_left = %d,"
						" hz55_right = %d, hz77_right = %d, hz110_right = %d, hz156_right = %d, hz220_right = %d, hz311_right = %d, hz440_right = %d, hz622_right = %d, hz880_right = %d, hz1244_right = %d, hz1760_right = %d, hz2489_right = %d, hz3520_right = %d, hz4978_right = %d, hz7040_right = %d, hz9956_right = %d, hz14080_right = %d, hz19912_right = %d WHERE equalizer_id = %d;",
			NULL, NULL, NULL,
			CROSSFADER_PRESET,
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
			eq.m_Right[17] );

	m_ProtectingLibrary->release();

	return nRes;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetEqualizer( int eq_id, CmusikEQSettings* target )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT hz55_left, hz77_left, hz110_left, hz156_left, hz220_left, hz311_left, hz440_left, hz622_left, hz880_left, hz1244_left, hz1760_left, hz2489_left, hz3520_left, hz4978_left, hz7040_left, hz9956_left, hz14080_left, hz19912_left,"
								" hz55_right, hz77_right, hz110_right, hz156_right, hz220_right, hz311_right, hz440_right, hz622_right, hz880_right, hz1244_right, hz1760_right, hz2489_right, hz3520_right, hz4978_right, hz7040_right, hz9956_right, hz14080_right hz19912_right, equalizer_name"
								" WHERE equalizer_id = %d;", &sqlite_GetEqualizer, &target, NULL, eq_id );
	
	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////

int CmusikLibrary::GetSongFormatFromID( int id, int* target )
{
	if ( !m_pDB )
		return -1;

	m_ProtectingLibrary->acquire();

	int nRet = sqlite_exec_printf( m_pDB, "SELECT format FROM %Q WHERE songid = %d;", 
		&sqlite_GetIDFromFilename, target, NULL,
		SONG_TABLE_NAME,
		id );

	m_ProtectingLibrary->release();

	return nRet;
}

///////////////////////////////////////////////////