/*
 *  MusikFrame.cpp
 *
 *  Musik's primary frame
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

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"
#include "../MusikApp.h"
//--- frames ---	//
#include "MusikLibraryFrame.h"
#include "MusikPrefsFrame.h"
#include "MusikTagFrame.h"

#ifdef __WXMSW__
	#include "../MMShellHook/MMShellHook.h"
#endif

//--- images ---//
#include "../images/sources/dynamic.xpm"
#include "../images/sources/library.xpm"
#include "../images/sources/standard.xpm"
#include "../images/sources/netstream.xpm"
#include "../images/sources/nowplaying.xpm"
#include "../images/ratings/rating0.xpm"
#include "../images/ratings/rating1.xpm"
#include "../images/ratings/rating2.xpm"
#include "../images/ratings/rating3.xpm"
#include "../images/ratings/rating4.xpm"
#include "../images/ratings/rating5.xpm"
#include "../images/tray.xpm"
DECLARE_APP( MusikApp )

#ifdef wxHAS_TASK_BAR_ICON
enum {
    PU_RESTORE = 11101,
    PU_HIDE ,
    PU_PLAYPAUSE,
	PU_PREV,
	PU_NEXT,
	PU_STOP,
    PU_EXIT
};


BEGIN_EVENT_TABLE(MusikTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE,	MusikTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_HIDE,		MusikTaskBarIcon::OnMenuHide)
    EVT_MENU(PU_PLAYPAUSE,	MusikTaskBarIcon::OnMenuPlayPause)
    EVT_MENU(PU_PREV,	MusikTaskBarIcon::OnMenuPrev)
    EVT_MENU(PU_NEXT,	MusikTaskBarIcon::OnMenuNext)
    EVT_MENU(PU_STOP,	MusikTaskBarIcon::OnMenuStop)

    EVT_MENU(PU_EXIT,		MusikTaskBarIcon::OnMenuExit)
    EVT_TASKBAR_RIGHT_UP     (MusikTaskBarIcon::OnRButtonUp)
    EVT_TASKBAR_LEFT_DCLICK  (MusikTaskBarIcon::OnLButtonDClick)
END_EVENT_TABLE()

void MusikTaskBarIcon::RestoreFrame()
{
	m_pFrame->Show(TRUE);
	//RemoveIcon();
}
void MusikTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
	RestoreFrame();
}
void MusikTaskBarIcon::OnMenuHide(wxCommandEvent& )
{
	m_pFrame->Show(FALSE);
}
void MusikTaskBarIcon::OnMenuPlayPause(wxCommandEvent& )
{
	g_Player.PlayPause();
}
void MusikTaskBarIcon::OnMenuPrev(wxCommandEvent& )
{
	g_Player.PrevSong();
}
void MusikTaskBarIcon::OnMenuNext(wxCommandEvent& )
{
	g_Player.NextSong();
}
void MusikTaskBarIcon::OnMenuStop(wxCommandEvent& )
{
	g_Player.Stop();
}

void MusikTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    m_pFrame->Close(TRUE);

    // Nudge wxWindows into destroying the dialog, since
    // with a hidden window no messages will get sent to put
    // it into idle processing.
    wxTheApp->ProcessIdle();
}



// Overridables
void MusikTaskBarIcon::OnRButtonUp(wxEvent&)
{
    wxMenu      menu;

	if(!m_pFrame->IsShown())
	    menu.Append(PU_RESTORE, _("&Restore wxMusik"));
	else
		menu.Append(PU_HIDE, _("&Hide wxMusik"));
	menu.AppendSeparator();

	if ( g_Player.IsPlaying() && !g_Player.IsPaused() )
		menu.Append(PU_PLAYPAUSE, _("&Pause"));
	else if ( g_Player.IsPlaying() && g_Player.IsPaused() )
		menu.Append(PU_PLAYPAUSE, _("&Resume"));
	else if ( !g_Player.IsPlaying() )
		menu.Append(PU_PLAYPAUSE, _("&Play"));
	menu.Append(PU_PREV, _("&Previous"));
	menu.Append(PU_NEXT, _("&Next"));
	if ( g_Player.IsPlaying() )
		menu.Append(PU_STOP, _("&Stop"));
	menu.AppendSeparator();

    menu.Append(PU_EXIT,    _("E&xit"));

    PopupMenu(&menu);
}

void MusikTaskBarIcon::OnLButtonDClick(wxEvent&)
{
    RestoreFrame();
}


#endif //#ifdef wxHAS_TASK_BAR_ICON

// main dialog constructor
MusikFrame::MusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, MUSIKAPPNAME_VERSION, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN )
{
	//--- no selection in progress / no dragging yet ---//
	g_DragInProg		= false;
	g_PlaylistChanged	= true;

	//--- pointer to main dialog, defined in MusikGlobals ---//
	g_MusikFrame = this;

	//--- prevent destroying saved placement info while constructing the frame ---//
	g_DisablePlacement = true;

	//--- icon ---//
	#ifdef __WXMSW__
		SetIcon( wxICON( musicbox ) );
	#endif
#ifdef wxHAS_TASK_BAR_ICON
  m_pTaskBarIcon = new MusikTaskBarIcon(this);
#endif
	//--- colours ---//
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

	//--- load image lists and fonts ---//
	LoadImageLists();
	GetFonts();

	//-------------------------//
	//--- initialize sizers ---//
	//-------------------------//
	vsTopBottom			= new wxBoxSizer	( wxVERTICAL	);



	m_pBottomPanel = new wxSashLayoutWindow(this,-1,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxCLIP_CHILDREN);
	m_pBottomPanel->SetDefaultSize(wxSize(1000,70));
	m_pBottomPanel->SetAlignment(wxLAYOUT_BOTTOM);
	m_pBottomPanel->SetOrientation(wxLAYOUT_HORIZONTAL);
	//----------------//
	//--- playlist ---//
	//----------------//
	g_PlaylistBox  = new CPlaylistBox( this );
	GetListCtrlFont();

	//---------------//
	//--- sources ---//
	//---------------//
	g_SourcesCtrl = new CSourcesBox( this );

	g_SourcesCtrl->SetSashVisible(wxSASH_RIGHT, true);
	g_SourcesCtrl->SetDefaultSize(wxSize(g_Prefs.nSourceBoxWidth,1000));
	g_SourcesCtrl->SetAlignment(wxLAYOUT_LEFT);
	g_SourcesCtrl->SetOrientation(wxLAYOUT_VERTICAL);
	g_SourcesCtrl->SetSashBorder(wxSASH_RIGHT, true);
	g_SourcesCtrl->SetSashBorder(wxSASH_RIGHT, true);
	g_SourcesCtrl->SetDefaultBorderSize(3);
	g_SourcesCtrl->SetExtraBorderSize(1);
	//---------------------//
	//--- activity area ---//
	//---------------------//
	g_ActivityAreaCtrl = new CActivityAreaCtrl( this );
	g_ActivityAreaCtrl->SetSashVisible(wxSASH_BOTTOM, true);
	g_ActivityAreaCtrl->SetDefaultSize(wxSize(1000,g_Prefs.nActivityCtrlHeight));
	g_ActivityAreaCtrl->SetAlignment(wxLAYOUT_TOP);
	g_ActivityAreaCtrl->SetOrientation(wxLAYOUT_HORIZONTAL);
	g_ActivityAreaCtrl->SetSashBorder(wxSASH_BOTTOM, true);
	g_ActivityAreaCtrl->SetDefaultBorderSize(3);
	g_ActivityAreaCtrl->SetExtraBorderSize(1);
	

	//-------------------//
	//--- now playing ---//
	//-------------------//
	g_NowPlayingCtrl = new CNowPlayingCtrl( m_pBottomPanel );

	//---------------------------------//
	//--- progress bar for whatever ---//
	//---------------------------------//
	g_Progress = new wxGauge( m_pBottomPanel, -1, 100, wxPoint( 0, 0 ), wxSize( 0, 18 ), wxGA_SMOOTH );

	//--------------------------//
	//--- top / bottom sizer ---//
	//--------------------------//
//	vsTopBottom->Add( hsLeftRight,		1, wxEXPAND | wxALL				  );
	vsTopBottom->Add( g_Progress,		0, wxEXPAND | wxLEFT | wxRIGHT, 1 );
	vsTopBottom->Add( g_NowPlayingCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT, 1 );

	//--- hide progress bar for the time being, and set its abort var to false ---//
	vsTopBottom->Show( g_Progress, false );

	m_pBottomPanel->SetSizer(vsTopBottom);
	//--- taylor ui ---//
	ShowPlaylistInfo();
	ShowSources();
	SetStayOnTop(( bool )g_Prefs.nStayOnTop);


	//--- restore placement or use defaults ---//
	g_DisablePlacement = false;

	g_Player.SetPlaymode();

	//--- update database information, then set sound volume ---//
	g_Player.SetVolume();

	SetActiveThread( NULL );


	#ifdef __WXMSW__
		SetMMShellHook((HWND)GetHWND());
	#endif
}

MusikFrame::~MusikFrame()
{
#ifdef wxHAS_TASK_BAR_ICON
  delete m_pTaskBarIcon;
#endif 
}

void MusikFrame::AutoUpdate	( const wxArrayString & Filenames ,bool bFilesWereDropped)
{
	g_MusikLibraryFrame = new MusikLibraryFrame( ( wxFrame* )this, Filenames,bFilesWereDropped );
	this->Enable	( FALSE );
	g_MusikLibraryFrame->Show	( TRUE	); 
}
bool MusikFrame::Show( bool show )
{
	bool bRet = false;
    bRet =	 wxWindow::Show( show );
	
	if ( g_FirstRun && show )
	{
		g_FirstRun = false;


		//--- autostart stuff ---//
		if ( g_Prefs.nFirstRun || g_Prefs.nAutoAdd )
		{
			if(g_Prefs.nFirstRun)
			{
				g_MusikLibraryFrame = new MusikLibraryFrame( ( wxFrame* )this, wxPoint( 0, 0 ), wxSize( 480, 240 ) );
				this->Enable	( FALSE );
				g_MusikLibraryFrame->Show	( TRUE	);
			}
			else
				AutoUpdate();
          return bRet;
		}
		else
		{
			g_SourcesCtrl->SelectNowPlaying();
			g_PlaylistBox->Update();

			/*
			ShowActivityArea( g_Prefs.nShowActivities );
    		g_ActivityAreaCtrl->ResetAllContents();
			if ( g_Prefs.nShowAllSongs == 1 )
			{
				g_Library.GetAllSongs( g_Playlist );
				g_SourcesCtrl->SelectLibrary();
				g_PlaylistBox->Update(true);
			}
			*/
		}
		//--- startup the crossfader			---//
		g_FaderThread = new MusikFaderThread();
		g_FaderThread->Create();
		g_FaderThread->Run();
	}
	return bRet;	
}

//----------------------------------//
//--- Image and image list stuff ---//
//----------------------------------//
void MusikFrame::LoadImageLists()
{
	//--- null image list ---//
	g_NullImageList = new wxImageList( 1, 1, true );

	//--- sources image list ---//
	g_SourcesImages = new wxImageList( 20, 20, true );
	bmpLibrary	= wxIcon( library_xpm );
	bmpPlaylist = wxIcon( standard_xpm );
	bmpDynamic	= wxIcon( dynamic_xpm );
	bmpNowplaying =  wxIcon( nowplaying_xpm );
	bmpNetstream =  wxIcon( netstream_xpm );
	g_SourcesImages->Add( bmpLibrary );
	g_SourcesImages->Add( bmpPlaylist );
	g_SourcesImages->Add( bmpDynamic );
///// just to fill it up
	g_SourcesImages->Add( bmpPlaylist );
	g_SourcesImages->Add( bmpNetstream );
	g_SourcesImages->Add( bmpNowplaying );

	//--- ratings image list ---//
	g_RatingImages = new wxImageList( 32, 14, true );
	bmpRating0 = wxIcon( rating0_xpm );
	bmpRating1 = wxIcon( rating1_xpm );
	bmpRating2 = wxIcon( rating2_xpm );
	bmpRating3 = wxIcon( rating3_xpm );
	bmpRating4 = wxIcon( rating4_xpm );
	bmpRating5 = wxIcon( rating5_xpm );

	g_RatingImages->Add( bmpRating0 );
	g_RatingImages->Add( bmpRating1 );
	g_RatingImages->Add( bmpRating2 );
	g_RatingImages->Add( bmpRating3 );
	g_RatingImages->Add( bmpRating4 );
	g_RatingImages->Add( bmpRating5 );
}

void MusikFrame::DeleteImageLists()
{
	//-------------------------------------------------//
	//--- wxListCtrl::SetImageList() is supposed to	---//
	//--- NOT delete the image list, but in wxGTK	---//
	//--- it apparently does. 						---//
	//-------------------------------------------------//
	#ifdef __WXMSW__
		g_SourcesImages->RemoveAll();
		g_NullImageList->RemoveAll();
		g_RatingImages->RemoveAll();
		delete g_SourcesImages;
		delete g_NullImageList;
		delete g_RatingImages;
	#endif
}

void MusikFrame::GetFonts()
{
	wxString sFace;
	#if defined __WXMSW__
		sFace = wxT( "Tahoma" );

		g_fntSong.SetFaceName	( sFace );
		g_fntSong.SetPointSize	( 14 );

		g_fntArtist.SetFaceName	( sFace );	
		g_fntArtist.SetWeight	( wxBOLD );
		g_fntArtist.SetPointSize( 10 );

		g_fntTime.SetFaceName	( sFace );
		g_fntTime.SetWeight		( wxBOLD );
		g_fntTime.SetPointSize	( 8 );

		g_fntInfo = g_fntTime;

		g_fntRegular.SetFaceName( sFace );

		g_fntBold.SetFaceName	( sFace );
		g_fntBold.SetWeight		( wxBOLD );

	#elif defined __WXGTK__
		sFace = wxT( "Sans" );
	
		g_fntSong.SetFaceName	( sFace );
		g_fntSong.SetPointSize	( 14 );

		g_fntArtist.SetFaceName	( sFace );	
		g_fntArtist.SetPointSize( 10 );

		g_fntTime.SetFaceName	( sFace );
		g_fntTime.SetPointSize	( 8 );

		g_fntInfo = g_fntTime;

		g_fntRegular.SetFaceName( sFace );
		g_fntRegular.SetPointSize( 8 );

		g_fntBold.SetFaceName	( sFace );
		g_fntBold.SetWeight		( wxBOLD );
		g_fntBold.SetPointSize	( 8 );
	#endif

}

void MusikFrame::GetListCtrlFont()
{
	g_fntListBold = g_PlaylistBox->PlaylistCtrl().GetFont();
	g_fntListBold.SetWeight( wxBOLD );
}

//--------------------------------------------//
//--- showing / hiding / toggling controls ---//
//--------------------------------------------//
void MusikFrame::TogglePlaylistInfo()
{
	g_Prefs.nShowPLInfo = !g_Prefs.nShowPLInfo;
	ShowPlaylistInfo();

	view_menu->Check( MUSIK_MENU_PLAYLISTINFO_STATE, ( bool )g_Prefs.nShowPLInfo );
}

void MusikFrame::ShowPlaylistInfo()
{
	g_PlaylistBox->ShowPlaylistInfo();
}

void MusikFrame::ShowSources()
{
	g_SourcesCtrl->Show(  ( bool )g_Prefs.nShowSources );
	wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,g_PlaylistBox);
}

void MusikFrame::ToggleSources()
{
	g_Prefs.nShowSources = !g_Prefs.nShowSources;

	ShowSources();

	view_menu->Check( MUSIK_MENU_SOURCES_STATE,	( bool )g_Prefs.nShowSources );
}
void MusikFrame::SetStayOnTop( bool bStayOnTop )
{
#ifdef __WXMSW__
	long style = GetWindowStyle();
	if ( bStayOnTop )
		style |= wxSTAY_ON_TOP;
	else
		style &= ~wxSTAY_ON_TOP;
	SetWindowStyle( style );
#endif
}
void MusikFrame::ShowActivityArea( bool bShow )
{
	if(g_SourcesCtrl->GetSelType() != MUSIK_SOURCES_LIBRARY)
		bShow=false;
	g_PlaylistBox->TextSimpleQuery().Enable(bShow);
	g_ActivityAreaCtrl->Show( bShow );
	wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,g_PlaylistBox);

	g_PlaylistBox->Layout();
	g_PlaylistBox->Refresh();
}

void MusikFrame::ToggleActivities()
{
	
	g_Prefs.nShowActivities = !g_Prefs.nShowActivities;

	ShowActivityArea( (bool)g_Prefs.nShowActivities );

	view_menu->Check( MUSIK_MENU_ACTIVITIES_STATE,	( bool )g_Prefs.nShowActivities );
}

void MusikFrame::EnableProgress( bool enable )
{

	vsTopBottom->Show( g_Progress, enable );
	Enable( !enable );
	m_pBottomPanel->Layout();
   	m_pBottomPanel->SetDefaultSize(vsTopBottom->GetMinSize());
	wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,g_PlaylistBox);
	m_pBottomPanel->Layout();
}

void MusikFrame::SetTitle(const wxString& title)
{
#ifdef wxHAS_TASK_BAR_ICON
	m_pTaskBarIcon->SetIcon(wxIcon(tray_xpm), title);
#endif
	wxFrame::SetTitle(title);
}


//---------------------------------------------------------//
//--- support for microsoft windows multimedia keyboard ---//
//---------------------------------------------------------//
#ifdef __WXMSW__

	#ifndef WM_APPCOMMAND
		#define WM_APPCOMMAND                   0x0319
		#define FAPPCOMMAND_MASK  0xF000
		#define GET_APPCOMMAND_LPARAM(lParam) ((short)(HIWORD(lParam) & ~FAPPCOMMAND_MASK))
		#define APPCOMMAND_MEDIA_NEXTTRACK        11
		#define APPCOMMAND_MEDIA_PREVIOUSTRACK    12
		#define APPCOMMAND_MEDIA_STOP             13
		#define APPCOMMAND_MEDIA_PLAY_PAUSE       14
	#endif

	long MusikFrame::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
	{
		if(message == WM_APPCOMMAND)
		{
			wxCommandEvent cev;
			switch( GET_APPCOMMAND_LPARAM( lParam ) )
			{
			case APPCOMMAND_MEDIA_NEXTTRACK:
				if( g_Player.IsPlaying() )
					g_NowPlayingCtrl->PlayerNext( cev );
				return 1;
			case APPCOMMAND_MEDIA_PREVIOUSTRACK:
				if( g_Player.IsPlaying() )
					g_NowPlayingCtrl->PlayerPrev( cev );
				return 1;
			case APPCOMMAND_MEDIA_STOP:
				if( g_Player.IsPlaying() )
					g_NowPlayingCtrl->PlayerStop( cev );
				return 1;
			case APPCOMMAND_MEDIA_PLAY_PAUSE:
					g_NowPlayingCtrl->PlayerPlayPause( cev );
				return 1;		
			}
		}
		return wxFrame::MSWWindowProc(message,wParam,lParam);
	}	
#endif
