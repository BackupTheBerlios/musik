///////////////////////////////////////////////////

#include "stdafx.h"
#include "Musik.h"

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
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::UpdateInfo( bool refresh )
{
	int type;
	for ( size_t i = 0; i < m_Captions.size(); i++ )
	{
		type = m_Captions.at( i )->GetType();
		m_Captions.at( i )->SetDynText( (CString)m_Player->GetCurrPlaying()->GetField( type ) );		
	}
	Layout( refresh );
}

///////////////////////////////////////////////////

void CMusikNowPlayingInfo::Layout( bool refresh )
{
	CSize rect = GetSize();

	int nBaseline = GetBaseline( GetHeight() );

	int nCurrX = 0;
	int nCurrY = 0;
	int nCurrBase;
	CRect rcClient;
	for ( size_t i = 0; i < m_LayoutOrder.size(); i++ )
	{
		nCurrBase = GetBaseline( m_LayoutOrder.at( i )->GetFontSize() );
		nCurrY = nBaseline - nCurrBase;

		m_LayoutOrder.at( i )->GetClientRect( rcClient );
		m_LayoutOrder.at( i )->MoveWindow( CRect( CPoint( nCurrX, nCurrY ), CSize( rcClient.Width(), rcClient.Height() ) ) );

		if ( refresh )
			m_LayoutOrder.at( i )->RedrawWindow();

		nCurrX += m_LayoutOrder.at( i )->GetWidth();
	}
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
	if ( !mask.IsEmpty() )
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
		if ( sCheck == _T( "f" ) )
		{
			sCurr = m_Items.at( i ).Right( m_Items.at( i ).GetLength()  - 1 );
			sCurr.TrimLeft();
			sCurr.TrimRight();

			m_FontSize = atoi( sCurr.GetBuffer() );
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
			pTemp->SetDynFont( -1 );
			pTemp->SetDynText( sCurr );
			pTemp->SetType( -1 );

			// add to comment array
			m_Comments.push_back( pTemp );
			m_LayoutOrder.push_back( pTemp );
		}

		// found a new object, so create it
		// with the current font and text
		else
		{
			sCurr = m_Items.at( i ).Right( m_Items.at( i ).GetLength()  - 1 );
			sCurr.TrimLeft();
			sCurr.TrimRight();

			CMusikDynamicText* pTemp;
			pTemp = new CMusikDynamicText();
			pTemp->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this );

			pTemp->SetDynFont( m_FontSize );
			pTemp->SetDynText( "Null" );
			pTemp->SetType( atoi( sCurr.GetBuffer() ) );

			m_Captions.push_back( pTemp );
			m_LayoutOrder.push_back( pTemp );
		}
	}
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

int CMusikNowPlayingInfo::GetBaseline( int font_size )
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

	return metrics.tmAscent;
}

///////////////////////////////////////////////////