#ifndef C_MUSIK_LIST_CTRL_H
#define C_MUSIK_LIST_CTRL_H

#include "wx/wxprec.h"
#include "wx/listctrl.h"

class CMusikEditInPlaceCtrl : public wxTextCtrl
{
public:
	CMusikEditInPlaceCtrl( wxWindow *parent, wxWindowID id );
	~CMusikEditInPlaceCtrl();

	void OnChar( wxKeyEvent& event );
	void OnKillFocus( wxFocusEvent& event );

	DECLARE_EVENT_TABLE()
};

class CMusikListCtrl : public wxListCtrl
{
public:
	CMusikListCtrl( wxWindow *parent, wxWindowID id, long flags = 0 );
	~CMusikListCtrl();

	//---------------------------------------------------------//
	//--- this class will remain strictly virtual. here		---//
	//--- are the classes that every inherited class can	---//
	//--- or should override. includes events.				---//
	//---------------------------------------------------------//
	virtual void OnItemSelect		( wxListEvent& event )		{}
	virtual void OnItemActivate		( wxListEvent& event )		{}
	virtual void OnItemBeginDrag	( wxListEvent& event )		{}
	virtual void OnItemBeginEdit	( wxListEvent& event )		{}
	virtual void OnItemEndEdit		( wxListEvent& event )		{}
	virtual void OnColBeginDrag		( wxListEvent& event )		{}
	virtual void OnColClick			( wxListEvent& event )		{}
	virtual void OnKeyDown			( wxListEvent& event )		{}
	virtual void OnContextMenu		( wxCommandEvent& event )	{}
	virtual void OnResize			( wxSizeEvent& event )		{}

	virtual void RescaleColumns		( bool refresh = false ){};
	virtual void Reset				( bool rescale = true, bool refresh = false ){};
	virtual void Update				( bool refresh = false ){};

	virtual void StartEditInPlace();

	DECLARE_EVENT_TABLE();

private:
	CMusikEditInPlaceCtrl* m_EditInPlace;
	wxWindow* m_Parent;
};

#endif
