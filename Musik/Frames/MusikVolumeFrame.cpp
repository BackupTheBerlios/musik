/*
 *  MusikVolumeFrame.cpp
 *
 *  Frame that popups to adjust volume
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

//--- main header ---//
#include "MusikVolumeFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

BEGIN_EVENT_TABLE(MusikVolumeFrame, wxFrame)
	EVT_ACTIVATE			(MusikVolumeFrame::OnActivate		)
	EVT_CLOSE				(MusikVolumeFrame::OnClose			)
	EVT_CHAR				(MusikVolumeFrame::OnChar			)
END_EVENT_TABLE()

//--- default constructor ---//
MusikVolumeFrame::MusikVolumeFrame( wxFrame* pParent, wxPoint pos )
	: wxFrame( pParent, -1, _("Playback"), pos, wxSize( 16, 50 ), wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxCLIP_CHILDREN )
{
	//------------------------------//
	//--- initialize needed vars ---//
	//------------------------------//
	parent			= pParent;
	

	//----------------------------//
	//--- colours and controls ---//
	//----------------------------//
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
	pCrossfade = new wxCheckBox( this, MUSIK_CHK_CROSSFADE, _("Crossfade"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	gSeek = new wxGauge	( this, -1, 255, wxPoint( -1, -1 ), wxSize( wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y), -1 ), wxGA_SMOOTH | wxGA_VERTICAL | wxCLIP_CHILDREN );
	pSeekEvt = new CGaugeSeekEvt( gSeek, wxGA_VERTICAL );
	gSeek->PushEventHandler( pSeekEvt );
	static wxString choices[3] = {_("Normal"),_("Loop"),_("Shuffle")};
	m_pRadioBoxPlaymode = new wxRadioBox( this, -1, _("Play Mode"), wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices),choices, 1, wxRA_SPECIFY_COLS );

	pChecks = new wxBoxSizer( wxVERTICAL );
	pChecks->Add( m_pRadioBoxPlaymode, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT|wxALL, 2 );
	pChecks->Add( pCrossfade, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT|wxALL, 2 );

	//-----------------//
	//--- top sizer ---//
	//-----------------//
	wxBoxSizer *pTopSizer = new wxBoxSizer( wxHORIZONTAL );
	pTopSizer->Add( pChecks, 1, wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );
	pTopSizer->Add( gSeek, 0, wxEXPAND| wxALIGN_RIGHT| wxALL, 2 );


	//-------------------------//
	//--- initialize values ---//
	//-------------------------//
	gSeek->SetValue( g_Prefs.nSndVolume );
	pCrossfade->SetValue( g_Prefs.nGlobalFadeEnable );
	if( g_Prefs.nRepeat )
		m_pRadioBoxPlaymode->SetSelection( 1 );
	else if( g_Prefs.nShuffle )
		m_pRadioBoxPlaymode->SetSelection( 2 );
	else
		m_pRadioBoxPlaymode->SetSelection( 0 );

	SetSizerAndFit( pTopSizer );

	Layout();
}

//--- when window gets activated ---//
void MusikVolumeFrame::OnActivate( wxActivateEvent& event )
{
	if ( !event.GetActive() )
		Close();

}

//--- translate keypress ---//
void MusikVolumeFrame::OnChar( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE )
		Close();
}

//--- close func ---//
void MusikVolumeFrame::Close()
{
	int modesel = m_pRadioBoxPlaymode->GetSelection();
	g_Prefs.nRepeat = 0;
	g_Prefs.nShuffle = 0;

	switch(modesel)
	{
	case 0:
		break;
	case 1:
		g_Prefs.nRepeat = 1;
		break;
	case 2:
		g_Prefs.nShuffle = 1;
		break;
		
	default:
		break;
	}
	g_Prefs.nGlobalFadeEnable = pCrossfade->GetValue();
	g_Player.SetPlaymode();
	Destroy();
}
