/*
 *  MusikGlobals.cpp
 *
 *  Global objects that are common to (most) all classes and
 *  derived / inherited objects.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

//----------------//
//--- includes ---//
//----------------//
#include "MusikGlobals.h"

//----------------------//
//--- pointer to app ---//
//----------------------//
MusikApp *pMusikApp;

//----------------------------//
//--- pointer to MusikFrame ---//
//----------------------------//
MusikFrame *g_MusikFrame;

//-----------------------------------//
//--- pointer to MusikVolumeFrame ---//
//-----------------------------------//
MusikVolumeFrame *g_VolumeFrame;

//--------------------------------//
//--- pointer to MusikTagFrame ---//
//--------------------------------//
MusikTagFrame *g_MusikTagFrame;

//------------------------------------//
//--- pointer to MusikLibraryFrame ---//
//------------------------------------//
MusikLibraryFrame *g_MusikLibraryFrame;

//------------------------//
//--- internal objects ---//
//------------------------//
CMusikLibrary		g_Library;
CMusikPlayer		g_Player;
CMusikPrefs			g_Prefs;
CMusikPaths			g_Paths;
CMusikSongArray		g_Playlist;	
CMusikSongArray		g_LibPlaylist;
wxArrayString		g_Items1;
wxArrayString		g_Items2;
wxArrayString		g_Items3;
wxArrayString		g_Items4;
wxArrayString		g_SourcesList;
CMusikWebServer		g_WebServer;
CMusikStreamArray	g_ActiveStreams;
wxArrayInt			g_ActiveChannels;
MusikFaderThread*	g_FaderThread;
wxImageList*		g_SourcesImages; 
wxImageList*		g_NullImageList;
wxImageList*		g_RatingImages;

//----------------//
//--- controls ---//
//----------------//
CActivityAreaCtrl	*g_ActivityAreaCtrl;
CPlaylistCtrl		*g_PlaylistCtrl;
CSourcesBox			*g_SourcesCtrl;
CNowPlayingCtrl		*g_NowPlayingCtrl;
CPlaylistInfoCtrl	*g_PlaylistInfoCtrl;
wxGauge				*g_Progress;
wxTextCtrl			*g_SimpleQuery;

//-----------------------//
//--- main menu stuff ---//
//-----------------------//
wxMenu *file_menu;
wxMenu *library_menu;
wxMenu *library_writetags_menu;
wxMenu *view_menu;

//-------------//
//--- fonts ---//
//-------------//
wxFont g_fntArtist;
wxFont g_fntSong;
wxFont g_fntTime;
wxFont g_fntRegular;
wxFont g_fntBold;
wxFont g_fntInfo;

//------------//
//--- misc ---//
//------------//
bool g_SelInProg;
bool g_FocusDone;
bool g_DragInProg;
bool g_PlaylistChanged;
bool g_FirstRun;
bool g_TimeSeeking;
