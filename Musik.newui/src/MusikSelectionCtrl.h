#ifndef C_MUSIK_SELECTION_CTRL_H
#define C_MUSIK_SELECTION_CTRL_H

#include "wx/wxprec.h"
#include "MusikListCtrl.h"
#include "MusikLibrary.h"

//---------------------------------------------------------//
//--- these are the types of activity boxes that are	---//
//--- allowed.											---//
//---------------------------------------------------------//
enum
{
	MUSIK_SELECTION_ARTISTS = MUSIK_LIBRARY_TYPE_ARTIST,
	MUSIK_SELECTION_ALBUMS = MUSIK_LIBRARY_TYPE_ALBUM,
	MUSIK_SELECTION_GENRES = MUSIK_LIBRARY_TYPE_GENRE,
	MUSIK_SELECTION_YEARS = MUSIK_LIBRARY_TYPE_YEAR
};

class CMusikSelectionCtrl : public CMusikListCtrl
{
public:
	CMusikSelectionCtrl( wxWindow* parent, CMusikLibrary* library, wxWindowID id );
	~CMusikSelectionCtrl();

	//---------------------------------------------------------//
	//--- functions having to do with getting and setting	---//
	//--- selectionctrl type.								---//
	//---------------------------------------------------------//
	wxString GetDBName		( )	{ return m_MusikLibrary->GetSongFieldDB( m_Type ); }
	wxString GetColumnName	( )	{ return m_MusikLibrary->GetSongField( m_Type ); }
	int  GetType			( )	{ return m_Type; }
	void SetType			( int type, bool reset = true );

protected:
	//---------------------------------------------------------//
    //--- these are our virtual overrides. they control		---//
	//--- dialog resizing, column dragging, and updating.	---//
	//---------------------------------------------------------//
	virtual void OnResize			( wxSizeEvent& event );
	virtual void OnColBeginDrag		( wxListEvent& event );
	virtual void OnKeyDown			( wxListEvent& event );
	virtual void RescaleColumns		( bool refresh = false );
	virtual void Reset				( bool rescale = true, bool refresh = false );
	virtual void Update				( bool refresh = false );

private:

	int m_Type;
	CMusikLibrary* m_MusikLibrary;

};

#endif