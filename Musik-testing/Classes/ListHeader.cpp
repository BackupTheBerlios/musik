/*
 *  ListHeader.cpp
 *
 *  Header used for ActivityBox and SourcesBox controls
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "ListHeader.h"

//--- globals ---//
#include "../MusikGlobals.h"

//-------------------//
//--- CListHeader ---//
//-------------------//
CListHeader::CListHeader( wxPanel *pParent, wxString sCaption )
	: wxPanel( pParent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxNO_BORDER )
{
	//--- colors we'll use ---//
	wxColour background = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW );
	wxColour white = wxColour( 255, 255, 255 );
	wxColour black = wxColour( 0, 0, 0 );
	int nAvg = ( background.Red() + background.Blue() + background.Green() ) / 3;
	this->SetBackgroundColour( background );

	//--- header object ---//
	pHeader = new wxStaticText( this, -1, sCaption, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_CENTRE | wxTRANSPARENT_WINDOW );
	pHeader->SetBackgroundColour( background );

	//--- font for header ---//
	if ( nAvg > 200 )
		pHeader->SetForegroundColour( black );
	else
		pHeader->SetForegroundColour( white );
	pHeader->SetFont( g_fntBold );

	//--- sizer ---//
	pSizer = new wxBoxSizer( wxHORIZONTAL );
	pSizer->Add( pHeader, 1, wxTOP | wxBOTTOM, 2 );
	SetSizerAndFit( pSizer );
}

CListHeader::~CListHeader()
{
	delete pHeader;
}

void CListHeader::SetCaption( wxString sCaption )
{
	pHeader->SetLabel( sCaption );
	Layout();
}
