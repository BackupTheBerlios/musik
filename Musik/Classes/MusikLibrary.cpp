/*
 *  MusikLibrary.cpp
 *
 *  Object which controls database manipulation.
 *
 *  Uses: SQLite, id3lib, and ogg/vorbis
 *  Information about SQLite - An Embeddable SQL Database Engine is available at http://www.hwaci.com/sw/sqlite/
 *  Information about id3lib is available at http://www.id3lib.org
 *  Information about ogg/vorbis is available at http://www.vorbis.com/
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "MusikLibrary.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- mp3 / ogg helpers ---//
#include "Library/CMP3Info.h"
#include "Library/COggInfo.h"
#include "id3/misc_support.h"

//--- wx ---//
#include <wx/file.h>
#include <wx/filename.h>


#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY( CMusikSongArray )
WX_DECLARE_STRING_HASH_MAP( CMusikSong *, myStringToMusikSongPtrMap );

//--- frames ---//
//#include "../Frames/MusikFrame.h"

CMusikSong::CMusikSong()
{
	Rating 		= 0;
	TimesPlayed = 0;
	Check1 		= 0;
	songid		= -1;
	LastPlayed = TimeAdded	= 0.0;

}

CMusikLibrary::CMusikLibrary()
	: wxEvtHandler()
{
	m_nCachedSongCount = -1;
	m_pDB = NULL;
	SetSortOrderField( PLAYLISTCOLUMN_ARTIST );
}

CMusikLibrary::~CMusikLibrary()
{
	Shutdown();
}

void CMusikLibrary::Shutdown()
{
	//--- close database if necessary ---//
	wxCriticalSectionLocker lock( m_csDBAccess );
	if ( m_pDB )
	{
		sqlite_close( m_pDB );
		m_pDB = NULL;
	}	
}

bool CMusikLibrary::Load()
{
	wxString sFilename = MUSIK_DB_FILENAME;
		
	//--- look for database.. if need be, create it and create tables ---//
	char *errmsg = NULL;
	static	const char *szCreateVersionQuery =
		"CREATE TABLE version ( " 	
		"name, major, majorsub ,minor, minorsub "
		" );";
	
	//--- create the tables ---//
	static const char *szCreateSongTableQuery  = 
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
	const char* szCreateSongTableIdxQuery =
			"CREATE INDEX songs_title_idx on songs (title);"
			"CREATE UNIQUE INDEX songs_filename_idx on songs (filename);"
			"CREATE INDEX songs_artist_idx on songs (artist);"
			"CREATE INDEX songs_album_idx on songs (album);"
			"CREATE INDEX songs_genre_idx on songs (genre);"
			"CREATE INDEX songs_tracknum_idx on songs (tracknum);"
			"CREATE INDEX songs_artist_album_tracknum_idx on songs (artist,album,tracknum);"
			"CREATE INDEX songs_timeadded_idx on songs (timeadded);"
			"CREATE INDEX songs_lastplayed_idx on songs (lastplayed);"
			;
	static	const char *szCreateSongHistoryQuery =
			"CREATE TABLE songhistory ( "	
				"songid number(10), "
				"date_played timestamp , "
				"percent_played number(10),"
				"selected_by_user number(1) "
				" );";

	static const char* szCreateSongHistoryTableIdxQuery =
		"CREATE INDEX songhistory_songid_idx on songhistory (songid);"
		"CREATE INDEX songhistory_date_played_idx on songhistory (date_played);"
		"CREATE INDEX songhistory_percent_played_idx on songhistory (percent_played);"
		;
	
	m_pDB = sqlite_open( wxConvertWX2MB(sFilename), 0666, &errmsg );

	if( m_pDB )
	{

		// always create table, if it exists can error will be returned by sqlite_exec, but we dont care.
		sqlite_exec( m_pDB, szCreateVersionQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateSongTableQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateSongTableIdxQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateSongHistoryQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB, szCreateSongHistoryTableIdxQuery, NULL, NULL, NULL );
		sqlite_exec( m_pDB,	"CREATE TRIGGER song_deleted_trigger DELETE ON songs " 
							"BEGIN "
							"DELETE FROM songhistory WHERE songid = old.songid;"
							"END;", NULL, NULL, NULL );

		sqlite_exec( m_pDB,	"CREATE VIEW valid_albums as select album,artist,most_lastplayed from ("
							"select album,artist,sum(duration) as sum_duration,max(lastplayed+0) as most_lastplayed "  
							"from songs where album != '' group by album) where sum_duration > 1500000;"
							, NULL, NULL, NULL );	
		CreateDBFuncs();
		sqlite_exec( m_pDB, "PRAGMA synchronous = OFF;", NULL, NULL, NULL );
		sqlite_exec( m_pDB, "PRAGMA cache_size = 10000;", NULL, NULL, NULL );



		CheckVersion();
	}
	if ( errmsg )
			free( errmsg );
	return m_pDB != NULL;
}

void CMusikLibrary::CheckVersion()
{
	bool bVersionInfoPresent = false;
	int ver_major = -1;
	int ver_majorsub = -1;
	int ver_minor = -1;
	int ver_minorsub = -1;
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;

	sqlite_compile( m_pDB, "select name,major,majorsub,minor,minorsub from version where name='wxMusik';", &pTail, &pVM, NULL );
	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
	{
		bVersionInfoPresent= true;
		ver_major = atoi(coldata[1]);
		ver_majorsub = atoi(coldata[2]);
		ver_minor = atoi(coldata[3]);
		ver_minorsub = atoi(coldata[4]);
	}
	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem(errmsg);

	if(bVersionInfoPresent)
	{
		// update version info
		sqlite_exec_printf( m_pDB,	"update version set major=%d,majorsub=%d,minor=%d,minorsub=%d,where name = 'wxMusik';",
			NULL,NULL,NULL,MUSIK_VERSION_MAJOR,MUSIK_VERSION_MAJORSUB,MUSIK_VERSION_MINOR,MUSIK_VERSION_MINORSUB
			);
		// 
		if((ver_major == MUSIK_VERSION_MAJOR) &&
			(ver_majorsub == MUSIK_VERSION_MAJORSUB) &&
			(ver_minor == MUSIK_VERSION_MINOR)	 )
		{
			// nothing to do
			return;
		}
		// maybe  we call sometime conversion routines from here

	}
	else
	{
	   // version must be before 0.3.1.0

		// check for old datetime format 
		wxArrayString aOldDateStrings;
		Query(wxT(" select timeadded  from songs where timeadded like '%:%' limit 1;"),aOldDateStrings);// does an entry contains a colon?
		if(aOldDateStrings.GetCount() == 1)
		{
			// db contains old string format. m/d/y h:m:s
			//convert it to julianday
			sqlite_exec( m_pDB, "update songs set lastplayed = cnvMusikOldDTFormatToJulianday(lastplayed), timeadded = cnvMusikOldDTFormatToJulianday(timeadded) where 1;", NULL, NULL, NULL );

		}

		// convert <unknown> and 0 entrys to ''
		sqlite_exec( m_pDB, "update songs set album = '' where album = '<unknown>';"
							"update songs set artist = '' where artist = '<unknown>';"		
							"update songs set genre = '' where genre = '<unknown>';"		
							"update songs set year = '' where year = '<unknown>' or year=0;"		
							"update songs set notes = '' where  notes = '<unknown>' ;"		
							"update songs set lastplayed = '' where  lastplayed isnull;"		
							"update songs set tracknum = '' where tracknum = '<unknown>' OR tracknum = 0 OR tracknum isnull;"		
							, NULL, NULL, NULL );


		// convert iso8859-1 encoding to utf-8
		sqlite_exec( m_pDB, "update songs set artist = cnvISO8859_1ToUTF8(artist),"
							" album = cnvISO8859_1ToUTF8(album),"		
							" title = cnvISO8859_1ToUTF8(title),"		
							" genre = cnvISO8859_1ToUTF8(genre),"		
							" filename = cnvISO8859_1ToUTF8(filename) where 1; "
							, NULL, NULL, NULL );

		// insert version info
		sqlite_exec_printf( m_pDB,	"INSERT INTO version values('wxMusik',%d,%d,%d,%d);",
			NULL,NULL,NULL,MUSIK_VERSION_MAJOR,MUSIK_VERSION_MAJORSUB,MUSIK_VERSION_MINOR,MUSIK_VERSION_MINORSUB
			);

	}
}
void CMusikLibrary::CreateDBFuncs()
{
	wxASSERT(m_pDB);
	static struct 
	{
		char *zName;
		int nArg;
		int dataType;
		void (*xFunc)(sqlite_func*,int,const char**);
		void * userdata;
	} aFuncs[] = 
			{
				{ "remprefix",      1, SQLITE_TEXT, remprefixFunc, 0 },
				{ "cnvISO8859_1ToUTF8",     1, SQLITE_TEXT, cnvISO8859_1ToUTF8Func, 0 },
				{ "wxjulianday", 1, SQLITE_TEXT, wxjuliandayFunc,0 },// for backward compatibility
			    { "cnvMusikOldDTFormatToJulianday",	1, SQLITE_NUMERIC, cnvMusikOldDTFormatToJuliandayFunc, 0 },

			};
	/*  static struct {
	char *zName;
	int nArg;
	int dataType;
	void (*xStep)(sqlite_func*,int,const char**);
	void (*xFinalize)(sqlite_func*);
	} aAggs[] = {
	{ "min",    1, 0,              minStep,      minMaxFinalize },
	{ "max",    1, 0,              maxStep,      minMaxFinalize },
	{ "sum",    1, SQLITE_NUMERIC, sumStep,      sumFinalize    },
	{ "avg",    1, SQLITE_NUMERIC, sumStep,      avgFinalize    },
	{ "count",  0, SQLITE_NUMERIC, countStep,    countFinalize  },
	{ "count",  1, SQLITE_NUMERIC, countStep,    countFinalize  },
	};
	*/
	size_t i;

	for(i=0; i<sizeof(aFuncs)/sizeof(aFuncs[0]); i++)
	{
		sqlite_create_function(m_pDB, aFuncs[i].zName,aFuncs[i].nArg, aFuncs[i].xFunc, aFuncs[i].userdata);
		if( aFuncs[i].xFunc )
		{
			sqlite_function_type(m_pDB, aFuncs[i].zName, aFuncs[i].dataType);
		}
	}
	/* 
	for(i=0; i<sizeof(aAggs)/sizeof(aAggs[0]); i++){
	sqlite_create_aggregate(m_pDB, aAggs[i].zName,
			aAggs[i].nArg, aAggs[i].xStep, aAggs[i].xFinalize, 0);
	sqlite_function_type(m_pDB, aAggs[i].zName, aAggs[i].dataType);
	}
	*/
}
void CMusikLibrary::cnvISO8859_1ToUTF8Func(sqlite_func *context, int argc, const char **argv)
{
	if( argc<1 || argv[0]==0 ) return;
	const wxCharBuffer buf = ConvFromISO8859_1ToUTF8(argv[0]);
	int len =strlen(buf);
	sqlite_set_result_string(context,buf, len);
}
void CMusikLibrary::remprefixFunc(sqlite_func *context, int argc, const char **argv)
{
	if( argc<1 || argv[0]==0 ) return;
//	char * pPrefixArray[] = sqlite_user_data(context);
	static const char * pPrefixArray[] =
					{
						"The ",
						"Der ",
						"Die ",
						"Das ",
					};
	int argvlen = strlen(argv[0]);
	for(size_t i=0; i<sizeof(pPrefixArray)/sizeof(pPrefixArray[0]); i++)
	{
		int prefixlen = strlen(pPrefixArray[i]);
		if(strncasecmp(pPrefixArray[i],argv[0],prefixlen) == 0)
		{
			if(argvlen >= prefixlen)
			{
				sqlite_set_result_string(context, argv[0]+ prefixlen, argvlen - prefixlen);
				return;
			}
		}
	}
  sqlite_set_result_string(context, argv[0], argvlen);
}
void CMusikLibrary::cnvMusikOldDTFormatToJuliandayFunc(sqlite_func *context, int argc, const char **argv)
{
  if( argc<1 || argv[0]==0 ) return;
#ifdef __WXMSW__
  wxDateTime x;
  if( x.ParseFormat(ConvA2W(argv[0])))
  {
    sqlite_set_result_double(context, x.GetJulianDayNumber());
  }
#else
  struct tm tm;
  const char *result = strptime(argv[0], "%c", &tm);
  if ( !result )
        return;
  wxDateTime x(tm);
  sqlite_set_result_double(context, x.GetJulianDayNumber());
#endif
}
void CMusikLibrary::wxjuliandayFunc(sqlite_func *context, int argc, const char **argv)
{
	if( argc<1 || argv[0]==0 ) return;
	sqlite_set_result_string(context,argv[0], strlen(argv[0]));
}
//---  if true, compares the full path, if false, just looks for the filename itself   ---//
//--- obviously the filename you pass will either be full or just filename accordingly ---//
bool CMusikLibrary::FileInLibrary( const wxString & filename, bool fullpath )
{
	bool result = false;

	char *query;
	if ( fullpath )
		query = sqlite_mprintf( "select songid from songs where filename = %Q;", ( const char* )ConvToUTF8(filename) );
	else
		query = sqlite_mprintf( "select songid from songs where filename like '%%%q%%';", ( const char* )ConvToUTF8(filename) );
	
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = true;

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );
	sqlite_freemem(errmsg);
	return result;
}

bool CMusikLibrary::AddSongDataFromFile( const wxString & filename )
{
	//-----------------------------------------------------//
	//--- we don't need to worry about duplicate files	---//
	//--- because the MusikLibraryFrame will take care	---//
	//--- of any conflicts.								---//
	//-----------------------------------------------------//

	if ( filename.IsEmpty() )
		return true;

	
	CSongMetaData MetaData;
	MetaData.Filename = filename;
	bool bRet = GetMetaData( MetaData );
	if(bRet)
	{

		//--- run the query ---//
		wxCriticalSectionLocker lock( m_csDBAccess );
		m_nCachedSongCount = -1;
		sqlite_exec_printf( m_pDB, "insert into songs values (%Q ,%d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, julianday('now'), %d, %d );", NULL, NULL, NULL, 
			NULL,	
			(int)MetaData.eFormat,	
			MetaData.bVBR, 
			( const char* )ConvToUTF8(MetaData.Filename.GetFullPath()) ,
			( const char* )MetaData.Artist, 
			( const char* )MetaData.Title, 
			( const char* )MetaData.Album, 
			MetaData.nTracknum, 
			( const char* )MetaData.Year, 
			( const char* )MetaData.Genre, 
			0,//rating 
			MetaData.nBitrate, 
			"",//lastplayed 
			( const char* )MetaData.Notes,//notes 
			0,//timesplayed 
			MetaData.nDuration_ms, 
			MetaData.nFilesize,
			0); //dirty
	
	}

	return bRet;
}

bool CMusikLibrary::UpdateSongDataFromFile( const wxString & filename )
{
	if ( filename.IsEmpty() )
		return true;

	
	CSongMetaData MetaData;
	MetaData.Filename = filename;
	bool bRet = GetMetaData( MetaData );
	if(bRet)
	{

		//--- run the query ---//
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "update songs set format=%d, vbr=%d, artist=%Q, title=%Q, album=%Q, tracknum=%d, year=%Q, genre=%Q, notes=%Q, bitrate=%d, duration=%d, filesize=%d, dirty=0 where filename = %Q;", NULL, NULL, NULL, 
			(int)MetaData.eFormat,	
			MetaData.bVBR, 
			( const char* )MetaData.Artist, 
			( const char* )MetaData.Title, 
			( const char* )MetaData.Album, 
			MetaData.nTracknum, 
			( const char* )MetaData.Year, 
			( const char* )MetaData.Genre, 
			( const char* )MetaData.Notes, 
			MetaData.nBitrate, 
			MetaData.nDuration_ms, 
			MetaData.nFilesize,
			( const char* )ConvToUTF8(MetaData.Filename.GetFullPath())
			);
	
	}
	return bRet;
}

bool CMusikLibrary::GetMetaData( CSongMetaData & MetaData  )
{
	
	//--- get format ---//
	wxString ext = MetaData.Filename.GetExt().MakeLower();
    bool bRet = false;
	if ( ext == wxT("mp3") )
		bRet = GetMP3MetaData( MetaData );
	else if ( ext == wxT("ogg") )
		bRet = GetOggMetaData( MetaData );

	if ( MetaData.Title.Length() == 0 )
			MetaData.Title = ConvToUTF8( MetaData.Filename.GetFullPath() );
	if(bRet == false)
	{
		::wxLogWarning(_("Parsing of file %s failed."),(const wxChar *)MetaData.Filename.GetFullPath());
	}

	return bRet;

}
bool CMusikLibrary::GetMP3MetaData( CSongMetaData & MetaData )
{
	//--- first get the things that can be gleaned from the header ---//
	CMP3Info mp3info;
	wxString sFilename = MetaData.Filename.GetFullPath();
	if ( mp3info.loadInfo( sFilename ) == 0 )
	{
		MetaData.eFormat = MUSIK_FORMAT_MP3;
		MetaData.nDuration_ms = mp3info.getLengthInSeconds() * 1000;
		MetaData.nBitrate     = mp3info.getBitrate();
		MetaData.bVBR         = mp3info.isVBitRate();
		MetaData.nFilesize	  = mp3info.getFileSize();
		
		//--- link and load mp3 ---//
		ID3_Tag		id3Tag;
		id3Tag.Link( ( const char* )ConvW2A( sFilename ), (flags_t)ID3TT_ALL );
		
		MetaData.Artist.Attach	( ID3_GetArtist	( &id3Tag ));
		MetaData.Title.Attach	( ID3_GetTitle	( &id3Tag ));
		MetaData.Album.Attach	( ID3_GetAlbum	( &id3Tag ));
		MetaData.Year.Attach	( ID3_GetYear	( &id3Tag ));
		MetaData.Notes.Attach	( ID3_GetComment( &id3Tag ));
		MetaData.Genre = ID3_V1GENRE2DESCRIPTION(ID3_GetGenreNum( &id3Tag ));
		if(MetaData.Genre.IsEmpty())
            MetaData.Genre.Attach	( ID3_GetGenre	( &id3Tag ));

		char *szTrack = ID3_GetTrack( &id3Tag );
		if(szTrack)
		{
			MetaData.nTracknum = atol(szTrack);
			ID3_FreeString(szTrack);
		}

		MetaData.Artist = ConvFromISO8859_1ToUTF8(MetaData.Artist);
		MetaData.Title = ConvFromISO8859_1ToUTF8(MetaData.Title);
		MetaData.Album = ConvFromISO8859_1ToUTF8(MetaData.Album);
		MetaData.Notes = ConvFromISO8859_1ToUTF8(MetaData.Notes);
		MetaData.Genre = ConvFromISO8859_1ToUTF8(MetaData.Genre);
		return true;
	}
	return false;
}

bool CMusikLibrary::WriteTag(  CMusikSong & song, bool ClearAll , bool bUpdateDB )
{

	bool bRet = true;
	if ( song.MetaData.eFormat == MUSIK_FORMAT_MP3 )
		bRet = WriteMP3Tag( song.MetaData, ClearAll );
	else if ( song.MetaData.eFormat == MUSIK_FORMAT_OGG )
		bRet = WriteOGGTag( song.MetaData, ClearAll );
	else
		return false;

	if( bRet && bUpdateDB )
	{
		//-----------------------------//
		//--- flag item as clean	---//
		//-----------------------------//
		UpdateItem( song , false );
	}
	else if(bRet == false)
		::wxLogWarning(_("Writing tags to file %s failed."),(const wxChar *)song.MetaData.Filename.GetFullPath());
   return bRet;
}

bool CMusikLibrary::WriteMP3Tag( const CSongMetaData & MetaData, bool ClearAll )
{
	ID3_Tag	id3Tag;
	id3Tag.Link( ( const char* )ConvW2A( MetaData.Filename.GetFullPath () ) , (flags_t)ID3TT_ALL );

	//--- iterate through and delete ALL TAG INFO ---//
	if ( ClearAll )
	{
		ID3_Tag::Iterator* iter = id3Tag.CreateIterator();
		ID3_Frame* frame = NULL;
		while (NULL != (frame = iter->GetNext()))
		{
			frame = id3Tag.RemoveFrame(frame);
			delete frame;
		}
	}

	//--- clear only fields of interest ---//
	else if ( !ClearAll )
	{
		ID3_RemoveTitles	( &id3Tag ); 
		ID3_RemoveArtists	( &id3Tag );
		ID3_RemoveAlbums	( &id3Tag );
		ID3_RemoveTracks	( &id3Tag );
		ID3_RemoveYears		( &id3Tag );
		ID3_RemoveGenres	( &id3Tag );
		ID3_RemoveComments	( &id3Tag );

	}

	//--- tag ---//
	ID3_AddTitle	( &id3Tag,  ConvFromUTF8ToISO8859_1( MetaData.Title ),	true ); 
	ID3_AddArtist	( &id3Tag, ConvFromUTF8ToISO8859_1( MetaData.Artist ),	true );
	ID3_AddAlbum	( &id3Tag, ConvFromUTF8ToISO8859_1( MetaData.Album ),	true );
	ID3_AddYear		( &id3Tag, ConvFromUTF8ToISO8859_1( MetaData.Year ), 	true );
	ID3_AddComment	( &id3Tag, ConvFromUTF8ToISO8859_1( MetaData.Notes ), 	true );
	ID3_AddTrack	( &id3Tag, MetaData.nTracknum,				true );
	
	int genreid = GetGenreID( MetaData.Genre );
	if( genreid == -1 )
		ID3_AddGenre( &id3Tag, ConvFromUTF8ToISO8859_1( MetaData.Genre ),	true ); // write ID3V2 string genre tag
	else
		ID3_AddGenre( &id3Tag, genreid,	true );											// write ID3V1 integer genre id

	//--- write to file ---//
	return (id3Tag.Update() != ID3TT_NONE);
}

bool CMusikLibrary::WriteOGGTag( const CSongMetaData & MetaData, bool bClearAll )
{
	COggInfo ogg;
	return ogg.WriteMetaData (MetaData, bClearAll);
}

int CMusikLibrary::ClearDirtyTags()
{
	int nCount = QueryCount("select count(*) from songs where dirty = 1;");
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "update songs set dirty = 0 where dirty = 1;",
			NULL, NULL, NULL);
	return nCount;
}

void CMusikLibrary::AddMod( const wxString & WXUNUSED(filename) )
{
	//int format = MUSIK_FORMAT_MOD;
}

bool CMusikLibrary::GetOggMetaData( CSongMetaData & MetaData )
{	

	COggInfo ogg; 
	return  ogg.ReadMetaData(MetaData);
}

void CMusikLibrary::AddWav( const wxString & WXUNUSED(filename) )
{
	//int format = MUSIK_FORMAT_WAV;
}

void CMusikLibrary::AddWMA( const wxString & WXUNUSED(filename) )
{
	//int format = MUSIK_FORMAT_WMA;
}

void CMusikLibrary::AddAIFF( const wxString & WXUNUSED(filename) )
{
	//int format = MUSIK_FORMAT_AIFF;
}

void CMusikLibrary::VerifyYearList( const wxArrayString & aList,wxArrayString & aVerifiedList )
{
	aVerifiedList.Clear();
	for ( int i = 0; i < (int)aList.GetCount(); i++ )
	{
		if ( ( !aList.Item( i ).IsEmpty() ) && ( aList.Item( i ).IsNumber() ) )
			aVerifiedList.Add( aList.Item( i ) );
	}
	return;
}
int CMusikLibrary::sqlite_callbackAddToStringArray(void *args, int WXUNUSED(numCols), char **results, char ** WXUNUSED(columnNames))
{

	wxArrayString * p = (wxArrayString*)args;
	p->Add( ConvFromUTF8( results[0] )); 
    return 0;
}
void CMusikLibrary::GetInfo( const wxArrayString & aList, int nInType, int nOutType ,wxArrayString & aReturn )
{
	aReturn.Clear();
	wxString sInfo;
	if(nInType == MUSIK_LIB_INVALID || nOutType == MUSIK_LIB_INVALID)
	{
		wxASSERT(0);
		return;
	}
	wxString query;
	query.Alloc(50 * aList.GetCount()+ 40);
	switch ( nOutType )
	{
	case MUSIK_LIB_ARTIST:
		if( wxGetApp().Prefs.bSortArtistWithoutPrefix)
			query = wxT("select distinct artist,UPPER(REMPREFIX(artist)) as UP from songs where ");
		else
			query = wxT("select distinct artist,UPPER(artist) as UP from songs where ");
		break;

	case MUSIK_LIB_ALBUM:
		query = wxT("select distinct album,UPPER(album) as UP from songs where ");
		break;

	case MUSIK_LIB_GENRE:
		query = wxT("select distinct genre,UPPER(genre) as UP from songs where ");
		break;

	case MUSIK_LIB_YEAR:
		query = wxT("select distinct year from songs where ");
		break;

	case MUSIK_LIB_DECADE:
		break;
	}

	
	for ( int i = 0; i < (int)aList.GetCount(); i++ )
	{
		wxString itemstr = aList.Item( i );
		itemstr.Replace( wxT("'"), wxT("''") );

		if ( i > 0 )
			query += wxT("or ");

		switch ( nInType )
		{
		case MUSIK_LIB_ARTIST:
			query += wxT("artist = '");
			break;

		case MUSIK_LIB_ALBUM:
			query += wxT("album = '");
			break;

		case MUSIK_LIB_GENRE:
			query += wxT("genre = '");
			break;

		case MUSIK_LIB_YEAR:
			query += wxT("year = '");
			break;

		case MUSIK_LIB_DECADE:
			break;
		}
		query += itemstr;
		query += wxT("' ");

	}

	switch ( nOutType )
	{
	case MUSIK_LIB_ARTIST:
	case MUSIK_LIB_ALBUM:
	case MUSIK_LIB_GENRE:
		query += wxT("order by UP");
		break;

	case MUSIK_LIB_YEAR:
		query += wxT("order by year");
		break;

	case MUSIK_LIB_DECADE:
		break;
	}

	query += wxT(";");

	//--- run query ---//
	{
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec(m_pDB, ConvQueryToMB( query ), &sqlite_callbackAddToStringArray, &aReturn, NULL);
	}
	if ( nOutType == MUSIK_LIB_YEAR )
	{
		wxArrayString aVerifiedList;
		VerifyYearList( aReturn , aVerifiedList );
		aReturn = aVerifiedList;
	}
	
}
 int CMusikLibrary::sqlite_callbackAddToSongArray(void *args, int WXUNUSED(numCols), char **results, char ** WXUNUSED(columnNames))
{

	
	CMusikSongArray * p = (CMusikSongArray*)args;

	CMusikSong *pLibItem = new CMusikSong();
	_AssignSongTableColumnDataToSong(pLibItem,(const char**)results);
	p->Add( pLibItem );

    return 0;
}
void CMusikLibrary::GetSongs( const wxArrayString & aList, int nInType, CMusikSongArray & aReturn )
{
	aReturn.Clear();
	wxString sInfo;
	aReturn.Alloc(GetSongCount()); // optimize item adding performance,
  	wxString sQuery;
  
   
  	if ( nInType == MUSIK_LIB_ARTIST )
    		sQuery +=wxT("artist in(");
	else if ( nInType == MUSIK_LIB_ALBUM )
		sQuery += wxT( "album in( ");
	else if ( nInType == MUSIK_LIB_GENRE )
		sQuery += wxT( "genre in( ");
	else if ( nInType == MUSIK_LIB_YEAR )
		sQuery += wxT( "year in(");
	sQuery.Alloc(sQuery.Len() + 30 + aList.GetCount() * 30); // optimization ( the 30 is a wild guess)

	//--- for each item in the input list, we're going to query the database ---//
	for ( size_t i = 0; i < aList.GetCount(); i++ )
	{	
    	aList.Item( i ).Replace( wxT( "'" ), wxT( "''" ), true );
  	 	sQuery += wxT("'");
		sQuery += aList.Item( i );
		
		//--- not at the end ---//
		if ( i != aList.GetCount() - 1 )
			sQuery += wxT("', ");
		 
		//--- at the end ---//
		else
			sQuery += wxT("' ) ");
 	}
	QuerySongsWhere( sQuery, aReturn,true); // query sorted
	return;
}

void CMusikLibrary::Query( const wxString & query, wxArrayString & aReturn ,bool bClearArray )
{
	if(bClearArray)
{

	aReturn.Clear();
	//--- run the query ---//
	aReturn.Alloc( GetSongCount() );
	}
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec(m_pDB, ConvQueryToMB( query ), &sqlite_callbackAddToStringArray, &aReturn, NULL);
}

int CMusikLibrary::sqlite_callbackAddToSongMap(void *args, int WXUNUSED(numCols), char **results, char ** WXUNUSED(columnNames))
{
	//-------------------------------------------------------------------------//
	//--- maps filename to CMusingSong objects ptrs, ptrs because this		---//
	//--- way an additional call to a copy constructer is saved when adding	---//
	//--- the objects to the map											---//
	//-------------------------------------------------------------------------//

	myStringToMusikSongPtrMap * p = (myStringToMusikSongPtrMap*)args;

	CMusikSong *pLibItem = new CMusikSong();
	_AssignSongTableColumnDataToSong(pLibItem,(const char **)results);
	(*p)[pLibItem->MetaData.Filename.GetFullPath()]= pLibItem;

    return 0;
}
void CMusikLibrary::GetFilelistSongs( const wxArrayString & aFiles, CMusikSongArray & aReturn )
{
	aReturn.Clear();
	
	wxString sQuery = wxT("select songid,filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where filename in (");

	sQuery.Alloc(sQuery.Len() + aFiles.GetCount() * 30); // optimization ( the 30 is a wild guess)
	for ( size_t i = 0; i < aFiles.GetCount(); i++ )
	{
		//--- if song has a ' ---//	
		wxString filename( aFiles.Item( i ) );
		filename.Replace( wxT("'"), wxT("''"), TRUE );

		sQuery += wxT("'");
		sQuery += filename;
		//--- not at the end ---//
		if ( i != aFiles.GetCount() - 1 )
			sQuery += wxT("', ");
		//--- at the end ---//
		else
			sQuery += wxT("' );");
	}

	myStringToMusikSongPtrMap theMap;
	//---------------------------------------------------------------------//
	//--- we fill the map and afterwards a array from the map because	---//
	//--- we can have multiple filenames in the same list				---//
	//---------------------------------------------------------------------//
	{
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec(m_pDB, ConvQueryToMB( sQuery ), &sqlite_callbackAddToSongMap, &theMap, NULL);
	}

	aReturn.Alloc( aFiles.GetCount() );
	for ( size_t i = 0; i < aFiles.GetCount(); i++ )
	{
		CMusikSong *pSong = theMap[ aFiles.Item( i ) ];
	//	wxASSERT_MSG( pSong, wxString(aFiles.Item( i ) + wxT( " is not on the map!" ) ) );

		//---------------------------------------------------------------------//
		//--- add the object(of the map) by value, to create duplicate		---// 
		//--- entries if needed.											---//
		//---------------------------------------------------------------------//
		if( pSong )
			aReturn.Add( *pSong ); 
	}

	//-------------------------------------------------------------------------//
	//--- delete all map objects( this is done explicitly, because the map	---//
	//--- contains pointers to objects)										---//
	//-------------------------------------------------------------------------//
	WX_CLEAR_HASH_MAP(myStringToMusikSongPtrMap, theMap); 

	return;
}

void CMusikLibrary::SetSortOrderField( int nField, bool descending )
{
	m_sSortAllSongsQuery.Empty();
	
	bool numeric = false;
	const wxString & sortstr = g_PlaylistColumnDBNames[ nField ];	
	if ( ( sortstr == wxT("duration") ) || ( sortstr == wxT("tracknum") ) || ( sortstr == wxT("timesplayed") ) || ( sortstr == wxT("bitrate") ) || ( sortstr == wxT("lastplayed") ) )
		numeric = true;

	if ( !numeric )
	{
		m_sSortAllSongsQuery = wxT("select songid,filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize, UPPER(");
		if(wxGetApp().Prefs.bSortArtistWithoutPrefix && (sortstr == wxT("artist")) )
		{
			m_sSortAllSongsQuery += wxT("REMPREFIX(");
			m_sSortAllSongsQuery += sortstr;
			m_sSortAllSongsQuery += wxT(")");
		}
		else
			m_sSortAllSongsQuery += sortstr;

		m_sSortAllSongsQuery += wxT(") as up");
		m_sSortAllSongsQuery += sortstr;		
		m_sSortAllSongsQuery += wxT(" from songs ");
	}
	else
		m_sSortAllSongsQuery = wxT("select songid,filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs ");

	m_sSortAllSongsQuery += wxT("%s"); // add placeholder for possible where clause

	if ( !numeric )
		m_sSortAllSongsQuery += wxT(" order by up");
	else
		m_sSortAllSongsQuery += wxT(" order by ");
	m_sSortAllSongsQuery += sortstr;
	if ( descending )
		m_sSortAllSongsQuery += wxT(" desc");

	if(sortstr == wxT("artist"))
		m_sSortAllSongsQuery += wxT(" ,album , tracknum");
	else if(sortstr == wxT("album")) 
		m_sSortAllSongsQuery += wxT(" ,tracknum,artist");
	else if(sortstr == wxT("genre")) 
		m_sSortAllSongsQuery += wxT(" ,artist,album,tracknum");
	else if(sortstr == wxT("year")) 
		m_sSortAllSongsQuery += wxT(" ,artist,album,tracknum");
	m_sSortAllSongsQuery += wxT(";");

	return;
}

///////////////////////////////////////////////////////////////////////////////

double CMusikLibrary::GetTotalPlaylistSize()
{
	wxString sQuery;
	
	sQuery = wxT("select sum(filesize) from songs where songid in (");

	size_t count = g_Playlist.GetCount();
	if ( count < 1 )
		return 0.0;

	sQuery.Alloc(sQuery.Len() + count * 10); // optimization ( the 10 is a wild guess)
	for ( size_t i = 0; i < count ; i++ )
	{
		//--- if song has a ' ---//	
		const CMusikSong& song = g_Playlist.Item ( i );
		
		sQuery += wxString::Format(wxT("%d"),song.songid);
		//--- not at the end ---//
		if ( i != count - 1 )
			sQuery += wxT(", ");
		//--- at the end ---//
		else
		{
			sQuery += wxT(" );");
		}
	}


	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, ConvQueryToMB( sQuery ), &pTail, &pVM, NULL );
	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	double  totsize = 0.0;
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
	{
		totsize = atof( coldata[0] );		
	}

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem(errmsg);

	return totsize;
}

///////////////////////////////////////////////////////////////////////////////
void CMusikLibrary::RedoLastQuerySongsWhere( CMusikSongArray & aReturn ,bool bSorted)
{
	QuerySongsWhere(m_lastQueryWhere,aReturn,bSorted);
}
void CMusikLibrary::QuerySongsWhere( const wxString & queryWhere, CMusikSongArray & aReturn ,bool bSorted)
{
	aReturn.Clear();
	//--- run query ---//
	wxString query;
	wxString myqueryWhere = queryWhere.IsEmpty()  ? wxT("") : wxT(" where ") + queryWhere;
	m_lastQueryWhere = queryWhere;
	if( bSorted && !m_sSortAllSongsQuery.IsEmpty() )
	{
		query = wxString::Format(  m_sSortAllSongsQuery , (const wxChar *)myqueryWhere );
	}
	else
	{
		query = wxT("select songid,filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs");
		query += myqueryWhere; 
		query += wxT(";");		
	}
	
	const wxCharBuffer pQuery = ConvQueryToMB(query);
	aReturn.Alloc(GetSongCount());
	{
		// keep lock as short as possible by using {} scope
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec(m_pDB, pQuery, &sqlite_callbackAddToSongArray, &aReturn, NULL);
	}

	aReturn.Shrink();
	return;
}


void CMusikLibrary::UpdateItemLastPlayed( const CMusikSong & song )
{

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "update songs set lastplayed = julianday('now'), timesplayed = timesplayed + 1 where songid = %d;",
		NULL, NULL, NULL, song.songid );
}


void CMusikLibrary::RecordSongHistory( const CMusikSong & song ,int playedtime,bool bSelectedByUser)
{

	{
		int percentplayed = playedtime * 100 / song.MetaData.nDuration_ms;  
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "insert into songhistory values ( %d, julianday('now'),%d,%d );",
			NULL, NULL, NULL,song.songid ,percentplayed ,bSelectedByUser);

	}
}

void CMusikLibrary::UpdateItemResetDirty( const CMusikSong & song )
{
	{
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "update songs set dirty = 0 where songid = %d;",
			NULL, NULL, NULL,  song.songid );
	}
}
int CMusikLibrary::GetTimesPlayed( const CMusikSong & song )
{
	char *query = sqlite_mprintf( "select timesplayed from songs where songid = %d;", song.songid );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

int CMusikLibrary::GetSongCount()
{

	if(m_nCachedSongCount == -1 )
	{
		char *query = "select count(*) from songs;";
		m_nCachedSongCount = QueryCount(query);
	}
	return m_nCachedSongCount;
}

bool CMusikLibrary::GetSongFromSongid( int songid, CMusikSong *pSong )
{
	char *query = sqlite_mprintf( "select songid,filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where songid = %d;", songid );
		
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	bool bFoundSong = false;
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
	{
		_AssignSongTableColumnDataToSong(pSong,coldata);
		bFoundSong = true;
	}

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );
	sqlite_freemem(errmsg);

	return bFoundSong;
}

void CMusikLibrary::UpdateItem( CMusikSong & newsonginfo, bool bDirty )
{
	// this only updates user changeable properties of the song.	
	int result = 0;
	{// keep lock as short as possible by using {} scope
		wxCriticalSectionLocker lock( m_csDBAccess );
		result = sqlite_exec_printf( m_pDB, "update songs set filename=%Q, artist=%Q, title=%Q,"
											"album=%Q, tracknum=%d, year=%Q, genre=%Q, rating=%d,"
											"notes=%Q, dirty=%d where songid = %d;", NULL, NULL, NULL, 
			( const char* )ConvToUTF8( newsonginfo.MetaData.Filename.GetFullPath() ), 
			( const char* )newsonginfo.MetaData.Artist, 
			( const char* )newsonginfo.MetaData.Title , 
			( const char * )newsonginfo.MetaData.Album , 
			newsonginfo.MetaData.nTracknum, 
			( const char* )newsonginfo.MetaData.Year, 
			( const char* )newsonginfo.MetaData.Genre,
			newsonginfo.Rating, 
			( const char* )newsonginfo.MetaData.Notes, 
			(int)bDirty, 
			 newsonginfo.songid);
	}
	if ( result != SQLITE_OK )
		wxMessageBox( _( "An error occurred when attempting to update the database" ), MUSIKAPPNAME_VERSION, wxOK | wxICON_ERROR );

	newsonginfo.Check1 = 0;
}


int CMusikLibrary::GetSongDirCount( wxString sDir )
{
	char *query = sqlite_mprintf( "select count(*) from songs where filename like '%q%%';", ( const char* )ConvToUTF8(sDir) );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}
int CMusikLibrary::QueryCount(const char * szQuery )
{
	int result = -1;
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, szQuery, &pTail, &pVM, NULL );
	char *errmsg = NULL;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = coldata[0] ? atoi( coldata[0] ) : 0;

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem(errmsg);
	return result;
}

void CMusikLibrary::SetRating( int songid, int nVal )
{
	if ( nVal < 0 )
		nVal = 0;
	if ( nVal > 5 )
		nVal = 5;

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "update songs set rating = %d where songid = %d;",
		NULL, NULL, NULL, nVal, songid );

}

bool CMusikLibrary::CheckAndPurge( const wxString & filename )
{
	if ( !wxFileExists( filename ) )
	{   
		RemoveSong( filename );
		return true;
	}
	return false;
}

void CMusikLibrary::RemoveSongDir( const wxString &  sDir )
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs where filename like '%q%%'", NULL, NULL, NULL, ( const char* )ConvToUTF8(sDir) );	
	m_nCachedSongCount = -1;
}

void CMusikLibrary::RemoveSong( const wxString & sSong	)	
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs where filename = '%q'", NULL, NULL, NULL, ( const char* )ConvToUTF8( sSong ) );
	m_nCachedSongCount = -1;
}
void CMusikLibrary::RemoveSong( int songid )	
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs where songid = %d", NULL, NULL, NULL,songid);
	m_nCachedSongCount = -1;
}

void CMusikLibrary::RemoveAll()
{ 
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs;", NULL, NULL, NULL );	
	m_nCachedSongCount = 0;
}

bool CMusikLibrary::ReplaceMask( wxString *sSrc, wxString sMask, wxString sTarget, bool bReplaceAll )
{
	wxString sCheck( *sSrc ); 
	sCheck.Replace( wxT( " " ), wxT( "" ), true );

	if ( !sCheck.IsEmpty() && sCheck != wxT( "<unknown>" ) )
	{
		sSrc->Replace( sMask, sTarget, bReplaceAll );
		return true;
	}

	return false;

}


bool CMusikLibrary::RenameFile( CMusikSong & song )
{
	//--------------------------------//
	//--- new filename information ---//
	//--------------------------------//
	
	wxString sPrePath	= song.MetaData.Filename.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
	wxString sFile		= wxGetApp().Prefs.sAutoRename;

	wxString sCheck;

	//---------------------------------------------//
	//--- the song's root directory. this will	---//
	//--- only affect us if there is directory	---//
	//--- seperation stuff going on.			---//
	//---------------------------------------------//
	wxString sRootPath = sPrePath;
	if ( wxGetApp().Prefs.sAutoRename.Find( wxFileName::GetPathSeparator() ) > - 1 )
	{
		if ( g_Paths.GetCount() == 0 )
			return false;

		sCheck = song.MetaData.Filename.GetPath();
		for ( size_t i = 0; i < g_Paths.GetCount(); i++ )
		{
			if ( sCheck.Find( g_Paths.Item( i ) ) > -1 )
			{
				sRootPath = g_Paths.Item( i );
				break;
			}

			else if ( i == g_Paths.GetCount() - 1 )
				return false;
		}
	}

	//--------------------------------------------//
	//--- format the output filename (without) ---//
	//--- path. e.g.. 01 - STP - Meat Plow.mp3 ---//
	//--------------------------------------------//
	wxString sTrackNum;
	sTrackNum.sprintf( wxT("%.2d"), song.MetaData.nTracknum );

	//----------------------------------------------//
	//--- replace masked values					 ---//
	//----------------------------------------------//
	if ( !ReplaceMask( &sFile, wxT( "%1" ),ConvFromUTF8( song.MetaData.Title )) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%2" ),ConvFromUTF8( song.MetaData.Artist )) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%3" ),ConvFromUTF8( song.MetaData.Album )) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%4" ),ConvFromUTF8( song.MetaData.Genre )) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%5" ), ConvFromUTF8(song.MetaData.Year ) ) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%6" ), sTrackNum ) )
		return false;

	//--- on some platfroms, we can't have certain characters, so blank them out ---//
	wxString sForbiddenChars = GetForbiddenChars();
	for(size_t i = 0; i < sForbiddenChars.Len();i++)
        	sFile.Replace( sForbiddenChars.Mid(i,1), wxT(""), true );

	//--- final name ---//
	wxFileName newfilename(sRootPath,sFile, song.MetaData.Filename.GetExt());

	//--- filename already the same? return ---//
	if ( song.MetaData.Filename == newfilename )
	{
		return true;
	}

	//-----------------------------------------//
	//--- create needed directories and do	---//
	//--- any sort of filename modification	---//
	//--- that is needed					---//
	//-----------------------------------------//
	if(!wxFileName::Mkdir(sRootPath,0777,wxPATH_MKDIR_FULL))
		return false;


	//-----------------------------------------//
	//--- file does need to be renamed, so	---//
	//--- rename it, then return			---//
	//-----------------------------------------//
	if ( wxRenameFile( song.MetaData.Filename.GetFullPath(), newfilename.GetFullPath() ) )
	{
		song.MetaData.Filename = newfilename;
		{
			wxCriticalSectionLocker lock( m_csDBAccess );
			sqlite_exec_printf( m_pDB, "update songs set filename =%Q where songid = %d;",
				NULL, NULL, NULL,
				( const char* )ConvToUTF8( song.MetaData.Filename.GetFullPath() ),
				 song.songid );
		}
		return true;
	}
	
	//-----------------------------------------//
	//--- how could we get here? we can't	---//
	//--- so just return false				---//
	//-----------------------------------------//
	return false;
}

bool CMusikLibrary::RetagFile(const CMusikTagger & tagger, CMusikSong* Song )
{
	
	if(!tagger.Retag(Song))
		return false;
	UpdateItem( *Song, true );
	return true;
}

//-----------------------------------//
//--- pre-defined queries to make ---//
//---   life a little bit easier  ---//
//-----------------------------------//
void CMusikLibrary::GetAllYears(wxArrayString & years)
{
	years.Clear();
	Query( wxT("select distinct year from songs order by year;"), years );
	wxArrayString verifiedyears;
	VerifyYearList( years, verifiedyears );
	years = verifiedyears;
	return;
}

void CMusikLibrary::GetAllSongs( CMusikSongArray & aReturn, bool bSorted )
{
	//QuerySongsWhere( wxT(""), aReturn ,bSorted); for some unknown reason linux (suse 8.2 with wxGTK2.5.1 segfaults
	QuerySongsWhere( wxString(), aReturn ,bSorted);
}

void CMusikLibrary::GetAllArtists( wxArrayString & aReturn )
{
	if(wxGetApp().Prefs.bSortArtistWithoutPrefix)
		Query( wxT("select distinct artist,UPPER(REMPREFIX(artist)) as UP from songs order by UP;"), aReturn );
	else
		Query( wxT("select distinct artist,UPPER(artist) as UP from songs order by UP;"), aReturn );

}

void CMusikLibrary::GetAllAlbums( wxArrayString & aReturn )
{
	Query( wxT("select distinct album,UPPER(album) as UP from songs order by UP;"), aReturn );
}

void CMusikLibrary::GetAllGenres( wxArrayString & aReturn )	
{ 
	Query( wxT("select distinct genre,UPPER(genre) as UP from songs order by UP;"), aReturn );					
}

