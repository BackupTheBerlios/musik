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
	: wxFrame( pParent, -1, wxT("Playback"), pos, wxSize( 16, 90 ), wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxCLIP_CHILDREN )
{
	//------------------------------//
	//--- initialize needed vars ---//
	//------------------------------//
	parent			= pParent;
	nFirstRun		= 1;

	//----------------------------//
	//--- colours and controls ---//
	//----------------------------//
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
	pRepeat = new wxCheckBox( this, MUSIK_CHK_REPEAT, _("Repeat"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	pShuffle = new wxCheckBox( this, MUSIK_CHK_SHUFFLE, _("Shuffle"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	pCrossfade = new wxCheckBox( this, MUSIK_CHK_CROSSFADE, _("Crossfade"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	gSeek = new wxGauge		( this, -1, 255, wxPoint( 0, 0 ), wxSize( wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y), 72 ), wxGA_SMOOTH | wxGA_VERTICAL | wxCLIP_CHILDREN );
	pSeekEvt = new CGaugeSeekEvt	( gSeek, wxGA_VERTICAL );
	gSeek->PushEventHandler			( pSeekEvt );

	pChecks = new wxBoxSizer( wxVERTICAL );
	pChecks->Add( pRepeat, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 2 );
	pChecks->Add( pShuffle, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 2 );
	pChecks->Add( pCrossfade, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 2 );

	//-----------------//
	//--- top sizer ---//
	//-----------------//
	wxBoxSizer *pTopSizer = new wxBoxSizer( wxHORIZONTAL );
	pTopSizer->Add( gSeek, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );
	pTopSizer->Add( pChecks, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );

	SetSizerAndFit( pTopSizer );

	Layout();
}

//--- when window gets activated ---//
void MusikVolumeFrame::OnActivate( wxActivateEvent& event )
{
	if ( !event.GetActive() )
		Close();

	if ( nFirstRun == 1 )
	{
		gSeek->SetValue( g_Prefs.nSndVolume );
		pRepeat->SetValue( g_Prefs.nRepeat );
		pShuffle->SetValue( g_Prefs.nShuffle );
		pCrossfade->SetValue( g_Prefs.nGlobalFadeEnable );
		nFirstRun = 0;
	}
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
	g_Prefs.nRepeat = pRepeat->GetValue();
	g_Prefs.nShuffle = pShuffle->GetValue();
	g_Prefs.nGlobalFadeEnable = pCrossfade->GetValue();
	g_Player.SetPlaymode();
	Destroy();
}
