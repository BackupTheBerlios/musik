/*
 *  PlaylistInfoCtrl.cpp
 *
 *  The "Playlist Info" information
 *	This is the control visible on the main frame between
 *	the activity area and the playlist.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "PlaylistInfoCtrl.h"
#include "PlaylistCtrl.h"
//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

CPlaylistInfoCtrl::CPlaylistInfoCtrl( wxWindow *parent ,IPlaylistInfo *pIPlaylistInfo )
	: wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxCLIP_CHILDREN|wxTAB_TRAVERSAL |wxTRANSPARENT_WINDOW)
	,m_pIPlaylistInfo( pIPlaylistInfo )
{

//	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );
	SetBackgroundColour( WXSYSTEMCOLOUR(wxT("LIGHT STEEL BLUE")));

 	//--- static text objects ---/
	stTotal			= new wxStaticText_NoFlicker( this, -1, _( "Total Songs: " ),			wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stTotalVal		= new wxStaticText_NoFlicker( this, -1, wxT( "0" ),					wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stPlaylist		= new wxStaticText_NoFlicker( this, -1, _( "Playlist Songs: " ),		wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stPlaylistVal	= new wxStaticText_NoFlicker( this, -1, wxT( "0" ),					wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stRuntime		= new wxStaticText_NoFlicker( this, -1, _( "Playlist Runtime: " ),	wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stRuntimeVal	= new wxStaticText_NoFlicker( this, -1, wxT( "0:00" ),				wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stFilesize		= new wxStaticText_NoFlicker( this, -1, _( "Playlist size: " ),		wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stFilesizeVal	= new wxStaticText_NoFlicker( this, -1, wxT( "Empty" ),				wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	

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
	pSizer->Add( stTotal,		0, wxLEFT | wxALIGN_CENTER   , 0 );
	pSizer->Add( stTotalVal,	0, wxLEFT | wxALIGN_CENTER | wxADJUST_MINSIZE  , 2 );
	pSizer->Add( stPlaylist,	0, wxLEFT | wxALIGN_CENTER   , 4 );
	pSizer->Add( stPlaylistVal,	0, wxLEFT | wxALIGN_CENTER | wxADJUST_MINSIZE  , 2 );
	pSizer->Add( stRuntime,		0, wxLEFT | wxALIGN_CENTER   , 4 );
	pSizer->Add( stRuntimeVal,	0, wxLEFT | wxALIGN_CENTER | wxADJUST_MINSIZE  , 2 );   
	pSizer->Add( stFilesize,	0, wxLEFT | wxALIGN_CENTER   , 4 );
	pSizer->Add( stFilesizeVal,	0, wxLEFT | wxALIGN_CENTER | wxADJUST_MINSIZE  , 2 );

	//--- top sizer, vertical ---//
	pVert = new wxBoxSizer( wxVERTICAL );
	pVert->Add( pSizer, 0, wxALIGN_CENTER , 0 );

	SetSizerAndFit( pVert );
	Layout();
}

void CPlaylistInfoCtrl::Update()
{
	//--- playlist info ---//
	if ( wxGetApp().Prefs.bShowPLInfo == 1 )
	{
		stTotalVal->SetLabel( wxString::Format( wxT( "%d" ), wxGetApp().Library.GetSongCount() ) );
		stPlaylistVal->SetLabel( wxString::Format( wxT( "%d" ), m_pIPlaylistInfo->GetCount() ) );
		int nPlaylistTime = m_pIPlaylistInfo->GetTotalPlayingTimeInSeconds();
		wxTimeSpan PlayTime( 0, 0, nPlaylistTime );
		stRuntimeVal->SetLabel ( PlayTime.Format() );
		wxString strsize( wxT("0.0 mb") );
		double kbsize = 1024.0;
		double totsize = m_pIPlaylistInfo->GetTotalFilesize().GetValue();
		if ( totsize < kbsize )
			strsize = wxString::Format( wxT("%.2f b"),totsize );
		else if ( totsize < ( kbsize * kbsize ) )
			strsize = wxString::Format( wxT("%.2f kb"), totsize / kbsize );
		else if ( totsize < ( kbsize * kbsize * kbsize ) )
			strsize = wxString::Format( wxT("%.2f mb"), totsize / kbsize / kbsize);
		else if ( totsize < ( kbsize * kbsize * kbsize * kbsize ) )
			strsize = wxString::Format( wxT("%.2f gb"), totsize / kbsize / kbsize / kbsize );
		stFilesizeVal->SetLabel( strsize );
	}
	Layout();
}
