///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikDynamicTextArray,
//   CmusikFontBaseline,
//   CmusikNowPlayingInfo
//   
//
// Filename(s): 
//
//   musikNowPlayingInfo.h,
//   musikNowPlayingInfo.cpp
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

#pragma once

///////////////////////////////////////////////////

#include <vector>

#include "musikDynamicText.h"

///////////////////////////////////////////////////

class CmusikPlayer;
class CmusikPrefs;

///////////////////////////////////////////////////

typedef std::vector<CmusikDynamicText*> CmusikDynamicTextArray;
typedef std::vector<CString> CCStringArray;

///////////////////////////////////////////////////

class CmusikFontBaseline
{
public:

	CmusikFontBaseline()
	{
		m_Height = -1;
		m_Ascending = -1;
		m_Descending = -1;
	}

    ~CmusikFontBaseline()
	{

	}

	int m_Height;
	int m_Ascending;
	int m_Descending;
};

///////////////////////////////////////////////////

class CmusikNowPlayingInfo : public CWnd
{
public:
	// construct and destruct
	CmusikNowPlayingInfo( CmusikPlayer* player, CmusikPrefs *prefs );
	virtual ~CmusikNowPlayingInfo();

	// getting
	CSize GetSize();
	int GetHeight();
	int GetWidth();
	void GetBaseline( int font_size, CmusikFontBaseline* baseline );

	// setting
	void Set( CString mask );

	// updating
	void UpdateInfo( bool refresh = true );
	void Layout( bool refresh = true );


protected:

	// core stuff
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// initalizing the objects
	void InitObjects();

	// current font size
	int m_FontSize;
	int m_FontBold;
	int m_FontItalic;

	// arrays so we can easily keep track
	// of which font goes to which item...
	CmusikDynamicTextArray m_Captions;
	CmusikDynamicTextArray m_Comments;
	CmusikDynamicTextArray m_LayoutOrder;
	CCStringArray m_Items;

	// macros
	DECLARE_DYNAMIC(CmusikNowPlayingInfo)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

///////////////////////////////////////////////////

