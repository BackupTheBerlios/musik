#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

#include <iostream>
#include "sqlite.h"
#include "StdString.h"
#include "MusikArrays.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

#include "MusikPlaylist.h"

enum
{
	MUSIK_LIBRARY_TYPE_NONE = -1,
	MUSIK_LIBRARY_TYPE_ARTIST,
	MUSIK_LIBRARY_TYPE_ALBUM,
    MUSIK_LIBRARY_TYPE_YEAR,
	MUSIK_LIBRARY_TYPE_GENRE,
	MUSIK_LIBRARY_TYPE_TITLE,
	MUSIK_LIBRARY_TYPE_TRACKNUM,
	MUSIK_LIBRARY_TYPE_TIMEADDED,
	MUSIK_LIBRARY_TYPE_LASTPLAYED,
	MUSIK_LIBRARY_TYPE_FILESIZE,
	MUSIK_LIBRARY_TYPE_FORMAT,
	MUSIK_LIBRARY_TYPE_DURATION,
	MUSIK_LIBRARY_TYPE_RATING,
	MUSIK_LIBRARY_TYPE_TIMESPLAYED,
	MUSIK_LIBRARY_TYPE_BITRATE,
	MUSIK_LIBRARY_TYPE_FILENAME,
	MUSIK_LIBRARY_FIELD_COUNT
};

enum
{
	MUSIK_LIBRARY_FORMAT_MP3 = 0,
	MUSIK_LIBRARY_FORMAT_OGG,
	MUSIK_LIBRARY_FORMAT_MOD,
	MUSIK_LIBRARY_FORMAT_WAV,
	MUSIK_LIBRARY_FORMAT_WMA,
	MUSIK_LIBRARY_FORMAT_AIFF
};


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

	//-----------------------------------------------------//
	//--- for creating and retrieving playlists			---//
	//-----------------------------------------------------//
	void CreateStdPlaylist	( CStdString name, CIntArray songids );
	void CreateDynPlaylist	( CStdString name, CStdString query );
	void DeleteStdPlaylist	( CStdString name );
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

protected:

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
	boost::mutex m_ProtectingLibrary;
	

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

#endif