/*
 *  MusikApp.cpp
 *
 *  Application start point
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"
#include "wx/textfile.h"

#include "MusikApp.h"
IMPLEMENT_APP( MusikApp )

//--- related frames ---//
#include "Frames/MusikFrame.h"

bool MusikApp::OnInit()
{
    MusikFrame *pMain = new MusikFrame();
	pMain->Show(TRUE);
	SetTopWindow( pMain );

	return TRUE;
}