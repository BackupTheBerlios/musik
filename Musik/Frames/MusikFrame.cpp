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

//--- frames ---	//
#include "MusikLibraryFrame.h"
#include "MusikPrefsFrame.h"
#include "MusikTagFrame.h"

#ifdef __WXMSW__
	#include "MMShellHook.h"
#endif

//--- images ---//
#include "../images/sources/dynamic.xpm"
#include "../images/sources/library.xpm"
#include "../images/sources/standard.xpm"
#include "../images/ratings/rating0.xpm"
#include "../images/ratings/rating1.xpm"
#include "../images/ratings/rating2.xpm"
#include "../images/ratings/rating3.xpm"
#include "../images/ratings/rating4.xpm"
#include "../images/ratings/rating5.xpm"

// main dialog constructor
MusikFrame::MusikFrame() 
	: wxFrame( (wxFrame*)NULL, -1, MUSIK_VERSION, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxCLIP_CHILDREN )
{
	//--- no selection in progress / no dragging yet ---//
	g_DragInProg		= false;
	g_PlaylistChanged	= true;

	//--- pointer to main dialog, defined in MusikGlobals ---//
	g_MusikFrame = this;

	//--- prevent destroying saved placement info while constructing the frame ---//
	g_DisablePlacement = true;

	wxImage::AddHandler( new wxXPMHandler );

	//--- icon ---//
	#ifdef __WXMSW__
		SetIcon( wxICON( musicbox ) );
	#endif

	//--- colours ---//
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

	//--- load image lists and fonts ---//
	LoadImageLists();
	GetFonts();

	//-------------------------//
	//--- initialize sizers ---//
	//-------------------------//
	vsRightSide			= new wxBoxSizer	( wxVERTICAL	);
	vsLeftSide			= new wxBoxSizer	( wxVERTICAL	);
	hsLeftRight			= new wxBoxSizer	( wxHORIZONTAL	);
	vsTopBottom			= new wxBoxSizer	( wxVERTICAL	);

	//----------------//
	//--- playlist ---//
	//----------------//
	g_PlaylistCtrl  = new CPlaylistCtrl( this, MUSIK_PLAYLIST, wxPoint( 0, 0 ), wxSize( 0, 0 ) );

	//---------------------//
	//--- activity area ---//
	//---------------------//
	g_ActivityAreaCtrl = new CActivityAreaCtrl( this );

	//---------------------//
	//--- playlist info ---//
	//---------------------//
	g_PlaylistInfoCtrl = new CPlaylistInfoCtrl( this );
	GetListCtrlFont();

	//-------------------//
	//--- now playing ---//
	//-------------------//
	g_NowPlayingCtrl = new CNowPlayingCtrl( this );

	//---------------------------------//
	//--- progress bar for whatever ---//
	//---------------------------------//
	g_Progress = new wxGauge( this, -1, 100, wxPoint( 0, 0 ), wxSize( 0, 18 ), wxGA_SMOOTH );

	//---------------//
	//--- sources ---//
	//---------------//
	g_SourcesCtrl = new CSourcesBox( this, MUSIK_SOURCES );

	//--------------------//
	//--- simple query ---//
	//--------------------//
	g_SimpleQuery = new wxTextCtrl( this, MUSIK_SIMPLEQUERY, wxT( "" ), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxSIMPLE_BORDER );

	//-----------------------//
	//--- left area sizer ---//
	//-----------------------//
	vsLeftSide->Add( g_SourcesCtrl, 1, wxEXPAND | wxLEFT | wxBOTTOM | wxTOP, 1 );
	vsLeftSide->Add( g_SimpleQuery, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxTOP, 1 );

	//------------------------//
	//--- right area sizer ---//
	//------------------------//
	vsRightSide->Add( g_ActivityAreaCtrl, 1, wxEXPAND | wxALL, 0 );	
	vsRightSide->Add( g_PlaylistInfoCtrl, 0, wxEXPAND | wxALIGN_CENTER );
	vsRightSide->Add( g_PlaylistCtrl, 2, wxEXPAND | wxALL, 1 );

	//--------------------------//
	//--- left / right sizer ---//
	//--------------------------//
	hsLeftRight->Add( vsLeftSide, 1, wxEXPAND | wxALL );
	hsLeftRight->Add( vsRightSide, 5, wxEXPAND | wxLEFT | wxRIGHT, 1 );

	//--------------------------//
	//--- top / bottom sizer ---//
	//--------------------------//
	vsTopBottom->Add( hsLeftRight,		1, wxEXPAND | wxALL				  );
	vsTopBottom->Add( g_Progress,		0, wxEXPAND | wxLEFT | wxRIGHT, 1 );
	vsTopBottom->Add( g_NowPlayingCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT, 1 );

	//--- hide progress bar for the time being, and set its abort var to false ---//
	vsTopBottom->Show( g_Progress, false );

	//--- taylor ui ---//
	ShowPlaylistInfo();
	ShowSources();
	SetStayOnTop(( bool )g_Prefs.nStayOnTop);
	ShowActivityArea( g_Prefs.nShowActivities );

	//--- set sizer, center dialog ---//
	SetSizer/*AndFit*/( vsTopBottom );

	//--- restore placement or use defaults ---//
	g_DisablePlacement = false;

	//--- initialize playmode, seed randomness ---//
	srand( wxGetLocalTime() );
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
}

bool MusikFrame::Show( bool show )
{
	bool bRet = wxWindow::Show( show );
	
	if ( g_FirstRun && show )
	{
		g_FirstRun = false;

		//--- startup the crossfader			---//
		g_FaderThread = new MusikFaderThread();
		g_FaderThread->Create();
		g_FaderThread->Run();

		//--- autostart stuff ---//
		if ( g_Prefs.nFirstRun == 1 )
		{
			g_MusikLibraryFrame = new MusikLibraryFrame( ( wxFrame* )this, wxPoint( 0, 0 ), wxSize( 480, 240 ) );
			this->Enable	( FALSE );
			g_MusikLibraryFrame->Show	( TRUE	);
		}
		else if ( g_Prefs.nAutoAdd == 1 )
		{
			g_MusikLibraryFrame = new MusikLibraryFrame( ( wxFrame* )this );
			this->Enable	( FALSE );
			g_MusikLibraryFrame->Show	( TRUE	);
		}
		else
		{
			g_ActivityAreaCtrl->ResetAllContents();
			if ( g_Prefs.nShowAllSongs == 1 )
			{
				g_Playlist = g_Library.GetAllSongs();
				g_PlaylistCtrl->Update();
			}
		}
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
	g_SourcesImages = new wxImageList( 16, 20, true );
	bmpLibrary	= wxIcon( library_xpm );
	bmpPlaylist = wxIcon( standard_xpm );
	bmpDynamic	= wxIcon( dynamic_xpm );

	g_SourcesImages->Add( bmpLibrary );
	g_SourcesImages->Add( bmpPlaylist );
	g_SourcesImages->Add( bmpDynamic );

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
	g_SourcesImages->RemoveAll();
	g_NullImageList->RemoveAll();
	g_RatingImages->RemoveAll();
	delete g_SourcesImages;
	delete g_NullImageList;
	delete g_RatingImages;
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
	g_fntListBold = g_PlaylistCtrl->GetFont();
	g_fntListBold.SetWeight( wxBOLD );
}

//--------------------------------------------//
//--- showing / hiding / toggling controls ---//
//--------------------------------------------//
void MusikFrame::TogglePlaylistInfo()
{
	if ( g_Prefs.nShowPLInfo == 0 )
	{
		g_Prefs.nShowPLInfo = 1;
		g_PlaylistInfoCtrl->Update();
	}
	else
		g_Prefs.nShowPLInfo = 0;

	ShowPlaylistInfo();

	view_menu->Check( MUSIK_MENU_PLAYLISTINFO_STATE, ( bool )g_Prefs.nShowPLInfo );
}

void MusikFrame::ShowPlaylistInfo()
{
	vsRightSide->Show( g_PlaylistInfoCtrl, ( bool )g_Prefs.nShowPLInfo );

	Layout();

	g_PlaylistCtrl->RescaleColumns();
	g_SourcesCtrl->RescaleColumns();
	g_ActivityAreaCtrl->RescaleColumns();
}

void MusikFrame::ShowSources()
{
	hsLeftRight->Show( vsLeftSide, ( bool )g_Prefs.nShowSources );

	Layout();

	g_PlaylistCtrl->RescaleColumns();
	g_SourcesCtrl->RescaleColumns();
	g_ActivityAreaCtrl->RescaleColumns();
}

void MusikFrame::ToggleSources()
{
	if ( g_Prefs.nShowSources == 0 )
		g_Prefs.nShowSources = 1;
	else
		g_Prefs.nShowSources = 0;

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
	vsRightSide->Show( g_ActivityAreaCtrl, bShow );
	Layout();
}

void MusikFrame::ToggleActivities()
{
	if ( g_Prefs.nShowActivities == 0 )
		g_Prefs.nShowActivities = 1;
	else
		g_Prefs.nShowActivities = 0;

	ShowActivityArea( (bool)g_Prefs.nShowActivities );

	view_menu->Check( MUSIK_MENU_ACTIVITIES_STATE,	( bool )g_Prefs.nShowActivities );
}

void MusikFrame::EnableProgress( bool enable )
{
	vsTopBottom->Show( g_Progress, enable );
	Enable( !enable );
	Layout();
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
				if( g_Player.IsPaused() )
					g_NowPlayingCtrl->PlayerPlayPause( cev );
				return 1;		
			}
		}
		return wxFrame::MSWWindowProc(message,wParam,lParam);
	}	
#endif
