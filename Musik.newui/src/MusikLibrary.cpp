#include "wx/wxprec.h"
#include "MusikLibrary.h"
#include "MusikUTF8.h"

//-------------------------------------------------//
//--- this is a callback for sqlite to use when ---//
//--- adding songs to a CMusikPlaylist.			---//
//-------------------------------------------------//
static int sqlite_AddSongCallback(void *args, int WXUNUSED(numCols), char **results, char ** WXUNUSED(columnNames))
{
	CMusikPlaylist* p = (CMusikPlaylist*)args;

	CMusikSong *pLibItem = new CMusikSong();
	
	pLibItem->sFilename		= MBTowxString	( results[0]);
	pLibItem->sTitle		= MBTowxString	( results[1] );
	pLibItem->nTrackNum		= StringToInt	( results[2] );
	pLibItem->sArtist		= MBTowxString	( results[3] );
	pLibItem->sAlbum		= MBTowxString	( results[4] );
	pLibItem->sGenre		= MBTowxString	( results[5] );
	pLibItem->nDuration		= StringToInt	( results[6] );
	pLibItem->nFormat		= StringToInt	( results[7] );
	pLibItem->bVBR			= StringToInt	( results[8] );
	pLibItem->sYear			= MBTowxString	( results[9] );
	pLibItem->nRating		= StringToInt	( results[10] );
	pLibItem->nBitrate		= StringToInt	( results[11] );
	pLibItem->sLastPlayed	= MBTowxString	( results[12] );
	pLibItem->sNotes		= MBTowxString	( results[13] );
	pLibItem->nTimesPlayed	= StringToInt	( results[14] );
	pLibItem->sTimeAdded	= MBTowxString	( results[15] );
	pLibItem->nFilesize		= StringToInt	( results[16] );
	
	p->Add( pLibItem );

    return 0;
}

CMusikLibrary::CMusikLibrary( const wxString& filename )
{
	m_Filename = filename;
	InitFields();
	Start();
}

CMusikLibrary::~CMusikLibrary()
{
	Shutdown();
}


void CMusikLibrary::InitFields()
{
	m_Fields.Add( wxT( "Filename" ) );
	m_Fields.Add( wxT( "Title" ) );
	m_Fields.Add( wxT( "Artist" ) );
	m_Fields.Add( wxT( "Album" ) );
	m_Fields.Add( wxT( "Year" ) );
	m_Fields.Add( wxT( "Genre" ) );
	m_Fields.Add( wxT( "Track Number" ) );
	m_Fields.Add( wxT( "Time Added" ) );
	m_Fields.Add( wxT( "Last Played" ) );
	m_Fields.Add( wxT( "Filesize" ) );
	m_Fields.Add( wxT( "Format" ) );
	m_Fields.Add( wxT( "Duration" ) );
	m_Fields.Add( wxT( "Rating" ) );
	m_Fields.Add( wxT( "Times Played" ) );
	m_Fields.Add( wxT( "Bitrate" ) );

	m_FieldsDB.Add( wxT( "filename" ) );
	m_FieldsDB.Add( wxT( "title" ) );
	m_FieldsDB.Add( wxT( "artist" ) );
	m_FieldsDB.Add( wxT( "album" ) );
	m_FieldsDB.Add( wxT( "year" ) );
	m_FieldsDB.Add( wxT( "genre" ) );
	m_FieldsDB.Add( wxT( "tracknum" ) );
	m_FieldsDB.Add( wxT( "timeadded" ) );
	m_FieldsDB.Add( wxT( "lastplayed" ) );
	m_FieldsDB.Add( wxT( "filesize" ) );
	m_FieldsDB.Add( wxT( "format" ) );
	m_FieldsDB.Add( wxT( "duration" ) );
	m_FieldsDB.Add( wxT( "rating" ) );
	m_FieldsDB.Add( wxT( "timesplayed" ) );
	m_FieldsDB.Add( wxT( "bitrate" ) );
}

bool CMusikLibrary::Start()
{
	//-------------------------------------------------//
	//--- construct the table						---//
	//-------------------------------------------------//
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

	//-------------------------------------------------//
	//--- construct the index						---//
	//-------------------------------------------------//
	const char* szCreateIdxQuery =
		"CREATE INDEX songs_title_idx on songs (title);"
		"CREATE UNIQUE INDEX songs_filename_idx on songs (filename);"
		"CREATE INDEX songs_artist_idx on songs (artist);"
		"CREATE INDEX songs_album_idx on songs (album);"
		"CREATE INDEX songs_genre_idx on songs (genre);"
		"CREATE INDEX songs_artist_album_tracknum_idx on songs (artist,album,tracknum);";

	//-------------------------------------------------//
	//--- lock database access, open it up			---//
	//-------------------------------------------------//
	wxCriticalSectionLocker lock( m_csDBAccess );
	char *pErr = NULL;
	m_pDB = sqlite_open( wxStringToMB( m_Filename ), 0666, &pErr );

	if ( m_pDB )
	{
		sqlite_exec( m_pDB, szCreateDBQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateIdxQuery, NULL, NULL, NULL );
	}

	if ( pErr )
		free( pErr );

	return ( m_pDB != NULL );
}

void CMusikLibrary::Shutdown()
{
	//-------------------------------------------------//
	//--- lock it up and close it down.				---//
	//-------------------------------------------------//
	wxCriticalSectionLocker lock( m_csDBAccess );
	if ( m_pDB )
	{
		sqlite_close( m_pDB );
		m_pDB = NULL;
	}	
}

void CMusikLibrary::BeginTransaction()
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
}

void CMusikLibrary::EndTransaction()
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );
}

void CMusikLibrary::GetSongs( int source_type, const wxArrayString& source_items, CMusikPlaylist& target )
{
	target.Clear();
	target.Alloc( GetSongCount() );

  	wxString sQuery(wxT( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where "));
	sQuery += GetSongFieldDB( source_type );
	sQuery += wxT( " in(" );
	sQuery.Alloc( sQuery.Len() + 30 + source_items.GetCount() * 30 );

	//-------------------------------------------------//
	//--- we need to setup the query for each item	---//
	//--- in sources_items							---//
	//-------------------------------------------------//
	for ( size_t i = 0; i < source_items.GetCount(); i++ )
	{	
    	source_items.Item( i ).Replace( wxT( "'" ), wxT( "''" ), true );
  	 	sQuery += wxT("'");
		sQuery += source_items.Item( i );
		
		if ( i != source_items.GetCount() - 1 )
			sQuery += wxT("', ");
		else
			sQuery += wxT("' ) ");
 	}

	/*
	if ( nInType == MUSIK_LIB_ARTIST )
    	sQuery +=wxT("order by artist,album,tracknum;");
	else if ( nInType == MUSIK_LIB_ALBUM )
		sQuery += wxT( "order by album,tracknum,artist;");
	else if ( nInType == MUSIK_LIB_GENRE )
		sQuery += wxT( "order by genre,artist,album,tracknum;");
	else if ( nInType == MUSIK_LIB_YEAR )
		sQuery += wxT( "order by year,artist,album,tracknum;");
	*/

	{
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec(m_pDB, wxStringToMB( sQuery ), &sqlite_AddSongCallback, &target, NULL);
	}

	target.Shrink();
}

void CMusikLibrary::QuerySongs( const wxString& query, CMusikPlaylist& target )
{
	target.Clear();

	wxString queryWhere( wxT( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where " ) );
	queryWhere += query + wxT(";");

	const wxCharBuffer pQuery = wxStringToMB(query);
	target.Alloc( GetSongCount() );
	{
		//-------------------------------------------------//
		//--- lock as short as possible by using {}		---//
		//-------------------------------------------------//
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec( m_pDB, pQuery, &sqlite_AddSongCallback, &target, NULL );
	}

	target.Shrink();
}

int CMusikLibrary::GetSongCount()
{
	char *query = sqlite_mprintf( "select count(*) from songs;" );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

int CMusikLibrary::QueryCount( const char* pQueryResult )
{
	const char *pTail;
	sqlite_vm *pVM;

	wxCriticalSectionLocker lock( m_csDBAccess );
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

