#ifndef MUSIKFXFRAME_H
#define MUSIKFXFRAME_H

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

enum EMUSIK_FX_OBJECT_ID
{
};

class MusikFXFrame : public wxFrame
{
public:
	MusikFXFrame( wxFrame* pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size );

	//--------------//
	//--- events ---//
	//--------------//
	void OnClose			( wxCommandEvent& WXUNUSED(event) );
	void OnSlider(wxScrollEvent& event);
	
	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void Close();
	
	//----------------------//
	//--- system buttons ---//
	//----------------------//
	

	//------------------//
	//--- eq sliders ---//
	//------------------//
	wxSlider	*slLeft[18];
	wxSlider	*slRight[18];
	
	//--------------//
	//--- sizers ---//
	//--------------//
	wxBoxSizer	*hsSliders;
	
	DECLARE_EVENT_TABLE()
private:

};

#endif
