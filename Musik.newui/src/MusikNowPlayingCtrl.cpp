#include "wx/wxprec.h"

#include "MusikNowPlayingCtrl.h"

//--- images ---//
#include "images/playback/next.xpm"
#include "images/playback/next_down.xpm"
#include "images/playback/pause.xpm"
#include "images/playback/pause_down.xpm"
#include "images/playback/play.xpm"
#include "images/playback/play_down.xpm"
#include "images/playback/prev.xpm"
#include "images/playback/prev_down.xpm"
#include "images/playback/stop.xpm"
#include "images/playback/stop_down.xpm"
#include "images/playback/volume.xpm"
#include "images/playback/volume_down.xpm"

BEGIN_EVENT_TABLE( CMusikNowPlayingCtrl, wxPanel )

END_EVENT_TABLE()

CMusikNowPlayingCtrl::CMusikNowPlayingCtrl( wxWindow *parent, wxWindowID id )
	: wxPanel( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxTRANSPARENT_WINDOW | wxCLIP_CHILDREN )
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

	//-----------------------------------------//
	//--- initialize aesthetics				---//
	//-----------------------------------------//
	SetupFonts();
	SetupIcons();

	//-----------------------------------------//
	//--- controls							---//
	//-----------------------------------------//
	pLeftPanel		= new wxPanel( this );
	pRightPanel		= new wxPanel( this );

	m_stSong		= new wxStaticText( pLeftPanel, -1, _( "" ),	wxPoint( 0, 0 ), wxSize( 0, 0 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	m_stArtist		= new wxStaticText( pLeftPanel, -1, _( "" ),	wxPoint( 0, 0 ), wxSize( 0, 0 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	m_stCurtime		= new wxStaticText( pLeftPanel, -1, _( "" ),	wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );	

	m_btnPrev		= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_PREV,		m_bmPrev,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	m_btnNext		= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_NEXT,		m_bmNext,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	m_btnPlayPause	= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_PLAYPAUSE,	m_bmPlay,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );	
	m_btnStop		= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_STOP,		m_bmStop,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	m_btnVolume		= new wxBitmapButton( pRightPanel, MUSIK_NOWPLAYINGCTRL_VOLUME,		m_bmVolume,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );

	m_Seek			= new wxGauge( pRightPanel, -1, 100, wxPoint( 0, 0 ), wxSize( 200, wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ) ), wxGA_SMOOTH | wxGA_HORIZONTAL | wxCLIP_CHILDREN );

	//-----------------------------------------//
	//--- set fonts							---//
	//-----------------------------------------//
	m_stSong->SetFont		( m_fntSong );
	m_stArtist->SetFont		( m_fntArtist );
	m_stCurtime->SetFont	( m_fntTime );

	//---------------------------------//
	//--- sizer for artist and time ---//
	//---------------------------------//
	hsArtistTime = new wxBoxSizer( wxHORIZONTAL );
	hsArtistTime->Add( m_stArtist, 0, wxADJUST_MINSIZE | wxALIGN_CENTRE_VERTICAL );
	hsArtistTime->Add( m_stCurtime, 0, wxADJUST_MINSIZE | wxALIGN_CENTRE_VERTICAL );

	//--- song title, artist and time ---//
	vsLeftCol = new wxBoxSizer( wxVERTICAL );
	vsLeftCol->Add( m_stSong, 0, wxADJUST_MINSIZE | wxBOTTOM, 4 );
	vsLeftCol->Add( hsArtistTime, 0, wxEXPAND );

	pLeftPanel->SetSizerAndFit( vsLeftCol );

	//--- buttons, seek bar panel ---//
	hsButtons = new wxBoxSizer( wxHORIZONTAL );
	hsButtons->Add( m_btnPrev );
	hsButtons->Add( m_btnPlayPause );
	hsButtons->Add( m_btnStop );
	hsButtons->Add( m_btnNext );
	hsButtons->Add( m_btnVolume );

	vsRightCol = new wxBoxSizer( wxVERTICAL );
	vsRightCol->Add( hsButtons, 0 );
	vsRightCol->Add( m_Seek, 0, wxTOP, 2 ); //-- small top border --//

	pRightPanel->SetSizerAndFit( vsRightCol );

	hsCols = new wxBoxSizer( wxHORIZONTAL );
	hsCols->Add( pLeftPanel,	0, wxADJUST_MINSIZE | wxALL, 2	);
	hsCols->Add( 0, 0,			1, wxEXPAND);
	hsCols->Add( pRightPanel,	0, wxEXPAND | wxALL, 2	);

	SetSizerAndFit( hsCols );

	ResetInfo();
}

CMusikNowPlayingCtrl::~CMusikNowPlayingCtrl()
{
}

void CMusikNowPlayingCtrl::SetupFonts()
{
	wxString sFace;
	#if defined __WXMSW__
		sFace = wxT( "Tahoma" );

		m_fntSong.SetFaceName	( sFace );
		m_fntSong.SetPointSize	( 14 );

		m_fntArtist.SetFaceName	( sFace );	
		m_fntArtist.SetWeight	( wxBOLD );
		m_fntArtist.SetPointSize( 10 );

		m_fntTime.SetFaceName	( sFace );
		m_fntTime.SetWeight		( wxBOLD );
		m_fntTime.SetPointSize	( 8 );
	#elif defined __WXGTK__
		sFace = wxT( "Sans" );
	
		m_fntSong.SetFaceName	( sFace );
		m_fntSong.SetPointSize	( 14 );

		m_fntArtist.SetFaceName	( sFace );	
		m_fntArtist.SetPointSize( 10 );

		m_fntTime.SetFaceName	( sFace );
		m_fntTime.SetPointSize	( 8 );
	#endif	
}

void CMusikNowPlayingCtrl::SetupIcons()
{
	m_bmPrev		= wxBitmap( prev_xpm );
	m_bmPrevDown	= wxBitmap( prev_down_xpm );
	m_bmNext		= wxBitmap( next_xpm );
	m_bmNextDown	= wxBitmap( next_down_xpm );
	m_bmPlay		= wxBitmap( play_xpm );
	m_bmPlayDown	= wxBitmap( play_down_xpm );
	m_bmPause		= wxBitmap( pause_xpm );
	m_bmPauseDown	= wxBitmap( pause_down_xpm );
	m_bmStop		= wxBitmap( stop_xpm );
	m_bmStopDown	= wxBitmap( stop_down_xpm );
	m_bmVolume		= wxBitmap( volume_xpm );
	m_bmVolumeDown	= wxBitmap( volume_down_xpm );
}

void CMusikNowPlayingCtrl::ResetInfo()
{
	m_stSong->SetLabel		( _( "Playback Stopped" )	);
	m_stArtist->SetLabel	( _( "Playback Stopped" )	);
	m_stCurtime->SetLabel	( _( " - 0:00" )			);

	#ifdef __WXMSW__
		m_Seek->SetValue( 0 );
	#elif defined __WXGTK__
		m_Seek->SetValue( 1 );
	#endif

	Layout();
}