/*
 *  MusikLibraryThreads.h
 *
 *  Threads related to MusikLibraryFrame.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_LIBRARY_THREADS_H
#define MUSIK_LIBRARY_THREADS_H

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "wx/thread.h"
#include <wx/dir.h>
#include <wx/filename.h>

//---------------------------------------------//
//--- NOTE: use g_MusikLibraryFrame as the	---//
//--- parent window.						---//
//---------------------------------------------//
class MusikUpdateLibThread : public wxThread
{
public:
	MusikUpdateLibThread( wxArrayString* del );

	virtual void *Entry();
	virtual void OnExit();

private:
	wxArrayString* m_Add;
	wxArrayString* m_Del;
};

class MusikScanNewThread : public wxThread
{
public:
	MusikScanNewThread();

	virtual void *Entry();
	virtual void OnExit();

};

class MusikPurgeLibThread : public wxThread
{
public:
	MusikPurgeLibThread();

	virtual void *Entry();
	virtual void OnExit();

};

#endif

