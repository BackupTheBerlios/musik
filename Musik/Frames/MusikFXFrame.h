#ifndef MUSIKFXFRAME_H
#define MUSIKFXFRAME_H

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "../Classes/MusikFXGauge.h"

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
	void OnSlider			(wxScrollEvent& event);
	void OnSlidePitch		( wxScrollEvent& event );
	void OnClickReset		( wxCommandEvent& WXUNUSED(event) );
	void OnRightClick		( wxCommandEvent& event );
	void OnToggleEQEnable	( wxCommandEvent& WXUNUSED(event) );
	
	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void Close();
	void SlidersFromBands();
	void BandsFromSliders();
	
	//----------------//
	//--- controls ---//
	//----------------//
	wxCheckBox	*chkLock;
	wxCheckBox	*chkEQEnable;
	wxButton	*btnReset;
	wxSlider	*slPitch;
	
	//------------------//
	//--- eq sliders ---//
	//------------------//
	CMusikFXGauge	*slLeft[18];
	CMusikFXGauge	*slRight[18];
	
	//--------------//
	//--- sizers ---//
	//--------------//
	wxBoxSizer	*hsLeftSliders;
	wxBoxSizer	*hsRightSliders;
	wxBoxSizer	*vsSliders;
	wxBoxSizer	*vsEQControls;
	wxBoxSizer	*hsEQ;
	wxBoxSizer	*hsControls;
	wxBoxSizer	*vsMain;

	//-------------//
	//--- bands ---//
	//-------------//
	float ldata[18];	
	float rdata[18];
	
	DECLARE_EVENT_TABLE()
private:


};

#endif
