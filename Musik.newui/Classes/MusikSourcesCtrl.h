#ifndef C_MUSIK_SOURCES_CTRL_H
#define C_MUSIK_SOURCES_CTRL_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "MusikListCtrl.h"

class CMusikSourcesCtrl : public CMusikListCtrl
{
public:
	CMusikSourcesCtrl( wxWindow* parent, wxWindowID id );
	~CMusikSourcesCtrl();

	virtual void OnResize( wxSizeEvent& event );
	virtual void RescaleColumns( bool refresh = false );
	virtual void Reset( bool rescale = true, bool refresh = false );
	virtual void Update( bool refresh = false );

};

#endif