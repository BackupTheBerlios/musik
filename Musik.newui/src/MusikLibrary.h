#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

#include "wx/wxprec.h"
#include "wx/filename.h"
#include "sqlite.h"
#include "MusikPlaylist.h"

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
	//--- used for querying the database. if nInType is	---//
	//--- -1, we will will get all the songs relating	---//
	//--- to the out type.								---//
	//-----------------------------------------------------//
	void GetAllSongs		( CMusikPlaylist & target );
	void GetSongs			( int source_type, const wxArrayString & source_items, CMusikPlaylist & target );
	void GetRelatedSongs	( int source_type, const wxArrayString & source_items, int target_type, wxArrayString & target );
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
	int QueryCount( const char* pQueryResult );

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