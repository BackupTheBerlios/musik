/*
 *  NowPlayingCtrl.h
 *
 *  The "Now Playing" information
 *	This is the control visible on the very bottom of the frame
 *	Includes currently playing songs and playlist navigation
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- globals ---//
#include "NowPlayingCtrl.h"
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- frames ---//
#include "../Frames/MusikFrame.h"

//--- images ---//
#include "../images/playback/next.xpm"
#include "../images/playback/next_down.xpm"
#include "../images/playback/pause.xpm"
#include "../images/playback/pause_down.xpm"
#include "../images/playback/play.xpm"
#include "../images/playback/play_down.xpm"
#include "../images/playback/prev.xpm"
#include "../images/playback/prev_down.xpm"
#include "../images/playback/stop.xpm"
#include "../images/playback/stop_down.xpm"
#include "../images/playback/volume.xpm"
#include "../images/playback/volume_down.xpm"

BEGIN_EVENT_TABLE(CNowPlayingCtrl, wxPanel)
	EVT_BUTTON	(MUSIK_NOWPLAYINGCTRL_STOP,				CNowPlayingCtrl::PlayerStop				)	// stop button pressed
	EVT_BUTTON	(MUSIK_NOWPLAYINGCTRL_PLAYPAUSE,		CNowPlayingCtrl::PlayerPlayPause		)	// play/pause/resume perssed
	EVT_BUTTON	(MUSIK_NOWPLAYINGCTRL_NEXT,				CNowPlayingCtrl::PlayerNext				)	// next button pressed
	EVT_BUTTON	(MUSIK_NOWPLAYINGCTRL_PREV,				CNowPlayingCtrl::PlayerPrev				)	// prev button pressed
	EVT_BUTTON	(MUSIK_NOWPLAYINGCTRL_VOLUME,			CNowPlayingCtrl::PlayerVolume			)	// prev button pressed
	EVT_TIMER	(MUSIK_NOWPLAYING_TIMER,				CNowPlayingCtrl::OnTimer				)	// timer
END_EVENT_TABLE()

CNowPlayingCtrl::CNowPlayingCtrl( wxWindow *parent )
	: wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxTRANSPARENT_WINDOW )
{
	wxColour bg = wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE );

	SetBackgroundColour( bg );

	pLeftPanel = new wxPanel( this );
	pRightPanel = new wxPanel( this );

	//-----------------------------//
	//--- title / artist / time ---//
	//-----------------------------//
	//--- now playing static text objects ---//
	stSong			= new wxStaticText	( pLeftPanel, -1, _( "" ),	wxPoint( 0, 0 ), wxSize( 0, 0 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stArtist		= new wxStaticText	( pLeftPanel, -1, _( "" ),	wxPoint( 0, 0 ), wxSize( 0, 0 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	stCurtime		= new wxStaticText	( pLeftPanel, -1, _( "" ),	wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );	

	//--- fonts ---//
	stSong->SetFont		( g_fntSong );
	stArtist->SetFont	( g_fntArtist );
	stCurtime->SetFont	( g_fntTime );

	//---------------------------//
	//--- buttons and bitmaps ---//
	//---------------------------//
	//--- bitmaps ---//
	bmPrev			= wxBitmap( prev_xpm );
	bmPrevDown		= wxBitmap( prev_down_xpm );
	bmNext			= wxBitmap( next_xpm );
	bmNextDown		= wxBitmap( next_down_xpm );
	bmPlay			= wxBitmap( play_xpm );
	bmPlayDown		= wxBitmap( play_down_xpm );
	bmPause			= wxBitmap( pause_xpm );
	bmPauseDown		= wxBitmap( pause_down_xpm );
	bmStop			= wxBitmap( stop_xpm );
	bmStopDown		= wxBitmap( stop_down_xpm );
	bmVolume		= wxBitmap( volume_xpm );
	bmVolumeDown	= wxBitmap( volume_down_xpm );

	//--- buttons ---//
	btnPrev			= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_PREV, bmPrev, wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	btnNext			= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_NEXT, bmNext, wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	btnPlayPause	= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_PLAYPAUSE, bmPlay, wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );	
	btnStop			= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_STOP, bmStop, wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	btnVolume		= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_VOLUME, bmVolume, wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );

	//--- events ---//
	btnPrev->SetBitmapLabel				( bmPrev );
	btnNext->SetBitmapLabel				( bmNext );
	btnPlayPause->SetBitmapLabel		( bmPlay );
	btnStop->SetBitmapLabel				( bmStop );
	btnVolume->SetBitmapLabel			( bmVolume );
	#ifdef __WXGTK__
	btnPrev->SetBitmapFocus				( bmPrevDown );
	btnNext->SetBitmapFocus				( bmNextDown );
	btnPlayPause->SetBitmapFocus		( bmPlayDown );
	btnStop->SetBitmapFocus				( bmStopDown );
	btnVolume->SetBitmapFocus			( bmVolumeDown );
	#elif defined __WXMSW__
	pPrevEvt	= new CBtnDownEvt	( btnPrev, &bmPrevDown, &bmPrev );
	pNextEvt	= new CBtnDownEvt	( btnNext, &bmNextDown, &bmNext );
	pPlayEvt	= new CBtnDownEvt	( btnPlayPause, &bmPlayDown, &bmPlay );
	pPauseEvt	= new CBtnDownEvt	( btnPlayPause, &bmPauseDown, &bmPause );
	pStopEvt	= new CBtnDownEvt	( btnStop, &bmStopDown, &bmStop );
	pVolumeEvt	= new CBtnDownEvt	( btnVolume, &bmVolumeDown, &bmVolume );
    btnPrev->PushEventHandler		( pPrevEvt );
	btnNext->PushEventHandler		( pNextEvt );
	btnPlayPause->PushEventHandler	( pPlayEvt );
	btnStop->PushEventHandler		( pStopEvt );
	btnVolume->PushEventHandler		( pVolumeEvt );
	#endif

	//----------------//
	//--- seek bar ---//
	//----------------//
	gSeek		= new wxGauge		( pRightPanel, -1, 100, wxPoint( 0, 0 ), wxSize( 200, wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ) ), wxGA_SMOOTH | wxGA_HORIZONTAL | wxCLIP_CHILDREN );
	pSeekEvt	= new CGaugeSeekEvt	( gSeek );
	gSeek->PushEventHandler( pSeekEvt );

	//---------------------------------//
	//--- sizer for artist and time ---//
	//---------------------------------//
	hsArtistTime = new wxBoxSizer( wxHORIZONTAL );
	hsArtistTime->Add( stArtist, 0, wxADJUST_MINSIZE | wxALIGN_CENTRE_VERTICAL /*| wxEXPAND*/ );
	hsArtistTime->Add( stCurtime, 0, wxADJUST_MINSIZE | wxALIGN_CENTRE_VERTICAL /*| wxEXPAND*/ );

	//--- song title, artist and time ---//
	vsLeftCol = new wxBoxSizer( wxVERTICAL );
	vsLeftCol->Add( stSong, 0, wxADJUST_MINSIZE | wxBOTTOM /*| wxEXPAND*/, 4 );
	vsLeftCol->Add( hsArtistTime, 0, wxEXPAND );

	pLeftPanel->SetSizerAndFit( vsLeftCol );

	//--- buttons, seek bar panel ---//
	hsButtons = new wxBoxSizer( wxHORIZONTAL );
	hsButtons->Add( btnPrev );
	hsButtons->Add( btnPlayPause );
	hsButtons->Add( btnStop );
	hsButtons->Add( btnNext );
	hsButtons->Add( btnVolume );

	vsRightCol = new wxBoxSizer( wxVERTICAL );
	vsRightCol->Add( hsButtons, 1 );
	vsRightCol->Add( gSeek, 0, wxTOP, 2 ); //-- small top border --//

	pRightPanel->SetSizerAndFit( vsRightCol );

	hsCols = new wxBoxSizer( wxHORIZONTAL );
	hsCols->Add( pLeftPanel,	0, wxADJUST_MINSIZE /*| wxEXPAND*/ | wxALL, 2	);
	hsCols->Add( 0, 0,			1, wxEXPAND);
	hsCols->Add( pRightPanel,	0, wxEXPAND | wxALL, 2	);

	SetSizerAndFit( hsCols );

	pSecTimer = NULL;
	StartTimer();
	g_TimeSeeking = false;

	ResetInfo();
}

CNowPlayingCtrl::~CNowPlayingCtrl()
{
	//--- stop timer ---//
	KillTimer();

	#ifdef __WXMSW__
    btnPrev->PopEventHandler();
	btnNext->PopEventHandler();
	btnPlayPause->PopEventHandler();
	btnStop->PopEventHandler();
	btnVolume->PopEventHandler();
	delete pPrevEvt;
	delete pNextEvt;
	delete pPlayEvt;
	delete pPauseEvt;
	delete pStopEvt;
	delete pVolumeEvt;
	#endif

	gSeek->PopEventHandler();
	delete pSeekEvt;
}

//-------------------//
//--- timer stuff ---//
//-------------------//
void CNowPlayingCtrl::OnTimer( wxTimerEvent& WXUNUSED(event) )
{
	if ( g_Player.IsPlaying() )
		UpdateTime();
}

void CNowPlayingCtrl::StartTimer()
{
	if ( pSecTimer == NULL )
	{
		pSecTimer = new wxTimer( this, MUSIK_NOWPLAYING_TIMER );
		pSecTimer->Start( 1000, false );
	}
}

void CNowPlayingCtrl::KillTimer()
{
	if ( pSecTimer )
	{
		pSecTimer->Stop();
		delete pSecTimer;
	}
}

void CNowPlayingCtrl::PlayBtnToPauseBtn()
{
	#ifdef __WXGTK__
	btnPlayPause->SetBitmapLabel( bmPause );
	btnPlayPause->SetBitmapFocus( bmPauseDown );
	#elif defined __WXMSW__
	btnPlayPause->SetBitmapLabel( bmPause );
	btnPlayPause->PopEventHandler();
	btnPlayPause->PushEventHandler( pPauseEvt );
	#endif
	btnPlayPause->Refresh();
}

void CNowPlayingCtrl::PauseBtnToPlayBtn()
{
	#ifdef __WXGTK__
	btnPlayPause->SetBitmapLabel( bmPlay );
	btnPlayPause->SetBitmapFocus( bmPlayDown );
	#elif defined __WXMSW__
	btnPlayPause->SetBitmapLabel( bmPlay );
	btnPlayPause->PopEventHandler();
	btnPlayPause->PushEventHandler( pPlayEvt );
	#endif
	btnPlayPause->Refresh();
}

void CNowPlayingCtrl::UpdateTime()
{
	if ( !g_TimeSeeking )
	{
		stCurtime->Freeze();

		float fPos = (float)100* ( (float)g_Player.GetTime( FMOD_SEC ) / (float)g_Player.GetDuration( FMOD_SEC ) );
	
		//-----------------------------------------------------//
		//--- now, if we're in gtk and we set the wxGauge 	---//
		//--- to a value below 2.0, it changes to 100%		---//
		//--- why? who knows. i hope they fix this			---//
		//-----------------------------------------------------//
		#ifdef __WXGTK__
		if ( fPos < 2.0f )
			fPos = 2.0f;
		#endif
		
		gSeek->SetValue( (int)fPos );

		//--- time label ---//
		stCurtime->SetLabel( wxT( " - " ) + g_Player.GetTimeStr() );

		stCurtime->Thaw();
	}
}

void CNowPlayingCtrl::SetTime( wxString sTimeStr )
{
	stCurtime->SetLabel( wxT( " - " ) + sTimeStr );
}

void CNowPlayingCtrl::ResetInfo()
{
	stSong->SetLabel	( _( "Playback Stopped" )	);
	stArtist->SetLabel	( _( "Playback Stopped" )	);
	stCurtime->SetLabel	( _( " - 0:00" )			);
	#ifdef __WXMSW__
		gSeek->SetValue( 0 );
	#elif defined __WXGTK__
		gSeek->SetValue( 1 );
	#endif

	Layout();
}

void CNowPlayingCtrl::UpdateInfo( wxString sFilename )
{
	Freeze();

	//--- first things first, verify data in song ---//
	CMusikSong song;
	g_Library.GetSongFromFilename( sFilename, &song );
	song.Artist = SanitizedString( song.Artist );
	song.Title = SanitizedString( song.Title );
	if ( song.Artist == _( "<unknown>" ) )
		song.Artist = _( "Unknown Artist" );
	if ( song.Title == _( "<unknown> " ) )
		song.Title = _( "Unknown Song" );

	//--- caption bar title ---//
	g_MusikFrame->SetTitle( wxT( "Musik [ " ) + song.Artist + wxT( " - " ) +  song.Title + wxT( " ]" ) );

	//--- title / artist / time -//
	song.Title.Replace	( wxT( "&" ), wxT( "&&" ), TRUE );
	song.Artist.Replace	( wxT( "&" ), wxT( "&&" ), TRUE );

	stSong->SetLabel( song.Title );
	stArtist->SetLabel( song.Artist );
	stCurtime->SetLabel( wxT( " - 0:00 " ) );

	Thaw();

	Layout();
}

void CNowPlayingCtrl::PlayerStop( wxCommandEvent& WXUNUSED(event) )
{	
	g_Player.Stop();
	ResetInfo();
}

void CNowPlayingCtrl::PlayerPlayPause( wxCommandEvent& WXUNUSED(event) )	
{	
	//--- paused, so play ---//
	if ( g_Player.IsPlaying() && !g_Player.IsPaused() )
		g_Player.Pause();

	//--- paused, so resume ---//
	else if ( g_Player.IsPlaying() && g_Player.IsPaused() )
		g_Player.Resume();

	//--- start playing ---//
	else if ( !g_Player.IsPlaying() )
	{
		int nCurSel = g_PlaylistCtrl->GetNextItem( -1, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nCurSel > -1 )
		g_Player.PlayCurSel();
	}
}

void CNowPlayingCtrl::PlayerNext( wxCommandEvent& WXUNUSED(event) )	
{	
	g_Player.NextSong();
}

void CNowPlayingCtrl::PlayerPrev( wxCommandEvent& WXUNUSED(event) )	
{	
	g_Player.PrevSong();
}

void CNowPlayingCtrl::PlayerVolume( wxCommandEvent& WXUNUSED(event) )	
{	
	wxPoint pos = wxGetMousePosition();
	pos.x -= 14;
	pos.y -= 80;
	wxFrame *pDlg = new MusikVolumeFrame( ( wxFrame* )g_MusikFrame, pos );
	pDlg->Show();
}
