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
	EVT_ERASE_BACKGROUND		( MusikFXFrame::OnEraseBackground )
END_EVENT_TABLE()

void MusikFXFrame::OnEraseBackground( wxEraseEvent& (event) )
{	
	// empty => no background erasing to avoid flicker

	wxDC * TheDC = event.m_dc;
	wxColour BGColor =  GetBackgroundColour();
	wxBrush MyBrush(BGColor ,wxSOLID);
	TheDC->SetBackground(MyBrush);

	wxCoord width,height;
	TheDC->GetSize(&width,&height);
	wxCoord x,y,w,h;
	TheDC->GetClippingBox(&x,&y,&w,&h); 

	// Now  declare the Clipping Region which is
	// what needs to be repainted
	wxRegion MyRegion(x,y,w,h); 

	//Get all the windows(controls)  rect's on the dialog
	wxWindowList & children = GetChildren();
	for ( wxWindowList::Node *node = children.GetFirst(); node; node = node->GetNext() )
	{
		wxWindow *current = (wxWindow *)node->GetData();

		// now subtract out the controls rect from the
		//clipping region
		MyRegion.Subtract(current->GetRect());
	}

	



	

	// now destroy the old clipping region
	TheDC->DestroyClippingRegion();

	//and set the new one
	TheDC->SetClippingRegion(MyRegion);
	TheDC->Clear();
}
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0
#endif

MusikFXFrame::MusikFXFrame( wxFrame *pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size ) 
	: wxFrame ( pParent, MUSIK_FRAME_ID_FX, sTitle, pos, size, wxRESIZE_BORDER|wxCAPTION | wxTAB_TRAVERSAL |wxCLIP_CHILDREN| wxCLOSE_BOX | wxSYSTEM_MENU | wxFRAME_NO_TASKBAR )
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

	vsMain->Add( pEQ, 1, wxEXPAND|wxALL, 4 );
	
	vsMain->Add( hsPitch, 0,wxEXPAND | wxALL, 4 );
	

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
