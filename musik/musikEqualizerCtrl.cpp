///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"

#include "musikEqualizerCtrl.h"
#include "musikTrackCtrl.h"
#include "musikPrefs.h"
#include "musikEqualizerSets.h"

#include "../musikCore/include/musikEqualizer.h"
#include "../musikCore/include/musikEQSettings.h"
#include "../musikCore/include/musikPlayer.h"
#include "../musikCore/include/musikLibrary.h"

#include "MEMDC.H"
#include ".\musikequalizerctrl.h"

///////////////////////////////////////////////////

// CmusikEqualizerBar

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerBar, baseCmusikEqualizerBar)

///////////////////////////////////////////////////

int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage( "CLOSEEQUALIZERPRESETS" );

///////////////////////////////////////////////////

CmusikEqualizerBar::CmusikEqualizerBar( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikEqualizerCtrl( library, player, prefs );
	m_Presets = NULL;
}

///////////////////////////////////////////////////

CmusikEqualizerBar::~CmusikEqualizerBar()
{
	OnClosePresets( NULL, NULL );
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerBar, baseCmusikEqualizerBar)
	// mfc maps
	ON_WM_CREATE()
	ON_WM_SIZE()

	// custom messages
	ON_REGISTERED_MESSAGE( WM_CLOSEEQUALIZERPRESETS, OnClosePresets )

	// menu commands
	ON_COMMAND(ID_EQUALIZER_PRESETS, OnEqualizerPresets)
	ON_COMMAND(ID_EQUALIZER_LOCKCHANNELS, OnEqualizerLockchannels)
	ON_COMMAND(ID_EQUALIZER_STATE_18BAND, OnEqualizerState18band)
	ON_COMMAND(ID_EQUALIZER_STATE_6BAND, OnEqualizerState6band)
	ON_COMMAND(ID_EQUALIZER_SET_AS_DEFAULT, OnEqualizerSetAsDefault)
	ON_COMMAND(ID_EQUALIZER_RESETDEFAULT, OnEqualizerResetdefault)
	ON_COMMAND(ID_EQUALIZER_AUTOMATICALLYSAVE, OnEqualizerAutomaticallySave)
	ON_COMMAND(ID_EQUALIZER_ENABLED2, OnEqualizerEnabled)
	ON_COMMAND(ID_EQUALIZER_CLOSE_VIEW, OnEqualizerCloseView)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikEqualizerBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikEqualizerBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_ANY );

	// child
	if ( !m_wndChild->Create( NULL, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0), this, 123 ) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnOptions()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_EQUALIZER_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// update ui does not work becuase the
	// menu does not belong to a CView or
	// CFrameWnd inherited class, so this
	// updates the menu items...
	popup_menu->CheckMenuItem( ID_EQUALIZER_LOCKCHANNELS, GetCtrl()->IsChannelsLocked() ? MF_CHECKED : MF_UNCHECKED );
	popup_menu->EnableMenuItem( ID_EQUALIZER_PRESETS, ( m_Presets == 0 ) ? MF_ENABLED : MF_GRAYED | MF_DISABLED );
	popup_menu->CheckMenuItem( ID_EQUALIZER_AUTOMATICALLYSAVE, ( m_Prefs->IsEqualizerAutoManaged() == true ) ? MF_CHECKED : MF_UNCHECKED );
	popup_menu->CheckMenuItem( ID_EQUALIZER_ENABLED2, ( m_Prefs->IsEqualizerEnabled() == true ) ? MF_CHECKED : MF_UNCHECKED );

	int band_state = GetCtrl()->GetBandState();
	if ( band_state == MUSIK_EQUALIZER_CTRL_18BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_18BAND, MF_CHECKED );
	else if ( band_state == MUSIK_EQUALIZER_CTRL_6BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_6BAND, MF_CHECKED );
	else
		ASSERT( 0 );

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
} 

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerPresets()
{
	if ( !m_Presets )
	{
		m_Presets = new CmusikEqualizerSets( this, GetCtrl()->m_Library );
		m_Presets->Create( IDD_EQUALIZER_SETS, this );
		m_Presets->ShowWindow( SW_SHOW );
	}	
}

///////////////////////////////////////////////////

LRESULT CmusikEqualizerBar::OnClosePresets( WPARAM wParam, LPARAM lParam )
{
	if ( m_Presets )
	{
		delete m_Presets;
		m_Presets = NULL;
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerLockchannels()
{
	if ( GetCtrl()->IsChannelsLocked() )
	{
		GetCtrl()->SetChannelsLocked( false );
		m_Prefs->SetEqualizerChannelsLocked( false );
	}
	else
	{
		GetCtrl()->SetChannelsLocked( true );
		m_Prefs->SetEqualizerChannelsLocked( true );
	}

	GetCtrl()->LayoutNewState();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState18band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_18BANDS );
	GetCtrl()->LayoutNewState();
	GetCtrl()->DisplayChanged();

	m_Prefs->SetEqualizerBandState( MUSIK_EQUALIZER_CTRL_18BANDS );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState6band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_6BANDS );
	GetCtrl()->LayoutNewState();
	GetCtrl()->DisplayChanged();

	m_Prefs->SetEqualizerBandState( MUSIK_EQUALIZER_CTRL_6BANDS );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerSetAsDefault()
{
	GetCtrl()->SetAsDefault();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerResetdefault()
{
	CmusikEQSettings settings;
	GetCtrl()->ResetDefault();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerAutomaticallySave()
{
	if ( m_Prefs->IsEqualizerAutoManaged() )
		m_Prefs->SetEqualizerAutoManaged( false );
	else
		m_Prefs->SetEqualizerAutoManaged( true );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerEnabled()
{
	GetParent()->SendMessage( WM_COMMAND, ID_AUDIO_EQUALIZER_ENABLED );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerCloseView()
{
	CFrameWnd* parent = (CFrameWnd*)GetParent();
	parent->ShowControlBar( this, FALSE, FALSE );
}

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerCtrl, CWnd)

///////////////////////////////////////////////////

int WM_BANDCHANGE = RegisterWindowMessage( "TRACKDRAGFINISH" );

///////////////////////////////////////////////////

CmusikEqualizerCtrl::CmusikEqualizerCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;

	m_ChannelsLocked = true;
	m_BandState = MUSIK_EQUALIZER_CTRL_18BANDS;
}

///////////////////////////////////////////////////

CmusikEqualizerCtrl::~CmusikEqualizerCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_BANDCHANGE, OnBandChange)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, m_Prefs->MUSIK_COLOR_BTNFACE );

	DefWindowProc( WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0 );
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

int CmusikEqualizerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	for ( size_t i = 0; i < 18; i++ )
	{
		m_LeftBands[i].Create( WS_CHILD | WS_VISIBLE | TBS_VERT, CRect( 0, 0, 0, 0 ), this, 123 );
		m_LeftBands[i].m_Prefs = m_Prefs;
		m_LeftBands[i].SetRange( 0, 100 );
		m_LeftBands[i].SetPos( 50 );

		m_RightBands[i].Create( WS_CHILD | WS_VISIBLE | TBS_VERT, CRect( 0, 0, 0, 0 ), this, 123 );
		m_RightBands[i].m_Prefs = m_Prefs;
		m_RightBands[i].SetRange( 0, 100 );
		m_RightBands[i].SetPos( 50 );
	}	

	LoadDefault();

	return 0;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int width_remaining = ( ( cx - ( 16 * ( GetBandState() ) ) ) / GetBandState() );

	CRect curr;
	int space;
	if ( GetBandState() == MUSIK_EQUALIZER_CTRL_18BANDS )
	{
		for ( int i = 0; i < 18; i++ )
		{
			if ( i == 0 )
				space = ( width_remaining / 2 );
			else
				space = ( width_remaining / 2 ) + ( i * width_remaining );

			curr.top = 0;
			curr.bottom = IsChannelsLocked() ? cy : cy / 2;
			curr.left = ( i * 16 ) + space;
			curr.right = curr.left + 16;

			m_LeftBands[i].MoveWindow( curr );

			// if the channels are locked, these bands
			// will be hidden...
			if ( !IsChannelsLocked() )
			{
				curr.top = curr.bottom;
				curr.bottom = cy;

				m_RightBands[i].MoveWindow( curr );
			}
		}
	}

	else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS )
	{
		int band_at;
		for ( size_t i = 0; i < 6; i++ )
		{
			switch( i )
			{
			case 0:
				band_at = 0;
				break;
			case 1:
				band_at = 3;
				break;
			case 2:
				band_at = 7;
				break;
			case 3: 
				band_at = 10;
				break;
			case 4:
				band_at = 14;
				break;
			case 5:
				band_at = 17;
				break;
			}

			if ( i == 0 )
				space = ( width_remaining / 2 );
			else
				space = ( width_remaining / 2 ) + ( i * width_remaining );

			curr.top = 0;
			curr.bottom = IsChannelsLocked() ? cy : cy / 2;
			curr.left = ( ( i ) * 16 ) + space;
			curr.right = curr.left + 16;

			m_LeftBands[band_at].MoveWindow( curr );

			// if the channels are locked, these bands
			// will be hidden...
			if ( !IsChannelsLocked() )
			{
				curr.top = curr.bottom;
				curr.bottom = cy;

				m_RightBands[band_at].MoveWindow( curr );
			}
		}
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LayoutNewState()
{
	if ( GetBandState() == MUSIK_EQUALIZER_CTRL_18BANDS )
	{
		if ( IsChannelsLocked() )
		{
			for ( size_t i = 0; i < 18; i++ )
			{
				m_LeftBands[i].ShowWindow( SW_SHOW );
				m_RightBands[i].ShowWindow( SW_HIDE );
			}
		}
		else
		{
			for ( size_t i = 0; i < 18; i++ )
			{
				m_LeftBands[i].ShowWindow( SW_SHOW );
				m_RightBands[i].ShowWindow( SW_SHOW );
			}
		}
	}

	else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS )
	{
		m_LeftBands[0].ShowWindow( SW_SHOW );
		m_LeftBands[1].ShowWindow( SW_HIDE );
		m_LeftBands[2].ShowWindow( SW_HIDE );
		m_LeftBands[3].ShowWindow( SW_SHOW );
		m_LeftBands[4].ShowWindow( SW_HIDE );
		m_LeftBands[5].ShowWindow( SW_HIDE );
		m_LeftBands[6].ShowWindow( SW_HIDE );
		m_LeftBands[7].ShowWindow( SW_SHOW );
		m_LeftBands[8].ShowWindow( SW_HIDE );
		m_LeftBands[9].ShowWindow( SW_HIDE );
		m_LeftBands[10].ShowWindow( SW_SHOW );
		m_LeftBands[11].ShowWindow( SW_HIDE );
		m_LeftBands[12].ShowWindow( SW_HIDE );
		m_LeftBands[13].ShowWindow( SW_HIDE );
		m_LeftBands[14].ShowWindow( SW_SHOW );
		m_LeftBands[15].ShowWindow( SW_HIDE );
		m_LeftBands[16].ShowWindow( SW_HIDE );
		m_LeftBands[17].ShowWindow( SW_SHOW );

		m_RightBands[0].ShowWindow( !IsChannelsLocked() ? SW_SHOW : SW_HIDE );
		m_RightBands[1].ShowWindow( SW_HIDE );
		m_RightBands[2].ShowWindow( SW_HIDE );
		m_RightBands[3].ShowWindow( !IsChannelsLocked() ? SW_SHOW : SW_HIDE );
		m_RightBands[4].ShowWindow( SW_HIDE );
		m_RightBands[5].ShowWindow( SW_HIDE );
		m_RightBands[6].ShowWindow( SW_HIDE );
		m_RightBands[7].ShowWindow( !IsChannelsLocked() ? SW_SHOW : SW_HIDE );
		m_RightBands[8].ShowWindow( SW_HIDE );
		m_RightBands[9].ShowWindow( SW_HIDE );
		m_RightBands[10].ShowWindow( !IsChannelsLocked() ? SW_SHOW : SW_HIDE );
		m_RightBands[11].ShowWindow( SW_HIDE );
		m_RightBands[12].ShowWindow( SW_HIDE );
		m_RightBands[13].ShowWindow( SW_HIDE );
		m_RightBands[14].ShowWindow( !IsChannelsLocked() ? SW_SHOW : SW_HIDE );
		m_RightBands[15].ShowWindow( SW_HIDE );
		m_RightBands[16].ShowWindow( SW_HIDE );
		m_RightBands[17].ShowWindow( !IsChannelsLocked() ? SW_SHOW : SW_HIDE );
	}

	CRect rcClient;
	GetClientRect( &rcClient );
	OnSize( NULL, rcClient.Width(), rcClient.Height() );
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetChannelsLocked( bool locked )
{
	m_ChannelsLocked = locked;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetBandState( int state )
{
	m_BandState = state;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LoadCurrSong()
{
	if ( m_Player->IsPlaying() && m_Player->IsEqualizerActive() && m_Prefs->IsEqualizerAutoManaged() )
		SetBandsFrom( m_Player->GetEqualizer()->m_EQ_Values );
}


///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LoadDefault()
{
	if ( m_Player->IsEqualizerActive() )
	{
		m_Library->GetDefaultEqualizer( &m_Player->GetEqualizer()->m_EQ_Values );
		m_Player->GetEqualizer()->UpdateTable();

		SetBandsFrom( m_Player->GetEqualizer()->m_EQ_Values );
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetBandsFrom( const CmusikEQSettings& settings )
{
	int left = 0;
	int right = 0;
	for ( size_t i = 0; i < 18; i++ )
	{
		if ( IsChannelsLocked() )
		{
			left = right = 100 - (int)( settings.m_Left[i] * 50.0f );

			m_LeftBands[i].SetPos( left );
			m_RightBands[i].SetPos( right );
		}
		else
		{
			left = 100 - (int)( settings.m_Left[i] * 50.0f );
			right = 100 - (int)( settings.m_Right[i] * 50.0f );

			m_LeftBands[i].SetPos( left );
			m_RightBands[i].SetPos( right );
		}
	}

	Invalidate();
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::BandsToEQSettings( CmusikEQSettings* settings )
{
	float left_chan[18];
	float right_chan[18];

	for ( size_t i = 0; i < 18; i++ )
	{
		if ( IsChannelsLocked() )
		{
			left_chan[i] = right_chan[i] = (float)( ( 100.0f - (float)m_LeftBands[i].GetPos() ) / 50.0f );
		}
		else
		{
			left_chan[i] = (float)( ( 100.0f - (float)m_LeftBands[i].GetPos() ) / 50.0f );
			right_chan[i] = (float)( ( 100.0f - m_RightBands[i].GetPos() ) / 50.0f );
		}
	}

	// must interpolate
	if ( GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS )
	{
		float chan_set_l[6];
		float chan_set_r[6];

		if ( IsChannelsLocked() )
		{
			chan_set_l[0] = chan_set_r[0] = (float)( ( 100.0f - (float)m_LeftBands[0].GetPos() ) / 50.0f );
			chan_set_l[1] = chan_set_r[1] = (float)( ( 100.0f - (float)m_LeftBands[3].GetPos() ) / 50.0f );
			chan_set_l[2] = chan_set_r[2] = (float)( ( 100.0f - (float)m_LeftBands[7].GetPos() ) / 50.0f );
			chan_set_l[3] = chan_set_r[3] = (float)( ( 100.0f - (float)m_LeftBands[10].GetPos() ) / 50.0f );
			chan_set_l[4] = chan_set_r[4] = (float)( ( 100.0f - (float)m_LeftBands[14].GetPos() ) / 50.0f );
			chan_set_l[5] = chan_set_r[5] = (float)( ( 100.0f - (float)m_LeftBands[17].GetPos() ) / 50.0f );
		
		}
		else
		{
			chan_set_l[0] = (float)( ( 100.0f - (float)m_LeftBands[0].GetPos() ) / 50.0f );
			chan_set_r[0] = (float)( ( 100.0f - (float)m_RightBands[0].GetPos() ) / 50.0f );
			chan_set_l[1] = (float)( ( 100.0f - (float)m_LeftBands[3].GetPos() ) / 50.0f );
			chan_set_r[1] = (float)( ( 100.0f - (float)m_RightBands[3].GetPos() ) / 50.0f );
			chan_set_l[2] = (float)( ( 100.0f - (float)m_LeftBands[7].GetPos() ) / 50.0f );
			chan_set_r[2] = (float)( ( 100.0f - (float)m_RightBands[7].GetPos() ) / 50.0f );
			chan_set_l[3] = (float)( ( 100.0f - (float)m_LeftBands[10].GetPos() ) / 50.0f );
			chan_set_r[3] = (float)( ( 100.0f - (float)m_RightBands[10].GetPos() ) / 50.0f );
			chan_set_l[4] = (float)( ( 100.0f - (float)m_LeftBands[14].GetPos() ) / 50.0f );
			chan_set_r[4] = (float)( ( 100.0f - (float)m_RightBands[14].GetPos() ) / 50.0f );
			chan_set_l[5] = (float)( ( 100.0f - (float)m_LeftBands[17].GetPos() ) / 50.0f );
			chan_set_r[5] = (float)( ( 100.0f - (float)m_RightBands[17].GetPos() ) / 50.0f );
		}

		// interpolate the 12 missing bands
		left_chan[0]	= chan_set_l[0];
		left_chan[1]	= left_chan[0]  + ( ( chan_set_l[1] - chan_set_l[0] ) / 2.0f );
		left_chan[2]	= left_chan[1]  + ( ( chan_set_l[1] - chan_set_l[0] ) / 2.0f );
		left_chan[3]	= chan_set_l[1];
		left_chan[4]	= left_chan[3]  + ( ( chan_set_l[2] - chan_set_l[1] ) / 3.0f );
		left_chan[5]	= left_chan[4]  + ( ( chan_set_l[2] - chan_set_l[1] ) / 3.0f );
		left_chan[6]	= left_chan[5]  + ( ( chan_set_l[2] - chan_set_l[1] ) / 3.0f );
		left_chan[7]	= chan_set_l[2];					
		left_chan[8]	= left_chan[7]  + ( ( chan_set_l[3] - chan_set_l[2] ) / 2.0f );		
		left_chan[9]	= left_chan[8]  + ( ( chan_set_l[3] - chan_set_l[2] ) / 2.0f );							
		left_chan[10]	= chan_set_l[3];
		left_chan[11]	= left_chan[10] + ( ( chan_set_l[4] - chan_set_l[3] ) / 3.0f );
		left_chan[12]	= left_chan[11] + ( ( chan_set_l[4] - chan_set_l[3] ) / 3.0f );
		left_chan[13]	= left_chan[12] + ( ( chan_set_l[4] - chan_set_l[3] ) / 3.0f );
		left_chan[14]	= chan_set_l[4];
		left_chan[15]	= left_chan[14] + ( ( chan_set_l[5] - chan_set_l[4] ) / 2.0f );
		left_chan[16]	= left_chan[15] + ( ( chan_set_l[5] - chan_set_l[4] ) / 2.0f );
		left_chan[17]	= chan_set_l[5];

		right_chan[0]	= chan_set_r[0];
		right_chan[1]	= right_chan[0]  + ( ( chan_set_r[1] - chan_set_r[0] ) / 2.0f );
		right_chan[2]	= right_chan[1]  + ( ( chan_set_r[1] - chan_set_r[0] ) / 2.0f );
		right_chan[3]	= chan_set_r[1];
		right_chan[4]	= right_chan[3]  + ( ( chan_set_r[2] - chan_set_r[1] ) / 3.0f );
		right_chan[5]	= right_chan[4]  + ( ( chan_set_r[2] - chan_set_r[1] ) / 3.0f );
		right_chan[6]	= right_chan[5]  + ( ( chan_set_r[2] - chan_set_r[1] ) / 3.0f );
		right_chan[7]	= chan_set_r[2];					
		right_chan[8]	= right_chan[7]  + ( ( chan_set_r[3] - chan_set_r[2] ) / 2.0f );		
		right_chan[9]	= right_chan[8]  + ( ( chan_set_r[3] - chan_set_r[2] ) / 2.0f );							
		right_chan[10]	= chan_set_r[3];
		right_chan[11]	= right_chan[10] + ( ( chan_set_r[4] - chan_set_r[3] ) / 3.0f );
		right_chan[12]	= right_chan[11] + ( ( chan_set_r[4] - chan_set_r[3] ) / 3.0f );
		right_chan[13]	= right_chan[12] + ( ( chan_set_r[4] - chan_set_r[3] ) / 3.0f );
		right_chan[14]	= chan_set_r[4];
		right_chan[15]	= right_chan[14] + ( ( chan_set_r[5] - chan_set_r[4] ) / 2.0f );
		right_chan[16]	= right_chan[15] + ( ( chan_set_r[5] - chan_set_r[4] ) / 2.0f );
		right_chan[17]	= chan_set_r[5];
	}

	settings->Set( MUSIK_EQ_SETTINGS_LEFT_BAND, left_chan );
	settings->Set( MUSIK_EQ_SETTINGS_RIGHT_BAND, right_chan );
}

///////////////////////////////////////////////////

LRESULT CmusikEqualizerCtrl::OnBandChange( WPARAM wParam, LPARAM lParam )
{
	if ( m_Player->IsEqualizerActive() )
	{
		BandsToEQSettings( &m_Player->GetEqualizer()->m_EQ_Values );

		m_Player->GetEqualizer()->UpdateTable();
		m_Player->GetEqualizer()->m_EQ_Values_Modified = true;
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetAsDefault()
{
	CmusikEQSettings curr_eq;
	BandsToEQSettings( &curr_eq );
	m_Library->UpdateDefaultEqualizer( curr_eq );
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::DisplayChanged()
{
	if ( m_Player->IsEqualizerActive() )
		SetBandsFrom( m_Player->GetEqualizer()->m_EQ_Values );
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::ResetDefault()
{
	if ( m_Player->IsEqualizerActive() )
	{
		// get default
		CmusikEQSettings reset;
		SetBandsFrom( reset );

		// send to player
		m_Player->GetEqualizer()->m_EQ_Values = reset;
		m_Player->GetEqualizer()->UpdateTable();

		// save to library
		m_Library->UpdateDefaultEqualizer( reset );
	}
}

///////////////////////////////////////////////////
