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
///////////////////////////////////////////////////

#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikPlaylist.h"
#include "musikCrossfader.h"
#include "musikEQSettings.h"

#include "sqlite.h"

#include <OpenThreads/Mutex>
using namespace OpenThreads;

#include <iostream>

///////////////////////////////////////////////////

namespace musik {

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
	MUSIK_PLAYLIST_TYPE_UNKNOWN = -1,
	MUSIK_PLAYLIST_TYPE_LIBRARY = 0,
	MUSIK_PLAYLIST_TYPE_SUBLIBRARY,
	MUSIK_PLAYLIST_TYPE_STANDARD,
	MUSIK_PLAYLIST_TYPE_DYNAMIC,
	MUSIK_PLAYLIST_TYPE_NOWPLAYING,
	MUSIK_PLAYLIST_TYPE_COUNT // must be last
};

///////////////////////////////////////////////////

#define SONG_TABLE_NAME "songs"
#define TEMP_SONG_TABLE_NAME "temp_songs"

#define STD_PLAYLIST_TABLE_NAME "std_playlist"
#define STD_PLAYLIST_SONGS "std_playlist_song"

#define DYN_PLAYLIST_TABLE_NAME "dyn_playlist"
#define DYN_PLAYLIST_QUERY "dyn_playlist_query"

#define CROSSFADER_PRESET "crossfader_preset"
#define CROSSFADER_DEFAULT "crossfader_default"

#define EQUALIZER_PRESET "equalizer_preset"
#define EQUALIZER_DEFAULT "equalizer_default"

#define MANAGED_PATH "managed_path"

#define MUSIK_LIBRARY_OK SQLITE_OK
#define MUSIK_LIBRARY_ID_EXISTS -2
#define MUSIK_LIBRARY_NO_ID_EXISTS -3
#define MUSIK_LIBRARY_NOT_OPEN -4
#define MUSIK_LIBRARY_INVALID_PLAYLIST_TYPE -5

///////////////////////////////////////////////////

class CmusikLibrary
{
	// we trust the player
	friend class CmusikPlayer;

public:

	// construct and destruct
	CmusikLibrary( const CmusikString& filename );
	~CmusikLibrary();

	// starting up and shutting down the database
	bool Startup();
	void Shutdown();

	// clears all songs by default,
	// but will clear everything if the right flag
	// is passed
	void ClearLibrary( bool clear_all_tables = false );
	void DeleteSongs( CmusikPlaylist& songs, bool delete_from_disk = false, bool use_temp_table = false );

	// sorting playlist by field
	int SortPlaylist( CmusikPlaylist* playlist, int field, bool ascending = true );

	// functions that external threads should use
	// if they attempt to modify the database directly
	void InitTimeAdded();
	void BeginTransaction();
	void EndTransaction();
	int  GetOpenTransactions();

	// functions specifically dealing with the
	// the temporary song table.
	void ClearTempSongTable();
	int  PopulateTempSongTable( CmusikPlaylist& source ); 

	// querying library fields
	CmusikStringArray*	GetSongFields	( )					{ return &m_Fields; }
	CmusikStringArray*	GetSongFieldsDB	( )					{ return &m_FieldsDB; }
	const CmusikString	GetSongField	( int n )			{ return m_Fields.at( n ); }
	const CmusikString	GetSongFieldDB	( int n )			{ return m_FieldsDB.at( n ); }
	int					GetSongFieldCnt	( )					{ return MUSIK_LIBRARY_FIELD_COUNT; }
	int					GetSongFieldID	( CmusikString field );
	int					GetSongFieldDBID( CmusikString field );

	// querying songs
	int  GetAllSongs				( CmusikPlaylist & target, bool use_temp_table = false );
	int  GetRelatedItems			( int source_type, const CmusikStringArray & source_items, int target_type, CmusikStringArray & target, bool use_temp_table = false );
	int  GetRelatedItems			( CmusikString partial_query, int order_by, CmusikStringArray& target, bool sub_query = false, bool use_temp_table = false );
	int  GetRelatedSongs			( CmusikString partial_query, int order_by, CmusikPlaylist& target, bool sub_query = false, bool use_temp_table = false );
	int  GetAllDistinct				( int source_type, CmusikStringArray& target, bool clear_target = true, bool use_temp_table = false );
	int	 GetSongCount				( bool use_temp_table = false );
	int  GetFieldFromID				( int id, int field, CmusikString& string );
	int  GetSongInfoFromID			( int id, CmusikSongInfo* info );
	int  GetSongFromFilename		( CmusikString fn, CmusikSong& song );
	bool IsSongInLibrary			( CmusikString fn );
	int  GetSongFormatFromID		( int id, int* target );
	void GetInfoArrayFromPlaylist	( CmusikPlaylist* playlist, CmusikSongInfoArray* info, int replace_field_type = -1, CmusikString new_field = "", bool clear = true );
	int  GetDirtySongs				( CmusikPlaylist* target, bool clear = true );
	int  FinalizeDirtySongs			( );
	int  QuickQuery					( CmusikString str, CmusikPlaylist& target, bool use_temp_table = false );

	// updating songs
	bool SetSongInfo				( CmusikSongInfo* info, int songid = -1 );
	bool SetLastPlayedToNow			( int songid );
	bool SetSongRating				( int songid, int rating );
	bool SetSongEqualizer			( int songid, int eq_id );

	// standard playlist
	int  CreateStdPlaylist			( const CmusikString& name, const CmusikStringArray& files, int type = MUSIK_PLAYLIST_TYPE_STANDARD, bool add_to_library = true );
	int  CreateStdPlaylist			( const CmusikString& name, CmusikPlaylist& files, int type = MUSIK_PLAYLIST_TYPE_STANDARD );
	int  AppendStdPlaylist			( int id, const CmusikStringArray& files, bool add_to_library = true );
	int  AppendStdPlaylist			( int id, CmusikPlaylist& playlist );
	int  RenameStdPlaylist			( int id, const CmusikString& str );
	int  DeleteStdPlaylist			( const CmusikString& name );
	int  RewriteStdPlaylist			( CmusikPlaylist* playlist );
	int  DeleteStdPlaylist			( int id );

	int  GetAllStdPlaylists			( CmusikPlaylistInfoArray* target, int type = MUSIK_PLAYLIST_TYPE_STANDARD, bool clear_target = true );
	int  GetStdPlaylist				( int id, CmusikPlaylist& target, bool clear_target = true );
	bool GetStdPlaylistFns			( int id, CmusikStringArray& target, bool clear_target = true );
	bool GetStdPlaylistFns			( CmusikPlaylist& playlist, CmusikStringArray& target, bool clear_target = true );
	
	// dynamic playlist
	int  CreateDynPlaylist			( const CmusikString& name, const CmusikStringArray& query );
	int  RewriteDynPlaylist			( int id, const CmusikStringArray& query );
	int  DeleteDynPlaylist			( const CmusikString& name );
	int  DeleteDynPlaylist			( int id );

	int  GetAllDynPlaylists			( CmusikPlaylistInfoArray* target, bool clear_target = true );
	int  GetDynPlaylist				( int id, CmusikPlaylist& target, bool clear_target = true );

	// querying crossfaders
	int  CreateCrossfader			( CmusikCrossfader* fader );
	int  DeleteCrossfader			( int id );
	int  GetCrossfader				( int id, CmusikCrossfader* fader );
	int  GetAllCrossfaderPresets	( CmusikStringArray* target, CIntArray* target_ids = NULL, bool clear_targets = true );
	int  GetCrossfaderIDFromName	( const CmusikString& name );

	int  GetDefaultCrossfader		( CmusikCrossfader* fader );
	int  UpdateDefaultCrossfader	( const CmusikCrossfader& fader );
	int  ResetDefaultCrossfader		( bool clear_old_default = true );

	// equalizers
	int  CreateEqualizer			( CmusikEQSettings& eq, bool is_preset = false );
	int  DeleteEqualizer			( int id );
	int  GetEqualizer				( int eq_id, CmusikEQSettings* eq );
	int  UpdateEqualizer			( int id, const CmusikEQSettings& eq );
	int  GetEqualizerIDFromSongID	( int id );
	int  GetEqualizerIDFromName		( const CmusikString& name );
	int  GetAllEqualizerPresets		( CmusikStringArray* target, CIntArray* target_ids = NULL, bool clear_targets = true );
	
	int  GetDefaultEqualizer		( CmusikEQSettings* eq );
	int  UpdateDefaultEqualizer		( const CmusikEQSettings& eq );
	int  ResetDefaultEqualizer		( bool clear_old_default = true );

	// paths
	int	 AddPath					( const CmusikString& path );
	int  RemovePath					( const CmusikString& path );
	int  GetAllPaths				( CmusikStringArray* target, bool clear_target = true );

	// misc stuff
	static int GetFilesize( const CmusikString& fn );

	// adding and removing files
	bool AddSong	( const CmusikString& fn );
	bool AddOGG		( const CmusikString& fn );
	bool AddMP3		( const CmusikString& fn );
	bool RemoveSong	( int songid );
	bool RemoveSong	( const CmusikString& fn );

private:

	// filename of the currently opened database
	CmusikString m_Filename;

	// currently opened database
	sqlite *m_pDB;
	bool m_DatabaseOpen;

	// number of functions that have called
	// begin transaction 
	int m_Transactions;

	// the mutex that will be used to protect the
	// library within any given scope.
	Mutex m_ProtectingLibrary;
	
	// fields... artist/title/album/etc
	void InitFields();
	CmusikStringArray m_Fields;
	CmusikStringArray m_FieldsDB;

	// table creation
	bool InitLibTable();
	bool InitTempLibTable();
	bool InitStdTables();
	bool InitDynTable();
	bool InitEqTable();
	bool InitPathTable();
	bool InitCrossfaderTable();

	// player will call this
	int  IncLastPlayed( int songid );

	// only needs to be initialized once
	CmusikString m_TimeAdded;

	// internal song functions
	int				QuerySongs		( const CmusikString & query, CmusikPlaylist & target, bool use_temp_table = false );
	int				RawQuerySongs	( const CmusikString & query, CmusikPlaylist & target );
	CmusikString	GetOrder		( int type, bool terminate = true, bool by_upper = false, bool descend = false );
	int				QueryCount		( const char* pQueryResult );
	void			VerifyYearList	( CmusikStringArray & list );
	int             InsertTempSong	( int songid );
};

///////////////////////////////////////////////////

} // namespace musik

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

