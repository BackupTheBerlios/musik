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
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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

enum EMUSIK_FXFRAME_ID
{
	SLD_PITCH = wxID_HIGHEST
};

BEGIN_EVENT_TABLE(MusikFXFrame, wxFrame)
	EVT_CLOSE				(					MusikFXFrame::OnClose			)
	EVT_CONTEXT_MENU		(					MusikFXFrame::OnRightClick		)
	EVT_COMMAND_SCROLL		( SLD_PITCH,		MusikFXFrame::OnSlidePitch		)
	EVT_CHECKBOX			( CHK_PITCHENABLE,	MusikFXFrame::OnTogglePitchEnable )
END_EVENT_TABLE()

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0
#endif

MusikFXFrame::MusikFXFrame( wxFrame *pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size ) 
	: wxFrame ( pParent, -1, sTitle, pos, size, wxCAPTION | wxTAB_TRAVERSAL | wxCLOSE_BOX | wxSYSTEM_MENU | wxFRAME_NO_TASKBAR )
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
	vsMain = new wxBoxSizer( wxVERTICAL );
	hsPitch = new wxBoxSizer( wxHORIZONTAL );

	//------------------//
	//--- equalizers ---//
	//------------------//
	pEQ = new CMusikEQCtrl( this );

	//-------------//
	//--- pitch ---//
	//-------------//
	//-------------------------------------------------//
	//--- Simon: not working right. commented out	---//
	//--- for 0.1.3 release.						---//
	//--- See Also: MusikPlayer::SetFrequency()		---//
	//-------------------------------------------------//
	
	slPitch = new wxSlider( this, SLD_PITCH, 50, 0, 100, wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	slPitch->SetToolTip( _("Pitch control, right-click to reset") );
	
	chkPitchEnable = new wxCheckBox( this, CHK_PITCHENABLE, _("Enable Pitch control") );
	chkPitchEnable->SetValue( wxGetApp().Prefs.bUsePitch );
	hsPitch->Add( chkPitchEnable, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	hsPitch->Add( slPitch, 0, wxALL, 4 );

	vsMain->Add( pEQ, 0, wxALL, 4 );
	
	vsMain->Add( hsPitch, 0, wxALL, 4 );
	

	SetSizerAndFit( vsMain );

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

void MusikFXFrame::OnClose ( wxCloseEvent& WXUNUSED(event) )
{
	Close();
}

void MusikFXFrame::OnSlidePitch( wxScrollEvent &WXUNUSED(event) )
{
	g_FX.SetFrequency( (( (float)slPitch->GetValue()) / 50.0f) * 44100.0f );
}

void MusikFXFrame::OnRightClick( wxContextMenuEvent& event )
{
	if ( event.m_id == SLD_PITCH )
	{
		slPitch->SetValue( 50 );
		g_FX.SetFrequency( 44100 );
	}
}

void MusikFXFrame::OnTogglePitchEnable( wxCommandEvent& WXUNUSED(event) )
{
	wxGetApp().Prefs.bUsePitch = chkPitchEnable->IsChecked();
}