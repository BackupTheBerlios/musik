/*
 *  MusikFrameEvents.cpp
 *
 *  All events dealing with Musik's main frame, MusikFrame.h / .cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikFrame.h"

//--- frames ---//
#include "MusikLibraryFrame.h"
#include "MusikPrefsFrame.h"
#include "MusikFXFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

//--- wx stuff we need ---//
#include <wx/filedlg.h>

BEGIN_EVENT_TABLE(MusikFrame, wxFrame)
	EVT_MENU					(MUSIK_MENU_PREFERENCES,			MusikFrame::OnPreferences			)	// File->Preferences
	EVT_MENU					(MUSIK_MENU_EXIT,					MusikFrame::OnCloseEvt				)	// File->Exit
	EVT_MENU					(MUSIK_MENU_SOURCES_STATE,			MusikFrame::OnSourcesState			)	// View->Show Sources
	EVT_MENU					(MUSIK_MENU_ACTIVITIES_STATE,		MusikFrame::OnActivitiesState		)	// View->Show Selections
	EVT_MENU					(MUSIK_MENU_PLAYLISTINFO_STATE,		MusikFrame::OnPlaylistInfoState		)	// View->Show Playlist Info
	EVT_MENU					(MUSIK_MENU_SHOW_RATINGS,			MusikFrame::OnShowRatings			)	// View->Show Ratings
	EVT_MENU					(MUSIK_MENU_STAY_ON_TOP,			MusikFrame::OnStayOnTop				)	// View->Stay On Top
	EVT_UPDATE_UI				(MUSIK_MENU_STAY_ON_TOP,			MusikFrame::OnUpdateStayOnTop		)	// View->Stay On Top
	EVT_MENU					(MUSIK_MENU_FX,						MusikFrame::OnFX					)
	EVT_MENU					(MUSIK_MENU_PATHS,					MusikFrame::OnSetupPaths			)	// Library->Setup Paths
	EVT_MENU					(MUSIK_MENU_SIMPLEQUERY,			MusikFrame::OnSimpleQueryDlg		)	// Library->Simple Query
	EVT_MENU					(MUSIK_MENU_CUSTOMQUERY,			MusikFrame::OnCustomQuery			)	// Library->Custom Query
	EVT_MENU					(MUSIK_MENU_VIEW_DIRTY_TAGS,		MusikFrame::OnViewDirtyTags			)	// Library->Write Tags->View
	EVT_MENU					(MUSIK_MENU_WRITE_TAGS,				MusikFrame::OnWriteTags				)	// Library->Write Tags->Write
	EVT_MENU					(MUSIK_MENU_WRITE_CLEAR_DIRTY,		MusikFrame::OnWriteTagsClearDirty	)	// Library->Write Tags->Finalize DB
	EVT_MENU					(MUSIK_MENU_RESCAN_PLAYLIST_DIR,	MusikFrame::OnRescanPlaylistDir		)	// Library->Rescan Playlist Directory
	EVT_TEXT					(MUSIK_SIMPLEQUERY,					MusikFrame::OnSimpleQueryEdit		)	// simple query box change
	EVT_MOVE					(									MusikFrame::OnMove					)	// main dlg moved
	EVT_MAXIMIZE				(									MusikFrame::OnMaximize				)	// main dlg maximized
	EVT_ICONIZE					(									MusikFrame::OnIconize				)	// main dlg minimized
	EVT_CLOSE					(									MusikFrame::OnClose					)
	EVT_SIZE					(									MusikFrame::OnSize					)	// main dlg resized
	EVT_CHAR					(									MusikFrame::OnTranslateKeys			)	// key pressed					
	EVT_SOCKET					(MUSIK_SERVER_ID,					MusikFrame::OnServerEvent			)	// getting a connection to webserver

	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU					( MUSIK_FRAME_THREAD_START,		MusikFrame::OnStartProgress			) 
	EVT_MENU					( MUSIK_FRAME_THREAD_END,		MusikFrame::OnEndProgress			) 
	EVT_MENU					( MUSIK_FRAME_THREAD_PROG,		MusikFrame::OnUpdateProgress		)
END_EVENT_TABLE()


//------------------------------------------//
//--- non activity box event definitions ---//
//------------------------------------------//
void MusikFrame::OnMove ( wxCommandEvent& WXUNUSED(event) )	
{ 
	if ( !g_DisablePlacement )
		g_Prefs.sFramePlacement = GetFramePlacement( this );
}

void MusikFrame::OnMaximize( wxMaximizeEvent& event )
{
	if ( !g_DisablePlacement )
		g_Prefs.sFramePlacement = GetFramePlacement( this );
	event.Skip();
}

void MusikFrame::OnIconize( wxIconizeEvent& event )
{
	if ( !g_DisablePlacement )
		g_Prefs.sFramePlacement = GetFramePlacement( this );
	event.Skip();
}

void MusikFrame::OnSize	( wxCommandEvent& WXUNUSED(event) )	
{
	this->Layout();

	if ( !g_DisablePlacement )
		g_Prefs.sFramePlacement = GetFramePlacement( this );

	g_PlaylistCtrl->RescaleColumns();
	g_SourcesCtrl->RescaleColumns();
	g_ActivityAreaCtrl->RescaleColumns();
}

void MusikFrame::OnCloseEvt( wxCommandEvent& WXUNUSED(event) )
{
	Destroy();
}

void MusikFrame::OnClose( wxCloseEvent& WXUNUSED(event) )
{
	#ifdef __WXMSW__
		UnSetMMShellHook((HWND)GetHWND());
	#endif

    //-------------------------------------------------//
    //--- there can be a slight delay cleaning		---//
    //--- when we clean threads. update user		---//
    //-------------------------------------------------//
    SetTitle( wxString( MUSIK_VERSION ) + wxT( " - Cleaning up threads and saving settings..." ) );

    //-------------------------------------------------//
    //--- make sure the SQL database and the player ---//
    //--- shut down cleanly							---//
    //-------------------------------------------------//
    g_Player.Shutdown();
    g_Library.Shutdown();

	//-------------------------------------------------//
	//--- write playlist columns to prefs			---//
	//-------------------------------------------------//
	g_PlaylistCtrl->SaveColumns();

	//-------------------------------------------------//
	//--- save prefs and paths,						---//
	//-------------------------------------------------//
	g_Prefs.SavePrefs();
	g_Paths.Save();

    //-------------------------------------------------//
    //--- stop webserver if necessary				---//
    //-------------------------------------------------//
    if ( g_Prefs.nWebServerEnable )
            g_WebServer.Stop();

    //-------------------------------------------------//
    //--- clear up the image lists					---//
    //-------------------------------------------------//
    DeleteImageLists();

    //-------------------------------------------------//
    //--- these threads will always be active		---//
    //--- so just delete them, no checks required	---//
    //-------------------------------------------------//
    g_FaderThread->Delete();

	Destroy();	
}

void MusikFrame::OnSetupPaths( wxCommandEvent& WXUNUSED(event) )
{
	g_MusikLibraryFrame = new MusikLibraryFrame( ( wxFrame* )this, wxPoint( 0, 0 ), wxSize( 480, 240 ) );
	this->Enable( FALSE );	
	g_MusikLibraryFrame->Show();
}

void MusikFrame::OnPreferences( wxCommandEvent &event )
{
	#ifdef __WXMSW__
		wxSize prefs_size( 460, 370 );
	#elif __WXGTK__
		wxSize prefs_size( 520, 370 );
	#endif

	wxFrame *pDlg = new MusikPrefsFrame( ( wxFrame* )this, _("Musik Preferences"), wxPoint( 0, 0 ), prefs_size );
	this->Enable( FALSE );
	pDlg->Show();
}

void MusikFrame::OnFX( wxCommandEvent &event )
{
	wxSize fx_size( 400, 450 );
	
	wxFrame *pDlg = new MusikFXFrame( ( wxFrame* )this, _("Musik FX"), wxPoint( 0, 0 ), fx_size );
	//this->Enable( FALSE );
	pDlg->Show();
}

void MusikFrame::OnShowRatings( wxCommandEvent& WXUNUSED(event) )	
{ 
	g_PlaylistCtrl->ToggleIcons();
}

void MusikFrame::OnStayOnTop( wxCommandEvent &event )	
{ 
	#ifdef __WXMSW__
		g_Prefs.nStayOnTop = !g_Prefs.nStayOnTop;
		SetStayOnTop(g_Prefs.nStayOnTop);
	#endif
}
void MusikFrame::OnUpdateStayOnTop( wxUpdateUIEvent& event)
{
	event.Check(g_Prefs.nStayOnTop);
}
void MusikFrame::OnPlaylistInfoState( wxCommandEvent& WXUNUSED(event) )	
{ 
	TogglePlaylistInfo();
}

void MusikFrame::OnSourcesState( wxCommandEvent& WXUNUSED(event) )
{ 
	ToggleSources();
}

void MusikFrame::OnActivitiesState( wxCommandEvent& WXUNUSED(event) )
{ 
	ToggleActivities();
}

void MusikFrame::OnSimpleQueryDlg( wxCommandEvent& WXUNUSED(event) )
{ 
	LibrarySimpleQueryDlg();
}

void MusikFrame::OnSimpleQueryEdit( wxCommandEvent& WXUNUSED(event) )
{
	LibrarySimpleQueryEdit();
}

void MusikFrame::OnCustomQuery( wxCommandEvent& WXUNUSED(event) )
{ 
	LibraryCustomQuery();		
}

void MusikFrame::OnViewDirtyTags( wxCommandEvent& WXUNUSED(event) )
{ 
	if ( g_PlaylistCtrl->ViewDirtyTags() )
		ShowActivityArea( false );
}

void MusikFrame::OnWriteTags( wxCommandEvent& WXUNUSED(event) )
{ 
	WriteTags();			
}

void MusikFrame::OnWriteTagsClearDirty( wxCommandEvent& WXUNUSED(event) )
{
	g_Library.ClearDirtyTags( true );	
}

void MusikFrame::OnRescanPlaylistDir( wxCommandEvent& WXUNUSED(event) )
{ 
	g_SourcesCtrl->RescanPlaylistDir();	
}

void MusikFrame::OnTranslateKeys( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE )		
	{
		if ( GetActiveThread() != NULL )
		{
			SetTitle( wxString( MUSIK_VERSION ) + _( ": Aborting process, please wait..." ) );
			GetActiveThread()->Delete();
		}
	}
}

void MusikFrame::WriteTags()
{
	if ( g_MusikFrame->GetActiveThread() == NULL )
        {
			//--- ask user if he wants to clear tags first ---//
			bool bClear = false;
			if ( wxMessageBox( _( "Clear old tag information before writing?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION  ) == wxYES )
				bClear = true;
			else
				bClear = false;		

			//--- fire up the thread ---//
			pWriteDirtyThread = new MusikWriteDirtyThread( bClear );
			pWriteDirtyThread->Create();
			pWriteDirtyThread->Run();
        }
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the Musik development team with this error." ), MUSIK_VERSION, wxICON_STOP );
}

//------------------------//
//--- Library Routines ---//
//------------------------//
void MusikFrame::LibraryCustomQuery()
{
	wxString sQuery = g_SourcesCtrl->PromptDynamicPlaylist( wxT( "" ) );

	if ( !sQuery.IsEmpty() )
	{
		m_customQuery = sQuery;

		g_Playlist = g_Library.QuerySongs( m_customQuery );
		g_PlaylistCtrl->Update( );
		g_PlaylistChanged = true;
	}
}

void MusikFrame::LibrarySimpleQueryEdit()
{
	wxString sQueryval = g_SimpleQuery->GetValue();
	if ( ( sQueryval.Length() < 2 ) || ( ( sQueryval.Left( 1 ) != wxT("!") ) && ( sQueryval.Length() < 4 ) ) || ( sQueryval == wxT("") ) )
		return;
	else
		LibrarySimpleQuery( sQueryval );
}

void MusikFrame::LibrarySimpleQueryDlg()
{
	wxTextEntryDialog dlg( this, _("Enter Query:"), MUSIK_VERSION, m_customQuery.c_str() );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sQuery = dlg.GetValue();
		LibrarySimpleQuery( sQuery );
	}
}

void MusikFrame::LibrarySimpleQuery( wxString sQueryVal )
{
		if ( sQueryVal.Left( 1 ) == wxT("!") )
			sQueryVal = sQueryVal.Right( sQueryVal.Length() - 1 );	//--- remove "!" ---//

		sQueryVal.Replace( wxT("'"), wxT("''") ); //--- double apostrophe to make valid syntax ---//
		wxString sString = wxT("'%") + sQueryVal + wxT("%'");
		wxString sQuery;
		sQuery.sprintf( _("artist like %s or album like %s or title like %s or filename like %s order by album,tracknum,filename"), sString.c_str(), sString.c_str(), sString.c_str(), sString.c_str() );
		g_Playlist = g_Library.QuerySongs( sQuery );
		g_PlaylistCtrl->Update( );
		g_PlaylistChanged = true;
}

//------------------------//
//--- Web server stuff ---//
//------------------------//
void MusikFrame::OnServerEvent(wxSocketEvent& event)
{
	if ( !g_WebServer.IsRunning() )
		return;

	//------------------------------------------------------//
	//--- if we get a connection, make the server listen ---//
	//------------------------------------------------------//
	switch( event.GetSocketEvent() )
	{
		case wxSOCKET_CONNECTION:
			g_WebServer.Listen();
			break;
		
		case wxSOCKET_INPUT:
			//--- dunno what to do, compiler warns ---//
			break;
		
		case wxSOCKET_OUTPUT:
			//--- dunno what to do, compiler warns ---//
			break;
		
		case wxSOCKET_LOST:
			//--- dunno what to do, compiler warns ---//
			break;
	}
}

//-----------------------------------//
//--- events dealing with threads ---//
//-----------------------------------//
void MusikFrame::OnStartProgress( wxCommandEvent& WXUNUSED(event) )
{
	EnableProgress( true );

	if ( GetProgressType() == MUSIK_ACTIVITY_RENAME_THREAD )
	{
		SetTitle( wxString( MUSIK_VERSION ) + _( ": Updating tags from selection box (ESC to abort)" ) );
	}
	else if ( GetProgressType() == MUSIK_PLAYLIST_RENAME_THREAD )
	{
		SetTitle( wxString( MUSIK_VERSION ) + _( ": Renaming selected playlist items (ESC to abort)" ) );
	}
	else if ( GetProgressType() == MUSIK_WRITE_DIRTY_THREAD )
	{
		SetTitle( wxString( MUSIK_VERSION ) + _( ": Writing dirty tags to file (ESC to abort)" ) );
	}
}

void MusikFrame::OnUpdateProgress( wxCommandEvent& WXUNUSED(event) )
{
	if ( GetActiveThread() != NULL )
	{
		g_Progress->SetValue( GetProgress() );
	}
}

void MusikFrame::OnEndProgress( wxCommandEvent& WXUNUSED(event) )
{
	EnableProgress( false );

	g_Progress->SetValue( 0 );
	
	SetProgress	( 0 );
	SetProgressType	( 0 );
	SetActiveThread	( NULL );

	SetTitle( MUSIK_VERSION );
}

