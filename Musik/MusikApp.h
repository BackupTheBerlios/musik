/*
 *  MusikApp.h
 *
 *  Application start point
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_APP_H
#define MUSIK_APP_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "wx/taskbar.h"

class MusikTaskBarIcon: public wxTaskBarIcon
{
public:
	MusikTaskBarIcon() {};

	void SetFrame(wxFrame * frame) {m_pFrame = frame;}
	void RestoreFrame();
    void OnRButtonUp(wxEvent&);
    void OnLButtonDClick(wxEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuHide(wxCommandEvent&);
    void OnMenuPlayPause(wxCommandEvent&);
    void OnMenuPrev(wxCommandEvent&);
    void OnMenuNext(wxCommandEvent&);
    void OnMenuStop(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);

DECLARE_EVENT_TABLE()

private:
	wxFrame *m_pFrame;
};


class MusikApp : public wxApp
{
public:
	virtual bool OnInit();
	MusikTaskBarIcon   TaskBarIcon;
private:
	wxString ReadVersion();
	void WriteVersion();
	void CheckVersion();
};

DECLARE_APP(MusikApp)

#endif
