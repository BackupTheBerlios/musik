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
//   CmusikNowPlayingCtrl, CmusikNowPlayingBar
//
// Filename(s): 
//
//   musikNowPlayingCtrl.h, musikNowPlayingCtrl.cpp
//
// Information:
//
//   Just a window that dynamically sizes itself
//   based on the window's text.
// 
// Usage: 
//
//	 SetDynText() to set the window text, 
//   and UpdateDynSize() to manually resize
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikPrefs;

///////////////////////////////////////////////////

class CmusikDynamicText : public CStatic
{
public:

	// construct and destruct
	CmusikDynamicText( CmusikPrefs* prefs );
	virtual ~CmusikDynamicText();

	// gets
	int GetFontSize(){ return abs( m_FontSize ); }
	int GetHeight() { return GetFontSize(); }
	int GetWidth() { return m_Width; }
	int GetType(){ return m_Type; }
	CSize GetSize() { return CSize( GetWidth(), GetHeight() ); }

	// sets
	void SetDynText( const CString& str, bool updatesize = true, bool movewindow = true );
	void SetDynFont( int size = -1, int bold = 0, int italic = 0 );
	void SetType( int n ){ m_Type = n; }

	// misc
	void UpdateDynSize( bool movewindow = true );
	CSize GetTextSize( const CString& str );
	CSize GetDynSize();

	// message handlers
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

protected:

	// fonts
	CFont* pFont;

	// prefs, used for theming...
	CmusikPrefs* m_Prefs;

	// vars
	int m_Width;
	int m_Type;
	int m_FontSize;

	// brush to hold bg color
	CBrush m_BgColor;

	// macros
	DECLARE_DYNAMIC(CmusikDynamicText)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////