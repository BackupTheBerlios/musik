///////////////////////////////////////////////////

#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

///////////////////////////////////////////////////

#include <iostream>

#include "sqlite.h"
#include "StdString.h"

#include "MusikArrays.h"
#include "MusikPlaylist.h"

///////////////////////////////////////////////////

enum
{
	MUSIK_LIBRARY_TYPE_NONE = -1,		// -1
	MUSIK_LIBRARY_TYPE_ARTIST,			// 0
	MUSIK_LIBRARY_TYPE_ALBUM,			// 1
    MUSIK_LIBRARY_TYPE_YEAR,			// 2
	MUSIK_LIBRARY_TYPE_GENRE,			// 3
	MUSIK_LIBRARY_TYPE_TITLE,			// 4
	MUSIK_LIBRARY_TYPE_TRACKNUM,		// 5
	MUSIK_LIBRARY_TYPE_TIMEADDED,		// 6
	MUSIK_LIBRARY_TYPE_LASTPLAYED,		// 7
	MUSIK_LIBRARY_TYPE_FILESIZE,		// 8
	MUSIK_LIBRARY_TYPE_FORMAT,			// 9
	MUSIK_LIBRARY_TYPE_DURATION,		// 10
	MUSIK_LIBRARY_TYPE_RATING,			// 11
	MUSIK_LIBRARY_TYPE_TIMESPLAYED,		// 12
	MUSIK_LIBRARY_TYPE_BITRATE,			// 13
	MUSIK_LIBRARY_TYPE_FILENAME,		// 14
	MUSIK_LIBRARY_FIELD_COUNT = MUSIK_LIBRARY_TYPE_FILENAME + 1
};

///////////////////////////////////////////////////

enum
{
	MUSIK_LIBRARY_FORMAT_MP3 = 0,
	MUSIK_LIBRARY_FORMAT_OGG,
	MUSIK_LIBRARY_FORMAT_MOD,
	MUSIK_LIBRARY_FORMAT_WAV,
	MUSIK_LIBRARY_FORMAT_WMA,
	MUSIK_LIBRARY_FORMAT_AIFF
};

///////////////////////////////////////////////////

#define SONG_TABLE_NAME "songs"
#define STD_PLAYLIST_TABLE_NAME "std_playlist"
#define STD_PLAYLIST_SONGS "std_playlist_song"

///////////////////////////////////////////////////

class ACE_Thread_Mutex;

///////////////////////////////////////////////////

class CMusikLibrary
{
public:
	CMusikLibrary( const CStdString& filename );
	~CMusikLibrary();

	//-----------------------------------------------------//
	//--- functions modifying the database should call	---//
	//--- BeginTransaction() before modifying, and		---//
	//--- EndTransaction() afterwords.					---//
	//-----------------------------------------------------//
	void BeginTransaction();
	void EndTransaction();

	//-----------------------------------------------------//
	//--- functions used to get fields and field types. ---//
	//-----------------------------------------------------//
	CStdStringArray*	GetSongFields	( )					{ return &m_Fields; }
	CStdStringArray*	GetSongFieldsDB	( )					{ return &m_FieldsDB; }
	const CStdString	GetSongField	( int n )			{ return m_Fields.at( n ); }
	const CStdString	GetSongFieldDB	( int n )			{ return m_FieldsDB.at( n ); }
	int					GetSongFieldCnt	( )					{ return MUSIK_LIBRARY_FIELD_COUNT; }
	int					GetSongFieldID	( CStdString field );
	int					GetSongFieldDBID( CStdString field );

	//-----------------------------------------------------//
	//--- for querying the database.					---//
	//-----------------------------------------------------//
	void GetAllSongs		( CMusikPlaylist & target );
	void GetSongs			( int source_type, const CStdStringArray & source_items, CMusikPlaylist & target );
	void GetRelatedItems	( int source_type, const CStdStringArray & source_items, int target_type, CStdStringArray & target );
	void GetRelatedItems	( CStdString sub_query, int order_by, CStdStringArray& target );
	void GetRelatedSongs	( CStdString sub_query, int order_by, CMusikPlaylist& target );
	void GetAllDistinct		( int source_type, CStdStringArray& target, bool clear_target = true );
	int	 GetSongCount		();
	void GetFieldFromID		( int id, int field, CStdString& string );
	void GetSongInfoFromID	( int id, CMusikSongInfo* info );
	void GetAllStdPlaylists ( CStdStringArray* target, bool clear_target = true );

	//-----------------------------------------------------//
	//--- for creating and retrieving playlists			---//
	//-----------------------------------------------------//
	void CreateStdPlaylist	( const CStdString& name, const CIntArray& songids );
	void DeleteStdPlaylist	( const CStdString& name );
	void CreateDynPlaylist	( CStdString name, CStdString query );
	void DeleteDynPlaylist	( CStdString name );

	//-----------------------------------------------------//
	//--- for updating the database						---//
	//-----------------------------------------------------//
	bool SetSongInfo		( int songid, CMusikSongInfo* info );
	bool SetLastPlayedToNow	( int songid );
	bool SetSongRating		( int songid, int rating );

	//-----------------------------------------------------//
	//--- if a thread is updating the database, it		---//
	//--- should first call this function.				---//
	//-----------------------------------------------------//
	void InitTimeAdded();

private:

	//-----------------------------------------------------//
	//--- filename for the database						---//
	//-----------------------------------------------------//
	CStdString m_Filename;

	//-----------------------------------------------------//
	//--- this is the actual SQL database object		---//
	//-----------------------------------------------------//
	sqlite	*m_pDB;

	//-----------------------------------------------------//
	//--- the mutex that will be used to protect the	---//
	//--- library within any given scope.				---//
	//-----------------------------------------------------//
	ACE_Thread_Mutex* m_ProtectingLibrary;
	
	//-----------------------------------------------------//
	//--- fields... artist/title/album/etc				---//
	//-----------------------------------------------------//
	void InitFields();
	CStdStringArray m_Fields;
	CStdStringArray m_FieldsDB;

	//-----------------------------------------------------//
	//--- starting up and shutting down the database	---//
	//-----------------------------------------------------//
	bool Startup();
	void Shutdown();

	//-----------------------------------------------------//
	//--- creating the various tables					---//
	//-----------------------------------------------------//
	bool InitLibTable();
	bool InitStdTables();
	bool InitDynTable();
	bool InitEqTable();
	bool InitCrossfaderTable();

	//-----------------------------------------------------//
	//--- field that will get shown up as "Time Added"	---//
	//-----------------------------------------------------//
	CStdString m_TimeAdded;

	//-----------------------------------------------------//
	//--- querying the database							---//
	//-----------------------------------------------------//
	void		QuerySongs		( const CStdString & query, CMusikPlaylist & target );	
	CStdString	GetOrder		( int type, bool terminate = true );
	int			QueryCount		( const char* pQueryResult );
	void		VerifyYearList	( CStdStringArray & list );
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////