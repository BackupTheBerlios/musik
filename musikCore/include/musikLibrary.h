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
///////////////////////////////////////////////////

#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

///////////////////////////////////////////////////

#include <iostream>

#include "sqlite.h"
#include "StdString.h"

#include "musikArrays.h"
#include "musikPlaylist.h"

#include "../musikCore/include/musikEQSettings.h"

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
	MUSIK_LIBRARY_TYPE_EQUALIZER,		// 15
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

enum
{
	MUSIK_PLAYLIST_TYPE_LIBRARY_OR_DEVICE = 0,
	MUSIK_PLAYLIST_TYPE_STANDARD,
	MUSIK_PLAYLIST_TYPE_DYNAMIC,
	MUSIK_PLAYLIST_TYPE_COUNT // must be last
};

///////////////////////////////////////////////////

#define SONG_TABLE_NAME "songs"
#define STD_PLAYLIST_TABLE_NAME "std_playlist"
#define STD_PLAYLIST_SONGS "std_playlist_song"
#define DYN_PLAYLIST_TABLE_NAME "dyn_playlist"
#define CROSSFADER_PRESET "crossfader_preset"
#define EQUALIZER_PRESET "equalizer_preset"
#define MANAGED_PATH "managed_path"

///////////////////////////////////////////////////

class CmusikCrossfader;
class CmusikBatchAdd;

///////////////////////////////////////////////////

class CmusikLibrary
{
public:

	// construct and destruct
	CmusikLibrary( const CStdString& filename );
	~CmusikLibrary();

	// starting up and shutting down the database
	bool Startup();
	void Shutdown();

	// functions that external threads should use
	// if they attempt to modify the database directly
	void InitTimeAdded();
	void BeginTransaction();
	void EndTransaction();
	int  GetOpenTransactions();

	// querying library fields
	CStdStringArray*	GetSongFields	( )					{ return &m_Fields; }
	CStdStringArray*	GetSongFieldsDB	( )					{ return &m_FieldsDB; }
	const CStdString	GetSongField	( int n )			{ return m_Fields.at( n ); }
	const CStdString	GetSongFieldDB	( int n )			{ return m_FieldsDB.at( n ); }
	int					GetSongFieldCnt	( )					{ return MUSIK_LIBRARY_FIELD_COUNT; }
	int					GetSongFieldID	( CStdString field );
	int					GetSongFieldDBID( CStdString field );

	// querying songs
	int  GetAllSongs				( CmusikPlaylist & target, bool new_transaction = true );
	int  GetRelatedItems			( int source_type, const CStdStringArray & source_items, int target_type, CStdStringArray & target, bool new_transaction = true );
	int  GetRelatedItems			( CStdString sub_query, int order_by, CStdStringArray& target, bool new_transaction = true );
	int  GetRelatedSongs			( CStdString sub_query, int order_by, CmusikPlaylist& target, bool new_transaction = true );
	int  GetAllDistinct				( int source_type, CStdStringArray& target, bool new_transaction = true );
	int	 GetSongCount				( bool new_transaction = true );
	int  GetFieldFromID				( int id, int field, CStdString& string, bool new_transaction = true );
	int  GetSongInfoFromID			( int id, CmusikSongInfo* info, bool new_transaction = true );
	int  GetIDFromFilename			( CStdString fn, bool new_transaction = true );
	int  GetSongFromFilename		( CStdString fn, CmusikSong& song, bool new_transaction = true );
	bool IsSongInLibrary			( CStdString fn, bool new_transaction = true );
	int  GetSongFormatFromID		( int id, int* target, bool new_transaction = true );
	void GetInfoArrayFromPlaylist	( CmusikPlaylist* playlist, CmusikSongInfoArray* info, int replace_field_type = -1, CStdString new_field = "", bool new_transaction = true );
	int  GetDirtySongs				( CmusikPlaylist* target, bool new_transaction = true );
	int  FinalizeDirtySongs			( bool new_transaction = true );

	// updating songs
	bool SetSongInfo				( CmusikSongInfo* info, int songid = -1, bool new_transaction = true );
	bool SetLastPlayedToNow			( int songid, bool new_transaction = true );
	bool SetSongRating				( int songid, int rating );

	// querying playlist
	int  GetAllStdPlaylists			( CmusikPlaylistInfoArray* target, bool new_transaction = true );
	int  GetAllDynPlaylists			( CmusikPlaylistInfoArray* target, bool new_transaction = true );
	int  GetStdPlaylist				( int id, CmusikPlaylist& target, bool new_transaction = true );
	bool GetStdPlaylistFns			( int id, CStdStringArray& target, bool new_transaction = true );
	bool GetStdPlaylistFns			( CmusikPlaylist& playlist, CStdStringArray& target, bool new_transaction = true );
	int  CreateStdPlaylist			( const CStdString& name, const CStdStringArray& files, bool new_transaction = true );
	int  AppendStdPlaylist			( int id, const CStdStringArray& files, bool new_transaction = true );
	int  RenameStdPlaylist			( int id, const CStdString& str, bool new_transaction = true );
	int  CreateDynPlaylist			( const CStdString& name, const CStdString& query, bool new_transaction = true );
	int  DeleteStdPlaylist			( const CStdString& name, bool new_transaction = true );
	int  RewriteStdPlaylist			( int id, CmusikPlaylist* playlist, bool new_transaction = true );
	int  DeleteStdPlaylist			( int id, bool new_transaction = true );
	int  DeleteDynPlaylist			( const CStdString& name, bool new_transaction = true );
	int  DeleteDynPlaylist			( int id, bool new_transaction = true );

	// querying crossfaders
	int  CreateCrossfader			( CmusikCrossfader* fader );
	int  DeleteCrossfader			( int id );
	int  GetCrossfader				( int id, CmusikCrossfader* fader );
	int  GetAllCrossfaders			( CStdStringArray* names );
	int  GetAllCrossfaders			( CIntArray* ids );

	// equalizers
	int  CreateEqualizer			( const CmusikEQSettings& eq, const CStdString& name );
	int  DeleteEqualizer			( int id );
	int  GetEqualizer				( int eq_id, CmusikEQSettings* eq );
	int  UpdateEqualizer			( int id, const CmusikEQSettings& eq );
	int  GetEqualizerIDFromSongID	( int id );

	// paths
	// NOTHING IS HERE YET!

	// misc stuff
	static int GetFilesize( const CStdString& fn );

	// adding and removing files
	bool AddSong	( const CStdString& fn, bool new_transaction = true );
	bool AddOGG		( const CStdString& fn, bool new_transaction = true );
	bool AddMP3		( const CStdString& fn, bool new_transaction = true );
	bool RemoveSong	( int songid, bool new_transaction = true );
	bool RemoveSong	( const CStdString& fn, bool new_transaction = true );

private:

	// filename of the currently opened database
	CStdString m_Filename;

	// currently opened database
	sqlite	*m_pDB;

	// number of functions that have called
	// begin transaction 
	int m_Transactions;

	// fields... artist/title/album/etc
	void InitFields();
	CStdStringArray m_Fields;
	CStdStringArray m_FieldsDB;

	// table creation
	bool InitLibTable( bool new_transaction = true );
	bool InitStdTables( bool new_transaction = true );
	bool InitDynTable( bool new_transaction = true );
	bool InitEqTable( bool new_transaction = true );
	bool InitPathTable( bool new_transaction = true );
	bool InitCrossfaderTable( bool new_transaction = true );

	// only needs to be initialized once
	CStdString m_TimeAdded;

	// internal song query functions
	int			QuerySongs		( const CStdString & query, CmusikPlaylist & target, bool new_transaction = true );	
	CStdString	GetOrder		( int type, bool terminate = true );
	int			QueryCount		( const char* pQueryResult, bool new_transaction = true );
	void		VerifyYearList	( CStdStringArray & list );
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////

