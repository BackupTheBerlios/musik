///////////////////////////////////////////////////
//
// Class(s): 
//
//   CmusikDynamicText 
//
// Filename(s): 
//
//   musikDynamicText.h,
//   musikDynamicText.cpp
//
// Information:
//
//   Just a window that dynamically sizes itself
//   based on the window's text.
//
// Example: 
//
//   Used in CmusikNowPlayingInfo
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
#include "musik.h"
#include "musikDynamicText.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDynamicText, CStatic)

///////////////////////////////////////////////////

CmusikDynamicText::CmusikDynamicText()
{
	m_Width = -1;
	m_FontSize = -1;
	pFont = NULL;
}

///////////////////////////////////////////////////

CmusikDynamicText::~CmusikDynamicText()
{
	if ( pFont )
		delete pFont;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDynamicText, CWnd)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikDynamicText::SetDynText( const CString& str )
{
	CString sVerify = str;
	sVerify.Replace( _T( "&" ), _T( "&&" ) );
	SetWindowText( sVerify );

	UpdateDynSize();
}

///////////////////////////////////////////////////

void CmusikDynamicText::UpdateDynSize()
{
	// get text
	CString str;
	GetWindowText( str );
	str.Replace( "&&", "&" );

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

void CmusikDynamicText::SetDynFont( int size, int bold, int italic )
{
	if ( pFont )
		delete pFont;

	pFont = new CFont();
	pFont->CreateStockObject( DEFAULT_GUI_FONT );

	// get the font
	LOGFONT pLogFont;
	pFont->GetLogFont( &pLogFont );

	pLogFont.lfHeight = size;

	if ( bold )
		pLogFont.lfWeight = FW_BOLD;
	if ( italic )
		pLogFont.lfItalic = true;

	// delete default font, make a new one
	// from the log font
	delete pFont;
	pFont = new CFont();
	pFont->CreateFontIndirect( &pLogFont );	

	SetFont( pFont, true );
	m_FontSize = abs( pLogFont.lfHeight );
}

///////////////////////////////////////////////////

