///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikPlaylistInfoCtrl
//
// Filename(s): 
//
//   musikPlaylistInfoCtrl.h, musikPlaylistInfoCtrl.cpp
//
// Information:
//
//   Shows information about playlist, such as number of songs, size, etc.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikPlaylistView.h"
#include "musikPlaylistInfoCtrl.h"
#include "musikPrefs.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPlaylistInfoCtrl, CWnd)

///////////////////////////////////////////////////

int CmusikPlaylistInfoWorker::open( void* parent )
{
	m_Parent = (CmusikPlaylistInfoCtrl*)parent;
	int ret_code = activate( THR_NEW_LWP | THR_JOINABLE | THR_USE_AFX );

	return ret_code;
}

///////////////////////////////////////////////////

int CmusikPlaylistInfoWorker::svc()
{
	// set flags
	m_Stop = false;
	m_Active = true;
	m_Finished = false;

	CmusikPlayer* m_Player = m_Parent->m_Player;
	CmusikPrefs* m_Prefs = m_Parent->m_Prefs;

	// this is the loop.. set it to true from 
	// anywhere and it will abort. threads run
	// very passivly, as to not interfere with 
	// the interface
	while ( !m_Stop )
	{
		if ( m_Player->IsPlaying() && !m_Player->IsPaused() )
		{
			if ( m_Prefs->GetPlaylistInfoVizStyle() != PLAYLIST_INFO_VIZ_STYLE_NONE )
				m_Parent->Invalidate();
		}
		// HEY SIMON DO WORK HERE
		Sleep( 100 );
	}

	// be sure to flag as finished
	m_Finished = true;

	return 0;
}

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;
	m_ListCtrl = parent;

	m_InfoWorker = NULL;
	m_hBGBitmap = NULL;
}

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::~CmusikPlaylistInfoCtrl()
{
	if ( m_hBGBitmap )
		DeleteObject ( m_hBGBitmap );

	if ( m_ListCtrl )
		m_ListCtrl->SetInfoCtrl( NULL );
	
	if ( m_InfoWorker )
	{
		m_InfoWorker->StopWait( 2 );
		delete m_InfoWorker;
		m_InfoWorker = NULL;
	}
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistInfoCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::OnPaint()
{
	m_ProtectingUpdate.acquire();

	CPaintDC dc(this);

	CRect rect;
	GetClientRect( &rect );
	CRect innerrect(rect);
	innerrect.DeflateRect( 1, 1, 1, 1 );
	CMemDC memDC( &dc, &rect );

	memDC.Draw3dRect( rect, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

	// draw the bitchin' background
	int style = m_Prefs->GetPlaylistInfoVizStyle();
	if ( !m_Player->IsPlaying() || m_Player->IsPaused() || style == PLAYLIST_INFO_VIZ_STYLE_NONE )
		memDC.FillSolidRect( innerrect, m_Prefs->MUSIK_COLOR_BTNFACE );	
	else
	{
		switch ( m_Prefs->GetPlaylistInfoVizStyle() )
		{
		case PLAYLIST_INFO_VIZ_STYLE_EQ:
			DrawEQ( memDC.GetSafeHdc() );
			break;
		}
	}	

	// put the text over it
	if ( m_Prefs->PlaylistInfoVisible() )
	{
		// select the proper font
		CFont* oldfont = memDC.SelectObject( &m_Font );

		SetTextAlign( memDC.GetSafeHdc(), TA_CENTER );
		SetBkMode( memDC.GetSafeHdc(), TRANSPARENT ); 
		memDC.TextOut( rect.Width() / 2, 3, m_strInfo.c_str() );

		// get the old font back
		memDC.SelectObject( oldfont );
	}

	m_ProtectingUpdate.release();
}

///////////////////////////////////////////////////

int CmusikPlaylistInfoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	m_Font.CreateFont(-10,0,0,0,FW_BOLD,FALSE,FALSE,
			0,ANSI_CHARSET,OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			FF_SWISS,"Tahoma");
	
	CDC* pDC = GetDC();
	pDC->SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );
	ReleaseDC( pDC );

	// get a slightly darker version of the BTNFACE color
	BYTE red = GetRValue( m_Prefs->MUSIK_COLOR_BTNFACE );
	BYTE green = GetGValue( m_Prefs->MUSIK_COLOR_BTNFACE );
	BYTE blue = GetBValue( m_Prefs->MUSIK_COLOR_BTNFACE );
	m_BGColor = RGB( blue, green, red ); // notice it's swapped
	if ( red > 30 )
		red -= 30;
	else
		red = 0;
	
	if ( green > 30 )
		green -= 30;
	else
		green = 0;
	
	if ( blue > 30 )
		blue -= 30;
	else
		blue = 0;
	m_EQColor = RGB( blue, green, red ); // notice it's swapped

	BITMAPINFO bminfo;	
	ZeroMemory ( &bminfo, sizeof (BITMAPINFO) );
	bminfo.bmiHeader.biWidth  = VIZ_WIDTH;
    bminfo.bmiHeader.biHeight = VIZ_HEIGHT;
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;
    bminfo.bmiHeader.biXPelsPerMeter = 0;
    bminfo.bmiHeader.biYPelsPerMeter = 0;
    bminfo.bmiHeader.biClrUsed = 0;
    bminfo.bmiHeader.biClrImportant = 0;
	bminfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	bminfo.bmiHeader.biSizeImage = VIZ_WIDTH * VIZ_HEIGHT * 4;
	
	m_hBGBitmap = CreateDIBSection ( NULL, &bminfo, DIB_RGB_COLORS, (void **)&m_pBGBitmapBits, NULL, NULL ); 

	FSOUND_DSP_SetActive( FSOUND_DSP_GetFFTUnit(), TRUE );

	// startup task
	m_InfoWorker = new CmusikPlaylistInfoWorker();
	m_InfoWorker->open( (void*)this );

	return 0;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistInfoCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

// draws a visualization of the current sound spectrum
// to a small bitmap, then stretches it to the size of
// the control to paint it

void CmusikPlaylistInfoCtrl::DrawEQ( HDC hdc )
{
	// get an array of 512 floats
    float *spectrum = FSOUND_DSP_GetSpectrum();
    if ( !spectrum )
        return;
	
	// fill with the proper bg color	
	for ( int n = 0; n < (VIZ_WIDTH * VIZ_HEIGHT)-1; n++ )
		m_pBGBitmapBits[n] = m_BGColor;

	// now draw the viz directly to the bitmap bits

    // we're only going to work with the lower frequencies (0 - 6.5khz) because the higher
	// ones don't really show much
    for ( int x = 0; x < VIZ_WIDTH-1; x++ )
    {
		// get the "height" of the sample
        int sample_y = min( (int)(spectrum[x] * 4.0f * (float)VIZ_HEIGHT), (VIZ_HEIGHT - 1) );
        
        for ( int y = 0; y < sample_y; y++ )
        {
			// every other pixel for fun effects
			if ( x % 2 )
			{
				if ( y % 2 )
    				m_pBGBitmapBits[(VIZ_WIDTH * y) + x] = m_EQColor;
			}
        }
    }

    CRect rcClient;
	GetClientRect( &rcClient );

	// create a DC to work with the viz bitmap
	HDC hSrcDC;
	hSrcDC = CreateCompatibleDC( hdc );
	HGDIOBJ hOldBmp = ::SelectObject( hSrcDC, m_hBGBitmap );
	
	// and draw it
	::StretchBlt ( hdc,
				1,					// destination rect
				1,					// '
				rcClient.right-2,	// '
				rcClient.bottom-2,	// '
				hSrcDC,
				0,					// source rect
				0,					// '
				VIZ_WIDTH-1,		// '
				VIZ_HEIGHT-1,		// '
				SRCCOPY );
	

	// restore stuff
	::SelectObject (hSrcDC, hOldBmp);
	::DeleteDC(hSrcDC);
	
}

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::UpdateInfo()
{
	if ( m_ListCtrl->GetPlaylist() && m_ListCtrl->GetPlaylist()->GetCount() != 0 )
	{
		m_strInfo.Format( "number of songs: %d    (calculating...)", m_ListCtrl->GetPlaylist()->GetCount() );
		m_ListCtrl->RedrawWindow();

		size_t runtime = m_ListCtrl->GetPlaylist()->GetTotalTime();
		double totsize = m_ListCtrl->GetPlaylist()->GetTotalSize();
		
		CTimeSpan span( 0, 0, 0, runtime );
		CString strtime;
		if ( runtime > 172800 )
			strtime = span.Format( "%D days, %H:%M:%S" );
		else if ( runtime > 86400 )
			strtime = span.Format( "%D day, %H:%M:%S" );
		else
			strtime = span.Format( "%H:%M:%S" );
	
		CString strsize( "0.0 mb" );
		if ( totsize < 1024.0 )
			strsize.Format( "%.2f b", totsize );
		else if ( totsize < ( 1024.0 * 1024.0 ) )
			strsize.Format( "%.2f kb", totsize / 1024.0 );
		else if ( totsize < ( 1024.0 * 1024.0 * 1024.0 ) )
			strsize.Format( "%.2f mb", totsize / 1024.0 / 1024.0 );
		else if ( totsize < ( 1024.0 * 1024.0 * 1024.0 * 1024.0 ) )
			strsize.Format( "%.2f gb", totsize / 1024.0 / 1024.0 / 1024.0 );

		m_strInfo.Format( "number of songs: %d    running time: %s    size: %s", m_ListCtrl->GetPlaylist()->GetCount(), strtime, strsize );
	}
	else
		m_strInfo = "empty playlist";

	Invalidate();
}

///////////////////////////////////////////////////

