/*
 *  MusikFrameEvents.cpp
 *
 *  All events dealing with Musik's main frame, MusikFrame.h / .cpp
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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
#include "../MusikApp.h"
#ifdef __WXMSW__
	#include "../MMShellHook/MMShellHook.h"
#endif
#include "../images/tray.xpm"

DECLARE_APP( MusikApp )
//--- wx stuff we need ---//
#include <wx/filedlg.h>

BEGIN_EVENT_TABLE(MusikFrame, wxFrame)
	EVT_MENU					(MUSIK_MENU_PREFERENCES,			MusikFrame::OnPreferences			)	// File->Preferences
	EVT_MENU					(MUSIK_MENU_EXIT,					MusikFrame::OnMenuClose				)	// File->Exit
	EVT_MENU					(MUSIK_MENU_SOURCES_STATE,			MusikFrame::OnSourcesState			)	// View->Show Sources
	EVT_UPDATE_UI				(MUSIK_MENU_SOURCES_STATE,			MusikFrame::OnUpdateUISourcesState		)	
	EVT_MENU					(MUSIK_MENU_ACTIVITIES_STATE,		MusikFrame::OnActivitiesState		)	// View->Show Selections
	EVT_UPDATE_UI				(MUSIK_MENU_ACTIVITIES_STATE,		MusikFrame::OnUpdateUIActivitiesState)	
	EVT_MENU					(MUSIK_MENU_PLAYLISTINFO_STATE,		MusikFrame::OnPlaylistInfoState		)	// View->Show Playlist Info
	EVT_UPDATE_UI				(MUSIK_MENU_PLAYLISTINFO_STATE,		MusikFrame::OnUpdateUIPlaylistInfoState	)	
	EVT_MENU					(MUSIK_MENU_STAY_ON_TOP,			MusikFrame::OnStayOnTop				)	// View->Stay On Top
	EVT_UPDATE_UI				(MUSIK_MENU_STAY_ON_TOP,			MusikFrame::OnUpdateUIStayOnTop		)	
	EVT_MENU					(MUSIK_MENU_FX,						MusikFrame::OnFX					)
	EVT_MENU					(MUSIK_MENU_PATHS,					MusikFrame::OnSetupPaths			)	// Library->Setup Paths
	EVT_MENU					(MUSIK_MENU_SIMPLEQUERY,			MusikFrame::OnSimpleQueryDlg		)	// Library->Simple Query
	EVT_MENU					(MUSIK_MENU_CUSTOMQUERY,			MusikFrame::OnCustomQuery			)	// Library->Custom Query
	EVT_MENU					(MUSIK_MENU_VIEW_DIRTY_TAGS,		MusikFrame::OnViewDirtyTags			)	// Library->Write Tags->View
	EVT_MENU					(MUSIK_MENU_WRITE_TAGS,				MusikFrame::OnWriteTags				)	// Library->Write Tags->Write
	EVT_MENU					(MUSIK_MENU_WRITE_CLEAR_DIRTY,		MusikFrame::OnWriteTagsClearDirty	)	// Library->Write Tags->Finalize DB
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
	EVT_MENU					( MUSIK_FRAME_EXIT_FADE_DONE,	MusikFrame::OnMenuClose				)
	EVT_SASH_DRAGGED			( MUSIK_SOURCES,				MusikFrame::OnSashDraggedSourcesBox	)
	EVT_SASH_DRAGGED			( MUSIK_ACTIVITYCTRL,			MusikFrame::OnSashDraggedActivityCtrl)
END_EVENT_TABLE()


//------------------------------------------//
//--- non activity box event definitions ---//
//------------------------------------------//
void MusikFrame::OnMove ( wxMoveEvent& WXUNUSED(event) )	
{ 
	if ( !g_DisablePlacement )
		wxGetApp().Prefs.sFramePlacement = GetFramePlacement( this );
}

void MusikFrame::OnMaximize( wxMaximizeEvent& event )
{
	if ( !g_DisablePlacement )
		wxGetApp().Prefs.sFramePlacement = GetFramePlacement( this );
	event.Skip();
}

void MusikFrame::OnIconize( wxIconizeEvent& event )
{
	if ( !g_DisablePlacement )
		wxGetApp().Prefs.sFramePlacement = GetFramePlacement( this );
#ifdef wxHAS_TASK_BAR_ICON
	if( m_pTaskBarIcon && wxGetApp().Prefs.bHideOnMinimize )
	{
		if(event.Iconized())
		{
			Show(FALSE);
		}
		event.Skip(FALSE);
	}
	else
		event.Skip();
#else
	event.Skip();
#endif
}

void MusikFrame::OnSize	( wxSizeEvent& WXUNUSED(event) )	
{
//	wxFrame::OnSize(event);
	m_pBottomPanel->SetDefaultSize(vsTopBottom->GetMinSize());
	m_pBottomPanel->Layout();

	wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,g_PlaylistBox);

	m_pBottomPanel->Layout();

	m_pNowPlayingCtrl->Refresh();
	m_pNowPlayingCtrl->Update();

	
	if ( !g_DisablePlacement )
		wxGetApp().Prefs.sFramePlacement = GetFramePlacement( this );
}

void MusikFrame::OnClose( wxCloseEvent& WXUNUSED(event) )
{
    //-------------------------------------------------//
    //--- if fade out on exit is enabled, hide the	---//
    //--- frame and allow the fade to happen. an 	---//
	//--- event will be posted back when done.		---//
    //-------------------------------------------------//
	Show( false );
	if ( wxGetApp().Prefs.bGlobalFadeEnable && wxGetApp().Prefs.bFadeExitEnable && wxGetApp().Player.IsPlaying() && !wxGetApp().Player.IsPaused() )
	{
		wxGetApp().Player.Stop( true, true );
		return;
	}

	#ifdef __WXMSW__
		UnSetMMShellHook((HWND)GetHWND());
	#endif

	//-------------------------------------------------//
	//--- write playlist columns to prefs			---//
	//-------------------------------------------------//
	g_PlaylistBox->PlaylistCtrl().SaveColumns();

	//-------------------------------------------------//
	//--- save prefs and paths,						---//
	//-------------------------------------------------//
	g_Paths.Save();

    //-------------------------------------------------//
    //--- stop webserver if necessary				---//
    //-------------------------------------------------//
    if ( wxGetApp().Prefs.bWebServerEnable )
		wxGetApp().WebServer.Stop();

    //-------------------------------------------------//
    //--- clear up the image lists					---//
    //-------------------------------------------------//
    DeleteImageLists();


	//-------------------------------------------------//
	//--- delete the thread object and destroy.		---//
	//-------------------------------------------------//
    g_FaderThread->Delete();
	g_FaderThread->Wait();
	delete g_FaderThread;
	g_FaderThread = NULL;
	wxGetApp().Player.Shutdown();
	Destroy();
}

void MusikFrame::OnSetupPaths( wxCommandEvent& WXUNUSED(event) )
{
	MusikLibraryFrame* pMusikLibraryFrame = new MusikLibraryFrame( ( wxFrame* )this, wxPoint( 0, 0 ), wxSize( 480, 240 ) );
	this->Enable( FALSE );	
	pMusikLibraryFrame->Show();
}

void MusikFrame::OnPreferences( wxCommandEvent &WXUNUSED(event) )
{
	
	#ifdef __WXMSW__
		wxSize prefs_size( 460, 370 );
	#elif __WXGTK__
		wxSize prefs_size( 520, 370 );
	#endif
	

	wxFrame *pDlg = new MusikPrefsFrame( ( wxFrame* )this, wxString(MUSIKAPPNAME) + _(" Preferences"), wxPoint( 0, 0 ), prefs_size );
	this->Enable( FALSE );
	pDlg->Show();
}

void MusikFrame::OnFX( wxCommandEvent &WXUNUSED(event) )
{
	wxSize fx_size( 400, 450 );
	
	wxFrame *pDlg = new MusikFXFrame( ( wxFrame* )this, wxString(MUSIKAPPNAME) + _(" FX"), wxPoint( 0, 0 ), fx_size );
	//this->Enable( FALSE );
	pDlg->Show();
}

void MusikFrame::OnStayOnTop( wxCommandEvent &WXUNUSED(event) )	
{ 
	#ifdef __WXMSW__
		wxGetApp().Prefs.bStayOnTop = !wxGetApp().Prefs.bStayOnTop;
		SetStayOnTop(wxGetApp().Prefs.bStayOnTop);
	#endif
}
void MusikFrame::OnUpdateUIStayOnTop( wxUpdateUIEvent& event)
{
	event.Check(wxGetApp().Prefs.bStayOnTop);
}
void MusikFrame::OnPlaylistInfoState( wxCommandEvent& WXUNUSED(event) )	
{ 
	TogglePlaylistInfo();
}
void MusikFrame::OnUpdateUIPlaylistInfoState( wxUpdateUIEvent& event)
{
	event.Check(wxGetApp().Prefs.bShowPLInfo);
}

void MusikFrame::OnSourcesState( wxCommandEvent& WXUNUSED(event) )
{ 
	ToggleSources();
}
void MusikFrame::OnUpdateUISourcesState( wxUpdateUIEvent& event)
{
	event.Check( wxGetApp().Prefs.bShowSources );
}

void MusikFrame::OnActivitiesState( wxCommandEvent& WXUNUSED(event) )
{ 
	ToggleActivities();
}
void MusikFrame::OnUpdateUIActivitiesState( wxUpdateUIEvent& event)
{
	event.Check(wxGetApp().Prefs.bShowActivities);
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
	if ( g_PlaylistBox->PlaylistCtrl().ViewDirtyTags() )
		ShowActivityArea( false );
}

void MusikFrame::OnWriteTags( wxCommandEvent& WXUNUSED(event) )
{ 
	WriteTags();			
}

void MusikFrame::OnWriteTagsClearDirty( wxCommandEvent& WXUNUSED(event) )
{
	int nCleared = wxGetApp().Library.ClearDirtyTags();
	if ( !nCleared )
		wxMessageBox( _( "There are no pending tags to finalize." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
	else
		wxMessageBox( IntTowxString( nCleared ) + wxT( " tags pending to be written were finalized for the database only. These tags will not be written to file." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
}

void MusikFrame::OnTranslateKeys( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE )		
	{
		if ( GetActiveThread() != NULL )
		{
			SetTitle( wxString( MUSIKAPPNAME_VERSION ) + _( ": Aborting process, please wait..." ) );
			GetActiveThread()->Delete();
		}
	}
	else
		event.Skip();
}

void MusikFrame::WriteTags()
{
	if ( g_MusikFrame->GetActiveThread() == NULL )
        {
			//--- ask user if he wants to clear tags first ---//
			bool bClear = false;
			if ( wxMessageBox( _( "Clear old tag information before writing?" ), MUSIKAPPNAME_VERSION, wxYES_NO | wxICON_QUESTION  ) == wxYES )
				bClear = true;
			else
				bClear = false;		

			//--- fire up the thread ---//
			pWriteDirtyThread = new MusikWriteDirtyThread( bClear );
			pWriteDirtyThread->Create();
			pWriteDirtyThread->Run();
        }
	else
		InternalErrorMessageBox(wxT("Previous thread not terminated correctly."));
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

		wxGetApp().Library.QuerySongsWhere( m_customQuery, g_Playlist );
		g_PlaylistBox->Update( );
		g_PlaylistChanged = true;
	}
}

void MusikFrame::LibrarySimpleQueryEdit()
{
	wxString sQueryval = g_PlaylistBox->TextSimpleQuery().GetValue();
	if ( ( sQueryval.Length() < 3 )  )
		return;
	else
		LibrarySimpleQuery( sQueryval );
}

void MusikFrame::LibrarySimpleQueryDlg()
{
	wxTextEntryDialog dlg( this, _("Enter Query:"), MUSIKAPPNAME_VERSION, m_customQuery );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sQuery = dlg.GetValue();
		LibrarySimpleQuery( sQuery );
	}
}

void MusikFrame::LibrarySimpleQuery( wxString sQueryVal )
{

	sQueryVal.Replace( wxT("'"), wxT("''") ); //--- double apostrophe to make valid syntax ---//

	wxArrayString sTokens;
	if ( sQueryVal.Left( 1 ) == wxT("!") )
	{
		sQueryVal = sQueryVal.Right( sQueryVal.Length() - 1 );	//--- remove "!" ---//
		sTokens.Add(sQueryVal);
	}
	else
		DelimitStr(sQueryVal,wxT(" "),sTokens,true);

	wxString sQuery;

	for ( size_t i = 0; i < sTokens.GetCount() ; i++)
	{
		
		wxString sString = wxT("'%") + sTokens[i] + wxT("%'");
		sQuery+= wxString::Format( wxT("(artist like %s or album like %s or title like %s or filename like %s or notes like %s)"),
			( const wxChar *)sString, (const wxChar *) sString, (const wxChar *)sString,(const wxChar*) sString,(const wxChar*) sString );
		if(i != sTokens.GetCount() - 1)
		{
			sQuery += wxT(" and ");
		}

	}

	wxGetApp().Library.QuerySongsWhere( sQuery, g_Playlist ,true);  // true means query sorted
	g_PlaylistBox->Update( );
	g_PlaylistChanged = true;
}

//------------------------//
//--- Web server stuff ---//
//------------------------//
void MusikFrame::OnServerEvent(wxSocketEvent& event)
{
	if ( !wxGetApp().WebServer.IsRunning() )
		return;

	//------------------------------------------------------//
	//--- if we get a connection, make the server listen ---//
	//------------------------------------------------------//
	switch( event.GetSocketEvent() )
	{
		case wxSOCKET_CONNECTION:
			wxGetApp().WebServer.Listen();
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
		SetTitle( wxString( MUSIKAPPNAME_VERSION ) + _( ": Updating tags from selection box (ESC to abort)" ) );
	}
	else if ( GetProgressType() == MUSIK_PLAYLIST_RENAME_THREAD )
	{
		SetTitle( wxString( MUSIKAPPNAME_VERSION ) + _( ": Renaming selected playlist items (ESC to abort)" ) );
	}
	else if ( GetProgressType() == MUSIK_WRITE_DIRTY_THREAD )
	{
		SetTitle( wxString( MUSIKAPPNAME_VERSION ) + _( ": Writing dirty tags to file (ESC to abort)" ) );
	}
}

void MusikFrame::OnUpdateProgress( wxCommandEvent& WXUNUSED(event) )
{
	if ( GetActiveThread() != NULL )
	{
		m_pProgressGauge->SetValue( GetProgress() );
	}
}

void MusikFrame::OnEndProgress( wxCommandEvent& WXUNUSED(event) )
{
	EnableProgress( false );

	m_pProgressGauge->SetValue( 0 );
	
	SetProgress	( 0 );
	SetProgressType	( 0 );
	SetActiveThread	( NULL );

	SetTitle( MUSIKAPPNAME_VERSION );
}

void MusikFrame::OnSashDraggedSourcesBox	(wxSashEvent & WXUNUSED(ev))
{
	wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,g_PlaylistBox);
}
void MusikFrame::OnSashDraggedActivityCtrl	(wxSashEvent & WXUNUSED(ev))
{
	wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,g_PlaylistBox);
}
