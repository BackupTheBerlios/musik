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
	void OnClickReset( wxCommandEvent& WXUNUSED(event) );
	void OnToggleEQEnable( wxCommandEvent& WXUNUSED(event) );
	
	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void Close();
	void SlidersFromBands();
	void BandsFromSliders();
	
	//----------------------//
	//--- system buttons ---//
	//----------------------//
	
	//----------------//
	//--- controls ---//
	//----------------//
	wxCheckBox	*chkLock;
	wxCheckBox	*chkEQEnable;
	wxButton	*btnReset;
	
	//------------------//
	//--- eq sliders ---//
	//------------------//
	wxSlider	*slLeft[18];
	wxSlider	*slRight[18];
	
	//--------------//
	//--- sizers ---//
	//--------------//
	wxBoxSizer	*hsLeftSliders;
	wxBoxSizer	*hsRightSliders;
	wxBoxSizer	*vsSliders;
	wxBoxSizer	*hsControls;
	wxBoxSizer	*vsMain;
	
	DECLARE_EVENT_TABLE()
private:

};

#endif
