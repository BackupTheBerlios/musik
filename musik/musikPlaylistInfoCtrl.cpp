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
#include "musikPrefs.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikArrays.h"
#include "../musikCore/include/musikPlayer.h"

#include "MEMDC.H"
#include ".\musikplaylistinfoctrl.h"

///////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CmusikPlaylistInfoCtrl, CWnd)

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;
	m_ListCtrl = parent;

	m_hBGBitmap = NULL;
}

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::~CmusikPlaylistInfoCtrl()
{
	if ( m_hBGBitmap )
		DeleteObject ( m_hBGBitmap );

	m_ListCtrl->SetInfoCtrl( NULL );
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistInfoCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect( &rect );
	CMemDC memDC( &dc, &rect );

	CFont* oldfont = memDC.SelectObject( &m_Font );

	memDC.FillSolidRect( rect, m_Prefs->MUSIK_COLOR_BTNFACE );
	memDC.Draw3dRect( rect, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

	// draw the bitchin' background
	DrawEQ( memDC.GetSafeHdc() );

	// put the text over it
	SetTextAlign( memDC.GetSafeHdc(), TA_CENTER );
	SetBkMode( memDC.GetSafeHdc(), TRANSPARENT ); 
	memDC.TextOut( rect.Width() / 2, 3, m_strInfo.c_str() );

	// get the old font back
	memDC.SelectObject( oldfont );
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
	if ( red > 30 )
		red -= 30;
	else
		red = 0;
	BYTE green = GetGValue( m_Prefs->MUSIK_COLOR_BTNFACE );
	if ( green > 30 )
		green -= 30;
	else
		green = 0;
	BYTE blue = GetBValue( m_Prefs->MUSIK_COLOR_BTNFACE );
	if ( blue > 30 )
		blue -= 30;
	else
		blue = 0;
	m_EQColor = RGB( red, green, blue );

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


	//UpdateInfo();



	FSOUND_DSP_SetActive( FSOUND_DSP_GetFFTUnit(), TRUE );



	m_TimerID = SetTimer( MUSIK_VIZ_TIMER, 10, NULL );

	return 0;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistInfoCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::UpdateBG()
{
	Invalidate();
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
	
	// fill the bitmap data with the background color
	for ( int n = 0; n < (VIZ_WIDTH * VIZ_HEIGHT); n++ )
		m_pBGBitmapBits[n] = m_Prefs->MUSIK_COLOR_BTNFACE;

	// now draw the viz directly to the bitmap bits

    // we're only going to work with the lower frequencies (0 - 6.5khz) because the higher
	// ones don't really show much
    for ( int x = 0; x < VIZ_WIDTH; x++ )
    {
		// get the "height" of the sample
        int sample_y = min( (int)(spectrum[x] * 4.0f * (float)VIZ_HEIGHT), VIZ_HEIGHT - 1 );
        
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

void CmusikPlaylistInfoCtrl::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == MUSIK_VIZ_TIMER )
	{
		if ( m_Player->IsPlaying() && !m_Player->IsPaused() )
		{
			UpdateBG();
		}
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::UpdateInfo()
{
	if ( m_ListCtrl->GetPlaylist() )
	{
		if ( m_ListCtrl->GetPlaylist()->GetCount() == 0 )
			m_strInfo = "empty playlist";
		else
		{
			size_t runtime = m_ListCtrl->GetPlaylist()->GetRunningTime();
			CTimeSpan span( 0, 0, 0, runtime );
			CString strtime;
			if ( runtime > 172800 )
				strtime = span.Format( "%D days, %H:%M:%S" );
			else if ( runtime > 86400 )
				strtime = span.Format( "%D day, %H:%M:%S" );
			else
				strtime = span.Format( "%H:%M:%S" );
		
			CString strsize( "0.0 mb" );
			double totsize = m_ListCtrl->GetPlaylist()->GetDiskspace();
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
	}
	else
		m_strInfo = "empty playlist";

	Invalidate();
}

///////////////////////////////////////////////////

