#include "stdafx.h"
#include "../include/MusikLibrary.h"
#include "time.h"

//-------------------------------------------------//
//--- this is a callback for sqlite to use when ---//
//--- adding songs to a CMusikPlaylist			---//
//-------------------------------------------------//
static int sqlite_AddSongToPlaylist(void *args, int numCols, char **results, char ** columnNames )
{
	CMusikPlaylist* p = (CMusikPlaylist*)args;

	CMusikSong *pLibItem = new CMusikSong();
	pLibItem->SetID( atoi( results[0] ) );
	
	p->push_back( *pLibItem );

    return 0;
}

//-------------------------------------------------//
//--- this is a callback for sqlite to use when ---//
//--- finding a song's field					---//
//-------------------------------------------------//
static int sqlite_GetSongFieldFromID( void *args, int numCols, char **results, char ** columnNames )
{
	CStdString* p = (CStdString*)args;
	*p = CStdString( results[0] ); 

    return 0;
}

//-------------------------------------------------//
//--- this is a callback for sqlite to use when ---//
//--- getting CMusikSongInfo info				---//
//-------------------------------------------------//
static int sqlite_GetSongInfoFromID( void *args, int numCols, char **results, char ** columnNames )
{
	CMusikSongInfo* p = (CMusikSongInfo*)args;

	p->m_TrackNum		= atoi			( results[0] );
	p->m_Artist			= CStdString	( results[1] );
	p->m_Album			= CStdString	( results[2] );
	p->m_Genre			= CStdString	( results[3] );
	p->m_Title			= CStdString	( results[4] );
	p->m_Duration		= atoi			( results[5] );
	p->m_Format			= atoi			( results[6] );
	p->m_VBR			= atoi			( results[7] );
	p->m_Year			= CStdString	( results[8] );
	p->m_Rating			= atoi			( results[9] );
	p->m_Bitrate		= atoi			( results[10] );
	p->m_LastPlayed		= CStdString	( results[11] );
	p->m_Notes			= CStdString	( results[12] );
	p->m_TimesPlayed	= atoi			( results[13] );
	p->m_TimeAdded		= CStdString	( results[14] );
	p->m_Filesize		= atoi			( results[15] );
	p->m_Filename		= CStdString	( results[16] );

    return 0;
}

//-------------------------------------------------//
//--- this is a callback for sqlite to use when ---//
//--- adding items (fields) to a wxArrayString	---//
//-------------------------------------------------//
static int sqlite_AddSongToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	CStdStringArray* p = (CStdStringArray*)args;
	p->push_back( results[0] ); 

    return 0;
}

CMusikLibrary::CMusikLibrary( const CStdString& filename )
{
	CMusikSong::SetLibrary( this );
	m_Filename = filename;
	InitFields();
	Startup();
}

CMusikLibrary::~CMusikLibrary()
{
	Shutdown();
}

void CMusikLibrary::InitTimeAdded()
{
	time_t curr_time;
	time( &curr_time );

	tm *ptm = NULL;
	ptm = localtime( &curr_time );

	m_TimeAdded.Format( "%02d:%02d:%02d %02d/%02d/%d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_mday, ptm->tm_mon, ptm->tm_year + 1900);
}

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

int CMusikLibrary::GetSongFieldID( CStdString field )
{
	for ( size_t i = 0; i < m_Fields.size(); i++ )
	{
		if ( field == m_Fields.at( i ) )
			return i;
	}
	return -1;
}

int CMusikLibrary::GetSongFieldDBID( CStdString field )
{
	for ( size_t i = 0; i < m_FieldsDB.size(); i++ )
	{
		if ( field == m_FieldsDB.at( i ) )
			return i;
	}
	return -1;
}


bool CMusikLibrary::Startup()
{
 	//-----------------------------------------------------//
	//--- construct the table							---//
	//-----------------------------------------------------//
	static const char *szCreateDBQuery  = 
		"CREATE TABLE songs ( "	
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

	//-----------------------------------------------------//
	//--- construct the index							---//
	//-----------------------------------------------------//
	const char* szCreateIdxQuery =
		"CREATE INDEX songs_title_idx on songs (title);"
		"CREATE UNIQUE INDEX songs_filename_idx on songs (filename);"
		"CREATE INDEX songs_artist_idx on songs (artist);"
		"CREATE INDEX songs_album_idx on songs (album);"
		"CREATE INDEX songs_genre_idx on songs (genre);"
		"CREATE INDEX songs_artist_album_tracknum_idx on songs (artist,album,tracknum);";

	//-----------------------------------------------------//
	//--- put a lock on the library and open it up		---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	char *pErr = NULL;
	m_pDB = sqlite_open( m_Filename.c_str(), 0666, &pErr );

	if ( m_pDB )
	{
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, NULL );
	}

	if ( pErr )
		sqlite_freemem( pErr );

	return ( m_pDB != NULL );   
}

void CMusikLibrary::Shutdown()
{
	//-----------------------------------------------------//
	//--- lock it up and close it down.					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	if ( m_pDB )
	{
		sqlite_close( m_pDB );
		m_pDB = NULL;
	}	
}

void CMusikLibrary::BeginTransaction()
{
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
}

void CMusikLibrary::EndTransaction()
{
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );
}

void CMusikLibrary::CreateStdPlaylist( CStdString name, CIntArray songids )
{
	//-----------------------------------------------------//
	//--- lock it up									---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
}

void CMusikLibrary::CreateDynPlaylist( CStdString name, CStdString query )
{
	//-----------------------------------------------------//
	//--- lock it up									---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );	
}

void CMusikLibrary::DeleteStdPlaylist( CStdString name )
{
	//-----------------------------------------------------//
	//--- lock it up and close it down.					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
}

void CMusikLibrary::DeleteDynPlaylist( CStdString name )
{
	//-----------------------------------------------------//
	//--- lock it up and close it down.					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
}

CStdString CMusikLibrary::GetOrder( int type, bool terminate )
{
	CStdString sTerminate = _T( "" );
	if ( terminate )
		sTerminate = _T( ";" );

	switch( type )
	{
	case MUSIK_LIBRARY_TYPE_ARTIST:
		sTerminate.Format( _T( "order by artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_ALBUM:
		sTerminate.Format( _T( "order by album,tracknum,artist,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_YEAR:
		sTerminate.Format( _T( "order by year,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_GENRE:
		sTerminate.Format( _T( "order by genre,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TITLE:
		sTerminate.Format( _T( "order by title,artist,album,tracknum%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TRACKNUM:
		sTerminate.Format( _T( "order by tracknum,artist,album,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TIMEADDED:
		sTerminate.Format( _T( "order by timeadded,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_LASTPLAYED:
		sTerminate.Format( _T( "order by lastplayed,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FILESIZE:
		sTerminate.Format( _T( "order by filesize,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FORMAT:
		sTerminate.Format( _T( "order by format,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_DURATION:
		sTerminate.Format( _T( "order by duration,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_RATING:
		sTerminate.Format( _T( "order by rating,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
		sTerminate.Format( _T( "order by timesplayed,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_BITRATE:
		sTerminate.Format( _T( "order by bitrate,artist,album,tracknum,title%s" ), sTerminate.c_str() );
		return sTerminate;
		break;
	case MUSIK_LIBRARY_TYPE_FILENAME:
		break;
	}

	return sTerminate;
}

int CMusikLibrary::QueryCount( const char* pQueryResult )
{
	const char *pTail;
	sqlite_vm *pVM;

	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	
	sqlite_compile( m_pDB, pQueryResult, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	int result = 0;
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = atoi( coldata[0] );

	sqlite_finalize( pVM, &errmsg );
	return result;
}

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

void CMusikLibrary::GetAllSongs( CMusikPlaylist& target )
{
	QuerySongs( _T( "filename <> ''" ), target );
}

void CMusikLibrary::QuerySongs( const CStdString& query, CMusikPlaylist& target )
{
	target.clear();

	CStdString queryWhere( _T( "select songid from songs where " ) );
	queryWhere += query;
	queryWhere += _T( ";" );

	//-------------------------------------------------//
	//--- lock it up and query it					---//
	//-------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec( m_pDB, queryWhere.c_str(), &sqlite_AddSongToPlaylist, &target, NULL );
}

void CMusikLibrary::GetRelatedItems( int source_type, const CStdStringArray& source_items, int target_type, CStdStringArray& target )
{
	target.clear();

	if ( source_type == -1 || target_type == -1 )
		return;

	const CStdString sInType = GetSongFieldDB( source_type );
	const CStdString sOutType = GetSongFieldDB( target_type );

	//-----------------------------------------------------//
	//--- construct the query							---//
	//-----------------------------------------------------//
	CStdString query;
	query.Format( _T( "select distinct %s,UPPER(%s) as UP from songs where " ), sOutType.c_str(), sOutType.c_str() );

	CStdString sCurrentItem;
	for ( size_t i = 0; i < source_items.size(); i++ )
	{
		sCurrentItem = source_items.at( i );
		sCurrentItem.Replace( _T("'"), _T("''") );

		if ( i > 0 )
			query += _T("or ");

        query.Format( _T( "%s%s = '%s' " ), query.c_str(), sInType.c_str(), sCurrentItem.c_str() );
	}

	//-----------------------------------------------------//
	//--- get sorting order								---//
	//-----------------------------------------------------//
	query += GetOrder( target_type );

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);

	//-----------------------------------------------------//
	//--- if target is years, verify only years			---//
	//--- get displayed.								---//
	//-----------------------------------------------------//
	if ( target_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );
}

void CMusikLibrary::GetRelatedItems( CStdString sub_query, int dst_type, CStdStringArray& target )
{
	target.clear();
	CStdString sOutType = GetSongFieldDB( dst_type );

	CStdString query;
	query.Format( _T( "select distinct %s,UPPER(%s) as UP from songs where %s;" ), 
		sOutType.c_str(), 
		sOutType.c_str(), 
		sub_query.c_str() );

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec(m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL);

	//-----------------------------------------------------//
	//--- if target is years, verify only years			---//
	//--- get displayed.								---//
	//-----------------------------------------------------//
	if ( dst_type == MUSIK_LIBRARY_TYPE_YEAR )
		VerifyYearList( target );
}

void CMusikLibrary::GetAllDistinct( int source_type, CStdStringArray& target, bool clear_target )
{
	if ( clear_target )
		target.clear();

	CStdString query;
	CStdString sField = GetSongFieldDB( source_type );
	query.Format( _T( "select distinct %s,UPPER(%s) as UP from songs order by UP;" ), sField.c_str(), sField.c_str() );

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec( m_pDB, query.c_str(), &sqlite_AddSongToStringArray, &target, NULL );
}

int CMusikLibrary::GetSongCount()
{
	char *query = sqlite_mprintf( "select count(*) from songs;" );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

void CMusikLibrary::GetFieldFromID( int id, int field, CStdString& string )
{
	CStdString query;
	CStdString type = GetSongFieldDB( field );

	query.Format( _T( "select %s from songs where songid = %d;" ), type.c_str(), id );

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	sqlite_exec( m_pDB, query.c_str(), &sqlite_GetSongFieldFromID, &string, NULL );
}

void CMusikLibrary::GetSongInfoFromID( int id, CMusikSongInfo* info )
{
	CStdString query;

	query.Format( _T( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where songid = %d;" ), id );
	info->SetID( id );

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
	int result = sqlite_exec( m_pDB, query.c_str(), &sqlite_GetSongInfoFromID, info, NULL );
}

bool CMusikLibrary::SetSongInfo( int songid, CMusikSongInfo* info )
{
	int result = 0;

	CStdString query;
	query.Format( _T( "update songs set format=%d, vbr=%d, filename='%s', artist='%s', title='%s', album='%s', tracknum=%d, year='%s', genre='%s', rating=%d, bitrate=%d, lastplayed='%s', notes='%s', timesplayed=%d, duration=%d, timeadded='%s', filesize=%d, dirty=%d where songid = %d;" ),
			info->GetFormat(),
			info->GetVBR(),
			info->GetFilename().c_str(),
			info->GetArtist().c_str(),
			info->GetTitle().c_str(),
			info->GetAlbum().c_str(),
			info->GetTrackNum(),
			info->GetYear().c_str(),
			info->GetGenre().c_str(),
			info->GetRating(),
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

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	{
		boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
		result = sqlite_exec( m_pDB, query.c_str(), NULL, NULL, NULL );
	}

	if ( result != SQLITE_OK )
		return false;
	
	return true;    
}

bool CMusikLibrary::SetSongRating( int songid, int rating )
{
	int result = 0;

	CStdString query;
	query.Format( _T( "update songs set rating=%d where songid=%d" ), rating, songid );

	//-----------------------------------------------------//
	//--- lock it up and run the query					---//
	//-----------------------------------------------------//
	{
		boost::mutex::scoped_lock scoped_lock( m_ProtectingLibrary );
		result = sqlite_exec( m_pDB, query.c_str(), NULL, NULL, NULL );
	}

	if ( result != SQLITE_OK )
		return false;
	
	return true;    
}