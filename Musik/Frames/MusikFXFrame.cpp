
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

int freqs[] = {55,77,110,156,220,311,440,622,880,1244,1760,2489,3520,4978,7040,9956,14080,19912};

enum
{
	SBASE = wxID_HIGHEST,
    SL0,
	SR0,
	SL1,
	SR1,
	SL2,
	SR2,
	SL3,
	SR3,
	SL4,
	SR4,
	SL5,
	SR5,
	SL6,
	SR6,
	SL7,
	SR7,
	SL8,
	SR8,
	SL9,
	SR9,
	SL10,
	SR10,
	SL11,
	SR11,
	SL12,
	SR12,
	SL13,
	SR13,
	SL14,
	SR14,
	SL15,
	SR15,
	SL16,
	SR16,
	SL17,
	SR17,
	BTN_RESET,
	CHK_EQENABLE,
	SLD_PITCH
};

BEGIN_EVENT_TABLE(MusikFXFrame, wxFrame)
	EVT_CLOSE ( MusikFXFrame::OnClose )
	EVT_COMMAND_SCROLL( SL0, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL1, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL2, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL3, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL4, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL5, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL6, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL7, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL8, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL9, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL10, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL11, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL12, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL13, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL14, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL15, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL16, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SL17, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR0, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR1, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR2, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR3, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR4, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR5, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR6, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR7, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR8, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR9, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR10, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR11, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR12, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR13, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR14, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR15, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR16, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SR17, MusikFXFrame::OnSlider )
	EVT_COMMAND_SCROLL( SLD_PITCH, MusikFXFrame::OnSlidePitch )
	EVT_CONTEXT_MENU( MusikFXFrame::OnRightClick )
	EVT_BUTTON( BTN_RESET, MusikFXFrame::OnClickReset )
	EVT_CHECKBOX( CHK_EQENABLE, MusikFXFrame::OnToggleEQEnable )
END_EVENT_TABLE()

MusikFXFrame::MusikFXFrame( wxFrame *pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size ) 
	: wxFrame( pParent, -1, sTitle, pos, size, wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxFRAME_NO_TASKBAR )
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
	
	hsLeftSliders = new wxBoxSizer( wxHORIZONTAL );
	hsRightSliders = new wxBoxSizer( wxHORIZONTAL );
	vsSliders = new wxBoxSizer( wxVERTICAL );
	vsEQControls = new wxBoxSizer( wxVERTICAL );
	hsEQ = new wxBoxSizer( wxHORIZONTAL );
	
	
	hsControls = new wxBoxSizer( wxHORIZONTAL );
	vsMain = new wxBoxSizer( wxVERTICAL );

	vsSliders->Add( hsLeftSliders );
	vsSliders->Add( hsRightSliders );
	hsEQ->Add( vsSliders );
	hsEQ->Add( vsEQControls );

	vsMain->Add( hsEQ );
	vsMain->Add( hsControls );

	SetSizer( vsMain );

	int leftslidercount = 1;
	int rightslidercount = 2;
	for ( int n = 0; n < 18; n++ )
	{
		slLeft[n] = new wxSlider( this, SBASE+leftslidercount, 50, 0, 100, wxPoint( -1, -1 ), wxSize( 16, 100 ), wxSL_VERTICAL );
		slLeft[n]->SetToolTip( wxString::Format( _("left %d hz"), freqs[n]) );
		hsLeftSliders->Add( slLeft[n] );
		slRight[n] = new wxSlider( this, SBASE+rightslidercount, 50, 0, 100, wxPoint( -1, -1 ), wxSize( 16, 100 ), wxSL_VERTICAL );
		hsRightSliders->Add( slRight[n] );
		slRight[n]->SetToolTip( wxString::Format( _("right %d hz"), freqs[n]) );

		leftslidercount += 2;
		rightslidercount += 2;
	}

	chkEQEnable = new wxCheckBox( this, CHK_EQENABLE, _("Enable EQ") );
	vsEQControls->Add( chkEQEnable, 1 );

	chkLock = new wxCheckBox( this, -1, _("Lock channels") );
	chkLock->SetValue( true );
	vsEQControls->Add( chkLock, 1 );
	btnReset = new wxButton( this, BTN_RESET, _("Reset bands") );
	vsEQControls->Add( btnReset, 1 );

	slPitch = new wxSlider( this, SLD_PITCH, 50, 0, 100, wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	slPitch->SetToolTip( _("Pitch control, right-click to reset") );
	hsControls->Add( slPitch );

	//--------------//
	//--- Layout ---//
	//--------------//
	Layout();
	Centre();
	
	chkEQEnable->SetValue( g_Prefs.nUseEQ );
	SlidersFromBands();

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

void MusikFXFrame::OnClickReset( wxCommandEvent& WXUNUSED(event) )
{
	g_FX.ResetBands();
	SlidersFromBands();
}

void MusikFXFrame::OnSlider( wxScrollEvent &event )
{
	// if we're supposed to be locked, figure out which was moved and move
	// the corresponding one
	if ( chkLock->IsChecked() )
	{
		int movedid = event.m_id;
		switch ( movedid )
		{
		case SL0:
			slRight[0]->SetValue( event.GetPosition() );
			break;
		case SL1:
			slRight[1]->SetValue( event.GetPosition() );
			break;
		case SL2:
			slRight[2]->SetValue( event.GetPosition() );
			break;
		case SL3:
			slRight[3]->SetValue( event.GetPosition() );
			break;
		case SL4:
			slRight[4]->SetValue( event.GetPosition() );
			break;
		case SL5:
			slRight[5]->SetValue( event.GetPosition() );
			break;
		case SL6:
			slRight[6]->SetValue( event.GetPosition() );
			break;
		case SL7:
			slRight[7]->SetValue( event.GetPosition() );
			break;
		case SL8:
			slRight[8]->SetValue( event.GetPosition() );
			break;
		case SL9:
			slRight[9]->SetValue( event.GetPosition() );
			break;
		case SL10:
			slRight[10]->SetValue( event.GetPosition() );
			break;
		case SL11:
			slRight[11]->SetValue( event.GetPosition() );
			break;
		case SL12:
			slRight[12]->SetValue( event.GetPosition() );
			break;
		case SL13:
			slRight[13]->SetValue( event.GetPosition() );
			break;
		case SL14:
			slRight[14]->SetValue( event.GetPosition() );
			break;
		case SL15:
			slRight[15]->SetValue( event.GetPosition() );
			break;
		case SL16:
			slRight[16]->SetValue( event.GetPosition() );
			break;
		case SL17:
			slRight[17]->SetValue( event.GetPosition() );
			break;
		case SR0:
			slLeft[0]->SetValue( event.GetPosition() );
			break;
		case SR1:
			slLeft[1]->SetValue( event.GetPosition() );
			break;
		case SR2:
			slLeft[2]->SetValue( event.GetPosition() );
			break;
		case SR3:
			slLeft[3]->SetValue( event.GetPosition() );
			break;
		case SR4:
			slLeft[4]->SetValue( event.GetPosition() );
			break;
		case SR5:
			slLeft[5]->SetValue( event.GetPosition() );
			break;
		case SR6:
			slLeft[6]->SetValue( event.GetPosition() );
			break;
		case SR7:
			slLeft[7]->SetValue( event.GetPosition() );
			break;
		case SR8:
			slLeft[8]->SetValue( event.GetPosition() );
			break;
		case SR9:
			slLeft[9]->SetValue( event.GetPosition() );
			break;
		case SR10:
			slLeft[10]->SetValue( event.GetPosition() );
			break;
		case SR11:
			slLeft[11]->SetValue( event.GetPosition() );
			break;
		case SR12:
			slLeft[12]->SetValue( event.GetPosition() );
			break;
		case SR13:
			slLeft[13]->SetValue( event.GetPosition() );
			break;
		case SR14:
			slLeft[14]->SetValue( event.GetPosition() );
			break;
		case SR15:
			slLeft[15]->SetValue( event.GetPosition() );
			break;
		case SR16:
			slLeft[16]->SetValue( event.GetPosition() );
			break;
		case SR17:
			slLeft[17]->SetValue( event.GetPosition() );
			break;
		}

	}

	
	if ( event.m_eventType == wxEVT_SCROLL_THUMBRELEASE )
		BandsFromSliders();
}

void MusikFXFrame::OnSlidePitch( wxScrollEvent &event )
{
	g_FX.SetFrequency( (( (float)slPitch->GetValue()) / 50.0f) * 44100.0f );
}

void MusikFXFrame::OnRightClick( wxCommandEvent& event )
{
	if ( event.m_id == SLD_PITCH )
	{
		slPitch->SetValue( 50 );
		g_FX.SetFrequency( 44100 );
	}
}

void MusikFXFrame::OnToggleEQEnable( wxCommandEvent& WXUNUSED(event) )
{
	g_Prefs.nUseEQ = chkEQEnable->IsChecked();
	g_Player.SetFX();
}

///////////////////////////////////////////////////////////////////////////////

void MusikFXFrame::SlidersFromBands()
{
	float ldata[18], rdata[18];
	g_FX.GetLeftBands( &ldata[0] );
	g_FX.GetRightBands( &rdata[0] );

	for ( int n = 0; n < 18; n++ )
	{
		slLeft[n]->SetValue( (int)( 100.0f - ( ldata[n] * 50.0f ) ) );
		slRight[n]->SetValue( (int)( 100.0f - ( rdata[n] * 50.0f ) ) );
	}
}

void MusikFXFrame::BandsFromSliders()
{
	float ldata[18], rdata[18];
	for ( int n = 0; n < 18; n++ )
	{
		ldata[n] = (100.0f - (float)slLeft[n]->GetValue()) / 50.0f;
		rdata[n] = (100.0f - (float)slRight[n]->GetValue()) / 50.0f;
	}
	g_FX.SetLeftBands( ldata );
	g_FX.SetRightBands( rdata );
	g_FX.MakeTable( 44100 );
}
