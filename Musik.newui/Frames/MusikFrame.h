/*
 *  MusikFrame.h
 *
 *  Musik's primary frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_FRAME_H
#define MUSIK_FRAME_H

#include <wx/socket.h>
#include "../Classes/MusikSourcesCtrl.h"

class MusikFrame : public wxFrame
{
public:
	MusikFrame();
	~MusikFrame();

	CMusikSourcesCtrl *m_SourcesCtrl;
	
};

#endif
