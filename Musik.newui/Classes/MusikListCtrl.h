#ifndef C_MUSIK_LIST_CTRL_H
#define C_MUSIK_LIST_CTRL_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "wx/listctrl.h"

class CMusikListCtrl : public wxListCtrl
{
public:
	CMusikListCtrl( wxWindow *parent, wxWindowID id );
	~CMusikListCtrl();

	virtual void OnItemSelect		( wxListEvent& event );
	virtual void OnItemActivate		( wxListEvent& event );
	virtual void OnItemBeginDrag	( wxListEvent& event );
	virtual void OnItemBeginEdit	( wxListEvent& event );
	virtual void OnItemEndEdit		( wxListEvent& event );
	virtual void OnColBeginDrag		( wxListEvent& event );
	virtual void OnColClick			( wxListEvent& event );
	virtual void OnKeyDown			( wxListEvent& event );
	virtual void OnContextMenu		( wxCommandEvent& event );
	virtual void OnResize			( wxSizeEvent& event );

	virtual void RescaleColumns		( bool refresh = false ){};
	virtual void Reset				( bool rescale = true, bool refresh = false ){};
	virtual void Update				( bool refresh = false ){};

	DECLARE_EVENT_TABLE();
};

#endif
