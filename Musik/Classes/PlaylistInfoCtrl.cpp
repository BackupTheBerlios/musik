/*
 *  PlaylistInfoCtrl.cpp
 *
 *  The "Playlist Info" information
 *	This is the control visible on the main frame between
 *	the activity area and the playlist.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "PlaylistInfoCtrl.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

CPlaylistInfoCtrl::CPlaylistInfoCtrl( wxWindow *parent )
	: wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxTRANSPARENT_WINDOW )
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

 	//--- static text objects ---/
	stTotal			= new wxStaticText( this, -1, _( "Total Songs: " ),			wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stTotalVal		= new wxStaticText( this, -1, wxT( "0" ),					wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stPlaylist		= new wxStaticText( this, -1, _( "Playlist Songs: " ),		wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stPlaylistVal	= new wxStaticText( this, -1, wxT( "0" ),					wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stRuntime		= new wxStaticText( this, -1, _( "Playlist Runtime: " ),	wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stRuntimeVal	= new wxStaticText( this, -1, wxT( "0:00" ),				wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stFilesize		= new wxStaticText( this, -1, _( "Playlist size: " ),		wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stFilesizeVal	= new wxStaticText( this, -1, wxT( "0.0 mb" ),				wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	
	//--- window spacers ---//
	wxWindow *pSpacer1 = new wxWindow( this, -1, wxPoint( -1, -1 ), wxSize( -1, 0 ), wxNO_BORDER );
	wxWindow *pSpacer2 = new wxWindow( this, -1, wxPoint( -1, -1 ), wxSize( -1, 0 ), wxNO_BORDER );

	//--- font stuff ---//
	stTotal->SetFont		( g_fntRegular );
	stTotalVal->SetFont		( g_fntBold );
	stPlaylist->SetFont		( g_fntRegular );
	stPlaylistVal->SetFont	( g_fntBold );
	stRuntime->SetFont		( g_fntRegular );
	stRuntimeVal->SetFont	( g_fntBold );
	stFilesize->SetFont		( g_fntRegular );
	stFilesizeVal->SetFont	( g_fntBold );

	//--- playlist info sizer ---//
	pSizer = new wxBoxSizer	( wxHORIZONTAL	);
	pSizer->Add( pSpacer1,		1, wxEXPAND );
	pSizer->Add( stTotal,		0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 0 );
	pSizer->Add( stTotalVal,	0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 4 );
	pSizer->Add( stPlaylist,	0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 24 );
	pSizer->Add( stPlaylistVal,	0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 4 );
	pSizer->Add( stRuntime,		0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 24 );
	pSizer->Add( stRuntimeVal,	0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 4 );   
	pSizer->Add( stFilesize,	0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 24 );
	pSizer->Add( stFilesizeVal,	0, wxLEFT | wxALIGN_CENTER_HORIZONTAL | wxADJUST_MINSIZE  , 4 );
	pSizer->Add( pSpacer2,		1, wxEXPAND );

	//--- top sizer, vertical ---//
	pVert = new wxBoxSizer( wxVERTICAL );
	pVert->Add( pSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER | wxALL, 2 );

	SetSizerAndFit( pVert );
	Layout();
}

void CPlaylistInfoCtrl::Update()
{
	//--- playlist info ---//
	if ( g_Prefs.nShowPLInfo == 1 )
	{
		stTotalVal->SetLabel( wxString::Format( wxT( "%d" ), g_Library.GetSongCount() ) );
		stPlaylistVal->SetLabel( wxString::Format( wxT( "%d" ), g_PlaylistCtrl->GetItemCount() ) );
		int nPlaylistTime = g_PlaylistCtrl->GetTotalPlayingTimeInSeconds();
		wxTimeSpan PlayTime( 0, 0, nPlaylistTime );
		stRuntimeVal->SetLabel ( PlayTime.Format() );
		stFilesizeVal->SetLabel( g_PlaylistCtrl->GetTotalFilesize() );
	}
	Layout();
}
