
//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikFXFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- classes ---//
#include "../Classes/MusikPlayer.h"

//--- related frames ---//
#include "../Frames/MusikFrame.h"

enum
{
    SliderMove = wxID_HIGHEST,
	SliderL0,
    SliderL1,
	SliderL2,
	SliderL3,
	SliderL4,
	SliderL5,
	SliderL6,
	SliderL7,
	SliderL8,
	SliderL9,
	SliderL10,
	SliderL11,
	SliderL12,
	SliderL13,
	SliderL14,
	SliderL15,
	SliderL16,
	SliderL17,
	SliderR0,
	SliderR1,
	SliderR2,
	SliderR3,
	SliderR4,
	SliderR5,
	SliderR6,
	SliderR7,
	SliderR8,
	SliderR9,
	SliderR10,
	SliderR11,
	SliderR12,
	SliderR13,
	SliderR14,
	SliderR15,
	SliderR16,
	SliderR17
};

BEGIN_EVENT_TABLE(MusikFXFrame, wxFrame)
	EVT_CLOSE ( MusikFXFrame::OnClose )
	EVT_COMMAND_SCROLL( SliderMove, MusikFXFrame::OnSlider)
END_EVENT_TABLE()

MusikFXFrame::MusikFXFrame( wxFrame *pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size ) 
	: wxFrame( pParent, -1, sTitle, pos, size, wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW  )
{
	//---------------//
	//--- colours ---//
	//---------------//
	static wxColour cBtnFace = wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE );
	this->SetBackgroundColour ( cBtnFace );

	//------------//
	//--- icon ---//
	//------------//
	#if defined (__WXMSW__)
		SetIcon( wxICON( musicbox ) );
	#endif

	//--------------------//
	//--- Dummy Window ---//
	//--------------------//
	wxWindow *wndDummy = new wxWindow( this, -1, wxPoint( 0, 0 ), wxSize( 0, 0 ) );

	hsSliders = new wxBoxSizer( wxHORIZONTAL );
	SetSizer( hsSliders );

	int leftslidercount = 0;
	int rightslidercount = 1;
	for ( int n = 0; n < 18; n++ )
	{
		slLeft[n] = new wxSlider( this, SliderMove+leftslidercount, 1, 0, 100, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxSL_VERTICAL );
		hsSliders->Add( slLeft[n] );
		slRight[n] = new wxSlider( this, SliderMove+rightslidercount, 1, 0, 100, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxSL_VERTICAL );
		hsSliders->Add( slRight[n] );

		leftslidercount += 2;
		rightslidercount += 2;
	}

	//--------------//
	//--- Layout ---//
	//--------------//
	Layout();
	Centre();

}

//--------------//
//--- Events ---//
//--------------//

void MusikFXFrame::Close()
{
	this->Destroy();
}

void MusikFXFrame::OnClose ( wxCommandEvent& WXUNUSED(event) )
{
	Close();
}

void MusikFXFrame::OnSlider( wxScrollEvent &event )
{
	float ldata[18], rdata[18];
	for ( int n = 0; n < 18; n++ )
	{
		ldata[n] = slLeft[n]->GetValue() * 0.1f;
		rdata[n] = slRight[n]->GetValue() * 0.01f;
	}
	g_FX.SetLeftBands( ldata );
	g_FX.SetRightBands( rdata );
}
