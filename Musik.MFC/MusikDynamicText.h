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


#pragma once

///////////////////////////////////////////////////

class CMusikDynamicText : public CStatic
{

public:

	// construct and destruct
	CMusikDynamicText();
	virtual ~CMusikDynamicText();

	// gets
	int GetFontSize(){ return abs( m_FontSize ); }
	int GetHeight() { return GetFontSize(); }
	int GetWidth() { return m_Width; }
	int GetType(){ return m_Type; }
	CSize GetSize() { return CSize( GetWidth(), GetHeight() ); }

	// sets
	void SetDynText( const CString& str );
	void SetDynFont( int size = -1, int bold = 0, int italic = 0 );
	void SetType( int n ){ m_Type = n; }

	// misc
	void UpdateDynSize();

protected:

	// fonts
	CFont* pFont;

	// vars
	int m_Width;
	int m_Type;
	int m_FontSize;

	// macros
	DECLARE_DYNAMIC(CMusikDynamicText)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////