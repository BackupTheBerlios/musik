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
//   CmusikPropertyPage, CmusikPropertySheet
//
// Filename(s): 
//
//   musikPropertyPage.h, musikPropertyPage.cpp
//
// Information:
//
//   Property page that accepts a pointer to a CmusikPrefs obj
// 
// Usage: 
//
//   Use like a regular property page
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikPrefs.h"
#include "3rdparty/TreePropSheet.h"

///////////////////////////////////////////////////

using namespace TreePropSheet;

class CmusikPrefs;
class CmusikLibrary;
class CmusikPlayer;

///////////////////////////////////////////////////

// CmusikPropertyPage

///////////////////////////////////////////////////

class CmusikPropertyPage : public CPropertyPage
{

public:

	CmusikPropertyPage( UINT nIDTemplate, CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player );
	virtual ~CmusikPropertyPage(){}

	bool IsModified(){ return m_Modified; }

	virtual void CommitChanges(){}
	virtual void LoadPrefs(){}

protected:

	bool m_Modified;

	CmusikPrefs* m_Prefs;
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;

};

///////////////////////////////////////////////////

// CmusikPropertySheet

///////////////////////////////////////////////////

class CmusikPropertySheet : public CTreePropSheet
{
public:

	CmusikPropertySheet( LPCSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual ~CmusikPropertySheet(){}

protected:
    
	void CommitChanges();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

///////////////////////////////////////////////////