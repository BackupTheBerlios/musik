#ifndef C_MUSIK_SELECTION_CTRL_H
#define C_MUSIK_SELECTION_CTRL_H

#include "wx/wxprec.h"
#include "MusikListCtrl.h"

class CMusikSelectionCtrl : public CMusikListCtrl
{
public:
	CMusikSelectionCtrl( wxWindow* parent, wxWindowID id );
	~CMusikSelectionCtrl();

	virtual void OnResize( wxSizeEvent& event );
	virtual void RescaleColumns( bool refresh = false );
	virtual void Reset( bool rescale = true, bool refresh = false );
	virtual void Update( bool refresh = false );

};

#endif