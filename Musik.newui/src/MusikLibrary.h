#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

#include "wx/wxprec.h"
#include "wx/filename.h"
#include "sqlite.h"
#include "MusikPlaylist.h"

//-----------------------------------------------------//
//--- enumerated value list to the types of entires	---//
//--- stored within every CMusikSong				---//
//-----------------------------------------------------//
enum
{
	MUSIK_LIBRARY_TYPE_NONE = -1,
	MUSIK_LIBRARY_TYPE_FILENAME,
	MUSIK_LIBRARY_TYPE_TITLE,
	MUSIK_LIBRARY_TYPE_ARTIST,
	MUSIK_LIBRARY_TYPE_ALBUM,
    MUSIK_LIBRARY_TYPE_YEAR,
	MUSIK_LIBRARY_TYPE_GENRE,
	MUSIK_LIBRARY_TYPE_TRACKNUM,
	MUSIK_LIBRARY_TYPE_TIMEADDED,
	MUSIK_LIBRARY_TYPE_LASTPLAYED,
	MUSIK_LIBRARY_TYPE_FILESIZE,
	MUSIK_LIBRARY_TYPE_FORMAT,
	MUSIK_LIBRARY_TYPE_DURATION,
	MUSIK_LIBRARY_TYPE_RATING,
	MUSIK_LIBRARY_TYPE_TIMESPLAYED,
	MUSIK_LIBRARY_TYPE_BITRATE,
};

class CMusikLibrary
{
public:
	CMusikLibrary( const wxString& filename );
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
	wxArrayString*	GetSongFields	( )			{ return &m_Fields; }
	wxArrayString*	GetSongFieldsDB	( )			{ return &m_FieldsDB; }
	const wxString	GetSongField	( int n )	{ return m_Fields.Item( n ); }
	const wxString	GetSongFieldDB	( int n )	{ return m_FieldsDB.Item( n ); }

	//-----------------------------------------------------//
	//--- for querying the database.					---//
	//-----------------------------------------------------//
	void GetAllSongs		( CMusikPlaylist & target );
	void GetSongs			( int source_type, const wxArrayString & source_items, CMusikPlaylist & target );
	void GetRelatedItems	( int source_type, const wxArrayString & source_items, int target_type, wxArrayString & target );
	void GetAllDistinct		( int source_type, wxArrayString& target, bool clear_target = true );
	int  GetSongCount		();
	void QuerySongs			( const wxString & query, CMusikPlaylist & target );

protected:

private:

	//-----------------------------------------------------//
	//--- filename for the database						---//
	//-----------------------------------------------------//
	wxString m_Filename;

	//-----------------------------------------------------//
	//--- private functions that relate to querying the	---//
	//--- database.										---//
	//-----------------------------------------------------//
	int			QueryCount( const char* pQueryResult );
	wxString	GetOrder( int type, bool terminate = true );
	void		VerifyYearList( const wxArrayString & source_list,wxArrayString & target_list );

	//-----------------------------------------------------//
	//--- start / shutdown database.					---//
	//-----------------------------------------------------//
	bool Start();
	void Shutdown();

	//-----------------------------------------------------//
	//--- fields... artist/title/album/etc				---//
	//-----------------------------------------------------//
	void InitFields();
	wxArrayString m_Fields;
	wxArrayString m_FieldsDB;

	//-----------------------------------------------------//
	//--- database access critical section. its sort	---//
	//--- of like a mutex, but faster in windows.		---//
	//-----------------------------------------------------//
	wxCriticalSection m_csDBAccess;

	//-----------------------------------------------------//
	//--- this is the actual SQL database object		---//
	//-----------------------------------------------------//
	sqlite	*m_pDB;
};

#endif