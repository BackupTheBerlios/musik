/*
 *  MusikApp.cpp
 *
 *  Application start point
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"
#include "wx/textfile.h"

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
	
	//--- setup our home dir ---//
	if ( !wxDirExists( MUSIK_HOME_DIR ) )
		wxMkdir( MUSIK_HOME_DIR );

	//-----------------------------------------//
	//--- check to see if a new version has	---//
	//--- been installed. if it has, see	---//
	//--- if any core changes need to be	---//
	//--- made.								---//
	//-----------------------------------------//
	CheckVersion();
	
	//--- assure playlists directory exists ---//
	if ( !wxDirExists( MUSIK_PLAYLIST_DIR ) )
		wxMkdir( MUSIK_PLAYLIST_DIR );

	//--- load library and paths ---//
	if(!g_Library.Load())
	{
		wxMessageBox( _("Initialization of library failed."), MUSIKAPPNAME_VERSION, wxOK | wxICON_ERROR );
		return FALSE;
	}
	g_Paths.Load();
	g_Player.Init();
	//--- initialize fmod ---//
	if ( g_Player.InitializeFMOD( FMOD_INIT_START ) != FMOD_INIT_SUCCESS )
		wxMessageBox( _("Initialization of FMOD sound system failed."), MUSIKAPPNAME_VERSION, wxOK | wxICON_ERROR );


	wxImage::AddHandler( new wxXPMHandler );

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
	view_menu->Check	( MUSIK_MENU_SOURCES_STATE,	( bool )g_Prefs.bShowSources );
	view_menu->Append	( MUSIK_MENU_ACTIVITIES_STATE, _("Show Selections\tCtrl-2"), wxT(""), wxITEM_CHECK );
	view_menu->Check	( MUSIK_MENU_ACTIVITIES_STATE, ( bool )g_Prefs.bShowActivities );
	view_menu->Append	( MUSIK_MENU_PLAYLISTINFO_STATE, _("Show Playlist Info\tCtrl-3"), wxT(""), wxITEM_CHECK );
	view_menu->Check	( MUSIK_MENU_PLAYLISTINFO_STATE, ( bool )g_Prefs.bShowPLInfo );
	view_menu->AppendSeparator();
	view_menu->Append	( MUSIK_MENU_FX, _("FX\tCtrl-F") );
	#ifdef __WXMSW__
		view_menu->AppendSeparator();
		view_menu->Append	( MUSIK_MENU_STAY_ON_TOP, _("Always On Top\tCtrl-T"), wxT(""), wxITEM_CHECK );
//		view_menu->Check	( MUSIK_MENU_STAY_ON_TOP, ( bool )g_Prefs.bStayOnTop );
	#endif

	//--- library -> pending tags ---//
	library_writetags_menu = new wxMenu;
	library_writetags_menu->Append( MUSIK_MENU_VIEW_DIRTY_TAGS, _("Vie&w") );
    library_writetags_menu->AppendSeparator();
	library_writetags_menu->Append( MUSIK_MENU_WRITE_TAGS, _("Write Changes to &File") );
	library_writetags_menu->Append( MUSIK_MENU_WRITE_CLEAR_DIRTY, _("Finalize for Database &Only") );

	//--- library ---//
    library_menu = new wxMenu;
	library_menu->Append( MUSIK_MENU_PATHS, _("&Setup Library\tCtrl-L") );
    library_menu->AppendSeparator();
	library_menu->Append( MUSIK_MENU_SIMPLEQUERY, _("S&imple Query") );
	library_menu->Append( MUSIK_MENU_CUSTOMQUERY, _("&Custom Query") );
	library_menu->AppendSeparator();
	library_menu->Append( MUSIK_MENU_WRITE, _("&Pending Tags"), library_writetags_menu );

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
	pMain->SetTitle( MUSIKAPPNAME_VERSION );
	pMain->SetMenuBar( menu_bar );
	pMain->Show(TRUE);

	SetTopWindow( pMain );

	//--- start webserver if necessary ---//
	if ( g_Prefs.bWebServerEnable )
		g_WebServer.Start();

	return TRUE;
}

void MusikApp::CheckVersion()
{
	wxString sVersion;
	if ( wxFileExists( MUSIK_VERSION_FILENAME ) )
	{
		sVersion = ReadVersion();

		if ( sVersion == wxT( "Musik 0.1.3" ) || sVersion == wxT( "Musik 0.1.3 CVS" ) )
		{
//			wxMessageBox( wxT( MUSIKAPPNAME" has detected 0.1.3 was previously installed. Due to the changes in the playlist display preferences, your columns will be reset. We apologize for any inconvenience this may cause." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
		}

		WriteVersion();
	}
	else
	{
		//-------------------------------------------------//
		//--- if these files exist, but the version.dat	---//
		//--- does not, some version prior to 0.1.3 was	---//
		//--- installed. give user a nice little		---//
		//--- warning.									---//
		//-------------------------------------------------//
		if ( wxFileExists( MUSIK_SOURCES_FILENAME ) || wxFileExists( MUSIK_DB_FILENAME ) )
		{
			wxMessageBox( wxT( MUSIKAPPNAME" has detected version 0.1.2 or earlier was previously installed.\n\nDue to the changes from 0.1.2 to the current version, your Sources list and Library must be reset. We apologize for any inconvenience this may cause." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
	
			if ( wxFileExists( MUSIK_SOURCES_FILENAME ) )
				wxRemoveFile( MUSIK_SOURCES_FILENAME );
			if ( wxFileExists( MUSIK_DB_FILENAME ) )
				wxRemoveFile( MUSIK_DB_FILENAME );
		}	

		WriteVersion();
	}
}

wxString MusikApp::ReadVersion()
{
	wxTextFile ver( MUSIK_VERSION_FILENAME );
	wxString sRet;
	if ( ver.Open() )
	{
		if ( ver.GetLineCount() != 0 )
			sRet = ver.GetLine( 0 );
		ver.Close();
	}
	return sRet;
}

void MusikApp::WriteVersion()
{
	wxTextFile ver;
	if ( !wxFileExists( MUSIK_VERSION_FILENAME ) )
	{
		ver.Create( MUSIK_VERSION_FILENAME );
	}
	ver.Open( MUSIK_VERSION_FILENAME );
    
	if ( ver.IsOpened() )
	{
		if ( ver.GetLineCount() != 0 )
    		ver.RemoveLine(0);
		ver.AddLine( MUSIKAPPNAME_VERSION );
		ver.Write();
		ver.Close();
	}
 	else
  	{
    		wxASSERT(0);
  	}
}

