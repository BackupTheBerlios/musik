
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

BEGIN_EVENT_TABLE(MusikFXFrame, wxFrame)
	EVT_CLOSE ( MusikFXFrame::OnClose )
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