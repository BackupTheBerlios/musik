/*
 *  MusikGlobals.h
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

#ifndef WX_INCLUDES_H
#define WX_INCLUDES_H

//--------------------------------------//
//--- ignore stupid warning messages ---//
//---  so we can see whats going on  ---//
//--------------------------------------//
#if defined (__WXMSW__)
	#pragma warning(disable : 4018)		//--- '<' signed/unsigned mismatch										---//
	#pragma warning(disable : 4288)		//--- nonstandard extension used : 'i'									---//
	#pragma warning(disable : 4238)		//--- nonstandard extension used : class rvalue as lvalue				---//
	#pragma warning(disable : 4245)		//--- conversion from ID3_TagType to flags_t signed/unsigned mismatch	---//
	#pragma warning(disable : 4267)		//--- '=' conversion from size_t to long								---//
	#pragma warning(disable : 4311)		//--- type cast from void* to long										---//
	#pragma warning(disable : 4700)		//--- local variable used without having been initialized				---//
	#pragma warning(disable : 4701)		//--- local variable used without being intialized						---//
	#pragma warning(disable : 4702)		//--- unreachable code													---//
	#pragma warning(disable : 4800)		//--- 'int' forcing bool to true or false								---//
	#pragma warning(disable : 4805)		//--- '!=' unsafe mix of type bool and type int							---//
	#pragma warning(disable : 4389)		//--- '==' signed/unsigned mismatch										---//
	#pragma warning(disable : 4390)		//--- empty controlled statement found; is this the intent?				---//
	#pragma warning(disable : 4996)		//--- declared deprecated												---//
#endif

//---------------//
//--- defines ---//
//---------------//
#define MUSIK_DND_ABOVE			0
#define MUSIK_DND_BELOW			1
#define MUSIK_DND_BOTTOM		2
#define MUSIK_SOURCES			9990
#define MUSIK_PLAYLIST			9991
#define MUSIK_ACTIVITYBOX1		9992
#define MUSIK_ACTIVITYBOX2		9993
#define	MUSIK_ACTIVITYBOX3		9994
#define	MUSIK_ACTIVITYBOX4		9995
#define	MUSIK_SIMPLEQUERY		9996
#define MUSIK_NOWPLAYING_TIMER	9997
#define MUSIK_VERSION			wxT( "Musik 0.1.3 CVS" )
#define MUSIKSERV_VERSION		wxT( "MusikServ 0.1.3 CVS" )

//-------------------------------------------------//
//--- for some reason some versions of wx do	---//
//--- not have this macro. pulled straight from	---//
//--- hashmap.h of the cvs head					---//
//-------------------------------------------------//
#include <wx/hashmap.h>
#ifndef WX_CLEAR_HASH_MAP
	#define WX_CLEAR_HASH_MAP(type, hashmap)                                     \
		{                                                                        \
			type::iterator it, en;                                               \
			for( it = (hashmap).begin(), en = (hashmap).end(); it != en; ++it )  \
				delete it->second;                                               \
			(hashmap).clear();                                                   \
		}
#endif 

//----------------------------------//
//--- path stuff ~/.Musik/ vs ./ ---//
//----------------------------------//
#include <wx/filename.h>
#define MUSIK_HOME_DIR 			wxFileName::GetHomeDir() + wxFileName::GetPathSeparator() + wxT( ".Musik" ) + wxFileName::GetPathSeparator()
#define MUSIK_PLAYLIST_DIR		MUSIK_HOME_DIR + wxT( "playlists" ) + wxFileName::GetPathSeparator()
#define MUSIK_DB_FILENAME		MUSIK_HOME_DIR + wxT( "musiklib.db" )
#define MUSIK_SOURCES_FILENAME	MUSIK_HOME_DIR + wxT( "musiksources.dat" )
#define MUSIK_PATHS_FILENAME	MUSIK_HOME_DIR + wxT( "musikpaths.dat" )
#define MUSIK_PATH_SEPARATOR	wxFileName::GetPathSeparator()

//-------------------------//
//--- enumerated values ---//
//-------------------------//
enum EPLAYLISTCOLUMNS
{
	PLAYLISTCOLUMN_RATING = 0,
	PLAYLISTCOLUMN_TRACK,
	PLAYLISTCOLUMN_TITLE,
	PLAYLISTCOLUMN_ARTIST,
	PLAYLISTCOLUMN_ALBUM,
	PLAYLISTCOLUMN_YEAR,
	PLAYLISTCOLUMN_GENRE,
	PLAYLISTCOLUMN_TIMES_PLAYED,
	PLAYLISTCOLUMN_LAST_PLAYED,
	PLAYLISTCOLUMN_TIME,
	PLAYLISTCOLUMN_BITRATE,
	PLAYLISTCOLUMN_FILENAME,
	NPLAYLISTCOLUMNS			//--- this is ALWAYS the last entry ---//
};

extern const int g_PlaylistColumnAlign[NPLAYLISTCOLUMNS];
extern const wxString g_PlaylistColumnLabels[NPLAYLISTCOLUMNS];

enum EMUSIK_ACTIVITY_TYPE
{
	MUSIK_LBTYPE_NULL = 0,
	MUSIK_LBTYPE_ARTISTS,
	MUSIK_LBTYPE_ALBUMS,
	MUSIK_LBTYPE_GENRES,
	MUSIK_LBTYPE_YEARS,
	MUSIK_LBTYPE_DECADES
};

enum EMUSIK_THREAD_EVENTS
{
	MUSIK_LIBRARY_THREAD_START = 1000,
	MUSIK_LIBRARY_THREAD_END,
	MUSIK_LIBRARY_THREAD_PROG,
	MUSIK_LIBRARY_THREAD_SCAN_PROG,

	MUSIK_TAG_THREAD_START,
	MUSIK_TAG_THREAD_END,
	MUSIK_TAG_THREAD_PROG,

	MUSIK_FRAME_THREAD_START,
	MUSIK_FRAME_THREAD_END,
	MUSIK_FRAME_THREAD_PROG,

	MUSIK_ACTIVITY_RENAME_THREAD_START,
	MUSIK_ACTIVITY_RENAME_THREAD_END,
	MUSIK_ACTIVITY_RENAME_THREAD_PROG,

	MUSIK_PLAYLIST_THREAD_START,
	MUSIK_PLAYLIST_THREAD_END,
	MUSIK_PLAYLIST_THREAD_PROG,

	MUSIK_PLAYER_NEXT_SONG,
	MUSIK_PLAYER_FADE_COMPLETE,
	MUSIK_PLAYER_STOP,

	MUSIK_LIBRARY_FILE_SCANNED
};

enum EMUSIK_THREAD_TYPE
{
	MUSIK_ACTIVITY_RENAME_THREAD = 0,
	MUSIK_PLAYLIST_RENAME_THREAD,
	MUSIK_PLAYLIST_RETAG_THREAD,
	MUSIK_LIBRARY_SCANNEW_THREAD,
	MUSIK_LIBRARY_UPDATE_THREAD,
	MUSIK_LIBRARY_PURGE_THREAD,
	MUSIK_TAG_SINGLE_THREAD,
	MUSIK_TAG_BATCH_THREAD,
	MUSIK_WRITE_DIRTY_THREAD
};

enum EMUSIK_MENU_ITEMS
{
	//--- main menu ---//
	MUSIK_MENU_PATHS = 0,
	MUSIK_MENU_WRITE,
	MUSIK_MENU_VIEW_DIRTY_TAGS,
	MUSIK_MENU_WRITE_TAGS,
	MUSIK_MENU_WRITE_CLEAR_DIRTY,
	MUSIK_MENU_RESCAN_PLAYLIST_DIR,
	MUSIK_MENU_PREFERENCES,
	MUSIK_MENU_FX,
    MUSIK_MENU_EXIT,
	MUSIK_MENU_SOURCES_STATE,
	MUSIK_MENU_ACTIVITIES_STATE,
	MUSIK_MENU_PLAYLISTINFO_STATE,
	MUSIK_MENU_STAY_ON_TOP,
	MUSIK_MENU_CLEARDB,
	MUSIK_MENU_REBUILDDB,
	MUSIK_MENU_SIMPLEQUERY,
	MUSIK_MENU_CUSTOMQUERY,

	//--- playlist popup menu ---//
	MUSIK_PLAYLIST_CONTEXT_RATENODE,
	MUSIK_PLAYLIST_CONTEXT_UNRATED,
	MUSIK_PLAYLIST_CONTEXT_RATE1,
	MUSIK_PLAYLIST_CONTEXT_RATE2,
	MUSIK_PLAYLIST_CONTEXT_RATE3,
	MUSIK_PLAYLIST_CONTEXT_RATE4,
	MUSIK_PLAYLIST_CONTEXT_RATE5,
	MUSIK_PLAYLIST_CONTEXT_TAGNODE,
	MUSIK_PLAYLIST_CONTEXT_TAG_TITLE,
	MUSIK_PLAYLIST_CONTEXT_TAG_TRACKNUM,
	MUSIK_PLAYLIST_CONTEXT_TAG_ARTIST,
	MUSIK_PLAYLIST_CONTEXT_TAG_ALBUM,
	MUSIK_PLAYLIST_CONTEXT_TAG_GENRE,
	MUSIK_PLAYLIST_CONTEXT_TAG_YEAR,
	MUSIK_PLAYLIST_CONTEXT_DELETENODE,
	MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,
	MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,
	MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,
	MUSIK_PLAYLIST_CONTEXT_RENAME_FILES,
	MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,

	//--- sources popup menu ---//
	MUSIK_SOURCE_CONTEXT_CREATE,
	MUSIK_SOURCE_CONTEXT_CREATE_CURRENT_PLAYLIST,
	MUSIK_SOURCE_CONTEXT_STANDARD_PLAYLIST,
	MUSIK_SOURCE_CONTEXT_DYNAMIC_PLAYLIST,
	MUSIK_SOURCE_CONTEXT_EDIT_QUERY,
	MUSIK_SOURCE_CONTEXT_RENAME,
	MUSIK_SOURCE_CONTEXT_DELETE,
	MUSIK_SOURCE_CONTEXT_SHOW_ICONS,
	MUSIK_SOURCE_CONTEXT_COPY_FILES,

	//--- library frame popup menu ---//
	MUSIK_PATHS_MENU_ADD,
	MUSIK_PATHS_MENU_REMOVESEL,
	MUSIK_PATHS_MENU_REMOVEALL,
	MUSIK_PATHS_MENU_SCAN_NEW,
	MUSIK_PATHS_MENU_CLEAR_LIBRARY,
	MUSIK_PATHS_MENU_UPDATE_LIBRARY,
	MUSIK_PATHS_MENU_PURGE_LIBRARY
    
};

//----------------------//
//--- pointer to app ---//
//----------------------//
class	MusikApp;
extern	MusikApp *pMusikApp;

//---------------------------//
//--- pointer to main dlg ---//
//---------------------------//
class	MusikFrame;
extern	MusikFrame *g_MusikFrame;

//-----------------------------//
//--- pointer to volume dlg ---//
//-----------------------------//
#include "Frames/MusikVolumeFrame.h"
class	MusikVolumeFrame;
extern	MusikVolumeFrame *g_VolumeFrame;

//----------------------------//
//--- pointer to tag frame ---//
//----------------------------//
#include "Frames/MusikTagFrame.h"
class MusikTagFrame;
extern MusikTagFrame *g_MusikTagFrame;

//--------------------------------//
//--- pointer to library frame ---//
//--------------------------------//
#include "Frames/MusikLibraryFrame.h"
class MusikLibraryFrame;
extern MusikLibraryFrame *g_MusikLibraryFrame;

//------------------------//
//--- internal objects ---//
//------------------------//
#include "Classes/MusikLibrary.h"
#include "Classes/MusikPlayer.h"
#include "Classes/MusikPrefs.h"
#include "Classes/WebServer.h"
#include "Threads/MusikThreads.h"
#include <wx/image.h>
#include <wx/imaglist.h>
#include "Classes/MusikFX.h"

//forward declarations
class CMusikPrefs;
class CMusikPaths;

extern CMusikLibrary		g_Library;
extern CMusikPlayer			g_Player;			
extern CMusikPrefs			g_Prefs;			
extern CMusikPaths			g_Paths;			
extern CMusikSongArray		g_Playlist;	
extern CMusikSongArray		g_LibPlaylist;	
extern wxArrayString		g_Items1;
extern wxArrayString		g_Items2;
extern wxArrayString		g_Items3;
extern wxArrayString		g_Items4;
extern wxArrayString		g_SourcesList;
extern CMusikStreamArray	g_ActiveStreams;
extern wxArrayInt			g_ActiveChannels;
extern CMusikWebServer		g_WebServer;
extern MusikFaderThread*	g_FaderThread;
extern wxImageList*			g_SourcesImages; 
extern wxImageList*			g_NullImageList;
extern wxImageList*			g_RatingImages;
extern CMusikFX				g_FX;

//----------------//
//--- controls ---//
//----------------//
#include "Classes/ActivityAreaCtrl.h"
#include "Classes/PlaylistCtrl.h"
#include "Classes/SourcesBox.h"
#include "Classes/NowPlayingCtrl.h"
#include "Classes/PlaylistInfoCtrl.h"

extern CActivityAreaCtrl	*g_ActivityAreaCtrl;
extern CPlaylistCtrl		*g_PlaylistCtrl;	
extern CSourcesBox			*g_SourcesCtrl;	
extern CNowPlayingCtrl		*g_NowPlayingCtrl;
extern CPlaylistInfoCtrl	*g_PlaylistInfoCtrl;
extern wxGauge				*g_Progress;
extern wxTextCtrl			*g_SimpleQuery;

//------------//
//--- misc ---//
//------------//
extern bool g_DragInProg;
extern bool g_SelInProg;
extern bool g_FocusDone;
extern bool g_PlaylistChanged;
extern bool g_FirstRun;
extern bool g_TimeSeeking;
extern bool g_DisablePlacement;

//-----------------------//
//--- main menu stuff ---//
//-----------------------//
extern wxMenu *file_menu;
extern wxMenu *view_menu;
extern wxMenu *library_writetags_menu;
extern wxMenu *library_menu;

//-------------//
//--- fonts ---//
//-------------//
extern wxFont g_fntArtist;
extern wxFont g_fntSong;
extern wxFont g_fntTime;
extern wxFont g_fntRegular;
extern wxFont g_fntBold;
extern wxFont g_fntInfo;

#endif
