// %a 1 <-- attribute
// %f 2 <-- regular font size
// %b 2 <-- bold
// %i 2 <-- italic
// %m 2 <-- bold / italic
// %c from the album <-- comment

///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include <vector>

#include "MusikDynamicText.h"

///////////////////////////////////////////////////

class CMusikPlayer;
class CMusikPrefs;

///////////////////////////////////////////////////

typedef std::vector<CMusikDynamicText*> CMusikDynamicTextArray;
typedef std::vector<CString> CCStringArray;

///////////////////////////////////////////////////

class CMusikFontBaseline
{
public:

	CMusikFontBaseline()
	{
		m_Height = -1;
		m_Ascending = -1;
		m_Descending = -1;
	}

    ~CMusikFontBaseline()
	{

	}

	int m_Height;
	int m_Ascending;
	int m_Descending;
};

///////////////////////////////////////////////////

class CMusikNowPlayingInfo : public CWnd
{
public:
	// construct and destruct
	CMusikNowPlayingInfo( CMusikPlayer* player, CMusikPrefs *prefs );
	virtual ~CMusikNowPlayingInfo();

	// getting
	CSize GetSize();
	int GetHeight();
	int GetWidth();
	void GetBaseline( int font_size, CMusikFontBaseline* baseline );

	// setting
	void Set( CString mask );

	// updating
	void UpdateInfo( bool refresh = true );
	void Layout( bool refresh = true );


protected:

	// core stuff
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// initalizing the objects
	void InitObjects();

	// current font size
	int m_FontSize;
	int m_FontBold;
	int m_FontItalic;

	// arrays so we can easily keep track
	// of which font goes to which item...
	CMusikDynamicTextArray m_Captions;
	CMusikDynamicTextArray m_Comments;
	CMusikDynamicTextArray m_LayoutOrder;
	CCStringArray m_Items;

	// macros
	DECLARE_DYNAMIC(CMusikNowPlayingInfo)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

///////////////////////////////////////////////////

