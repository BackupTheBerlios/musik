///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikDynamicText 
//
// Filename(s): 
//
//   MusikDynamicText.h,
//   MusikDynamicText.cpp
//
// Information:
//
//   Just a window that dynamically sizes itself
//   based on the window's text.
//
// Example: 
//
//   Used in CMusikNowPlayingInfo
//
// Usage: 
//
//	 SetDynText() to set the window text, 
//   and UpdateDynSize() to manually resize
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
#include "MusikDynamicText.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CMusikDynamicText, CStatic)

///////////////////////////////////////////////////

CMusikDynamicText::CMusikDynamicText()
{
	m_Width = -1;
	pFont = NULL;
}

///////////////////////////////////////////////////

CMusikDynamicText::~CMusikDynamicText()
{
	if ( pFont )
		delete pFont;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikDynamicText, CWnd)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikDynamicText::SetDynText( const CString& str )
{
	SetWindowText( str );
	UpdateDynSize();
}

///////////////////////////////////////////////////

void CMusikDynamicText::UpdateDynSize()
{
	// get text
	CString str;
	GetWindowText( str );

	// get text extent
	CSize szText;
	HDC	hMemDC	= NULL;
	hMemDC = CreateCompatibleDC(NULL);
	if ( hMemDC )
	{
		CDC* pDC = CDC::FromHandle( hMemDC );
		if ( pDC )
		{
			CFont* pOldFont = pDC->SelectObject( this->GetFont() );
			szText = pDC->GetTextExtent( (LPCTSTR)str, str.GetLength() );
			pOldFont = pDC->SelectObject( pOldFont );
		}
	}

	m_Width = szText.cx;

	// resize the window
	MoveWindow( CRect( CPoint( 0, 0 ), szText ) );
}

///////////////////////////////////////////////////

int CMusikDynamicText::GetFontSize()
{
	LOGFONT temp_log_font;
	GetFont()->GetLogFont( &temp_log_font );

	return temp_log_font.lfHeight;
}

///////////////////////////////////////////////////

void CMusikDynamicText::SetDynFont( int size, int bold, int italic, int underline, int strikethrough )
{
	if ( pFont )
		delete pFont;

	pFont = new CFont();
	pFont->CreateStockObject( DEFAULT_GUI_FONT );

	// comment
	if ( size == -1 || !size )
		return;

	// get the font
	LOGFONT pLogFont;
	pFont->GetLogFont( &pLogFont );

	pLogFont.lfHeight = size;
	if ( bold )
		pLogFont.lfWeight = FW_BOLD;
	if ( italic )
		pLogFont.lfItalic = true;
	if ( underline )
		pLogFont.lfUnderline = true;
	if ( strikethrough )
		pLogFont.lfStrikeOut = true;

	// delete default font, make a new one
	// from the log font
	delete pFont;
	pFont = new CFont();
	pFont->CreateFontIndirect( &pLogFont );	

	SetFont( pFont, true );
}

///////////////////////////////////////////////////

