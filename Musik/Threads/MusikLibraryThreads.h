/*
 *  MusikLibraryThreads.h
 *
 *  Threads related to MusikLibraryFrame.h / cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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

class wxMusicTraverser;
//---------------------------------------------//
//--- NOTE: use g_MusikLibraryFrame as the	---//
//--- parent window.						---//
//---------------------------------------------//

class MusikScanNewThread : public wxThread
{
public:
	MusikScanNewThread(wxArrayString & m_refFiles);

	virtual void *Entry();
	virtual void OnExit();
protected:

	void GetMusicDirs (  const wxArrayString & aDirs, wxArrayString & aFiles );
	void GetMusicDir  ( const wxString & sDir, wxArrayString & aFiles );


	wxArrayString & m_refFiles;
	
	friend class wxMusicTraverser;
};

class MusikUpdateLibThread : public MusikScanNewThread
{
public:
	MusikUpdateLibThread( wxArrayString* del , wxArrayString & m_refFiles, bool bCompleteRebuild );

	virtual void *Entry();
	virtual void OnExit();

private:
	wxArrayString* m_Add;
	wxArrayString* m_Del;
	bool m_bCompleteRebuild;
};



class MusikPurgeLibThread : public wxThread
{
public:
	MusikPurgeLibThread();

	virtual void *Entry();
	virtual void OnExit();

};

#endif

