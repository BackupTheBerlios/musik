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
	: wxPanel( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxTRANSPARENT_WINDOW )
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

	//-----------------------------------------------------//
	//--- initialize aesthetics							---//
	//-----------------------------------------------------//
	SetupFonts();
	SetupIcons();

	//-----------------------------------------------------//
	//--- controls										---//
	//-----------------------------------------------------//
	m_stSong		= new wxStaticText( this, -1, _( "" ),	wxPoint( 0, 0 ), wxSize( 0, 0 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	m_stArtist		= new wxStaticText( this, -1, _( "" ),	wxPoint( 0, 0 ), wxSize( 0, 0 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );
	m_stCurtime		= new wxStaticText( this, -1, _( "" ),	wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_LEFT | wxTRANSPARENT_WINDOW );	

	m_btnPrev		= new wxBitmapButton( this, MUSIK_NOWPLAYINGCTRL_PREV,			m_bmPrev,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	m_btnNext		= new wxBitmapButton( this, MUSIK_NOWPLAYINGCTRL_NEXT,			m_bmNext,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	m_btnPlayPause	= new wxBitmapButton( this, MUSIK_NOWPLAYINGCTRL_PLAYPAUSE,		m_bmPlay,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );	
	m_btnStop		= new wxBitmapButton( this, MUSIK_NOWPLAYINGCTRL_STOP,			m_bmStop,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );
	m_btnVolume		= new wxBitmapButton( this, MUSIK_NOWPLAYINGCTRL_VOLUME,		m_bmVolume,	wxPoint( -1, -1 ), wxSize( 40, 20 ), 0 );

	m_Seek			= new CMusikSeekCtrl( this, -1 );

	ResetInfo( false );

	//-----------------------------------------------------//
	//--- set fonts										---//
	//-----------------------------------------------------//
	m_stSong->SetFont		( m_fntSong );
	m_stArtist->SetFont		( m_fntArtist );
	m_stCurtime->SetFont	( m_fntTime );

	//-----------------------------------------------------//
	//--- sizer for artist and time						---//
	//-----------------------------------------------------//
	m_ArtistSizer = new wxBoxSizer( wxHORIZONTAL );
	m_ArtistSizer->Add( m_stArtist, 0, wxADJUST_MINSIZE | wxALIGN_CENTRE_VERTICAL );
	m_ArtistSizer->Add( m_stCurtime, 0, wxADJUST_MINSIZE | wxALIGN_CENTRE_VERTICAL );

	//-----------------------------------------------------//
	//--- sizer for buttons and gauge					---//
	//-----------------------------------------------------//
	m_ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	m_ButtonSizer->Add( m_btnPrev );
	m_ButtonSizer->Add( m_btnPlayPause );
	m_ButtonSizer->Add( m_btnStop );
	m_ButtonSizer->Add( m_btnNext );
	m_ButtonSizer->Add( m_btnVolume );

	//-----------------------------------------------------//
	//--- sizer for top									---//
	//-----------------------------------------------------//
	m_TopSizer = new wxBoxSizer( wxHORIZONTAL );
	m_TopSizer->Add( m_stSong, 0, wxADJUST_MINSIZE | wxALIGN_CENTER_VERTICAL );
	m_TopSizer->Add( 0, 0,	1, wxEXPAND);
	m_TopSizer->Add( m_ButtonSizer, 0, wxALIGN_CENTER_VERTICAL );

	//-----------------------------------------------------//
	//--- sizer for bottom								---//
	//-----------------------------------------------------//
	m_BottomSizer = new wxBoxSizer( wxHORIZONTAL );
	m_BottomSizer->Add( m_ArtistSizer, 0, wxADJUST_MINSIZE | wxALIGN_CENTER_VERTICAL );
	m_BottomSizer->Add( 0, 0,	1, wxEXPAND);
	m_BottomSizer->Add( m_Seek, 0 );

	//-----------------------------------------------------//
    //--- main sizer									---//
	//-----------------------------------------------------//
	m_MainSizer = new wxBoxSizer( wxVERTICAL );
	m_MainSizer->Add( m_TopSizer,		0, wxEXPAND | wxALL, 2	);
	m_MainSizer->Add( 0, 0,				1, wxEXPAND);
	m_MainSizer->Add( m_BottomSizer,	0, wxEXPAND | wxALL, 2	);

	SetSizerAndFit( m_MainSizer );
	Layout();
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

void CMusikNowPlayingCtrl::ResetInfo( bool layout )
{
	m_stSong->SetLabel		( _( "Playback Stopped" )	);
	m_stArtist->SetLabel	( _( "Playback Stopped" )	);
	m_stCurtime->SetLabel	( _( " - 0:00" )			);

	#ifdef __WXMSW__
		m_Seek->SetValue( 0 );
	#elif defined __WXGTK__
		m_Seek->SetValue( 1 );
	#endif

	if ( layout )
		Layout();
}