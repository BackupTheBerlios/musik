#include "wx/wxprec.h"
#include "MusikLibrary.h"
#include "MusikUTF8.h"

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
