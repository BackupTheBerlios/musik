/*
 *  MusikGlobals.cpp
 *
 *  Global objects that are common to (most) all classes and
 *  derived / inherited objects.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

//----------------//
//--- includes ---//
//----------------//
#include "MusikGlobals.h"


//----------------------------//
//--- pointer to MusikFrame ---//
//----------------------------//
MusikFrame *g_MusikFrame;


//------------------------------------//
//--- pointer to MusikLibraryFrame ---//
//------------------------------------//
MusikLibraryFrame *g_MusikLibraryFrame;

//-----------------------------//
//--- playlist column stuff ---//
//-----------------------------//
const int g_PlaylistColumnAlign[NPLAYLISTCOLUMNS] =
{
	wxLIST_FORMAT_CENTER,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_CENTER,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT	
};

extern const wxString g_PlaylistColumnLabels[NPLAYLISTCOLUMNS] =
{
	_( "Rating"		),
	_( "Track"		),
	_( "Title"		),
	_( "Artist"		),
	_( "Album"		),
	_( "Year"			),
	_( "Genre"		),
	_( "Times Played" ),
	_( "Last Played"	),
	_( "Time"			),
	_( "Bitrate"		),
	_( "Filename"		),
	_( "Notes"			),
	_( "Added on"		)

};
extern const wxString g_PlaylistColumnDBNames[NPLAYLISTCOLUMNS] =
{
	wxT( "rating"		),
	wxT( "tracknum"		),
	wxT( "title"		),
	wxT( "artist"		),
	wxT( "album"		),
	wxT( "year"			),
	wxT( "genre"		),
	wxT( "timesplayed" ),
	wxT( "lastplayed"	),
	wxT( "duration"		),
	wxT( "bitrate"		),
	wxT( "filename"		),
	wxT( "notes"		),
	wxT( "timeadded"	)

};
//------------------------//
//--- internal objects ---//
//------------------------//
CMusikPrefs			g_Prefs;
CMusikLibrary		g_Library;
CMusikPlayer		g_Player;
CMusikPaths			g_Paths;
CMusikSongArray		g_Playlist;	
CMusikSongArray		g_LibPlaylist;
CMusikWebServer		g_WebServer;

wxCriticalSection	g_protectingStreamArrays; // to protect access to ActiveStreams and ActiveChannels
CMusikStreamArray	g_ActiveStreams;
 wxArrayInt			g_ActiveChannels;
MusikFaderThread*	g_FaderThread;
wxImageList*		g_SourcesImages; 
wxImageList*		g_NullImageList;
wxImageList*		g_RatingImages;
CMusikFX			g_FX;

CTunage				g_Tunage;

//----------------//
//--- controls ---//
//----------------//
CActivityAreaCtrl	*g_ActivityAreaCtrl;
CPlaylistBox		*g_PlaylistBox;
CSourcesBox			*g_SourcesCtrl;
CNowPlayingCtrl		*g_NowPlayingCtrl;
wxGauge				*g_Progress;

//-------------//
//--- fonts ---//
//-------------//
wxFont g_fntArtist;
wxFont g_fntSong;
wxFont g_fntTime;
wxFont g_fntRegular;
wxFont g_fntBold;
wxFont g_fntInfo;
wxFont g_fntListBold;

//------------//
//--- misc ---//
//------------//
bool g_SelInProg;
bool g_FocusDone;
bool g_DragInProg;
bool g_PlaylistChanged;
bool g_TimeSeeking;
bool g_DisablePlacement;

