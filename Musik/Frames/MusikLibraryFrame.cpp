/*
 *  MusikLibraryFrame.cpp
 *
 *  Library setup frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikLibraryFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- frames ---//
#include "MusikFrame.h"

//--- threads ---//
#include "../Threads/MusikLibraryThreads.h"

BEGIN_EVENT_TABLE(MusikLibraryFrame, wxFrame)
	EVT_CONTEXT_MENU	( 									MusikLibraryFrame::PathsPopupMenu			)
	EVT_CHAR_HOOK		(									MusikLibraryFrame::TranslateKeys			)
	EVT_MENU			( MUSIK_PATHS_MENU_ADD,				MusikLibraryFrame::OnClickAdd				)
	EVT_MENU			( MUSIK_PATHS_MENU_REMOVESEL,		MusikLibraryFrame::OnClickRemoveSel			)
	EVT_MENU			( MUSIK_PATHS_MENU_REMOVEALL,		MusikLibraryFrame::OnClickRemoveAll			)
	EVT_MENU			( MUSIK_PATHS_MENU_CLEAR_LIBRARY,	MusikLibraryFrame::OnClickClearLibrary		)
	EVT_MENU			( MUSIK_PATHS_MENU_UPDATE_LIBRARY,	MusikLibraryFrame::OnRebuildAll				)
	EVT_MENU			( MUSIK_PATHS_MENU_PURGE_LIBRARY,	MusikLibraryFrame::OnPurgeLibrary			)
	EVT_BUTTON			( MUSIK_PATHS_OK,					MusikLibraryFrame::OnClickOK				)
	EVT_BUTTON			( MUSIK_PATHS_CANCEL,				MusikLibraryFrame::OnClickCancel			)
	EVT_LIST_KEY_DOWN	( MUSIK_PATHS_LIST,					MusikLibraryFrame::OnKeyPress				)
	EVT_CLOSE			(									MusikLibraryFrame::OnClose					)
	EVT_SIZE			(									MusikLibraryFrame::OnSize					)

	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU			( MUSIK_LIBRARY_THREAD_START,		MusikLibraryFrame::OnThreadStart		)
	EVT_MENU			( MUSIK_LIBRARY_THREAD_END,			MusikLibraryFrame::OnThreadEnd			)
	EVT_MENU			( MUSIK_LIBRARY_THREAD_PROG,		MusikLibraryFrame::OnThreadProg			)
	EVT_MENU			( MUSIK_LIBRARY_THREAD_SCAN_PROG,	MusikLibraryFrame::OnThreadScanProg		)
END_EVENT_TABLE()


//-----------------------------------//
//---     default constructor     ---//
//---  gets called automatically  ---//
//--- on startup to add new files ---//
//-----------------------------------//
MusikLibraryFrame::MusikLibraryFrame( wxFrame* pParent )
	: wxFrame( pParent, -1, _("Searching for and Adding New Files"), wxPoint( -1, -1 ), wxSize( 480, 48 ), wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxCLIP_CHILDREN )
{
	//------------------------------//
	//--- initialize needed vars ---//
	//------------------------------//
	m_Close			= true;
	m_AutoStart		= true;
	m_FirstStart	= false;
	m_MenuCreated	= false;
	bRebuild		= false;
	m_ActiveThread  = NULL;

	//-----------------------//
	//--- create controls ---//
	//-----------------------//
	CreateControls();
	PathsLoad();

	//------------------------------------------//
	//--- load paths and hide unneeded stuff ---//
	//------------------------------------------//
	vsTopSizer->Show( lcPaths, false );
	vsTopSizer->Show( hsSysButtons, false );
	
	#ifdef __WXGTK__
	wxSize size = vsTopSizer->GetMinSize();
	SetSize( 480, size.GetHeight() );
	#endif

	Centre();
}

//---------------------------------//
//---     main constructor      ---//
//---  gets called from menu    ---//
//--- or at program's first run ---//
//---------------------------------//
MusikLibraryFrame::MusikLibraryFrame( wxFrame* pParent, const wxPoint &pos, const wxSize &size ) 
	: wxFrame( pParent, -1, wxString(MUSIKAPPNAME) + _(" Library Setup"), pos, size, wxCAPTION | wxTAB_TRAVERSAL | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxCLIP_CHILDREN )
{
	//--------------------//
	//--- "Songs" menu ---//
	//--------------------//
    paths_setup_menu = new wxMenu;
    paths_setup_menu->Append( MUSIK_PATHS_MENU_ADD, _("&Add Directory") );
	paths_setup_menu->AppendSeparator();
	paths_setup_menu->Append( MUSIK_PATHS_MENU_REMOVESEL, _("&Remove &Selected Directories") );
	paths_setup_menu->Append( MUSIK_PATHS_MENU_REMOVEALL, _("Remove A&ll Directories") );

	//---------------------//
	//--- "Update" menu ---//
	//---------------------//	
    paths_update_menu = new wxMenu;
	paths_update_menu->Append( MUSIK_PATHS_MENU_UPDATE_LIBRARY, _("&Update Library") );
	paths_update_menu->Append( MUSIK_PATHS_MENU_PURGE_LIBRARY, _("Purge &Missing Songs") );
	paths_update_menu->AppendSeparator();
	paths_update_menu->Append( MUSIK_PATHS_MENU_CLEAR_LIBRARY, _("Clear &Library") );

	//--------------------//
	//--- "Setup" menu ---//
	//--------------------//	
	wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append( paths_setup_menu,_("&Directories") );
    menu_bar->Append( paths_update_menu,_("&Library") );

	//--------------------//
	//--- Context menu ---//
	//--------------------//
	paths_context_menu = new wxMenu;
    paths_context_menu->Append( MUSIK_PATHS_MENU_ADD, _("&Add Directory") );
	paths_context_menu->Append( MUSIK_PATHS_MENU_REMOVESEL, _("&Remove &Selected Directories") );
	paths_context_menu->Append( MUSIK_PATHS_MENU_REMOVEALL, _("Remove All Directories") );
	paths_context_menu->AppendSeparator();
	paths_context_menu->Append( MUSIK_PATHS_MENU_UPDATE_LIBRARY, _("&Update Library") );
	paths_context_menu->Append( MUSIK_PATHS_MENU_PURGE_LIBRARY, _("Purge &Missing Songs") );
	paths_context_menu->AppendSeparator();
	paths_context_menu->Append( MUSIK_PATHS_MENU_CLEAR_LIBRARY, _("Clear &Library") );

	//-----------------------//
	//--- create controls ---//
	//-----------------------//
	CreateControls();
	vsTopSizer->Show( gProgress, false );

	//----------------//
	//--- set menu ---//
	//----------------//
	SetMenuBar( menu_bar );
	
	//-----------------------------------//
	//--- load paths, initialize vars ---//
	//-----------------------------------//
	PathsLoad();
	bRebuild		= false;
	m_FirstStart	= true;
	m_AutoStart		= false;
	m_Close			= false;
	m_MenuCreated	= true;
	m_ActiveThread  = NULL;

	//--------------------//
	//--- center frame ---//
	//--------------------//
	Centre();
}

void MusikLibraryFrame::CreateControls()
{
	//--------------//
	//--- colors ---//
	//--------------//
	static wxColour cBtnFace = wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE );
	this->SetBackgroundColour ( cBtnFace );

	//------------//
	//--- icon ---//
	//------------//
	#if defined (__WXMSW__)
		SetIcon( wxICON( musicbox ) );
	#endif

	//--------------------//
	//--- dummy window ---//
	//--------------------//
	wxWindow *wndDummy = new wxWindow( this, -1, wxPoint( 0, 0 ), wxSize( 0, 0 ) );

	//----------------------//
	//--- Paths ListCtrl ---//
	//----------------------//	
	lcPaths  = new wxListCtrl( this, MUSIK_PATHS_LIST, wxPoint( 0, 0 ), wxSize( 0, 0 ), wxLC_REPORT | wxSIMPLE_BORDER );
	lcPaths->InsertColumn( 0, _("Path"), wxLIST_FORMAT_LEFT, -1 );
	lcPaths->InsertColumn( 1, _("Total"), wxLIST_FORMAT_RIGHT, -1 );
	lcPaths->InsertColumn( 2, _("New"), wxLIST_FORMAT_RIGHT, -1 );

	//--------------------//
	//--- progress bar ---//
	//--------------------//
	gProgress 	= new wxGauge( this, -1, 100, wxPoint( 0, 0 ), wxSize( 0, 18 ), wxGA_SMOOTH );

	//----------------------//
	//--- system buttons ---//
	//----------------------//
	btnCancel =	new wxButton( this, MUSIK_PATHS_CANCEL,	_("Cancel"),	wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnOK =		new wxButton( this, MUSIK_PATHS_OK,		_("OK"),		wxPoint( 0, 0 ), wxSize( 64, 24 ) );

	//----------------------------//
	//--- system buttons sizer ---//
	//----------------------------//
	hsSysButtons = new wxBoxSizer( wxHORIZONTAL );
	hsSysButtons->Add( btnCancel,	0, wxALIGN_LEFT		);
	hsSysButtons->Add( wndDummy,	1, wxEXPAND			);
	hsSysButtons->Add( btnOK,		0, wxALIGN_RIGHT	);	

	//-----------------//
	//--- top sizer ---//
	//-----------------//
	vsTopSizer = new wxBoxSizer( wxVERTICAL );
	vsTopSizer->Add( lcPaths, 1, wxEXPAND | wxALL, 2 );
	vsTopSizer->Add( gProgress, 0, wxEXPAND | wxALL, 2 );
	vsTopSizer->Add( hsSysButtons, 0, wxEXPAND | wxALL, 2 );

	SetSizer( vsTopSizer );
}

void MusikLibraryFrame::OnRebuildAll( wxCommandEvent& WXUNUSED(event) )	
{ 
	if ( wxMessageBox( _( "Would you like to clear Musik's library database?" ), MUSIKAPPNAME_VERSION, wxYES_NO | wxICON_QUESTION ) == wxYES )
		g_Library.RemoveAll();

	PathsSave(); 
	UpdateLibrary( false );	
}

bool MusikLibraryFrame::Show( bool show )
{
	bool bRet = wxWindow::Show( show );

	//---------------------------//
	//--- kill first run pref ---//
	//---------------------------//
	if ( g_Prefs.nFirstRun )
	{
		wxString sMessage = 	wxT( "This is the first time "MUSIKAPPNAME" has been run.\n\nTo begin, you must first add directories " 	)
								wxT( "to the database. Select \"Add Directory\" from the \"Songs\" menu, then press the " 		)
								wxT( "\"OK\" button to rebuild the library.\n\nTo display this window again, press " 			) 
								wxT( "CTRL+L in the main window, or select \"Library Setup\" from the \"Library\" menu."		);
							
		wxMessageBox( sMessage, MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
	
		g_Prefs.nFirstRun = 0;
		g_Prefs.SavePrefs();
	}
	
	//--- auto start ---//
	if ( m_AutoStart )
	{
		m_AutoStart	= false;
		UpdateLibrary( false );
	}

	//--- non-autstart ---//
	else if ( m_FirstStart )
	{
		m_FirstStart = false;
		ScanNew();
	}

	return bRet;
}
//------------------------------------------//
//--- Loading / Saving Paths in ListCtrl ---//
//------------------------------------------//
void MusikLibraryFrame::PathsLoad()
{
	for ( size_t i = 0; i < g_Paths.GetCount(); i++ )
	{
		if ( !g_Paths.Item( i ).IsEmpty() )
		{
			lcPaths->InsertItem( i, g_Paths.Item( i ) );
			lcPaths->SetItem( i, 1, wxT("-"), -1 );
			lcPaths->SetItem( i, 2, wxT("-"), -1 );
		}
	}
}

void MusikLibraryFrame::PathsSave()
{
	g_Paths.Clear();
	for ( int i = 0; i < lcPaths->GetItemCount(); i++ )
		g_Paths.Add( lcPaths->GetItemText( i ) );

	g_Paths.Save();
}

//---------------------//
//--- Dialog Events ---//
//---------------------//
void MusikLibraryFrame::Close( bool bCancel )
{
	//---------------------------------//
	//--- if cancel / ok is pressed ---//
	//---------------------------------//
	if ( !bCancel )
	{
		PathsSave();
		if ( bRebuild )
		{
			//-----------------------------------------------------//
			//--- setting m_Close allows the thread to close	---//
			//--- the dialog upon completion.					---//
			//-----------------------------------------------------//
			m_Close = true;
			UpdateLibrary( true );
			
			//-----------------------------------------------------//
			//--- we need to return prematurely, to allow the	---//
			//--- thread to process. it will handle dialog		---//
			//--- clean up										---//
			//-----------------------------------------------------//
			return;
		}
	}

	//--- we'll only have a menu if this is set ---//
	if ( m_MenuCreated )
	{
		delete paths_context_menu;	
		m_MenuCreated = false;
	}

	g_MusikFrame->Enable( TRUE );
	Destroy();
}

void MusikLibraryFrame::PathsResize()
{
	Layout();

	int nW, nH;
	lcPaths->GetClientSize( &nW, &nH );

	int nTotal = 50;
	int nDelta = 50;
	int nDir = nW - 100;
	lcPaths->SetColumnWidth( 0, nDir );
	lcPaths->SetColumnWidth( 1, nTotal );
	lcPaths->SetColumnWidth( 2, nDelta );
}

void MusikLibraryFrame::PathsGetSel( wxArrayString &aReturn )
{
	aReturn.Clear();
	for ( int i = 0; i < lcPaths->GetItemCount(); i++ )
	{
		if ( lcPaths->GetItemState( i, wxLIST_STATE_SELECTED ) > 2 )
			aReturn.Add( lcPaths->GetItemText( i ) );
	}
	return;
}

void MusikLibraryFrame::PathsPopupMenu( wxContextMenuEvent& WXUNUSED(event) )
{
	int nSel = lcPaths->GetSelectedItemCount();

	if ( nSel == 0 || nSel == -1 )
		paths_context_menu->Enable( MUSIK_PATHS_MENU_REMOVESEL, false );
	else
		paths_context_menu->Enable( MUSIK_PATHS_MENU_REMOVESEL, true );

	wxPoint pos = ScreenToClient( wxGetMousePosition() );
	PopupMenu( paths_context_menu, pos );
}

//-----------------------//
//--- ListCtrl Events ---//
//-----------------------//
void MusikLibraryFrame::PathsListRemoveSel()
{
	for ( int i = 0; i < lcPaths->GetItemCount(); i++ )
	{
		if ( lcPaths->GetItemState( i, wxLIST_STATE_SELECTED ) > 2 )	//THIS NEEDS TO BE FIXED?
		{
			aDelDirs.Add( lcPaths->GetItemText( i ) );
			lcPaths->DeleteItem( i );
			i--;
		}
	}

	PathsSave();
	bRebuild = true;
}

void MusikLibraryFrame::PathsListRemoveAll()
{
	if ( lcPaths->GetItemCount() )
	{
		lcPaths->DeleteAllItems();
		PathsSave();
	}
}

void MusikLibraryFrame::PathsListProcessKeys( wxListEvent & event )
{
	if ( event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_BACK )
		PathsListRemoveSel();
	else if ( event.GetKeyCode() == 65 )
		wxListCtrlSelAll( lcPaths );
	else if ( event.GetKeyCode() == 68 )
		wxListCtrlSelNone( lcPaths );
	else 
		event.Skip();
}

void MusikLibraryFrame::PathsListAdd()
{
	wxDirDialog *dlgBrowse = new wxDirDialog( this, _("Please choose music directory."), wxT(""), 0, wxPoint( 0, 0 ), wxSize( 0, 0 ) );
	if ( dlgBrowse->ShowModal() == wxID_OK )
	{
		wxString sPath = dlgBrowse->GetPath();
		if ( ( sPath != wxT("") ) && ( sPath != wxT("\\") ) )
		{	
			#if defined (__WXMSW__)
				if ( sPath.Right( 1 ) != wxT("\\") )
					sPath = sPath + wxT("\\");
			#else
				if ( sPath.Right( 1 ) != wxT("/") )
					sPath = sPath + wxT("/");
			#endif

			if ( ValidatePath( sPath ) )
			{
				lcPaths->InsertItem( lcPaths->GetItemCount(), sPath );
				lcPaths->SetItem( lcPaths->GetItemCount()-1, 1, wxT("-"), -1 );
				lcPaths->SetItem( lcPaths->GetItemCount()-1, 2, wxT("-"), -1 );

				PathsSave();
				bRebuild = true;
			}
		}
	}
	delete dlgBrowse;
}

//------------//
//--- Misc ---//
//------------//
bool MusikLibraryFrame::ValidatePath( wxString sPath )
{
	if ( g_Paths.GetCount() == 0 )
		return true;

	//--- loop through other paths, seeing if there is a conflict ---//
	wxString	sOldPath;
	wxString	sConflicts;
	wxArrayInt	aConflicts;

	#ifdef __WXMSW___
		sPath.MakeLower();
	#endif

	for ( size_t i = 0; i < g_Paths.GetCount(); i++ )
	{
		sOldPath = g_Paths.Item( i );

		#ifdef __WXMSW___
			sOldPath.MakeLower();
		#endif

		//---------------------------------------------------------//
		//--- if the new path equals the old path then return	---//
		//---------------------------------------------------------//
		if ( sPath == sOldPath )
		{
			wxMessageBox( wxT( "The path entered already exists." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
			return false;
		}

		//---------------------------------------------------------//
		//--- if new path is longer than path we're checking	---//
		//--- against, it will be a child folder. see if they	---//
		//--- have the same root								---//
		//---------------------------------------------------------//
		if ( sPath.Length() > sOldPath.Length() )
		{
			if ( sPath.Find( sOldPath ) > -1 )
			{
				wxMessageBox( wxT( "The path entered is already contained within the following path's scope:\n\n" ) + g_Paths.Item( i ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );
				return false;
			}
		}

		//---------------------------------------------------------//
		//--- if the old path is longer than the path we're		---//
		//--- checking against, it may be a path's parent dir	---//
		//---------------------------------------------------------//
		else 
		{
			if ( sOldPath.Find( sPath ) > -1 )
			{
				sConflicts += g_Paths.Item( i ) + wxT( "\n" );
				aConflicts.Add( i );
			}
		}

	}

	//-----------------------------------------------------//
	//--- display conflicts and ask user what to do		---//
	//-----------------------------------------------------//
	if ( sConflicts.Length() > 0 )
	{
		if ( wxMessageBox( wxT( "The path entered conflicts with the following paths:\n\n" ) + sConflicts + wxT( "\nDo you want me to fix this conflict for you?" ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION | wxYES_NO ) == wxYES )
		{
			size_t nCount = g_Paths.GetCount();
		
			for ( size_t i = 0; i < nCount; i++ )
				lcPaths->DeleteItem( aConflicts.Item( i ) - i );

			return true;
		}
		else 
			return false;
	}
	

	return true;
}

void MusikLibraryFrame::ClearLibrary()
{
	if ( wxMessageBox( _("This will wipe the library clean. Are you ABSOLUTELY SURE you want to do this?"), MUSIKAPPNAME_VERSION, wxYES_NO|wxICON_QUESTION  ) == wxYES )
	{
		g_Library.RemoveAll();
		g_ActivityAreaCtrl->ResetAllContents();
		g_Playlist.Clear();
		g_PlaylistBox->Update();

		ScanNew();
	}
}

void MusikLibraryFrame::ScanNew()
{
	if ( g_Paths.GetCount() == 0 )
		return;

	if ( GetActiveThread() == NULL )
	{
		pScanNewThread = new MusikScanNewThread();
		pScanNewThread->Create();
		SetActiveThread	( pScanNewThread );
		pScanNewThread->Run();
	}
	else
		wxMessageBox( _("An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error."), MUSIKAPPNAME_VERSION, wxICON_STOP );
}

void MusikLibraryFrame::UpdateLibrary( bool bConfirm )
{
	if ( bConfirm )
	{	
		if ( wxMessageBox( _(MUSIKAPPNAME" has detected that your library configuration has changed.\n\nIt is suggested that you update the internal library, which includes adding the new songs. Proceed?"), MUSIKAPPNAME_VERSION, wxYES_NO|wxICON_QUESTION  ) == wxNO )
			return;
	}
    
	if ( GetActiveThread() == NULL )
	{
		pUpdateLibThread = new MusikUpdateLibThread( &aDelDirs );
		pUpdateLibThread->Create();
		SetActiveThread	( pUpdateLibThread );
		pUpdateLibThread->Run();
	}
	else
		wxMessageBox( _("An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error."), MUSIKAPPNAME_VERSION, wxICON_STOP );
}

void MusikLibraryFrame::PurgeLibrary()
{
	if ( GetActiveThread() == NULL )
	{
		pPurgeLibThread = new MusikPurgeLibThread();
		pPurgeLibThread->Create();
		SetActiveThread	( pPurgeLibThread );
		pPurgeLibThread->Run();
	}
	else
		wxMessageBox( _("An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error."), MUSIKAPPNAME_VERSION, wxICON_STOP );

}

void MusikLibraryFrame::EnableProgress( bool enable )
{
	vsTopSizer->Show( gProgress, enable );

	lcPaths->Enable		( !enable );
	btnOK->Enable		( !enable );
	btnCancel->Enable	( !enable );
	Enable( !enable );

	Layout();
}

void MusikLibraryFrame::TranslateKeys( wxKeyEvent& event )
{
 	if ( event.GetKeyCode() == WXK_ESCAPE )
	{
		if ( GetActiveThread() != NULL )
			GetActiveThread()->Delete();
		else
			Close( true );
	}
	
	event.Skip(); // propagate esc to other frames too
}

//-----------------------------------------------------------//
//--- got a new thread start event, figure out what to do ---//
//-----------------------------------------------------------//
void MusikLibraryFrame::OnThreadStart( wxCommandEvent& WXUNUSED(event) )
{
	EnableProgress( true );
}

//-----------------------------------------------------//
//--- got a thread end event, figure out what to do ---//
//-----------------------------------------------------//
void MusikLibraryFrame::OnThreadEnd( wxCommandEvent& WXUNUSED(event) )
{
	SetActiveThread	( NULL );
	EnableProgress( false );

	if ( GetProgressType() == MUSIK_LIBRARY_SCANNEW_THREAD )
	{
	}

	else if ( GetProgressType() == MUSIK_LIBRARY_UPDATE_THREAD )
	{
		if ( m_Close )
			Close( true );

		g_ActivityAreaCtrl->ResetAllContents();
		if ( g_Prefs.nShowAllSongs == 1 )
		{
			g_Library.GetAllSongs( g_Playlist );
			g_PlaylistBox->Update();
		}
		bRebuild = false;
	}

	else if ( GetProgressType() == MUSIK_LIBRARY_PURGE_THREAD )
	{
		g_ActivityAreaCtrl->ResetAllContents();
		if ( g_Prefs.nShowAllSongs == 1 )
		{
			g_Library.GetAllSongs( g_Playlist );
			g_PlaylistBox->Update();
		}
	}

	SetProgress	( 0 );
	SetProgressType	( 0 );


	SetTitle(wxString(MUSIKAPPNAME) + _( " Library Setup" ) );
}

//---------------------------------------------------------//
//--- got a thread process event, figure out what to do ---//
//---------------------------------------------------------//
void MusikLibraryFrame::OnThreadProg( wxCommandEvent& WXUNUSED(event) )
{	
	if ( GetProgressType() == MUSIK_LIBRARY_SCANNEW_THREAD )
	{
		//----------------------------------------------------------//
		//--- MusikLibraryFrame::OnThreadScanProg will set title ---//
		//----------------------------------------------------------//

		lcPaths->SetItem( m_ListItem, 1, IntTowxString( m_Total ), -1 );
		lcPaths->SetItem( m_ListItem, 2, IntTowxString( m_New ), -1 );
	}

	else if ( GetProgressType() == MUSIK_LIBRARY_UPDATE_THREAD )
	{
		m_Title.sprintf( _( "Scanning for and adding new files: %d / %d (ESC to abort)" ), GetCurrent(), GetTotal() );
		SetTitle( m_Title );
	}

	else if ( GetProgressType() == MUSIK_LIBRARY_PURGE_THREAD )
	{
		m_Title.sprintf( _( "Scanning library for obselete files: %d / %d (ESC to abort)" ), GetCurrent(), GetTotal() );
		SetTitle( m_Title );
	}

	gProgress->SetValue( GetProgress() );
}

//-------------------------------------------------------------------//
//--- got a scan prog event, update to show users x files scanned ---//
//-------------------------------------------------------------------//
void MusikLibraryFrame::OnThreadScanProg( wxCommandEvent& WXUNUSED(event) )
{
	m_Title.sprintf( _( "Scanning directory for audio files: %d files scanned" ), GetScanCount() );
	SetTitle( m_Title );
}

void MusikLibraryFrame::SetActiveThread( wxThread* newactivethread)
{
	wxThread * pCurrThread = GetActiveThread();
	if(newactivethread == NULL)
	{
		
		wxASSERT(pCurrThread);
		pCurrThread->Wait();// wait until thread has completed
		delete pCurrThread;
		m_ActiveThread = NULL;
	}
	else
	{
		wxASSERT(pCurrThread == NULL); // ATTENTION!!! there is an active thread. someone forgot to call SetActiveThread(NULL)
	}
	m_ActiveThread = newactivethread;
}

