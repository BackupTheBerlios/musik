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

#include "MEMDC.H"

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
	ON_COMMAND(ID_EQUALIZER_STATE_16BAND, OnEqualizerState16band)
	ON_COMMAND(ID_EQUALIZER_STATE_8BAND, OnEqualizerState8band)
	ON_COMMAND(ID_EQUALIZER_STATE_4BAND, OnEqualizerState4band)
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
	if ( !m_wndChild->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0), this, 123 ) )
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
	
	int band_state = GetCtrl()->GetBandState();
	if ( band_state == MUSIK_EQUALIZER_CTRL_16BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_16BAND, MF_CHECKED );
	else if ( band_state == MUSIK_EQUALIZER_CTRL_8BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_8BAND, MF_CHECKED );
	else if ( band_state == MUSIK_EQUALIZER_CTRL_4BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_4BAND, MF_CHECKED );
	else
		ASSERT( 0 );

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerPresets()
{
	if ( !m_Presets )
	{
		m_Presets = new CmusikEqualizerSets( this );
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
		GetCtrl()->SetChannelsLocked( false );
	else
		GetCtrl()->SetChannelsLocked( true );

	GetCtrl()->LayoutNewState();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState16band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_16BANDS );
	GetCtrl()->LayoutNewState();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState8band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_8BANDS );
	GetCtrl()->LayoutNewState();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState4band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_4BANDS );
	GetCtrl()->LayoutNewState();
}

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerCtrl, CWnd)

///////////////////////////////////////////////////

CmusikEqualizerCtrl::CmusikEqualizerCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;

	m_ChannelsLocked = true;
	m_BandState = MUSIK_EQUALIZER_CTRL_16BANDS;
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

	for ( size_t i = 0; i < 16; i++ )
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

	return 0;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int width_remaining = ( ( cx - ( 16 * GetBandState() ) ) / GetBandState() );

	CRect curr;
	for ( int i = 0; i < 16; i++ )
	{
		if ( GetBandState() == MUSIK_EQUALIZER_CTRL_16BANDS )
		{
			curr.top = 0;
			curr.bottom = IsChannelsLocked() ? cy : cy / 2;
			curr.left = ( i * 16 ) + ( ( i + 1 ) * width_remaining );
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
		else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_8BANDS )
		{
			if ( i % 2 == 0 )
			{
				curr.top = 0;
				curr.bottom = IsChannelsLocked() ? cy : cy / 2;
				curr.left = ( ( i / 2 ) * 16 ) + ( ( ( i / 2 ) + 1 ) * width_remaining );
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
		else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_4BANDS )
		{
			if ( i % 4 == 0 )
			{
				curr.top = 0;
				curr.bottom = IsChannelsLocked() ? cy : cy / 2;
				curr.left = ( ( i / 4 ) * 16 ) + ( ( ( i / 4 ) + 1 ) * width_remaining );
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
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LayoutNewState()
{
	// if the channels are locked, hide all
	// of the right channel bands...
	if ( IsChannelsLocked() )
	{
		if ( GetBandState() == MUSIK_EQUALIZER_CTRL_16BANDS )
		{
			for ( size_t i = 0; i < 16; i++ )
			{
				m_LeftBands[i].ShowWindow( SW_SHOW );
				m_RightBands[i].ShowWindow( SW_HIDE );
			}
		}
		else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_8BANDS )
		{
			for ( size_t i = 0; i < 16; i++ )
			{
				if ( i % 2 == 0 ) // hide every other band
					m_LeftBands[i].ShowWindow( SW_SHOW );
				else
					m_LeftBands[i].ShowWindow( SW_HIDE );

				m_RightBands[i].ShowWindow( SW_HIDE );
			}
		}
		else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_4BANDS )
		{
			for ( size_t i = 0; i < 16; i++ )
			{
				if ( i % 4 == 0 ) // hide every other band
					m_LeftBands[i].ShowWindow( SW_SHOW );
				else
					m_LeftBands[i].ShowWindow( SW_HIDE );

				m_RightBands[i].ShowWindow( SW_HIDE );
			}
		}
	}

	// channels are unlocked, both bands
	// are visible...
	else
	{
		if ( GetBandState() == MUSIK_EQUALIZER_CTRL_16BANDS )
		{
			for ( size_t i = 0; i < 16; i++ )
			{
				m_LeftBands[i].ShowWindow( SW_SHOW );
				m_RightBands[i].ShowWindow( SW_SHOW );
			}
		}
		else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_8BANDS )
		{
			for ( size_t i = 0; i < 16; i++ )
			{
				if ( i % 2 == 0 )
				{
					m_LeftBands[i].ShowWindow( SW_SHOW );
					m_RightBands[i].ShowWindow( SW_SHOW );
				}
				else
				{
					m_LeftBands[i].ShowWindow( SW_HIDE );
					m_RightBands[i].ShowWindow( SW_HIDE );
				}
			}
		}
		else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_4BANDS )
		{
			for ( size_t i = 0; i < 16; i++ )
			{
				if ( i % 4 == 0 )
				{
					m_LeftBands[i].ShowWindow( SW_SHOW );
					m_RightBands[i].ShowWindow( SW_SHOW );
				}
				else
				{
					m_LeftBands[i].ShowWindow( SW_HIDE );
					m_RightBands[i].ShowWindow( SW_HIDE );
				}
			}
		}
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
	if ( m_Player->IsPlaying() && m_Player->IsEqualizerActive() )
	{
		CmusikEQSettings& curr_song = m_Player->GetEqualizer()->m_EQ_Values;
		
		SetRedraw( FALSE );

		for ( size_t i = 0; i < 16; i++ )
		{
			m_LeftBands[i].SetPos( (int)( curr_song.m_Left[i] * 50.0f ) );
			m_RightBands[i].SetPos( (int)( curr_song.m_Left[i] * 50.0f ) );
		}

		SetRedraw( TRUE );
		RedrawWindow();
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LoadDefault()
{

}

///////////////////////////////////////////////////