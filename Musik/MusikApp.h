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
#include <wx/snglinst.h>

class MusikApp : public wxApp
{
public:
	~MusikApp() 
	{
		delete m_pSingleInstanceChecker;
	}
	virtual bool OnInit();
private:
	wxString ReadVersion();
	void WriteVersion();
	void CheckVersion();

	wxSingleInstanceChecker *m_pSingleInstanceChecker;
};

DECLARE_APP(MusikApp)

#endif
