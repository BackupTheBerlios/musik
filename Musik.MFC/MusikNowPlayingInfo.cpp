///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikDynamicTextArray,
//   CMusikFontBaseline,
//   CMusikNowPlayingInfo
//   
//
// Filename(s): 
//
//   MusikNowPlayingInfo.h,
//   MusikNowPlayingInfo.cpp
//
// Information:
//
//   A single line of now playing text that accepts
//   a mask for formatting, including font attributes
//   like size, bold, and italic.
//
// Usage: 
//
//	 Use Set() to set the mask, that should be
//   it. Follow the following guidelines:
//
//     %a [id]    <-- attribute, such as title, artist, album, etc
//     %f [size]  <-- regular font
//     %b [size]  <-- bold font
//     %i [size]  <-- italic font
//     %m [size]  <-- bold and italic font
//     %c [str]   <-- comment
//
// Example: 
//
//   m_Info->Set( _T( "%b20 %a4 %c from the album %a1" ) );
//
//      %b20 initializes the font to 20 with a bold attribute
//      %a4 prints the current song's title
//      %c prints the comment "from the album" using default font
//      %a1 prints the album name, with the previously specified font  
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//
//   Casey Langen   : Lead Developer, Project Manager
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
// 
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"

#include "MEMDC.H"

#include "MusikPrefs.h"
#include "MusikNowPlayingInfo.h"

#include "../Musik.Core/include/MusikPlayer.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikNowPlayingInfo, CWnd)

///////////////////////////////////////////////////

CMusikNowPlayingInfo::CMusikNowPlayingInfo( CMusikPlayer* player, CMusikPrefs *prefs )
{
	m_Player = player;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CMusikNowPlayingInfo::~CMusikNowPlayingInfo()
{
	for ( size_t i = 0; i < m_Captions.size(); i++ )
		delete m_Captions.at( i );

	for ( size_t i = 0; i < m_Comments.size(); i++ )
		delete m_Comments.at( i );
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikNowPlayingInfo, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::UpdateInfo( bool refresh )
{
	SetRedraw( false );
	int type;
	for ( size_t i = 0; i < m_Captions.size(); i++ )
	{
		type = m_Captions.at( i )->GetType();
		m_Captions.at( i )->SetDynText( (CString)m_Player->GetCurrPlaying()->GetField( type ) );		
	}
	SetRedraw( true );

	Layout( refresh );
}

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::Layout( bool refresh )
{
	SetRedraw( false );

	CSize rect = GetSize();

	CMusikFontBaseline blLargest, blCurrent;
	GetBaseline( GetHeight(), &blLargest );

	int nCurrX = 0;
	int nCurrY = 0;

	CRect rcClient;
	CPoint ptLoc;
	for ( size_t i = 0; i < m_LayoutOrder.size(); i++ )
	{
		GetBaseline( m_LayoutOrder.at( i )->GetFontSize(), &blCurrent );

		ptLoc.x = nCurrX;
		ptLoc.y = 0;
		ptLoc.y = blLargest.m_Ascending - blCurrent.m_Ascending;	

		m_LayoutOrder.at( i )->GetClientRect( rcClient );
		m_LayoutOrder.at( i )->MoveWindow( CRect( ptLoc, CSize( rcClient.Width(), blLargest.m_Height/*rcClient.Height()*/ ) ) );

		nCurrX += m_LayoutOrder.at( i )->GetWidth();
	}

	SetRedraw( true );

	if ( refresh )
		RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ERASENOW | RDW_ALLCHILDREN );
}

///////////////////////////////////////////////////

int CMusikNowPlayingInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::Set( CString mask )
{
	if ( mask.IsEmpty() )
		return;

	// clear old items
	m_Items.clear();

	// separate them out
	int pos = 0;
	CString resToken;
	resToken = mask.Tokenize( _T( "%" ), pos);
	while ( resToken != "" )
	{
		resToken.Replace( _T( "%" ), _T( "" ) );
		m_Items.push_back( resToken.GetBuffer() );
		resToken = mask.Tokenize( _T( "%" ), pos );
	};

	// reinitalize the objects
	InitObjects();
}

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::InitObjects()
{
	// clear the old items
	for ( size_t i = 0; i < m_Captions.size(); i++ )
	{	
		delete m_Captions.at( i );
		delete m_Comments.at( i );
	}
	
	m_Captions.clear();
	m_Comments.clear();
	m_LayoutOrder.clear();

	// make new items
    size_t nObjs = m_Items.size();
	CString sCheck, sCurr;
	for ( size_t i = 0; i < nObjs; i++ )
	{
		sCheck = m_Items.at( i ).Left( 1 );
		sCheck.MakeLower();

		// we found a new font
		if ( sCheck == _T( "f" ) || sCheck == _T( "b" ) || sCheck == _T( "i" ) || sCheck == _T( "m" ) )
		{
			sCurr = m_Items.at( i ).Right( m_Items.at( i ).GetLength()  - 1 );
			sCurr.TrimLeft();
			sCurr.TrimRight();

			m_FontSize = atoi( sCurr.GetBuffer() );

			if ( sCheck == _T( "b" ) || sCheck == _T( "m" ) )
				m_FontBold = 1;
			else
				m_FontBold = 0;

			if ( sCheck == _T( "i" ) || sCheck == _T( "m" ) )
				m_FontItalic = 1;
			else
				m_FontItalic = 0;
		}

		// found a comment (default font)
		else if ( sCheck == _T( "c" ) )
		{
			// find the comment
			sCurr = m_Items.at( i ).Right( m_Items.at( i ).GetLength()  - 1 );

			// create the object
			CMusikDynamicText* pTemp;
			pTemp = new CMusikDynamicText();
			pTemp->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this );

			// set it to the right values
			pTemp->SetDynFont( m_Prefs->GetNowPlayingCaptionFontSize() );
			pTemp->SetDynText( sCurr );
			pTemp->SetType( -1 );

			// add to comment array
			m_Comments.push_back( pTemp );
			m_LayoutOrder.push_back( pTemp );
		}

		// found a new object, so create it
		// with the current font and text
		else if ( sCheck == _T( "a" ) )
		{
			sCurr = m_Items.at( i ).Right( m_Items.at( i ).GetLength()  - 1 );
			sCurr.TrimLeft();
			sCurr.TrimRight();

			CMusikDynamicText* pTemp;
			pTemp = new CMusikDynamicText();
			pTemp->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this );

			pTemp->SetDynFont( m_FontSize, m_FontBold, m_FontItalic );
			pTemp->SetDynText( "Stopped" );
			pTemp->SetType( atoi( sCurr.GetBuffer() ) );

			m_Captions.push_back( pTemp );
			m_LayoutOrder.push_back( pTemp );
		}
		else
			TRACE0( "Invalid mask specified." );
	}

	if ( m_Player->IsPlaying() )
		UpdateInfo();
}

///////////////////////////////////////////////////

int CMusikNowPlayingInfo::GetHeight()
{
	int largest = -1;
	for ( size_t i = 0; i < m_LayoutOrder.size(); i++ )
	{
		if ( m_LayoutOrder.at( i )->GetFontSize() > largest )
			largest = m_Captions.at( i )->GetFontSize();
	}

	return largest;
}

///////////////////////////////////////////////////

int CMusikNowPlayingInfo::GetWidth()
{
	int width = 0;

	for ( size_t i = 0; i < m_Captions.size(); i++ )
		width += m_Captions.at( i )->GetWidth();

	for ( size_t i = 0; i < m_Comments.size(); i++ )
		width += m_Comments.at ( i )->GetWidth();

	return width;
}

///////////////////////////////////////////////////

CSize CMusikNowPlayingInfo::GetSize()
{
	return CSize( GetWidth(), GetHeight() ); 
}

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::GetBaseline( int font_size, CMusikFontBaseline* baseline )
{
	CFont *temp_font;

	temp_font = new CFont();
	temp_font->CreateStockObject( DEFAULT_GUI_FONT );	
	
	LOGFONT temp_log_font;
	temp_font->GetLogFont( &temp_log_font );
	temp_log_font.lfHeight = font_size;

	delete temp_font;
	temp_font = new CFont();
	temp_font->CreateFontIndirect( &temp_log_font );
    
	CDC *pDC = GetDC();
	TEXTMETRIC metrics;
	CFont* pOldFont = pDC->SelectObject( temp_font );
	pDC->GetTextMetrics( &metrics );
	pDC->SelectObject( pOldFont );

	delete temp_font;

	baseline->m_Ascending = metrics.tmAscent;
	baseline->m_Descending = metrics.tmDescent;
	baseline->m_Height = metrics.tmHeight;
}

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::OnPaint()
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

BOOL CMusikNowPlayingInfo::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////
