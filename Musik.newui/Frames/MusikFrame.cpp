/*
 *  MusikFrame.cpp
 *
 *  Musik's primary frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

//--- images ---//
#include "../images/sources/dynamic.xpm"
#include "../images/sources/library.xpm"
#include "../images/sources/standard.xpm"
#include "../images/ratings/rating0.xpm"
#include "../images/ratings/rating1.xpm"
#include "../images/ratings/rating2.xpm"
#include "../images/ratings/rating3.xpm"	
#include "../images/ratings/rating4.xpm"
#include "../images/ratings/rating5.xpm"

// main dialog constructor
MusikFrame::MusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, wxT( "Musik Test UI" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN )
{
	SetIcon( wxICON( musicbox ) );
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
}

MusikFrame::~MusikFrame()
{
}