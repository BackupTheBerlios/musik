/*
 *  PlaylistInfoCtrl.h
 *
 *  The "Playlist Info" information
 *	This is the control visible on the main frame between
 *	the activity area and the playlist.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_PLAYLISTINFO_CTRL
#define MUSIK_PLAYLISTINFO_CTRL

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

class CPlaylistInfoCtrl : public wxPanel
{
public:
	CPlaylistInfoCtrl( wxWindow *pParent );

	wxStaticText* stTotal;		
	wxStaticText* stTotalVal;
	wxStaticText* stPlaylist;		
	wxStaticText* stPlaylistVal;
	wxStaticText* stRuntime;	
	wxStaticText* stRuntimeVal;	

	wxBoxSizer *pSizer;
	wxBoxSizer *pVert;

	void Update();
private:

};

#endif
