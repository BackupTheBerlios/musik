/*
 *  MusikEQCtrl.h
 *
 *  Equalizer control. Used by MusikFXFrame.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_EQ_CTRL_H
#define MUSIK_EQ_CTRL_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "MusikEQGauge.h"

enum EMUSIK_EQ_OBJECT_ID
{
	BTN_RESET = wxID_HIGHEST,
	CHK_EQENABLE
};

class CMusikEQCtrl : public wxPanel
{
public:

	CMusikEQCtrl( wxFrame* pParent );
	~CMusikEQCtrl();

	//-----------------//
	//--- functions ---//
	//-----------------//
	void OnToggleEQEnable	( wxCommandEvent& WXUNUSED(event) );
	void OnClickReset		( wxCommandEvent& WXUNUSED(event) );
	void SlidersFromBands	();
	void BandsFromSliders	();

	//------------------//
	//--- eq sliders ---//
	//------------------//
	CMusikEQGauge			*slLeft[18];
	CMusikEQGauge			*slRight[18];

	//------------------//
	//--- eq options ---//
	//------------------//
	wxCheckBox				*chkLock;
	wxCheckBox				*chkEQEnable;
	wxButton				*btnReset;

	//--------------//
	//--- sizers ---//
	//--------------//
	wxFlexGridSizer			*pBandsSizer;
	wxFlexGridSizer			*pOptionsSizer;
	wxBoxSizer				*pMainSizer;

	//-------------//
	//--- bands ---//
	//-------------//
	float ldata[18];	
	float rdata[18];

	DECLARE_EVENT_TABLE()
};

#endif