/*
 *  MusikFXFrame.h
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

#ifndef MUSIKFXFRAME_H
#define MUSIKFXFRAME_H

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "../Classes/MusikEQCtrl.h"

enum EMUSIK_FX_OBJECT_ID
{
};

class MusikFXFrame : public wxFrame
{
public:
	MusikFXFrame( wxFrame* pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size );

	//--------------//
	//--- events ---//
	//--------------//
	void OnClose			( wxCommandEvent& WXUNUSED(event) );
	void OnSlidePitch		( wxScrollEvent& event );
	void OnRightClick		( wxCommandEvent& event );

	
	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void Close();
	
	//----------------//
	//--- controls ---//
	//----------------//
	CMusikEQCtrl			*pEQ;
	wxSlider				*slPitch;
	
	//--------------//
	//--- sizers ---//
	//--------------//
	wxBoxSizer				*vsMain;
	
	DECLARE_EVENT_TABLE()

private:


};

#endif
