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
#include "Library/vcedit.h"
#include "id3/misc_support.h"

//--- wx ---//
#include <wx/file.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( CMusikSongArray );
WX_DECLARE_STRING_HASH_MAP( CMusikSong *, myStringToMusikSongPtrMap );

//--- frames ---//
#include "../Frames/MusikFrame.h"

CMusikSong::CMusikSong()
{
	Filename 	= wxT("");
	Title 		= _("<unknown>");
	TrackNum 	= 0;
	Artist 		= _("<unknown>");
	Album 		= _("<unknown>");
	Genre 		= _("<unknown>");
	Year 		= wxT("");
	LastPlayed 	= wxT("");
	TimeAdded	= wxT("");
	Filesize	= 0;
	Format 		= 0;
	Duration	= 0;
	Rating 		= 0;
	TimesPlayed = 0;
	Bitrate 	= 0;
	VBR 		= false;
	Notes 		= wxT("");
	Check1 		= 0;
}

CMusikLibrary::CMusikLibrary()
	: wxEvtHandler()
{
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
	
	// does file already exist, if nt we create the table
	wxString query;
	if ( !wxFileExists( sFilename ) )
	{
		//--- create the tables ---//
		query  = wxT( "create table songs ( "			);
		query += wxT( "songid INTEGER PRIMARY KEY, "	);
		query += wxT( "format number(1), "				);
		query += wxT( "vbr number(1), "					);
		query += wxT("filename varchar(255), "			);
		query += wxT( "artist varchar(255), "			);
		query += wxT( "title varchar(255), "			);
		query += wxT( "album varchar(255), "			);
		query += wxT( "tracknum number(3), "			);
		query += wxT( "year varchar(255), "				);
		query += wxT( "genre varchar(255), "			);
		query += wxT( "rating number(1), "				);
		query += wxT( "bitrate number(10), "			);
		query += wxT( "lastplayed timestamp, "			);
		query += wxT( "notes varchar(255), "			);
		query += wxT( "timesplayed number(5), "			);
		query += wxT( "duration number(10), "			);
		query += wxT( "timeadded timestamp, "			);
		query += wxT( "filesize number(10), "			);
		query += wxT( "dirty number(10) "				);
		query += wxT( " );"								);
	}
	wxCriticalSectionLocker lock( m_csDBAccess ); // just in case...
	m_pDB = sqlite_open( wxConvertWX2MB(sFilename), 0666, &errmsg );

	if ( !query.IsEmpty() )
	{
		const wxCharBuffer pQuery = ConvQueryToMB(query);
		sqlite_exec( m_pDB, pQuery, NULL, NULL, NULL );
	}
	if ( errmsg )
			free( errmsg );
	return true;
}

//---  if true, compares the full path, if false, just looks for the filename itself   ---//
//--- obviously the filename you pass will either be full or just filename accordingly ---//
bool CMusikLibrary::FileInLibrary( const wxString & filename, bool fullpath )
{
	bool result = false;

	char *query;
	if ( fullpath )
		query = sqlite_mprintf( "select filename from songs where filename = %Q;", ( const char* )ConvFNToFieldMB(filename) );
	else
		query = sqlite_mprintf( "select filename from songs where filename like '%%%q%%';", ( const char* )ConvFNToFieldMB(filename) );
	
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = true;

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );
	return result;
}

void CMusikLibrary::AddItem( const wxString & filename )
{
	//-----------------------------------------------------//
	//--- we don't need to worry about duplicate files	---//
	//--- becuase the MusikLibraryFrame will take care	---//
	//--- of any conflicts.								---//
	//-----------------------------------------------------//

	if ( filename.IsEmpty() )
		return;

	//--- get format ---//
	wxFileName fn( filename );
	wxString ext = fn.GetExt();
	ext.MakeLower();
	
	if ( ext == wxT("mp3") )
		AddMP3( filename );
	else if ( ext == wxT("ogg") )
		AddOgg( filename );
	else if ( ( ext == wxT("mod") ) || ( ext == wxT("it") ) || ( ext == wxT("s3m") ) || ( ext == wxT("xm") ) )
		AddMod( filename );
	else if ( ext == wxT("wma"))
		AddWMA( filename );
	else if ( ext == wxT("wav"))
		AddWav( filename );
	else if ( ( ext == wxT("aiff") ) || ( ext == wxT("iff")) )
		AddAIFF( filename );
}

void CMusikLibrary::InitTimeAdded()
{
	wxDateTime currtime = wxDateTime::Now();
	m_TimeAdded = currtime.Format();
}

void CMusikLibrary::AddMP3( const wxString & filename )
{
	wxFileName fn( filename );
	wxString fullname = fn.GetFullName();
	wxString justfilename = fullname.Left( fullname.Length() - fn.GetExt().Len()-1 );

	int format		= MUSIK_FORMAT_MP3;
	char * szTitle	= NULL;
	char * szArtist	= NULL;
	char * szAlbum	= NULL;
	char * szGenre	= NULL;
	char * szYear	= NULL;
	char * szTrack	= NULL;
	int rating		= 0;
	int bitrate		= 0;
	int tracknum	= 0;
	int duration	= 0;
	int vbr			= 0;
	int timesplayed = 0;
	int filesize = 0;
	int dirty = 0;

	//--- first get the things that can be gleaned from the header ---//
	CMP3Info mp3info;
	if ( mp3info.loadInfo( filename ) == 0 )
	{
		duration	= mp3info.getLengthInSeconds() * 1000;
		bitrate		= mp3info.getBitrate();
		vbr			= mp3info.isVBitRate();
		filesize	= mp3info.getFileSize();
		
		//--- link and load mp3 ---//
//		char* pFilename = StringToANSI( filename );
		ID3_Tag		id3Tag;
		id3Tag.Link( ( const char* )ConvFNToFieldMB( filename ), ID3TT_ALL );
		
		szArtist 	=  ID3_GetArtist	( &id3Tag );
		szTitle 	=  ID3_GetTitle		( &id3Tag );
		szAlbum 	=  ID3_GetAlbum		( &id3Tag );
		szGenre 	=  ID3_GetGenre		( &id3Tag );
		szYear 		=  ID3_GetYear		( &id3Tag );
		szTrack 	=  ID3_GetTrack		( &id3Tag );
		if(szTrack)
		{
			tracknum = atol(szTrack);
		}
		
		//--- get correct genre from (X) / (XX) / (XXX) format ---//
		wxString sGenre = ConvDBFieldToWX( szGenre );
		sGenre = GetGenre( sGenre );

		//--- run the query ---//
		{
			wxCriticalSectionLocker lock( m_csDBAccess );
			sqlite_exec_printf( m_pDB, "insert into songs values ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d );", NULL, NULL, NULL, NULL,
				format,	
				vbr, 
				( const char* )ConvFNToFieldMB( filename ), 
				szArtist 	?	szArtist	:	"<unknown>",
				szTitle 	?	szTitle		:	( const char* )ConvFNToFieldMB(justfilename),
				szAlbum 	?	szAlbum		:	"<unknown>",
				tracknum, 
				szYear 		?	szYear		:	"<unknown>", 
				szGenre 	?	( const char* )ConvDBFieldToMB( sGenre )	:	"<unknown>", 
				rating, 
				bitrate, 
				"",//lastplayed 
				"",//notes 
				timesplayed, 
				duration, 
				( const char* )ConvDBFieldToMB( m_TimeAdded ),
				filesize,
				dirty );
		}
		ID3_FreeString(szArtist);
		ID3_FreeString(szTitle);
		ID3_FreeString(szAlbum);
		ID3_FreeString(szGenre);
		ID3_FreeString(szYear);
		ID3_FreeString(szTrack);


	}
}

void CMusikLibrary::WriteTag(  CMusikSong & song, bool ClearAll , bool bUpdateDB )
{

	if ( song.Format == MUSIK_FORMAT_MP3 )
		WriteMP3Tag( song, ClearAll );
	else if ( song.Format == MUSIK_FORMAT_OGG )
		WriteOGGTag( song, ClearAll );
	if( bUpdateDB )
	{
		//-----------------------------//
		//--- flag item as clean	---//
		//-----------------------------//
		UpdateItem( song.Filename, song , false );
	}
}

void CMusikLibrary::WriteMP3Tag( const CMusikSong & song, bool ClearAll )
{
	ID3_Tag	id3Tag;
	id3Tag.Link( ( const char* )ConvFNToFieldMB( song.Filename ) , ID3TT_ALL );

	//--- itterate through and delete ALL TAG INFO ---//
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
	}

	//--- tag ---//
	ID3_AddTitle	( &id3Tag, ( const char* )ConvDBFieldToMB( song.Title ),	true ); 
	ID3_AddArtist	( &id3Tag, ( const char* )ConvDBFieldToMB( song.Artist ),	true );
	ID3_AddAlbum	( &id3Tag, ( const char* )ConvDBFieldToMB( song.Album ),	true );
	ID3_AddYear		( &id3Tag, ( const char* )ConvDBFieldToMB( song.Year ), 	true );
	ID3_AddTrack	( &id3Tag, song.TrackNum,									true );
	int genreid = GetGenreID( song.Genre );
	if(genreid == -1)
		ID3_AddGenre	( &id3Tag, song.Genre ,						true );// write ID3V2 string genre tag
	else
		ID3_AddGenre	( &id3Tag, genreid,						true );// write ID3V1 integer genre id
	//--- write to file ---//
	id3Tag.Update();
	
}

bool CMusikLibrary::WriteOGGTag( const CMusikSong & song, bool ClearAll )
{

	wxString filename = song.Filename;
	//--- generate a temp filename, then rename the ogg ---//
	wxString sRename = GenTempFilename( filename, 8 );
	while( wxFileExists( sRename ) )
		sRename = GenTempFilename( filename, 8 );

	if ( !wxRenameFile( filename, sRename ) )
		return false;

	//--- file objects, etc ---//
	wxFFile in, out;
    vcedit_state *state;
    vorbis_comment *vc;
    state = vcedit_new_state();

    	//--- if file couldn't be loaded, return ---//
	wxString read_state = wxT( "r+b" );
	if ( !in.Open( sRename.c_str(), read_state.c_str() ) )
	{
		wxRenameFile( sRename, filename );
		return false;
	}

	//--- if the file *DID* load, but is *NOT* and ogg, return ---//
	if( vcedit_open( state, in.fp() ) < 0 )
    	{
		in.Close();
		wxRenameFile( sRename, filename );
		return false;
	}

	//--- load the comments, and clear them out ---//
	vc = vcedit_comments( state );
    	vorbis_comment_clear( vc );
	vorbis_comment_init( vc );

	//--- add comments ---//
	wxString sTrack = IntTowxString	( song.TrackNum );
	vorbis_comment_add_tag( vc, "TITLE",		( char* )( ( const char* )ConvDBFieldToMB( song.Title )	)	);
	vorbis_comment_add_tag( vc, "TRACKNUMBER",	( char* )( ( const char* )ConvDBFieldToMB( sTrack )	)		);
	vorbis_comment_add_tag( vc, "ARTIST",		( char* )( ( const char* )ConvDBFieldToMB( song.Artist ) )	);
	vorbis_comment_add_tag( vc, "ALBUM",		( char* )( ( const char* )ConvDBFieldToMB( song.Album )	)	);
	vorbis_comment_add_tag( vc, "GENRE",		( char* )( ( const char* )ConvDBFieldToMB( song.Genre )	)	);
	vorbis_comment_add_tag( vc, "DATE",			( char* )( ( const char* )ConvDBFieldToMB( song.Year )	)	);

	//--- write new file ---//
	wxString write_state = wxT( "w+b" );
	if ( out.Open( filename.c_str(), write_state.c_str() ) )
		vcedit_write( state, out.fp() );

	//--- clean up ---//
	vcedit_clear( state );
	in.Close();
	out.Close();

	//--- make sure the new file exists before we remove it ---//
	if ( wxFileExists( filename ) )
		wxRemoveFile( sRename );
	else
		wxRenameFile( sRename, filename );

	return true;
}

int CMusikLibrary::ClearDirtyTags()
{
	CMusikSongArray aDirty;
	g_Library.QuerySongs( wxT("dirty = 1"), aDirty );
	size_t nCount = aDirty.GetCount();
	for( size_t i = 0; i < nCount; i++ )
		UpdateItem( aDirty.Item( i ).Filename, aDirty.Item( i ), false );

	return nCount;
}

void CMusikLibrary::AddMod( const wxString & filename )
{
	//int format = MUSIK_FORMAT_MOD;
}

void CMusikLibrary::AddOgg( const wxString & filename )
{	
	wxFileName fn( filename );
	wxString fullname = fn.GetFullName();
	wxString justfilename = fullname.Left( fullname.Length() - fn.GetExt().Len()-1 );

	int format		= MUSIK_FORMAT_OGG;
	wxString title	= _( "<unknown>" );
	wxString artist	= _( "<unknown>" );
	wxString album	= _( "<unknown>" );
	wxString genre	= _( "<unknown>" );
	wxString year;
	int rating		= 0;
	int bitrate		= 0;
	int tracknum	= 0;
	wxString lastplayed;
	int duration	= 0;
	int vbr			= 0;
	wxString notes;
	int timesplayed = 0;
	int dirty 		= 0;
	int filesize	= 0;

	COggInfo oggInfo;
	if ( oggInfo.loadInfo( filename ) )
	{
		filesize	= g_Player.GetFilesize( filename );
		duration	= g_Player.GetFileDuration( filename, FMOD_MSEC );
		bitrate		= oggInfo.GetBitrate();
		vbr			= true;

		artist		= oggInfo.GetArtist();
		title		= oggInfo.GetTitle();
		album		= oggInfo.GetAlbum();
		year		= oggInfo.GetYear();
		genre		= oggInfo.GetGenre();
		tracknum	= oggInfo.GetTrackNum();

		if ( artist.Length() < 0 )
			artist = justfilename;

		if ( album.Length() < 0 )
			album = justfilename;

		if ( title.Length() < 0 )
			title = justfilename;

		//--- run the query ---//
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "insert into songs values ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %Q, %d, %d );", NULL, NULL, NULL, NULL,	
			format,	
			vbr, 
			( const char* )ConvFNToFieldMB( filename ),
			( const char* )ConvDBFieldToMB( artist ), 
			( const char* )ConvDBFieldToMB( title ), 
			( const char* )ConvDBFieldToMB( album ), 
			tracknum, 
			( const char* )ConvDBFieldToMB( year ), 
			( const char* )ConvDBFieldToMB( genre ), 
			rating, 
			bitrate, 
			( const char* )ConvDBFieldToMB( lastplayed ), 
			( const char* )ConvDBFieldToMB( notes ), 
			timesplayed, 
			duration, 
			( const char* )ConvDBFieldToMB( m_TimeAdded ), 
			filesize,
			dirty );
	
	}
}

void CMusikLibrary::AddWav( const wxString & filename )
{
	//int format = MUSIK_FORMAT_WAV;
}

void CMusikLibrary::AddWMA( const wxString & filename )
{
	//int format = MUSIK_FORMAT_WMA;
}

void CMusikLibrary::AddAIFF( const wxString & filename )
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
static int sqlite_callbackAddToStringArray(void *args, int numCols, char **results, char **columnNames)
{

	wxArrayString * p = (wxArrayString*)args;
	p->Add( ConvDBFieldToWX( results[0] )); 
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
		query = wxT("select distinct artist from songs where ");
		break;

	case MUSIK_LIB_ALBUM:
		query = wxT("select distinct album from songs where ");
		break;

	case MUSIK_LIB_GENRE:
		query = wxT("select distinct genre from songs where ");
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
		query += wxT("order by artist");
		break;

	case MUSIK_LIB_ALBUM:
		query += wxT("order by album");
		break;

	case MUSIK_LIB_GENRE:
		query += wxT("order by genre");
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
static int sqlite_callbackAddToSongArray(void *args, int numCols, char **results, char **columnNames)
{

	CMusikSongArray * p = (CMusikSongArray*)args;

	CMusikSong *pLibItem = new CMusikSong();
	
	pLibItem->Filename		= wxConvertMB2WX	( results[0]);
	pLibItem->Title			= ConvDBFieldToWX	( results[1] );
	pLibItem->TrackNum		= StringToInt		( results[2] );
	pLibItem->Artist		= ConvDBFieldToWX	( results[3] );
	pLibItem->Album			= ConvDBFieldToWX	( results[4] );
	pLibItem->Genre			= ConvDBFieldToWX	( results[5] );
	pLibItem->Duration		= StringToInt		( results[6] );
	pLibItem->Format		= StringToInt		( results[7] );
	pLibItem->VBR			= StringToInt		( results[8] );
	pLibItem->Year			= ConvDBFieldToWX	( results[9] );
	pLibItem->Rating		= StringToInt		( results[10] );
	pLibItem->Bitrate		= StringToInt		( results[11] );
	pLibItem->LastPlayed	= ConvDBFieldToWX	( results[12] );
	pLibItem->Notes			= ConvDBFieldToWX	( results[13] );
	pLibItem->TimesPlayed	= StringToInt		( results[14] );
	pLibItem->TimeAdded		= ConvDBFieldToWX	( results[15] );
	pLibItem->Filesize		= StringToInt		( results[16] );
	
	p->Add( pLibItem );

    return 0;
}
void CMusikLibrary::GetSongs( const wxArrayString & aList, int nInType, CMusikSongArray & aReturn )
{
	aReturn.Clear();
	wxString sInfo;
	aReturn.Alloc(GetSongCount()); // optimize item adding performance,
  	wxString sQuery(wxT( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where "));
  
   
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
	if ( nInType == MUSIK_LIB_ARTIST )
    		sQuery +=wxT("order by artist,album,tracknum;");
	else if ( nInType == MUSIK_LIB_ALBUM )
		sQuery += wxT( "order by album,tracknum,artist;");
	else if ( nInType == MUSIK_LIB_GENRE )
		sQuery += wxT( "order by genre,artist,album,tracknum;");
	else if ( nInType == MUSIK_LIB_YEAR )
		sQuery += wxT( "order by year,artist,album,tracknum;");

	//--- run query ---//
	{
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec(m_pDB, ConvQueryToMB( sQuery ), &sqlite_callbackAddToSongArray, &aReturn, NULL);
	}
	aReturn.Shrink();
	return;
}

void CMusikLibrary::Query( const wxString & query, wxArrayString & aReturn )
{

	aReturn.Clear();
	wxString sInfo;

	//--- run the query ---//
	aReturn.Alloc( GetSongCount() );
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec(m_pDB, ConvQueryToMB( query ), &sqlite_callbackAddToStringArray, &aReturn, NULL);
}

static int sqlite_callbackAddToSongMap(void *args, int numCols, char **results, char **columnNames)
{
	//-------------------------------------------------------------------------//
	//--- maps filename to CMusingSong objects ptrs, ptrs because this		---//
	//--- way an additional call to a copy constructer is saved when adding	---//
	//--- the objects to the map											---//
	//-------------------------------------------------------------------------//

	myStringToMusikSongPtrMap * p = (myStringToMusikSongPtrMap*)args;

	CMusikSong *pLibItem = new CMusikSong();
	
	pLibItem->Filename		= wxConvertMB2WX	( results[0]);
	pLibItem->Title			= ConvDBFieldToWX	( results[1] );
	pLibItem->TrackNum		= StringToInt		( results[2] );
	pLibItem->Artist		= ConvDBFieldToWX	( results[3] );
	pLibItem->Album			= ConvDBFieldToWX	( results[4] );
	pLibItem->Genre			= ConvDBFieldToWX	( results[5] );
	pLibItem->Duration		= StringToInt		( results[6] );
	pLibItem->Format		= StringToInt		( results[7] );
	pLibItem->VBR			= StringToInt		( results[8] );
	pLibItem->Year			= ConvDBFieldToWX	( results[9] );
	pLibItem->Rating		= StringToInt		( results[10] );
	pLibItem->Bitrate		= StringToInt		( results[11] );
	pLibItem->LastPlayed	= ConvDBFieldToWX	( results[12] );
	pLibItem->Notes			= ConvDBFieldToWX	( results[13] );
	pLibItem->TimesPlayed	= StringToInt		( results[14] );
	pLibItem->TimeAdded		= ConvDBFieldToWX	( results[15] );
	pLibItem->Filesize		= StringToInt		( results[16] );
	
	(*p)[pLibItem->Filename]= pLibItem;

    return 0;
}
void CMusikLibrary::GetStdPlaylistSongs( const wxArrayString & aFiles, CMusikSongArray & aReturn )
{
	aReturn.Clear();
	
	wxString sQuery = wxT("select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where filename in (");

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

	aReturn.Alloc(aFiles.GetCount());
	for ( size_t i = 0; i < aFiles.GetCount(); i++ )
	{
		CMusikSong *pSong = theMap[ aFiles.Item( i ) ];
		wxASSERT_MSG( pSong, wxString(aFiles.Item( i ) + wxT( " is not on the map!" ) ) );

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


void CMusikLibrary::QuerySongs( const wxString & queryWhere, CMusikSongArray & aReturn )
{
	aReturn.Clear();
	wxString sInfo;

	//--- run query ---//
	wxString query(wxT("select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where "));
	query += queryWhere + wxT(";");

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


void CMusikLibrary::UpdateItemLastPlayed( const wxString & filename )
{

	wxDateTime currtime = wxDateTime::Now();
	wxString timestr = currtime.Format();

	int timesplayed = GetTimesPlayed( filename );
	timesplayed++;
	{
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "update songs set lastplayed = %Q, timesplayed = %d where filename = %Q;",
			NULL, NULL, NULL, ( const char* )ConvDBFieldToMB( timestr ), timesplayed, ( const char* )ConvFNToFieldMB( filename ) );
	}
}

int CMusikLibrary::GetTimesPlayed( const wxString & filename )
{
	char *query = sqlite_mprintf( "select timesplayed from songs where filename = %Q;", ( const char* )ConvFNToFieldMB( filename ) );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

int CMusikLibrary::GetSongCount()
{
	char *query = sqlite_mprintf( "select count(*) from songs;" );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}

bool CMusikLibrary::GetSongFromFilename( const wxString& filename, CMusikSong *pSong )
{
	char *query = sqlite_mprintf( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed,timeadded,filesize from songs where filename = %Q;", ( const char* )ConvFNToFieldMB( filename ) );
		
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	bool bFoundSong = false;
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
	{
		pSong->Filename		= wxConvertMB2WX	( coldata[0]);
		pSong->Title		= ConvDBFieldToWX	( coldata[1] );
		pSong->TrackNum		= StringToInt		( coldata[2] );
		pSong->Artist		= ConvDBFieldToWX	( coldata[3] );
		pSong->Album		= ConvDBFieldToWX	( coldata[4] );
		pSong->Genre		= ConvDBFieldToWX	( coldata[5] );
		pSong->Duration		= StringToInt		( coldata[6] );
		pSong->Format		= StringToInt		( coldata[7] );
		pSong->VBR			= StringToInt		( coldata[8] );
		pSong->Year			= ConvDBFieldToWX	( coldata[9] );
		pSong->Rating		= StringToInt		( coldata[10] );
		pSong->Bitrate		= StringToInt		( coldata[11] );
		pSong->LastPlayed	= ConvDBFieldToWX	( coldata[12] );
		pSong->Notes		= ConvDBFieldToWX	( coldata[13] );
		pSong->TimesPlayed	= StringToInt		( coldata[14] );	
		pSong->TimeAdded	= ConvDBFieldToWX	( coldata[15] );
		pSong->Filesize		= StringToInt		( coldata[16] );
		pSong->Filename		= filename;

		bFoundSong = true;
	}

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );

	return bFoundSong;
}

void CMusikLibrary::UpdateItem( const wxString & fn, CMusikSong & newsonginfo, bool bDirty )
{
	wxString filename( newsonginfo.Filename );
	if ( filename.Trim().Length() > 0 )
	{
		wxString OldFilename( fn );

		//--- run statement to update current item ---//

		int result = 0;
		{// keep lock as short as possible by using {} scope
			wxCriticalSectionLocker lock( m_csDBAccess );
			result = sqlite_exec_printf( m_pDB, "update songs set format=%d, vbr=%d, filename=%Q, artist=%Q, title=%Q, album=%Q, tracknum=%d, year=%Q, genre=%Q, rating=%d, bitrate=%d, lastplayed=%Q, notes=%Q, timesplayed=%d, duration=%d, timeadded=%Q, filesize=%d, dirty=%d where filename = %Q;", NULL, NULL, NULL, 
				newsonginfo.Format, 
				newsonginfo.VBR, 
				( const char* )ConvFNToFieldMB( filename ), 
				( const char* )ConvDBFieldToMB( newsonginfo.Artist ), 
				( const char* )ConvDBFieldToMB( newsonginfo.Title ), 
				( const char * )ConvDBFieldToMB( newsonginfo.Album ), 
				newsonginfo.TrackNum, 
				( const char* )ConvDBFieldToMB( newsonginfo.Year ), 
				( const char* )ConvDBFieldToMB( newsonginfo.Genre ), 
				newsonginfo.Rating, 
				newsonginfo.Bitrate, 
				( const char* )ConvDBFieldToMB( newsonginfo.LastPlayed ), 
				( const char* )ConvDBFieldToMB( newsonginfo.Notes ), 
				newsonginfo.TimesPlayed, 
				newsonginfo.Duration, 
				( const char* )ConvDBFieldToMB( newsonginfo.TimeAdded ), 
				newsonginfo.Filesize,
				(int)bDirty, 
				( const char* )ConvDBFieldToMB( OldFilename ) );
		}
		if ( result != SQLITE_OK )
			wxMessageBox( _( "An error occurred when attempting to update the database" ), MUSIK_VERSION, wxOK | wxICON_ERROR );
	}
	newsonginfo.Check1 = 0;
}

void CMusikLibrary::DeleteItem( const wxString & fn )
{
	wxString filename(fn); 
	if ( filename.Trim().Length() > 0 )
	{
		int result = 0;
		{// keep lock as short as possible by using {} scope
			wxCriticalSectionLocker lock( m_csDBAccess );
			result = sqlite_exec_printf( m_pDB, "delete from songs where filename=%Q;", NULL, NULL, NULL, ( const char* )ConvFNToFieldMB(filename) );
		}
		if ( result != SQLITE_OK )
			wxMessageBox( _( "An error occurred when attempting to delete a song from the database" ), MUSIK_VERSION, wxOK | wxICON_ERROR );
	}
}

int CMusikLibrary::GetSongDirCount( wxString sDir )
{
	char *query = sqlite_mprintf( "select count(*) from songs where filename like '%q%%';", ( const char* )ConvFNToFieldMB(sDir) );
	int result = QueryCount(query);
	sqlite_freemem( query );
	return result;
}
int CMusikLibrary::QueryCount(const char * szQuery )
{
	int result = 0;
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;

	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_compile( m_pDB, szQuery, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = atoi( coldata[0] );

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	return result;
}

void CMusikLibrary::SetRating( const wxString & sFile, int nVal )
{
	if ( nVal < 0 )
		nVal = 0;
	if ( nVal > 5 )
		nVal = 5;

	CMusikSong song;
	GetSongFromFilename( sFile, &song );
	song.Rating = nVal;
	UpdateItem( song.Filename, song, false );
}

void CMusikLibrary::CheckAndPurge( const wxString & filename )
{
	if ( !wxFileExists( filename ) )
		RemoveSong( filename );
}

void CMusikLibrary::RemoveSongDir( const wxString &  sDir )
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs where filename like '%q%%'", NULL, NULL, NULL, ( const char* )ConvFNToFieldMB(sDir) );	
}

void CMusikLibrary::RemoveSong( const wxString & sSong	)	
{
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs where filename = '%q'", NULL, NULL, NULL, ( const char* )ConvFNToFieldMB( sSong ) );
}

void CMusikLibrary::RemoveAll()
{ 
	wxCriticalSectionLocker lock( m_csDBAccess );
	sqlite_exec_printf( m_pDB, "delete from songs;", NULL, NULL, NULL );		
}

bool CMusikLibrary::ReplaceMask( wxString *sSrc, wxString sMask, wxString sTarget, bool bReplaceAll )
{
	wxString sCheck( *sSrc ); 
	sCheck.Replace( wxT( " " ), wxT( "" ), true );

	if ( !sCheck.IsEmpty() && sCheck != _( "<unknown>" ) )
	{
		sSrc->Replace( sMask, sTarget, bReplaceAll );
		return true;
	}

	return false;

}

bool CMusikLibrary::RenameFile( CMusikSong* song, bool bClearCheck )
{
	//--------------------------------//
	//--- new filename information ---//
	//--------------------------------//
	wxFileName filename( song->Filename );
	wxString sPrePath	= filename.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
	wxString sFile		= g_Prefs.sAutoRename;
	wxString sExt		= wxT(".") + filename.GetExt();

	wxString sCheck;

	//---------------------------------------------//
	//--- the song's root directory. this will	---//
	//--- only affect us if there is directory	---//
	//--- seperation stuff going on.			---//
	//---------------------------------------------//
	wxString sRootPath = sPrePath;
	if ( g_Prefs.sAutoRename.Find( wxString( MUSIK_PATH_SEPARATOR ) ) > - 1 )
	{
		if ( g_Paths.GetCount() == 0 )
			return false;

		sCheck = song->Filename;
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
	if ( song->TrackNum > 9 )
		sTrackNum.sprintf( wxT("%d"), song->TrackNum );
	else
		sTrackNum.sprintf( wxT("0%d"), song->TrackNum );	

	//----------------------------------------------//
	//--- replace masked values					 ---//
	//----------------------------------------------//
	if ( !ReplaceMask( &sFile, wxT( "%1" ), song->Title ) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%2" ), song->Artist ) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%3" ), song->Album ) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%4" ), song->Genre ) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%5" ), song->Year ) )
		return false;
	if ( !ReplaceMask( &sFile, wxT( "%6" ), sTrackNum ) )
		return false;

	//--- if we're in windows, we can't have certain characters, so blank them out ---//
	#ifdef __WXMSW__
	sFile.Replace( wxT("/"), wxT(""), true );
	sFile.Replace( wxT(":"), wxT(""), true );
	sFile.Replace( wxT("*"), wxT(""), true );
	sFile.Replace( wxT("?"), wxT(""), true );
	sFile.Replace( wxT("<"), wxT(""), true );
	sFile.Replace( wxT(">"), wxT(""), true );
	sFile.Replace( wxT("|"), wxT(""), true );
	sFile.Replace( wxT( "\"" ), wxT( "" ), true );
	#endif

	//-------------------------------------------------//
	//--- get just the filename of the current song	---//
	//--- c:\music\test\whatever\whatever.mp3 to...	---//
	//--- whatever.mp3								---//
	//-------------------------------------------------//
	wxString sJustFilename = GetJustFilename( sFile );
	sJustFilename.Trim( false ); sJustFilename.Trim( true );

	//-----------------------------------------//
	//--- create needed directories and do	---//
	//--- any sort of filename modification	---//
	//--- that is needed					---//
	//-----------------------------------------//
	wxArrayString aPaths = DelimitStr( sFile, wxString( MUSIK_PATH_SEPARATOR ), false );
	wxString sFinalPath = sRootPath;
	for ( size_t i = 0; i < aPaths.GetCount() - 1; i++ )
	{
		wxString sCur = aPaths.Item( i );
		sCur.Trim( false ); sCur.Trim( true );

		//---------------------------------------------//
		//--- don't use a period "." for directory	---//
		//--- names in windows unless you want a	---//
		//--- headache								---//
		//---------------------------------------------//
		#ifdef __WXMSW__
				sCur.Replace( wxT( "." ), wxT( "" ) );
		#endif

		if ( !sCur.IsEmpty() )
		{
			sFinalPath += sCur;
			if ( !wxDirExists( sFinalPath ) )
				wxMkdir( sFinalPath );
		}
	}

	//--- final name ---//
	wxString oldfilename = song->Filename;
	wxString newfilename = sFinalPath + sJustFilename + sExt;

	//--- filename already the same? return ---//
	if ( oldfilename == newfilename )
	{
		song->Filename = newfilename;
		g_Library.UpdateItem( oldfilename, *song, false );

		if ( bClearCheck )
			song->Check1 = 0;

		return true;
	}

	//-----------------------------------------//
	//--- file does need to be renamed, so	---//
	//--- rename it, then return			---//
	//-----------------------------------------//
	if ( wxRenameFile( oldfilename, newfilename ) )
	{
		song->Filename = newfilename;
		g_Library.UpdateItem( oldfilename, *song, false );

		if ( bClearCheck )
			song->Check1 = 0;

		return true;
	}
	
	//-----------------------------------------//
	//--- how could we get here? we can't	---//
	//--- so just return false				---//
	//-----------------------------------------//
	return false;
}

bool CMusikLibrary::RetagFile( CMusikSong* Song )
{
	wxString sMask	= g_Prefs.sAutoTag;
	wxString sFile	= Song->Filename;

	//-------------------------------------------------//
	//--- get rid of the file extension.			---//
	//-------------------------------------------------//
	sFile = sFile.Left( sFile.Length() - 4 );

	//-------------------------------------------------//
	//--- get the order of the values in the mask,	---//
	//--- as well as the order of the delimiters	---//
	//--- (space inbetween the mask)				---//
	//---											---//
	//--- Example:									---//
	//---    %1_-_%2_-_%3-.mp3						---//
	//-------------------------------------------------//
	wxArrayString aMaskDelimiters = DelimitStr( sMask, wxT("%"), true );
	aMaskDelimiters.Remove( 0, 1 );	//--- will be blank. working right to left, not left to right ---//

	wxString sIsNumber;
	wxArrayString aMaskOrder;
	for ( size_t i = 0; i < aMaskDelimiters.GetCount(); i++ )
	{
		sIsNumber= aMaskDelimiters.Item( i ).Left( 1 );
		if( sIsNumber.IsNumber() )
		{
			aMaskOrder.Add( sIsNumber );
			aMaskDelimiters.Item( i ) = aMaskDelimiters.Item( i ).Right( aMaskDelimiters.Item( i ).Length() - 1 );	//--- chop off mask value ---//
		}

		else
		{
			wxMessageBox( wxT( "Invalid Mask." ) );
			return false;
		}
	}

	//-------------------------------------------------//
	//--- sort delimiters in order of length		---//
	//-------------------------------------------------//
	SortArrayByLength( &aMaskDelimiters );

	//-------------------------------------------------//
	//--- Convert path separators and delimiters	---// 
	//--- to newline constants ("\n")				---//
	//-------------------------------------------------//
	sFile.Replace( wxString( MUSIK_PATH_SEPARATOR ), wxT( "\n" ), true );

	for( size_t i = 0; i < aMaskDelimiters.GetCount() - 1; i++ )
		sFile.Replace( aMaskDelimiters.Item( i ), wxT( "\n" ), true );

	//-------------------------------------------------//
	//--- delimit the string for future parsing		---//
	//-------------------------------------------------//
	wxArrayString aFileTokens = DelimitStr( sFile, wxT( "\n" ), true );
	for( size_t i = 0; i < aFileTokens.GetCount(); i++ )
	{
		aFileTokens.Item( i ).Trim( true );
		aFileTokens.Item( i ).Trim( false );
	}

	//-------------------------------------------------//
	//--- reading right to left, (bottom to top in	---//
	//--- array terms), assign the needed values	---//
	//--- to the new song.							---//
	//-------------------------------------------------//
	size_t nTokenIndex	= aFileTokens.GetCount() - 1;
	size_t nMaskIndex	= aMaskOrder.GetCount() - 1;

	for( size_t i = 0; i < aMaskOrder.GetCount(); i++ )
	{
		switch ( wxStringToInt( aMaskOrder.Item( nMaskIndex - i ) ) )
		{
		case 1:
			Song->Title = aFileTokens.Item	( nTokenIndex - i );
			break;
		case 2:
			Song->Artist = aFileTokens.Item	( nTokenIndex - i );
			break;
		case 3:
			Song->Album = aFileTokens.Item	( nTokenIndex - i );
			break;
		case 4:
			Song->Genre = aFileTokens.Item	( nTokenIndex - i );
			break;
		case 5:
			Song->Year = aFileTokens.Item	( nTokenIndex - i );
			break;
		case 6:
			Song->TrackNum = wxStringToInt( aFileTokens.Item( nTokenIndex - i ) );
			break;
		default:
			return false;
			break;
		}
	}

	g_Library.UpdateItem( Song->Filename, *Song, true );
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

void CMusikLibrary::GetAllSongs( CMusikSongArray & aReturn )	
{ 
	QuerySongs( wxT("filename <> ''"), aReturn );												
}

void CMusikLibrary::GetAllArtists( wxArrayString & aReturn )	
{ 
	Query( wxT("select distinct artist from songs order by artist;"), aReturn );			
}

void CMusikLibrary::GetAllAlbums( wxArrayString & aReturn )	
{ 
	Query( wxT("select distinct album from songs order by album;"), aReturn );					
}

void CMusikLibrary::GetAllGenres( wxArrayString & aReturn )	
{ 
	Query( wxT("select distinct genre from songs order by genre;"), aReturn );					
}

void CMusikLibrary::GetArtistAlbums( const wxArrayString & aArtists, wxArrayString & aReturn )	
{ 
	GetInfo( aArtists, MUSIK_LIB_ARTIST, MUSIK_LIB_ALBUM, aReturn );				
}

void CMusikLibrary::GetArtistGenres( const wxArrayString & aArtists, wxArrayString & aReturn )
{ 
	GetInfo( aArtists, MUSIK_LIB_ARTIST, MUSIK_LIB_GENRE, aReturn );					
}

void CMusikLibrary::GetArtistYears( const wxArrayString & aArtists, wxArrayString & aReturn )	
{ 
	GetInfo( aArtists, MUSIK_LIB_ARTIST, MUSIK_LIB_YEAR, aReturn );		
}

void CMusikLibrary::GetArtistSongs( const wxArrayString & aArtists, CMusikSongArray & aReturn )
{ 
	GetSongs( aArtists, MUSIK_LIB_ARTIST, aReturn );									
}

void CMusikLibrary::GetAlbumArtists( const wxArrayString & aAlbums, wxArrayString & aReturn )	
{ 
	GetInfo( aAlbums, MUSIK_LIB_ALBUM, MUSIK_LIB_ARTIST, aReturn );			
}

void CMusikLibrary::GetAlbumGenres( const wxArrayString & aAlbums, wxArrayString & aReturn )
{ 
	GetInfo( aAlbums, MUSIK_LIB_ALBUM, MUSIK_LIB_GENRE, aReturn );	
}

void CMusikLibrary::GetAlbumYears( const wxArrayString & aAlbums, wxArrayString & aReturn )	
{ 
	GetInfo( aAlbums, MUSIK_LIB_ALBUM, MUSIK_LIB_YEAR, aReturn );		
}

void CMusikLibrary::GetAlbumSongs( const wxArrayString & aAlbums, CMusikSongArray & aReturn )	
{ 
	GetSongs( aAlbums, MUSIK_LIB_ALBUM, aReturn );
}

void CMusikLibrary::GetGenreArtists( const wxArrayString & aGenres, wxArrayString & aReturn )	
{ 
	GetInfo( aGenres, MUSIK_LIB_GENRE, MUSIK_LIB_ARTIST, aReturn );		
}

void CMusikLibrary::GetGenreAlbums( const wxArrayString & aGenres, wxArrayString & aReturn )	
{ 
	GetInfo( aGenres, MUSIK_LIB_GENRE, MUSIK_LIB_ALBUM, aReturn );	
}

void CMusikLibrary::GetGenreYears( const wxArrayString & aGenres, wxArrayString & aReturn )	
{
	GetInfo( aGenres, MUSIK_LIB_GENRE, MUSIK_LIB_YEAR, aReturn );	
}

void CMusikLibrary::GetGenreSongs( const wxArrayString & aGenres, CMusikSongArray & aReturn )	
{ 
	GetSongs( aGenres, MUSIK_LIB_GENRE, aReturn );
}

void CMusikLibrary::GetYearArtists( const wxArrayString & aYears, wxArrayString & aReturn )	
{ 
	GetInfo( aYears, MUSIK_LIB_YEAR, MUSIK_LIB_ARTIST, aReturn );				
}

void CMusikLibrary::GetYearAlbums( const wxArrayString & aYears, wxArrayString & aReturn )	
{ 
	GetInfo( aYears, MUSIK_LIB_YEAR, MUSIK_LIB_ALBUM, aReturn );		
}

void CMusikLibrary::GetYearGenres( const wxArrayString & aYears, wxArrayString & aReturn )	
{ 
	GetInfo( aYears, MUSIK_LIB_YEAR, MUSIK_LIB_GENRE, aReturn );			
}

void CMusikLibrary::GetYearSongs( const wxArrayString & aYears, CMusikSongArray & aReturn )	
{ 
	GetSongs( aYears, MUSIK_LIB_YEAR, aReturn );		
}
