#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

#include "wx/wxprec.h"

class CMusikLibrary
{
public:
	CMusikLibrary();
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
	//--- fields... artist/title/album/etc				---//
	//-----------------------------------------------------//
	InitFields();
	wxArrayString m_Fields;
	wxArrayString m_FieldsDB;
};

#endif