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
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
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
	//--- close database if necessary ---//
	if ( m_pDB )
	{
		sqlite_close( m_pDB );
		m_pDB = NULL;
	}
}

void CMusikLibrary::Shutdown()
{
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
	
	//--- load the database ---//

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
		query += wxT( "dirty number(10) "				);
		query += wxT( " );"								);
	}

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
	//wxMutexLocker lock( LibMutex );

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
	int dirty = 0;

	//--- first get the things that can be gleaned from the header ---//
	CMP3Info mp3info;
	if ( mp3info.loadInfo( filename ) == 0 )
	{
		duration	= mp3info.getLengthInSeconds() * 1000;
		bitrate		= mp3info.getBitrate();
		vbr			= mp3info.isVBitRate();
		
		//--- link and load mp3 ---//
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
		sqlite_exec_printf( m_pDB, "insert into songs values ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %d );", NULL, NULL, NULL, NULL,
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
			dirty );

		ID3_FreeString(szArtist);
		ID3_FreeString(szTitle);
		ID3_FreeString(szAlbum);
		ID3_FreeString(szGenre);
		ID3_FreeString(szYear);
		ID3_FreeString(szTrack);


	}
}

void CMusikLibrary::WriteMP3Tag( const wxString & filename, bool ClearAll )
{
	//wxMutexLocker lock( LibMutex );
	CMusikSong song;

	if ( GetSongFromFilename( filename, &song ) )
	{
		ID3_Tag	id3Tag;
		id3Tag.Link( ( const char* )ConvFNToFieldMB( filename ) , ID3TT_ALL );

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
		ID3_AddGenre	( &id3Tag, GetGenreID( song.Genre ),						true );

		//--- write to file ---//
		id3Tag.Update();
	}
}

bool CMusikLibrary::WriteOGGTag( const wxString & filename, bool ClearAll )
{
	//wxMutexLocker lock( LibMutex );

	//--- grab song from DB comments ---//
	CMusikSong song;
	if ( GetSongFromFilename( filename, &song ) )
	{
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

	return false;
}

void CMusikLibrary::ClearDirtyTags( bool bInform )
{
	CMusikSongArray aDirty = g_Library.QuerySongs( wxT("dirty = 1") );
	size_t nCount = aDirty.GetCount();
	for( size_t i = 0; i < nCount; i++ )
		UpdateItem( aDirty.Item( i ).Filename, aDirty.Item( i ), false );
	
	if ( bInform )
	{
		if ( nCount < 1 )
			wxMessageBox( _( "There are no pending tags to finalize." ), MUSIK_VERSION, wxICON_INFORMATION );
		else
			wxMessageBox( IntTowxString( nCount ) + wxT( " tags pending to be written were finalized for the database only. These tags will not be written to file." ), MUSIK_VERSION, wxICON_INFORMATION );
	}
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

	COggInfo oggInfo;
	if ( oggInfo.loadInfo( filename ) )
	{
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
		sqlite_exec_printf( m_pDB, "insert into songs values ( %Q, %d, %d, %Q, %Q, %Q, %Q, %d, %Q, %Q, %d, %d, %Q, %Q, %d, %d, %d );", NULL, NULL, NULL, NULL,	
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

wxArrayString CMusikLibrary::VerifyYearList( wxArrayString *aList )
{
	wxArrayString aVerifiedList;
	for ( int i = 0; i < (int)aList->GetCount(); i++ )
	{
		if ( ( !aList->Item( i ).IsEmpty() ) && ( aList->Item( i ).IsNumber() ) )
			aVerifiedList.Add( aList->Item( i ) );
	}
	return aVerifiedList;
}

wxArrayString CMusikLibrary::GetInfo( wxArrayString *aList, int nInType, int nOutType )
{
	wxArrayString aReturn;
	wxString sInfo;

	wxString query;

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

	for ( int i = 0; i < (int)aList->GetCount(); i++ )
	{
		wxString itemstr = aList->Item( i );
		itemstr.Replace( wxT("'"), wxT("''") );

		if ( i > 0 )
			query += wxT("or ");

		switch ( nInType )
		{
		case MUSIK_LIB_ARTIST:
			query += wxT("artist = '") + itemstr + wxT("' ");
			break;

		case MUSIK_LIB_ALBUM:
			query += wxT("album = '") + itemstr + wxT("' ");
			break;

		case MUSIK_LIB_GENRE:
			query += wxT("genre = '") + itemstr + wxT("' ");
			break;

		case MUSIK_LIB_YEAR:
			query += wxT("year = '") + itemstr + wxT("' ");
			break;

		case MUSIK_LIB_DECADE:
			break;
		}

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
	const char *pTail;
	sqlite_vm *pVM;
	
	
	sqlite_compile( m_pDB, ConvQueryToMB( query ), &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- at each row of the result ---//
	while ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
	{
		wxString sItem = ConvDBFieldToWX( coldata[0] );
		aReturn.Add( sItem );
	}

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	if ( nOutType == MUSIK_LIB_YEAR )
		return VerifyYearList( &aReturn );
	else		
		return aReturn;
}

CMusikSongArray CMusikLibrary::GetSongs( wxArrayString *aList, int nInType )
{
	CMusikSongArray aReturn;
	wxString sInfo;

	//--- for each item in the input list, we're going to query the database ---//
	for ( int i = 0; i < (int)aList->GetCount(); i++ )
	{	
		//--- decide what the query should be ---//
		char *query;
		wxCharBuffer ItemStr =  ConvQueryToMB(aList->Item( i )) ;
			
		if ( nInType == MUSIK_LIB_ARTIST )
			query = sqlite_mprintf( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where artist = %Q order by album,tracknum;", (const char*)ItemStr );
		else if ( nInType == MUSIK_LIB_ALBUM )
			query = sqlite_mprintf( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where album = %Q order by album,tracknum;", (const char*)ItemStr  );
		else if ( nInType == MUSIK_LIB_GENRE )
			query = sqlite_mprintf( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where genre = %Q order by album,tracknum;", (const char*)ItemStr  );
		else if ( nInType == MUSIK_LIB_YEAR )
			query = sqlite_mprintf( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where year = %Q order by album,tracknum;", (const char*)ItemStr  );

		//--- run query ---//
		const char *pTail;
		sqlite_vm *pVM;
		
		sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
		char *errmsg;
		int numcols = 0;
		const char **coldata;
		const char **coltypes;
		aReturn.Alloc( GetSongCount() ); // optimize item adding performance, 
		//--- look at each row ---//
		while ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		{
			CMusikSong *pLibItem = new CMusikSong();// adding pointer to objects saves the calling of copy constructor, in contrast to adding object
			
			pLibItem->Filename		= ConvFNToFieldWX	( coldata[0]);
			pLibItem->Title			= ConvDBFieldToWX	( coldata[1] );
			pLibItem->TrackNum		= StringToInt		( coldata[2] );
			pLibItem->Artist		= ConvDBFieldToWX	( coldata[3] );
			pLibItem->Album			= ConvDBFieldToWX	( coldata[4] );
			pLibItem->Genre			= ConvDBFieldToWX	( coldata[5] );
			pLibItem->Duration		= StringToInt		( coldata[6] );
			pLibItem->Format		= StringToInt		( coldata[7] );
			pLibItem->VBR			= StringToInt		( coldata[8] );
			pLibItem->Year			= ConvDBFieldToWX	( coldata[9] );
			pLibItem->Rating		= StringToInt		( coldata[10] );
			pLibItem->Bitrate		= StringToInt		( coldata[11] );
			pLibItem->LastPlayed	= ConvDBFieldToWX	( coldata[12] );
			pLibItem->Notes			= ConvDBFieldToWX	( coldata[13] );
			pLibItem->TimesPlayed	= StringToInt		( coldata[14] );

			aReturn.Add( pLibItem );// array takes ownerhip of object
		}

		//--- close up ---//
		sqlite_finalize( pVM, &errmsg );
		sqlite_freemem( query );
	}
	aReturn.Shrink();
	return aReturn;
}

wxArrayString CMusikLibrary::Query( const wxString & query )
{
	//wxMutexLocker lock( LibMutex );

	wxArrayString aReturn;
	wxString sInfo;

	//--- run the query ---//
	const char *pTail;
	sqlite_vm *pVM;
	sqlite_compile( m_pDB, ConvQueryToMB( query ), &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;
	if(pVM)
	{
		aReturn.Alloc( GetSongCount() );
		//--- look at each row of the result ---//
		while ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		{
			
			aReturn.Add( wxConvertMB2WX( coldata[0]) );
		}

		//--- close up ---//
		sqlite_finalize( pVM, &errmsg );
		aReturn.Shrink();
	}
	return aReturn;
}

CMusikSongArray CMusikLibrary::GetStdPlaylistSongs( wxArrayString *aFiles )
{
	CMusikSongArray aReturn;
	
	wxString sQuery = wxT("select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where filename in (");

	sQuery.Alloc(sQuery.Len() + aFiles->GetCount() * 30); // optimization ( the 30 is a wild guess)
	for ( size_t i = 0; i < aFiles->GetCount(); i++ )
	{
		//--- if song has a ' ---//	
		wxString filename( aFiles->Item( i ) );
		filename.Replace( wxT("'"), wxT("''"), TRUE );

		sQuery += wxT("'");
		sQuery += filename;

		//--- not at the end ---//
		if ( i != aFiles->GetCount() - 1 )
			sQuery += wxT("', ");

		//--- at the end ---//
		else
			sQuery += wxT("' );");
	}

	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	char *errmsg;
	sqlite_compile( m_pDB, ConvQueryToMB( sQuery ), &pTail, &pVM, &errmsg );

	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- pVM will be null if the query was invalid ---//
	if ( pVM )
	{
		//-------------------------------------------------------------------------//
		//--- maps filename to CMusingSong objects ptrs, ptrs because this		---//
		//--- way an additional call to a copy constructer is saved when adding	---//
		//--- the objects to the map											---//
		//-------------------------------------------------------------------------//
		myStringToMusikSongPtrMap theMap;
	
		//--- look at each row of the result ---//
		while ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		{
			wxString sCurrFile = wxConvertMB2WX( coldata[0]);
			CMusikSong *pSong = new CMusikSong();

			//---------------------------------------------------------------------//
			//--- we fill the map and afterwards a array from the map because	---//
			//--- we can have multiple filenames in the same list				---//
			//---------------------------------------------------------------------//
			pSong->Filename		= 	sCurrFile;
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

			theMap[sCurrFile]	= pSong;            
		}

		//--- close up ---//
		sqlite_finalize( pVM, &errmsg );
		aReturn.Alloc(aFiles->GetCount());


		for ( size_t i = 0; i < aFiles->GetCount(); i++ )
		{
			CMusikSong *pSong = theMap[ aFiles->Item( i ) ];
			wxASSERT_MSG( pSong, wxString(aFiles->Item( i ) + wxT( " is not on the map!" ) ) );

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
	}
	return aReturn;

}

CMusikSongArray CMusikLibrary::QuerySongs( const wxString & queryWhere )
{
	//wxMutexLocker lock( LibMutex );

	CMusikSongArray aReturn;
	wxString sInfo;

	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	char *errmsg;

	wxString query(wxT("select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where "));
	query += queryWhere + wxT(";");
	const wxCharBuffer pQuery = ConvQueryToMB(query);
	sqlite_compile( m_pDB, pQuery, &pTail, &pVM, &errmsg );
	
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- pVM will be null if the query was invalid ---//
	if ( pVM )
	{
		aReturn.Alloc(GetSongCount());
		//--- look at each row of the result ---//
		while ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		{
			CMusikSong *pLibItem = new CMusikSong();;
			
			pLibItem->Filename		= wxConvertMB2WX	( coldata[0]);
			pLibItem->Title			= ConvDBFieldToWX	( coldata[1] );
			pLibItem->TrackNum		= StringToInt		( coldata[2] );
			pLibItem->Artist		= ConvDBFieldToWX	( coldata[3] );
			pLibItem->Album			= ConvDBFieldToWX	( coldata[4] );
			pLibItem->Genre			= ConvDBFieldToWX	( coldata[5] );
			pLibItem->Duration		= StringToInt		( coldata[6] );
			pLibItem->Format		= StringToInt		( coldata[7] );
			pLibItem->VBR			= StringToInt		( coldata[8] );
			pLibItem->Year			= ConvDBFieldToWX	( coldata[9] );
			pLibItem->Rating		= StringToInt		( coldata[10] );
			pLibItem->Bitrate		= StringToInt		( coldata[11] );
			pLibItem->LastPlayed	= ConvDBFieldToWX	( coldata[12] );
			pLibItem->Notes			= ConvDBFieldToWX	( coldata[13] );
			pLibItem->TimesPlayed	= StringToInt		( coldata[14] );
			
			aReturn.Add( pLibItem );
		}
		//--- close up ---//
		sqlite_finalize( pVM, &errmsg );
	}
	aReturn.Shrink();
	return aReturn;
}


void CMusikLibrary::UpdateItemLastPlayed( const wxString & filename )
{

	wxDateTime currtime = wxDateTime::Now();
	wxString timestr = currtime.Format();

	int timesplayed = GetTimesPlayed( filename );
	timesplayed++;
	sqlite_exec_printf( m_pDB, "update songs set lastplayed = %Q, timesplayed = %d where filename = %Q;",
		NULL, NULL, NULL, ( const char* )ConvDBFieldToMB( timestr ), timesplayed, ( const char* )ConvFNToFieldMB( filename ) );
	
}

int CMusikLibrary::GetTimesPlayed( const wxString & filename )
{
	int result = 0;

	char *query;
	query = sqlite_mprintf( "select timesplayed from songs where filename = %Q;", ( const char* )ConvFNToFieldMB( filename ) );
	
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = StringToInt( coldata[0] );

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );
	return result;
}

int CMusikLibrary::GetSongCount()
{
	int result = 0;

	char *query;
	query = sqlite_mprintf( "select count(*) from songs;" );
	
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;
	if(pVM)
	{
		//--- look and see if there's one row ---//
		if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
			result = StringToInt ( coldata[0] );

		//--- close up ---//
		sqlite_finalize( pVM, &errmsg );
	}
	sqlite_freemem( query );
	return result;
}

bool CMusikLibrary::GetSongFromFilename( const wxString& filename, CMusikSong *pSong )
{
	char *query = sqlite_mprintf( "select filename,title,tracknum,artist,album,genre,duration,format,vbr,year,rating,bitrate,lastplayed,notes,timesplayed from songs where filename = %Q;", ( const char* )ConvFNToFieldMB( filename ) );

	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	bool m_FoundSong = false;
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
		pSong->Filename		= filename;

		m_FoundSong = true;
	}

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );

	return m_FoundSong;
}

void CMusikLibrary::UpdateItem( const wxString & fn, CMusikSong & newsonginfo, bool bDirty )
{
	//wxMutexLocker lock( LibMutex );
	wxString filename( newsonginfo.Filename );
	if ( filename.Trim().Length() > 0 )
	{
		wxString OldFilename( fn );

		//--- run statement to update current item ---//
		int result = sqlite_exec_printf( m_pDB, "update songs set format=%d, vbr=%d, filename=%Q, artist=%Q, title=%Q, album=%Q, tracknum=%d, year=%Q, genre=%Q, rating=%d, bitrate=%d, lastplayed=%Q, notes=%Q, timesplayed=%d, duration=%d, dirty=%d where filename = %Q;", NULL, NULL, NULL, 
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
			(int)bDirty, 
			( const char* )ConvDBFieldToMB( OldFilename ) );

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
		int result = sqlite_exec_printf( m_pDB, "delete from songs where filename=%Q;", NULL, NULL, NULL, ( const char* )ConvFNToFieldMB(filename) );
		if ( result != SQLITE_OK )
			wxMessageBox( _( "An error occurred when attempting to delete a song from the database" ), MUSIK_VERSION, wxOK | wxICON_ERROR );
	}
}

int CMusikLibrary::GetSongDirCount( wxString sDir )
{
	//wxMutexLocker lock( LibMutex );

	int result = 0;

	char *query;
	query = sqlite_mprintf( "select count(*) from songs where filename like '%q%%';", ( const char* )ConvFNToFieldMB(sDir) );
	
	//--- run query ---//
	const char *pTail;
	sqlite_vm *pVM;
	
	sqlite_compile( m_pDB, query, &pTail, &pVM, NULL );
	char *errmsg;
	int numcols = 0;
	const char **coldata;
	const char **coltypes;

	//--- look and see if there's one row ---//
	if ( sqlite_step( pVM, &numcols, &coldata, &coltypes ) == SQLITE_ROW )
		result = StringToInt( coldata[0] );

	//--- close up ---//
	sqlite_finalize( pVM, &errmsg );
	sqlite_freemem( query );
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
	//wxMutexLocker lock( LibMutex );

	if ( !wxFileExists( filename ) )
		RemoveSong( filename );
}

void CMusikLibrary::RemoveSongDir( const wxString &  sDir )
{
	//wxMutexLocker lock( LibMutex );
	sqlite_exec_printf( m_pDB, "delete from songs where filename like '%q%%'", NULL, NULL, NULL, ( const char* )ConvFNToFieldMB(sDir) );	
}

void CMusikLibrary::RemoveSong( const wxString & sSong	)	
{
	sqlite_exec_printf( m_pDB, "delete from songs where filename = '%q'", NULL, NULL, NULL, ( const char* )ConvFNToFieldMB( sSong ) );
}

void CMusikLibrary::RemoveAll()
{ 
	sqlite_exec_printf( m_pDB, "delete from songs;", NULL, NULL, NULL );		
}

bool CMusikLibrary::RenameFile( CMusikSong* song, bool bClearCheck )
{
	//wxMutexLocker lock( LibMutex );

	//--------------------------------//
	//--- new filename information ---//
	//--------------------------------//
	wxFileName filename( song->Filename );
	wxString sPrePath	= filename.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
	wxString sFile		= g_Prefs.sAutoRename;
	wxString sExt		= wxT(".") + filename.GetExt();

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

		wxString sCheck = song->Filename;
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

	if ( sFile.Replace( wxT("%1"), song->Title, true ) > 0 && ( song->Title == wxT("") || song->Title == _("<unknown>") ) )
		return false;
	if ( sFile.Replace( wxT("%2"), song->Artist, true ) > 0 && ( song->Artist == wxT("") || song->Artist == _("<unknown>") ) )
		return false;
	if ( sFile.Replace( wxT("%3"), song->Album, true ) > 0 && ( song->Album == wxT("") || song->Album == _("<unknown>") ) ) 
		return false;
	if ( sFile.Replace( wxT("%4"), song->Genre, true ) > 0 && ( song->Genre == wxT("") || song->Genre == _("<unknown>") ) ) 
		return false;
	if ( sFile.Replace( wxT("%5"), song->Year, true ) > 0 && ( song->Year == wxT("") || song->Year == _("<unknown>") ) )
		return false;
	sFile.Replace( wxT("%6"), sTrackNum, true );

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
	sFile = aPaths.Item( aPaths.GetCount() - 1 );
	sFile.Trim( false ); sFile.Trim( true );

	//--- final name ---//
	wxString oldfilename = song->Filename;
	wxString newfilename = sFinalPath + sFile + sExt;

	//-----------------------------------------//
	//--- filename already the same? return ---//
	//-----------------------------------------//
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

bool CMusikLibrary::CheckTokenForInt( wxArrayString aTokens, size_t nStart, size_t nEnd )
{
	wxString *sTemp = new wxString;
	for( size_t i = 0; i < aTokens.GetCount(); i++ )
	{
		if( aTokens.Item( i ).StartsWith( wxT("2"), sTemp ) )
			wxMessageBox( wxT( "success" ) );
		else
		{
			wxMessageBox( wxT( "failure" ) );
		}
	}
	return true;
}

bool CMusikLibrary::RetagFile( CMusikSong* song )
{
	CMusikSong* NewSong		= new CMusikSong;
	wxFileName	filename	( song->Filename );

	wxString	sMask		= g_Prefs.sAutoTag;
	size_t		nValidStart	= 1;
	size_t		nValidEnd	= 6;

	wxString	sPath		= filename.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
	wxString	sFile		= filename.GetName();
	wxString	sExt		= wxT(".") + filename.GetExt();

	wxArrayString aMaskToken	= DelimitStr( sMask, wxT("%"), true );
	for( int i = 0; i < aMaskToken.GetCount(); i++ )
	{
		wxMessageBox( aMaskToken.Item( i ) );
	}
	if( !CheckTokenForInt( aMaskToken, nValidStart, nValidEnd ) )
	{
		wxMessageBox( wxT("Not a valid mask.") );
	}
/*
	sMask.Replace( wxT("%"), wxT(""), 1 );
	sMask.Replace( wxT(" "), wxT(""), 1 );

	size_t nMaskDel		= GetDelimitCount( sMask, wxT("-") );
	size_t nFilenameDel = GetDelimitCount( sFile, wxT("-") );

	wxArrayString aMask		= DelimitStr( sMask, wxT("%"), true );
	wxArrayString aTagInfo	= DelimitStr( sFile, wxT("-"), true );	

/*	1 - song title
	2 - artist name
	3 - album name
	4 - genre
	5 - year
	6 - track number
*/
/*	if( nMaskDel == nFilenameDel )
	{
		for( size_t i = 0; i < nMaskDel; i++ )
		{
			switch ( wxStringToInt( aTagInfo.Item( i ) ) )
			{
			case 1:
				NewSong->Title = aTagInfo.Item( i );
				break;
			case 2:
				NewSong->Artist = aTagInfo.Item( i );
				break;
			case 3:
				NewSong->Album = aTagInfo.Item( i );
				break;
			case 4:
				NewSong->Genre = aTagInfo.Item( i );
				break;
			case 5:
				NewSong->Year = aTagInfo.Item( i );
				break;
			case 6:
				NewSong->TrackNum = wxStringToInt( aTagInfo.Item( i ) );
				break;
			default:
				wxMessageBox( wxT("Invalid auto-tag pattern") );
				break;
			}
		}

//		g_Library.UpdateItem( 

		wxMessageBox( NewSong->Title );
		wxMessageBox( NewSong->Artist );
		wxMessageBox( NewSong->Album );
		wxMessageBox( NewSong->Genre );
		wxMessageBox( NewSong->Year );
		wxMessageBox( IntTowxString( NewSong->TrackNum ) );
	}
*/
	return true;
}



//-----------------------------------//
//--- pre-defined queries to make ---//
//---   life a little bit easier  ---//
//-----------------------------------//
wxArrayString CMusikLibrary::GetAllYears()	
{ 
	wxArrayString years = Query( wxT("select distinct year from songs order by year;") );
	VerifyYearList( &years );
	return years;	
}

CMusikSongArray CMusikLibrary::GetAllSongs()	
{ 
	return QuerySongs( wxT("filename <> ''") );												
}

wxArrayString CMusikLibrary::GetAllArtists()	
{ 
	return Query( wxT("select distinct artist from songs order by artist;") );			
}

wxArrayString CMusikLibrary::GetAllAlbums()	
{ 
	return Query( wxT("select distinct album from songs order by album;") );					
}

wxArrayString CMusikLibrary::GetAllGenres()	
{ 
	return Query( wxT("select distinct genre from songs order by genre;") );					
}

wxArrayString CMusikLibrary::GetArtistAlbums( wxArrayString *aArtists )	
{ 
	return GetInfo( aArtists, MUSIK_LIB_ARTIST, MUSIK_LIB_ALBUM );				
}

wxArrayString CMusikLibrary::GetArtistGenres( wxArrayString *aArtists )
{ 
	return GetInfo( aArtists, MUSIK_LIB_ARTIST, MUSIK_LIB_GENRE );					
}

wxArrayString CMusikLibrary::GetArtistYears( wxArrayString *aArtists )	
{ 
	return GetInfo( aArtists, MUSIK_LIB_ARTIST, MUSIK_LIB_YEAR );		
}

CMusikSongArray CMusikLibrary::GetArtistSongs( wxArrayString *aArtists )
{ 
	return GetSongs( aArtists, MUSIK_LIB_ARTIST );									
}

wxArrayString CMusikLibrary::GetAlbumArtists( wxArrayString *aAlbums )	
{ 
	return GetInfo( aAlbums, MUSIK_LIB_ALBUM, MUSIK_LIB_ARTIST );			
}

wxArrayString CMusikLibrary::GetAlbumGenres( wxArrayString *aAlbums	)
{ 
	return GetInfo( aAlbums, MUSIK_LIB_ALBUM, MUSIK_LIB_GENRE );	
}

wxArrayString CMusikLibrary::GetAlbumYears( wxArrayString *aAlbums )	
{ 
	return GetInfo( aAlbums, MUSIK_LIB_ALBUM, MUSIK_LIB_YEAR );		
}

CMusikSongArray CMusikLibrary::GetAlbumSongs( wxArrayString *aAlbums )	
{ 
	return GetSongs( aAlbums, MUSIK_LIB_ALBUM );
}

wxArrayString CMusikLibrary::GetGenreArtists( wxArrayString *aGenres )	
{ 
	return GetInfo( aGenres, MUSIK_LIB_GENRE, MUSIK_LIB_ARTIST );		
}

wxArrayString CMusikLibrary::GetGenreAlbums( wxArrayString *aGenres	)	
{ 
	return GetInfo( aGenres, MUSIK_LIB_GENRE, MUSIK_LIB_ALBUM );	
}

wxArrayString CMusikLibrary::GetGenreYears( wxArrayString *aGenres )	
{
	return GetInfo( aGenres, MUSIK_LIB_GENRE, MUSIK_LIB_YEAR );	
}

CMusikSongArray CMusikLibrary::GetGenreSongs( wxArrayString *aGenres )	
{ 
	return GetSongs( aGenres, MUSIK_LIB_GENRE );
}

wxArrayString	CMusikLibrary::GetYearArtists( wxArrayString *aYears )	
{ 
	return GetInfo( aYears, MUSIK_LIB_YEAR, MUSIK_LIB_ARTIST );				
}

wxArrayString	CMusikLibrary::GetYearAlbums( wxArrayString *aYears )	
{ 
	return GetInfo( aYears, MUSIK_LIB_YEAR, MUSIK_LIB_ALBUM );		
}

wxArrayString	CMusikLibrary::GetYearGenres( wxArrayString *aYears	)	
{ 
	return GetInfo( aYears, MUSIK_LIB_YEAR, MUSIK_LIB_GENRE );			
}

CMusikSongArray CMusikLibrary::GetYearSongs( wxArrayString *aYears )	
{ 
	return GetSongs( aYears, MUSIK_LIB_YEAR );		
}
