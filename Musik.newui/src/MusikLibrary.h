#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

#include "wx/wxprec.h"
#include "wx/filename.h"
#include "sqlite.h"



class CMusikLibrary
{
public:
	CMusikLibrary( const wxString& filename );
	~CMusikLibrary();

	//-----------------------------------------------------//
	//--- functions used to get fields and field types. ---//
	//-----------------------------------------------------//
	wxArrayString*	GetSongFields	( )			{ return &m_Fields; }
	wxArrayString*	GetSongFieldsDB	( )			{ return &m_FieldsDB; }
	const wxString	GetSongField	( int n )	{ return m_Fields.Item( n ); }
	const wxString	GetSongFieldDB	( int n )	{ return m_FieldsDB.Item( n ); }

protected:

private:

	//-----------------------------------------------------//
	//--- filename for the database						---//
	//-----------------------------------------------------//
	wxString m_Filename;

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