///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikNowPlayingCtrl.h"

#include "../Musik.Core/include/MusikPlayer.h"

#include "Memdc.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikNowPlayingCtrl, CWnd)

///////////////////////////////////////////////////

CMusikNowPlayingCtrl::CMusikNowPlayingCtrl( CMusikPlayer* player )
{
	m_Player = player;
}

///////////////////////////////////////////////////

CMusikNowPlayingCtrl::~CMusikNowPlayingCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, GetSysColor( COLOR_BTNFACE ) );

	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

int CMusikNowPlayingCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitFonts();


	m_Title.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect( 0, 0, 0, 0 ), this );
	m_Title.SetFont( &m_TitleFont );
	m_Title.SetDynText( _T( "No Song" ) );

	m_By.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect( 0, 0, 0, 0 ), this );
	m_By.SetFont( &m_DefaultFont );
	m_By.SetDynText( _T( " by " ) );

	m_Artist.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect( 0, 0, 0, 0 ), this );
	m_Artist.SetFont( &m_ArtistFont );
	m_Artist.SetDynText( _T( "No Artist" ) );

	m_From.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect( 0, 0, 0, 0 ), this );
	m_From.SetFont( &m_DefaultFont );
	m_From.SetDynText( _T( " from " ) );

	m_Album.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect( 0, 0, 0, 0 ), this );
	m_Album.SetFont( &m_AlbumFont );
	m_Album.SetDynText( _T( "No Album" ) );

	return 0;
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::Layout( bool redraw )
{
	CString str;
	CRect size;
	
	// get client rect
	CRect rcClient;
	GetClientRect( rcClient );

	// move artist to correct place
	m_Title.GetClientRect( &size );
    m_Title.MoveWindow( rcClient.left + 2, rcClient.top + 2, size.Width(), size.Height() );

	// move the "by"
	m_By.GetClientRect( &size );
	m_By.MoveWindow( rcClient.left + 2, rcClient.top + 28, size.Width(), size.Height() );

	// move artist 
	int last_x = size.right + 2;
	m_Artist.GetClientRect( &size );
	m_Artist.MoveWindow( last_x + 2, rcClient.top + 26, size.Width(), size.Height() );

	// move the "from"
	last_x += size.right + 2;
	m_From.GetClientRect( &size );
	m_From.MoveWindow( last_x + 2, rcClient.top + 28, size.Width(), size.Height() );

	// move the album
	last_x += size.right + 2;
	m_Album.GetClientRect( &size );
	m_Album.MoveWindow( last_x + 2, rcClient.top + 26, size.Width(), size.Height() );

	if ( redraw )
	{
		CRect rcClient;
		GetClientRect( rcClient );
		this->RedrawWindow( rcClient, NULL );
	}
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::InitFonts()
{
	m_DefaultFont.CreateStockObject( DEFAULT_GUI_FONT );

	LOGFONT pBoldFont;
	m_DefaultFont.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;
	pBoldFont.lfHeight = 20;

	m_TitleFont.CreateFontIndirect( &pBoldFont );

	pBoldFont.lfHeight = 16;
	m_ArtistFont.CreateFontIndirect( &pBoldFont );
	m_AlbumFont.CreateFontIndirect( &pBoldFont );

	SetFont( &m_DefaultFont );
}

///////////////////////////////////////////////////

void CMusikNowPlayingCtrl::UpdateInfo()
{
	CString str;

	// update title
	str = (CString)m_Player->GetCurrPlaying()->GetTitle();
	str.Replace( _T( "&" ), _T( "&&" ) );
	m_Title.SetDynText( str );

	// artist
	str = (CString)m_Player->GetCurrPlaying()->GetArtist();
	str.Replace( _T( "&" ), _T( "&&" ) );
	m_Artist.SetDynText( str );

	// album
	str = (CString)m_Player->GetCurrPlaying()->GetAlbum();
	str.Replace( _T( "&" ), _T( "&&" ) );
	m_Album.SetDynText( str );

	// redraw relevent windows
	Layout();
}

///////////////////////////////////////////////////
