/*
 *  MusikFXFrame.cpp
 *
 *  Front end to Musik's special FX engine.
 *
 *  Uses FMOD sound API and Shibatch EQ.
 *  Information about FMOD is available at http://www.fmod.org
 *	Information about Shibatch is available at http://shibatch.sourceforge.net
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

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

enum EMUSIK_FXFRAME_ID
{
	BTN_RESET = wxID_HIGHEST,
	CHK_EQENABLE,
	SLD_PITCH
};

BEGIN_EVENT_TABLE(MusikFXFrame, wxFrame)
	EVT_CLOSE ( MusikFXFrame::OnClose )
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
	
	//-------------------//
	//--- init sizers ---//
	//-------------------//
	hsLeftSliders	= new wxBoxSizer( wxHORIZONTAL );
	hsRightSliders	= new wxBoxSizer( wxHORIZONTAL );
	vsSliders		= new wxBoxSizer( wxVERTICAL );
	vsEQControls	= new wxBoxSizer( wxVERTICAL );
	hsEQ			= new wxBoxSizer( wxHORIZONTAL );
	hsControls		= new wxBoxSizer( wxHORIZONTAL );
	vsMain			= new wxBoxSizer( wxVERTICAL );

	vsSliders->Add( hsLeftSliders );
	vsSliders->Add( hsRightSliders );
	hsEQ->Add( vsSliders );
	hsEQ->Add( vsEQControls );

	vsMain->Add( hsEQ );
	vsMain->Add( hsControls );

	SetSizer( vsMain );

	for ( int n = 0; n < 18; n++ )
	{
		slLeft[n] = new CMusikFXGauge( this, MUSIK_FX_LEFT, n );
		slLeft[n]->SetToolTip( wxString::Format( _( "Left Channel: %d hz" ), freqs[n]) );
		hsLeftSliders->Add( slLeft[n] );

		slRight[n] = new CMusikFXGauge( this, MUSIK_FX_RIGHT, n );
		hsRightSliders->Add( slRight[n] );
		slRight[n]->SetToolTip( wxString::Format( _( "Right Channel: %d hz" ), freqs[n]) );
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
	g_Player.ActivateDSP();
}

///////////////////////////////////////////////////////////////////////////////

void MusikFXFrame::SlidersFromBands()
{
	g_FX.GetLeftBands( &ldata[0] );
	g_FX.GetRightBands( &rdata[0] );

	for ( int n = 0; n < 18; n++ )
	{
		slLeft[n]->SetValue( (int)( ldata[n] * 50.0f ) );
		slLeft[n]->Colourize();
		slRight[n]->SetValue( (int)( rdata[n] * 50.0f ) );
		slRight[n]->Colourize();
	}
}

void MusikFXFrame::BandsFromSliders()
{
	for ( int n = 0; n < 18; n++ )
	{
		ldata[n] = (float)slLeft[n]->GetValue() / 50.0f;
		rdata[n] = (float)slRight[n]->GetValue() / 50.0f;
	}

	g_FX.SetLeftBands( ldata );
	g_FX.SetRightBands( rdata );
	g_FX.MakeTable( 44100 );
}
