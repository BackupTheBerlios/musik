// %a 1 <-- attribute
// %f 2 <-- regular font size
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
	int GetBaseline();

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

	// arrays so we can easily keep track
	// of which font goes to which item...
	CMusikDynamicTextArray m_Captions;
	CMusikDynamicTextArray m_Comments;
	CCStringArray m_Items;

	// macros
	DECLARE_DYNAMIC(CMusikNowPlayingInfo)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

///////////////////////////////////////////////////

