/*
 *  MusikApp.cpp
 *
 *  Application start point
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikApp.h"
IMPLEMENT_APP( MusikApp )

//--- related frames ---//
#include "Frames/MusikFrame.h"
#include "Frames/MusikLibraryFrame.h"

//--- globals ---//
#include "MusikGlobals.h"
#include "MusikUtils.h"

bool MusikApp::OnInit()
{
	g_FirstRun = true;

	//-----------------------------------------//
	//--- check to see if a new version has	---//
	//--- been installed. if it has, see	---//
	//--- if any core changes need to be	---//
	//--- made.								---//
	//-----------------------------------------//
	g_Prefs.LoadPrefs();
	CheckVersion();
	
	//--- setup our home dir ---//
	if ( !wxDirExists( MUSIK_HOME_DIR ) )
		wxMkdir( MUSIK_HOME_DIR );

	//--- assure playlists directory exists ---//
	if ( !wxDirExists( MUSIK_PLAYLIST_DIR ) )
		wxMkdir( MUSIK_PLAYLIST_DIR );

	//--- load library and paths ---//
	g_Library.Load();
	g_Paths.Load();

	//--- initialize fmod ---//
	if ( g_Player.InitializeFMOD( FMOD_INIT_START, g_Prefs.nSndOutput, g_Prefs.nSndDevice, g_Prefs.nSndRate ) != FMOD_INIT_SUCCESS )
		wxMessageBox( _("Initialization of FMOD sound system failed."), MUSIK_VERSION, wxOK | wxICON_ERROR );

	//------------------//
	//--- menu stuff ---//
	//------------------//
	//--- file ---//
    file_menu = new wxMenu;
	file_menu->Append( MUSIK_MENU_PREFERENCES, _("&Preferences\tCtrl-P") );
	file_menu->AppendSeparator();
	file_menu->Append( MUSIK_MENU_EXIT, _("E&xit") );

	//--- view ---//
	view_menu = new wxMenu;
	view_menu->Append	( MUSIK_MENU_SOURCES_STATE,	_("Show Sources\tCtrl-1"), wxT(""), wxITEM_CHECK );
	view_menu->Check	( MUSIK_MENU_SOURCES_STATE,	( bool )g_Prefs.nShowSources );
	view_menu->Append	( MUSIK_MENU_ACTIVITIES_STATE, _("Show Selections\tCtrl-2"), wxT(""), wxITEM_CHECK );
	view_menu->Check	( MUSIK_MENU_ACTIVITIES_STATE, ( bool )g_Prefs.nShowActivities );
	view_menu->Append	( MUSIK_MENU_PLAYLISTINFO_STATE, _("Show Playlist Info\tCtrl-3"), wxT(""), wxITEM_CHECK );
	view_menu->Check	( MUSIK_MENU_PLAYLISTINFO_STATE, ( bool )g_Prefs.nShowPLInfo );
	view_menu->Append	( MUSIK_MENU_SHOW_RATINGS, _("Show Ratings\tCtrl-4"), wxT(""), wxITEM_CHECK );
	view_menu->Check	( MUSIK_MENU_SHOW_RATINGS, ( bool )g_Prefs.nShowRatings );
	view_menu->AppendSeparator();
	view_menu->Append	( MUSIK_MENU_FX, _("FX\tCtrl-F") );
	#ifdef __WXMSW__
		view_menu->AppendSeparator();
		view_menu->Append	( MUSIK_MENU_STAY_ON_TOP, _("Always On Top\tCtrl-T"), wxT(""), wxITEM_CHECK );
//		view_menu->Check	( MUSIK_MENU_STAY_ON_TOP, ( bool )g_Prefs.nStayOnTop );
	#endif

	//--- library -> pending tags ---//
	library_writetags_menu = new wxMenu;
	library_writetags_menu->Append( MUSIK_MENU_VIEW_DIRTY_TAGS, _("Vie&w") );
    library_writetags_menu->AppendSeparator();
	library_writetags_menu->Append( MUSIK_MENU_WRITE_TAGS, _("Write to &File") );
	library_writetags_menu->Append( MUSIK_MENU_WRITE_CLEAR_DIRTY, _("Finalize for Database &Only") );

	//--- library ---//
    library_menu = new wxMenu;
	library_menu->Append( MUSIK_MENU_PATHS, _("&Setup Library\tCtrl-L") );
    library_menu->AppendSeparator();
	library_menu->Append( MUSIK_MENU_SIMPLEQUERY, _("S&imple Query") );
	library_menu->Append( MUSIK_MENU_CUSTOMQUERY, _("&Custom Query") );
	library_menu->AppendSeparator();
	library_menu->Append( MUSIK_MENU_WRITE, _("&Pending Tags"), library_writetags_menu );
	library_menu->Append( MUSIK_MENU_RESCAN_PLAYLIST_DIR, _("R&escan Playlist Directory\tCtrl-R") );

	//----------------//
	//--- menu bar ---//
	//----------------//
	wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append( file_menu,	_("&File") );
	menu_bar->Append( view_menu, 	_("&View") );
	menu_bar->Append( library_menu,	_("&Library") );

	//-------------------//
	//--- main window ---//
	//-------------------//
    MusikFrame *pMain = new MusikFrame();

	//--- restore placement or use defaults ---//
	if ( !SetFramePlacement( pMain, g_Prefs.sFramePlacement ) )
	{
		wxSize Size( 
			wxSystemSettings::GetMetric( wxSYS_SCREEN_X ) * 75 / 100, 
			wxSystemSettings::GetMetric( wxSYS_SCREEN_Y ) * 75 / 100 );
		pMain->SetSize( Size );
		pMain->Center();
	}

	pMain->SetMenuBar( menu_bar );
	pMain->Show(TRUE);

	SetTopWindow( pMain );

	//--- start webserver if necessary ---//
	if ( g_Prefs.nWebServerEnable )
		g_WebServer.Start();

	return TRUE;
}

void MusikApp::CheckVersion()
{
	bool bSavePrefs = false;

	//--------------------------------//
	//--- if we have a new version ---//
	//--------------------------------//
	if ( g_Prefs.sMusikVersion != wxString( MUSIK_VERSION ) )
	{
		//------------------------------------//
		//--- if the old version was 0.1.2 ---//
		//------------------------------------//
		if ( g_Prefs.sMusikVersion == wxT( "Musik 0.1.2" ) )
		{
			if ( wxFileExists( MUSIK_SOURCES_FILENAME ) || wxFileExists( MUSIK_DB_FILENAME ) )
			{
				wxMessageBox( wxT( "Musik has detected version 0.1.2 or earlier was previously installed.\n\nDue to the changes from 0.1.2 to the current version, your Sources list and Library must be reset. We apologize for any inconvenience this may cause." ), MUSIK_VERSION, wxICON_INFORMATION );
		
				if ( wxFileExists( MUSIK_SOURCES_FILENAME ) )
					wxRemoveFile( MUSIK_SOURCES_FILENAME );
				if ( wxFileExists( MUSIK_DB_FILENAME ) )
					wxRemoveFile( MUSIK_DB_FILENAME );
			}
			g_Prefs.sMusikVersion = MUSIK_VERSION;
			bSavePrefs = true;
		}
	}

	if ( bSavePrefs )
		g_Prefs.SavePrefs();
}
