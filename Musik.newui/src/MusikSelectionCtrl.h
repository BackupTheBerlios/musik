#ifndef C_MUSIK_SELECTION_CTRL_H
#define C_MUSIK_SELECTION_CTRL_H

#include "wx/wxprec.h"
#include "MusikListCtrl.h"

enum E_MUSIK_SELECTION_TYPE
{
	MUSIK_SELECTION_ARTISTS = 0,
	MUSIK_SELECTION_ALBUMS,
	MUSIK_SELECTION_GENRES,
	MUSIK_SELECTION_YEARS,
	NSELECTIONCOUNT
};

const wxString E_MUSIK_SELECTION_COLUMN_LABELS[NSELECTIONCOUNT] = 
{
	wxT( "Artists" ),
	wxT( "Albums" ),
	wxT( "Genres" ),
	wxT( "Years" )
};

const wxString E_MUSIK_SELECTION_DB_NAMES[NSELECTIONCOUNT] =
{
	wxT( "artists" ),
	wxT( "albums" ),
	wxT( "genres" ),
	wxT( "years" )
};

class CMusikSelectionCtrl : public CMusikListCtrl
{
public:
	CMusikSelectionCtrl( wxWindow* parent, wxWindowID id );
	~CMusikSelectionCtrl();

	virtual void OnResize( wxSizeEvent& event );
	virtual void OnColBeginDrag( wxListEvent& event );
	virtual void OnKeyDown( wxListEvent& event );
	virtual void RescaleColumns( bool refresh = false );
	virtual void Reset( bool rescale = true, bool refresh = false );
	virtual void Update( bool refresh = false );

};

#endif